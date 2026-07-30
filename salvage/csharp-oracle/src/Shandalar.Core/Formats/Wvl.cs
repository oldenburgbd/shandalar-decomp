namespace Shandalar.Core.Formats;

/// <summary>A decoded WVL card-art image: RGBA8888 pixels, row-major top-to-bottom.</summary>
public sealed record WvlImage(int Width, int Height, byte[] Rgba);

/// <summary>
/// Decoder for the WVL wavelet-compressed card-art images stored inside the .cat archives.
/// Pipeline: VLC entropy decode of three planes (Y, Cb, Cr) → Haar-style wavelet pyramid
/// reconstruction from a small DC band → YCbCr→RGB with chroma upsampling.
/// Faithful port of the decompiled Magic.exe routines (ref: Magic.exe 0x4928AF VlcDecompress,
/// 0x505D9A/0x505EB2/0x506453 VLC, 0x4922CB/0x492410/0x4924E5 wavelet, 0x4925E6 YCbCr) via the
/// MIT-licensed ShandalarImageToolbox (ogamespec / CelestialAmber). See docs/formats/cat.md.
/// </summary>
public static class Wvl
{
    // WVL header byte offsets.
    private const int OffFlag = 0x00;        // nonzero → chroma is stored at half resolution
    private const int OffWidth = 0x1C;
    private const int OffHeight = 0x20;
    private const int OffDcBand = 0x24;      // DC band edge length (typically 9)
    private const int OffPieces = 0x28;
    private const int OffYSizes = 0x5C;      // per-piece compressed sizes: Y, Cb, Cr
    private const int OffCbSizes = 0x6C;
    private const int OffCrSizes = 0x7C;
    private const int OffDecompSize = 0x90;
    private const int HeaderSize = 0x9C;

    public static WvlImage Decode(byte[] wvl)
    {
        if (wvl.Length < HeaderSize)
            throw new InvalidDataException("WVL blob smaller than its header");

        int width = BitConverter.ToInt32(wvl, OffWidth);
        int height = BitConverter.ToInt32(wvl, OffHeight);
        int dcBand = BitConverter.ToInt32(wvl, OffDcBand);
        if (width <= 0 || height <= 0 || width > 4096 || height > 4096)
            throw new InvalidDataException($"WVL has implausible dimensions {width}x{height}");

        byte[] planes = new Vlc(wvl).Decompress();

        bool chromaHalf = BitConverter.ToInt32(wvl, OffFlag) != 0
            && BitConverter.ToInt32(wvl, OffPieces) == 1;
        int chromaEdge = chromaHalf ? width / 2 : width;
        int chromaW = chromaHalf ? width / 2 : width;
        int chromaH = chromaHalf ? height / 2 : height;

        // Plane starts (int indices): Y at 0; each plane is a square edge² block + 0x80-byte gap.
        int yPlane = 0;
        int cbPlane = (width * width * 4 + 0x80) / 4;
        int crPlane = cbPlane + (chromaEdge * chromaEdge * 4 + 0x80) / 4;

        var ints = new int[planes.Length / 4];
        Buffer.BlockCopy(planes, 0, ints, 0, ints.Length * 4);

        WaveletReconstruct(ints, yPlane, width, dcBand);
        WaveletReconstruct(ints, cbPlane, chromaEdge, dcBand);
        WaveletReconstruct(ints, crPlane, chromaEdge, dcBand);

        return ToRgba(ints, yPlane, width, height, cbPlane, crPlane, chromaW, chromaH, chromaHalf);
    }

    // ---------------------------------------------------------------- wavelet

    /// <summary>
    /// Rebuilds a square plane in place from its pyramid: starting at the DC band edge, each pass
    /// combines the stored sum/difference subbands, doubling the edge until it reaches
    /// <paramref name="width"/>. (ref: Magic.exe 0x4922CB)
    /// </summary>
    private static void WaveletReconstruct(int[] data, int plane, int width, int dcBand)
    {
        var buf1 = new int[0x10000];
        var buf2 = new int[0x10000];

        for (int size = dcBand; size < width; size *= 2)
        {
            Butterfly(data, plane, plane + size * size, buf1, size);
            Butterfly(data, plane + size * size * 2, plane + size * size * 3, buf2, size);
            Merge(buf1, buf2, data, plane, size);
        }
    }

