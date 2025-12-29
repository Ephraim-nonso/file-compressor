export function formatBytes(n: number): string {
  if (!Number.isFinite(n)) return '-';
  if (n < 1024) return `${n} B`;
  const kb = n / 1024;
  if (kb < 1024) return `${kb.toFixed(2)} KB`;
  const mb = kb / 1024;
  return `${mb.toFixed(2)} MB`;
}

export function toHexPreview(bytes: Uint8Array, max: number = 96): string {
  const len = Math.min(bytes.length, max);
  const parts: string[] = [];
  for (let i = 0; i < len; i++) {
    parts.push(bytes[i]!.toString(16).padStart(2, '0'));
  }
  const suffix = bytes.length > max ? ` … (+${bytes.length - max} bytes)` : '';
  return parts.join(' ') + suffix;
}


