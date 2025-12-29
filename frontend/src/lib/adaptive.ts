import { rleCompress, rleDecompress } from './rle';

/**
 * Adaptive, lossless format:
 * - empty -> empty
 * - non-empty: [1 byte tag][payload...]
 *   - 'R' => payload is RLE-compressed
 *   - 'I' => payload is identity/raw
 */

const TAG_RLE = 'R'.charCodeAt(0);
const TAG_ID = 'I'.charCodeAt(0);

export function adaptiveCompress(input: Uint8Array): Uint8Array {
  if (input.length === 0) return new Uint8Array(0);

  const rle = rleCompress(input);
  const rleSize = 1 + rle.length;
  const idSize = 1 + input.length;

  if (rleSize < idSize) {
    const out = new Uint8Array(rleSize);
    out[0] = TAG_RLE;
    out.set(rle, 1);
    return out;
  }

  const out = new Uint8Array(idSize);
  out[0] = TAG_ID;
  out.set(input, 1);
  return out;
}

export function adaptiveDecompress(input: Uint8Array): Uint8Array {
  if (input.length === 0) return new Uint8Array(0);

  const tag = input[0]!;
  const payload = input.subarray(1);

  if (tag === TAG_RLE) return rleDecompress(payload);
  if (tag === TAG_ID) return payload.slice();

  throw new Error('Unknown adaptive compression tag');
}