    /// <summary>Column butterfly: interleaves sums and differences of two subbands. (0x492410)</summary>
    private static void Butterfly(int[] data, int tab1, int tab2, int[] tmp, int size)
    {
        int tmpIndex = 0;
        for (int i = 0; i < size; i++)
        {
            int rowStart = tab1;
            int rowEnd = tab1 + size;
            int outp = tmpIndex + size * 2;
            tab1++;

            while (tab1 < rowEnd)
            {
                tmp[outp] = data[tab1] + data[tab2];
                tmp[outp + size] = data[tab1] - data[tab2];
                tab1++;
                tab2++;
                outp += size * 2;
            }

            tmp[tmpIndex] = data[rowStart] + data[tab2];
            tmp[tmpIndex + size] = data[rowStart] - data[tab2];
            tmpIndex++;
            tab2++;
        }
    }

    /// <summary>Row merge of the two butterflied buffers back into the plane, halved. (0x4924E5)</summary>
    private static void Merge(int[] buf1, int[] buf2, int[] data, int outTab, int size)
    {
        int b1 = 0, b2 = 0;
        for (int i = 0; i < size * 2; i++)
        {
            int rowStart = b1;
            int rowEnd = b1 + size;
            int outp = outTab + size * 4;
            b1++;

            while (b1 < rowEnd)
            {
                data[outp] = (buf1[b1] + buf2[b2]) / 2;
                data[outp + size * 2] = (buf1[b1] - buf2[b2]) / 2;
                b1++;
                b2++;
                outp += size * 4;
            }

            data[outTab] = (buf1[rowStart] + buf2[b2]) / 2;
            data[outTab + size * 2] = (buf1[rowStart] - buf2[b2]) / 2;
            outTab++;
            b2++;
        }
    }

    // ---------------------------------------------------------------- YCbCr → RGBA

    /// <summary>(ref: Magic.exe 0x4925E6; the table clamps and >>2 scales the fixed-point values.)</summary>
    private static WvlImage ToRgba(int[] data, int yTab, int width, int height,
        int cbTab, int crTab, int chromaW, int chromaH, bool chromaHalf)
    {
        var clamp = new byte[0x2000];
        for (int i = -0x400; i < 0x1C00; i++)
            clamp[i + 0x400] = (byte)(i > 0 ? Math.Min(i >> 2, 0xFF) : 0);

        var rgba = new byte[width * height * 4];
        int outp = 0;

        for (int y = 0; y < height; y++)
        {
            int cb = cbTab + (chromaHalf ? y / 2 : y) * chromaW;
            int cr = crTab + (chromaHalf ? y / 2 : y) * chromaW;

            for (int x = 0; x < width; x++)
            {
                int yVal = data[yTab++];
                int cbVal, crVal;

                if (chromaHalf && (x & 1) != 0)
                {
                    // Horizontal lerp with the next chroma sample (itself at the last column).
                    int next = (width - x - 1) == 0 ? 0 : 1;
                    cbVal = (data[cb] + data[cb + next]) / 2;
                    crVal = (data[cr] + data[cr + next]) / 2;
                    cb++;
                    cr++;
                }
                else
                {
                    cbVal = data[cb];
                    crVal = data[cr];
                    if (!chromaHalf)
                    {
                        cb++;
                        cr++;
                    }
                }

                int r = yVal + crVal + crVal / 2 + crVal / 8 - 0x333;
                int b = yVal + cbVal * 2 - 0x400;
                int g = yVal * 2 - yVal / 4 - r / 2 - b / 4 - b / 16;

                rgba[outp] = clamp[Math.Clamp(r + 0x400, 0, 0x1FFF)];
                rgba[outp + 1] = clamp[Math.Clamp(g + 0x400, 0, 0x1FFF)];
                rgba[outp + 2] = clamp[Math.Clamp(b + 0x400, 0, 0x1FFF)];
                rgba[outp + 3] = 255;
                outp += 4;
            }
        }

        return new WvlImage(width, height, rgba);
    }

    // ---------------------------------------------------------------- VLC entropy coder

    /// <summary>
    /// The VLC entropy decoder: a Huffman-style code tree (13-bit node indices) accelerated by a
    /// 256-entry lookup table, with zero-run codes, producing the three wavelet planes.
    /// (ref: Magic.exe 0x4928AF / 0x505D9A / 0x505EB2 / 0x506453 / 0x506AD0 / 0x506BF0)
    /// </summary>
    private sealed class Vlc
    {
        private readonly byte[] _wvl;

        private byte[] _stream = [];
        private int _start, _ptr, _limit;
        private uint _dword;
        private int _bitsLeft;

        private readonly (int Left, int Right)[] _tree = new (int, int)[0x2000];
        private int _treeCount;
        private int[] _symbols = [];
        private int _symbolCount;
        // Lookup: Kind 0 = literal (Value, Bits), 1 = zero-run (Bits), 2 = walk tree from Node.
        private readonly (int Kind, int Value, int Bits, int Node)[] _lookup = new (int, int, int, int)[0x100];

        public Vlc(byte[] wvl) => _wvl = wvl;

