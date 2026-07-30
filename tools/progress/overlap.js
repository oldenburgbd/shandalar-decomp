#!/usr/bin/env node
// Estimate shared-code overlap between two analyzed binaries using structural
// fingerprints from the Ghidra function inventory CSVs.
// Fingerprint = (size, basic-block count, callee count). Collisions are handled
// by greedy multiset matching, so the result is a lower bound on true overlap.
const fs = require('fs');

function load(path) {
  const lines = fs.readFileSync(path, 'utf8').trim().split(/\r?\n/);
  const hdr = lines[0].split(',');
  const ix = (n) => hdr.indexOf(n);
  const [iA, iN, iS, iT, iB, iCr, iCe, iSec] =
    ['address','name','size','is_thunk','num_blocks','num_callers','num_callees','section'].map(ix);
  const out = [];
  for (let i = 1; i < lines.length; i++) {
    // naive split is safe here: only name/namespace/convention can be quoted,
    // and generated FUN_ names never contain commas.
    const p = lines[i].split(',');
    if (p.length < hdr.length) continue;
    if (p[iT] === 'true') continue;                 // skip thunks
    out.push({ addr: p[iA], name: p[iN], size: +p[iS], blocks: +p[iB],
               callers: +p[iCr], callees: +p[iCe], section: p[iSec] });
  }
  return out;
}

const [fileA, fileB] = process.argv.slice(2);
const A = load(fileA), B = load(fileB);

const key = (f) => `${f.size}|${f.blocks}|${f.callees}`;
const bmap = new Map();
for (const f of B) {
  const k = key(f);
  if (!bmap.has(k)) bmap.set(k, []);
  bmap.get(k).push(f);
}

let matched = 0, matchedBytes = 0;
const unmatchedA = [];
for (const f of A) {
  const k = key(f);
  const bucket = bmap.get(k);
  if (bucket && bucket.length) { bucket.pop(); matched++; matchedBytes += f.size; }
  else unmatchedA.push(f);
}

const totalA = A.reduce((s, f) => s + f.size, 0);
const totalB = B.reduce((s, f) => s + f.size, 0);
const unmatchedBytes = totalA - matchedBytes;

const name = (p) => p.split(/[\\/]/).pop().replace('.functions.csv', '');
console.log(`A = ${name(fileA)}: ${A.length} funcs, ${totalA} bytes`);
console.log(`B = ${name(fileB)}: ${B.length} funcs, ${totalB} bytes`);
console.log(`\nSHARED  : ${matched} funcs (${(100*matched/A.length).toFixed(1)}% of A), ` +
            `${matchedBytes} bytes (${(100*matchedBytes/totalA).toFixed(1)}% of A)`);
console.log(`A-ONLY  : ${unmatchedA.length} funcs, ${unmatchedBytes} bytes ` +
            `(${(100*unmatchedBytes/totalA).toFixed(1)}% of A)`);
console.log(`\nUNION (unique code to reverse) ~ ${matchedBytes + unmatchedBytes + (totalB - matchedBytes)} bytes`);

unmatchedA.sort((a, b) => b.size - a.size);
console.log(`\n-- 20 largest A-only functions --`);
for (const f of unmatchedA.slice(0, 20))
  console.log(`  ${String(f.size).padStart(6)}  ${f.addr}  blocks=${String(f.blocks).padStart(4)}  callers=${String(f.callers).padStart(4)}  ${f.section}`);

const hist = { '<64': 0, '64-256': 0, '256-1K': 0, '1K-4K': 0, '>4K': 0 };
for (const f of unmatchedA)
  hist[f.size < 64 ? '<64' : f.size < 256 ? '64-256' : f.size < 1024 ? '256-1K' : f.size < 4096 ? '1K-4K' : '>4K']++;
console.log(`\n-- A-only size histogram --`);
for (const k of Object.keys(hist)) console.log(`  ${k.padEnd(8)} ${hist[k]}`);
