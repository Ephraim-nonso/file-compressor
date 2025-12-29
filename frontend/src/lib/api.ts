const DEFAULT_BASE = "http://localhost:8081";

function getApiBase(): string {
  // Waku exposes public env vars through import.meta.env
  // e.g. WAKU_PUBLIC_API_BASE="http://localhost:8081"
  const envBase = (import.meta as any)?.env?.WAKU_PUBLIC_API_BASE as
    | string
    | undefined;
  return envBase && envBase.trim().length > 0 ? envBase : DEFAULT_BASE;
}

async function postBinary(
  path: string,
  bytes: Uint8Array
): Promise<Uint8Array> {
  const url = `${getApiBase()}${path}`;
  // Some TS libdom versions are strict about Uint8Array<ArrayBufferLike>;
  // send a plain ArrayBuffer to satisfy BodyInit typing.
  const body = bytes.slice().buffer as ArrayBuffer;
  let res: Response;
  try {
    res = await fetch(url, {
      method: "POST",
      headers: {
        "Content-Type": "application/octet-stream",
        Accept: "application/octet-stream",
      },
      body,
    });
  } catch (e) {
    // Browser fetch throws TypeError on network failures (CORS blocked, connection refused, etc).
    const extra =
      `Failed to fetch from ${url}.\n` +
      `Make sure your C++ HTTP server is running and reachable.\n` +
      `- Local: run ./build/http_server 8081\n` +
      `- Docker: docker compose up --build api\n` +
      `If your frontend runs in Docker too, set WAKU_PUBLIC_API_BASE to the container DNS name (e.g. http://api:8081).`;
    throw new Error(extra);
  }

  if (!res.ok) {
    const text = await res.text().catch(() => "");
    throw new Error(
      `HTTP ${res.status} ${res.statusText}${text ? `: ${text}` : ""}`
    );
  }
  const buf = await res.arrayBuffer();
  return new Uint8Array(buf);
}

export async function apiCompress(bytes: Uint8Array): Promise<Uint8Array> {
  return postBinary("/compress", bytes);
}

export async function apiDecompress(bytes: Uint8Array): Promise<Uint8Array> {
  return postBinary("/decompress", bytes);
}