        public byte[] Decompress()
        {
            int width = BitConverter.ToInt32(_wvl, OffWidth);
            int pieces = BitConverter.ToInt32(_wvl, OffPieces);
            int dcBand = BitConverter.ToInt32(_wvl, OffDcBand);

            int lumaEdge = width / (pieces - 1 == 0 ? 1 : 2);
            int chromaEdge = lumaEdge / (BitConverter.ToInt32(_wvl, OffFlag) == 0 ? 1 : 2);
            int planeInts = lumaEdge * lumaEdge + 2 * chromaEdge * chromaEdge;

            var result = new byte[BitConverter.ToInt32(_wvl, OffDecompSize) + 20000];
            byte[] data = _wvl[HeaderSize..];

            int offset = 0;
            _symbolCount = BitConverter.ToInt32(data, 0);
            offset += 4;
            _symbols = new int[_symbolCount];
            Buffer.BlockCopy(data, offset, _symbols, 0, _symbolCount * 4);
            _symbols[0] = int.MinValue; // symbol 0 is the zero-run escape
            offset += _symbolCount * 4;

            offset += ReadTree(data, offset);

            int tableBytes = dcBand * dcBand * 4;
            for (int piece = 0; piece < pieces; piece++)
            {
                int yOff = (planeInts + 0x40) * piece * 4;
                int cbOff = yOff + lumaEdge * lumaEdge * 4 + 0x80;
                int crOff = cbOff + chromaEdge * chromaEdge * 4 + 0x80;

                offset = CopyDcAndDecode(data, offset, result, yOff, tableBytes,
                    BitConverter.ToInt32(_wvl, OffYSizes + piece * 4));
                offset = CopyDcAndDecode(data, offset, result, cbOff, tableBytes,
                    BitConverter.ToInt32(_wvl, OffCbSizes + piece * 4));
                offset = CopyDcAndDecode(data, offset, result, crOff, tableBytes,
                    BitConverter.ToInt32(_wvl, OffCrSizes + piece * 4));
            }
            return result;
        }

        private int CopyDcAndDecode(byte[] data, int offset, byte[] result, int outOff,
            int tableBytes, int chunkSize)
        {
            Array.Copy(data, offset, result, outOff, tableBytes);
            DecodeChunk(data, offset + tableBytes, result, outOff + tableBytes, chunkSize);
            return offset + tableBytes + chunkSize;
        }

        // --- bitstream ---

        private int GetBits(int bits)
        {
            if (bits <= _bitsLeft)
            {
                uint v = _dword & (0xFFFFFFFF >> (32 - bits));
                _dword >>= bits;
                _bitsLeft -= bits;
                return (int)v;
            }

            uint value = _dword;
            bits -= _bitsLeft;
            if (_ptr - _start + 4 < _limit && _ptr + 4 <= _stream.Length)
            {
                _dword = BitConverter.ToUInt32(_stream, _ptr);
                _ptr += 4;
                value |= (_dword & (0xFFFFFFFF >> (32 - bits))) << _bitsLeft;
                _dword >>= bits;
                _bitsLeft = 32 - bits;
                return (int)value;
            }
            return -1;
        }

        private int GetNextBit()
        {
            if (_bitsLeft == 0)
            {
                int bytesLeft = _limit - (_ptr - _start);
                if (bytesLeft <= 0 || _ptr >= _stream.Length)
                    return -1;
                bytesLeft = Math.Min(bytesLeft, _stream.Length - _ptr);
                if (bytesLeft < 4)
                {
                    _dword = 0;
                    for (int i = 0; i < bytesLeft; i++)
                        _dword += (uint)(_stream[_ptr + i] << (i * 8));
                    _ptr += bytesLeft;
                }
                else
                {
                    _dword = BitConverter.ToUInt32(_stream, _ptr);
                    _ptr += 4;
                }
                _bitsLeft = 32;
            }

            int bit = (int)(_dword & 1);
            _dword >>= 1;
            _bitsLeft--;
            return bit;
        }

        // --- code tree + lookup table ---

        /// <summary>Reads the 13-bit-node code tree and builds the 256-entry lookup. Returns bytes consumed.</summary>
        private int ReadTree(byte[] data, int offset)
        {
            _stream = data;
            _start = _ptr = offset;
            _limit = 100000;
            _dword = 0;
            _bitsLeft = 0;

            _treeCount = GetBits(13);
            int bitsProcessed = 13;
            for (int i = 0; i < _treeCount; i++)
            {
                _tree[i] = (GetBits(13), GetBits(13));
                bitsProcessed += 26;
            }

            BuildLookup(_treeCount);
            return (bitsProcessed + 7) / 8;
        }

