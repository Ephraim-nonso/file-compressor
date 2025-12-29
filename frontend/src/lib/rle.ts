/**
 * Run-Length Encoding (RLE) for binary data.
 *
 * Format: for each run -> [byte, count] where count is 1..255.
 * This is lossless (decompress restores original bytes).
 */

export function rleCompress(input: Uint8Array): Uint8Array {
  if (input.length === 0) return new Uint8Array(0);

  const out: number[] = [];

  let current = input[0]!;
  let run = 1;

  for (let i = 1; i < input.length; i++) {
    const b = input[i]!;
    if (b === current && run < 255) {
      run++;
      continue;
    }
    out.push(current, run);
    current = b;
    run = 1;
  }

  out.push(current, run);
  return Uint8Array.from(out);
}

export function rleDecompress(input: Uint8Array): Uint8Array {
  if (input.length === 0) return new Uint8Array(0);
  if (input.length % 2 !== 0) {
    throw new Error('RLE input is malformed (odd length)');
  }

  // Pre-compute output size for a single allocation.
  let total = 0;
  for (let i = 1; i < input.length; i += 2) {
    const count = input[i]!;
    if (count === 0) throw new Error('RLE input is malformed (zero count)');
    total += count;
  }

  const out = new Uint8Array(total);
  let off = 0;
  for (let i = 0; i < input.length; i += 2) {
    const value = input[i]!;
    const count = input[i + 1]!;
    out.fill(value, off, off + count);
    off += count;
  }
  return out;
}