        /// <summary>
        /// Enumerates all code paths up to 8 bits deep to fill the byte-indexed lookup table
        /// (paths ending in a leaf replicate over the unused high bits; deeper paths store the
        /// tree node to continue from). Faithful port of the original traversal. (0x505EB2)
        /// </summary>
        private void BuildLookup(int count)
        {
            var pending = new int[32];
            var path = new int[100];
            for (int i = 0; i < 32; i++)
                pending[i] = 1;

            int depth = 0;
            int node = count - 1;
            path[0] = node;

            while (pending[0] >= 0)
            {
                int branchBit = depth++;
                int next = pending[branchBit] != 0 ? _tree[node].Left : _tree[node].Right;

                node = next - _symbolCount;
                path[depth] = node;

                if (node >= 0)
                {
                    if (depth == 8)
                    {
                        int code = 0;
                        for (int b = 0; b < depth; b++)
                            code |= pending[b] << b;
                        _lookup[code] = (Kind: 2, Value: 0, Bits: depth, Node: node);

                        pending[depth] = 1;
                        depth--;
                        pending[depth]--;
                        node = path[depth];
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    int code = 0;
                    for (int b = 0; b < depth; b++)
                        code |= pending[b] << b;

                    int symbol = _symbols[next];
                    int kind = symbol == int.MinValue ? 1 : 0;
                    for (int hi = 0; hi < (1 << (8 - depth)); hi++)
                    {
                        int idx = (hi << depth) | code;
                        _lookup[idx] = (Kind: kind, Value: symbol, Bits: depth, Node: -1);
                    }

                    pending[depth] = 1;
                    depth--;
                    pending[depth]--;
                    node = path[depth];
                }

                while (pending[0] >= 0 && pending[depth] < 0)
                {
                    pending[depth] = 1;
                    depth--;
                    pending[depth]--;
                    node = path[depth];
                }
            }
        }

        // --- chunk decode ---

        private void DecodeChunk(byte[] data, int offset, byte[] outArray, int outOffset, int size)
        {
            int index = outOffset;
            _stream = data;
            _ptr = _start = offset;
            _limit = size;

            if ((offset & 3) != 0)
            {
                int unaligned = 4 - (offset & 3);
                _bitsLeft = unaligned * 8;
                _dword = BitConverter.ToUInt32(data, offset) & (0xFFFFFFFF >> (32 - _bitsLeft));
                _ptr += unaligned;
            }
            else
            {
                _bitsLeft = 0;
                _dword = 0;
            }

            int window = GetBits(8);
            while (window != -1)
            {
                var entry = _lookup[window];
                if (entry.Kind == 2)
                {
                    // Slow path: walk the tree bit-by-bit from the stored node.
                    int node = entry.Node;
                    int next;
                    do
                    {
                        int bit = GetNextBit();
                        if (bit == -1)
                            goto refill;
                        next = bit != 0 ? _tree[node].Left : _tree[node].Right;
                        node = next - _symbolCount;
                    } while (node >= 0);

                    if (next != 0)
                    {
                        WriteInt(outArray, ref index, _symbols[next]);
                    }
                    else
                    {
                        int zeroes = GetBits(10);
                        if (zeroes >= 0)
                            index = WriteZeroes(outArray, index, zeroes);
                    }

                refill:
                    window = GetBits(8);
                }
                else if (entry.Kind == 1)
                {
                    // Zero-run: length from extra bits combined with the leftover window bits.
                    int extra = GetBits(entry.Bits + 2);
                    if (extra < 0)
                        break;
                    int run = (extra << (8 - entry.Bits)) | (window >> entry.Bits);
                    index = WriteZeroes(outArray, index, run);
                    window = GetBits(8);
                }
                else
                {
                    // Literal: emit the symbol, slide the window.
                    WriteInt(outArray, ref index, entry.Value);
                    window >>= entry.Bits;
                    int refillBits = GetBits(entry.Bits);
                    if (refillBits == -1)
                        break;
                    window |= refillBits << (8 - entry.Bits);
                }
            }
        }

        private static void WriteInt(byte[] outArray, ref int index, int value)
        {
            if (index + 4 <= outArray.Length)
            {
                outArray[index] = (byte)value;
                outArray[index + 1] = (byte)(value >> 8);
                outArray[index + 2] = (byte)(value >> 16);
                outArray[index + 3] = (byte)(value >> 24);
            }
            index += 4;
        }

        private static int WriteZeroes(byte[] outArray, int index, int count)
        {
            int end = index + count * 4;
            int clampedEnd = Math.Min(end, outArray.Length);
            for (int i = index; i < clampedEnd; i++)
                outArray[i] = 0;
            return end;
        }
    }
}
