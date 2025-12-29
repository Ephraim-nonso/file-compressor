"use client";

import { useMemo, useState } from "react";
import { apiCompress, apiDecompress } from "../lib/api";
import { formatBytes, toHexPreview } from "../lib/format";

type Mode = "compress" | "decompress";

function downloadBytes(bytes: Uint8Array, filename: string) {
  // Create a copy so we have a plain ArrayBuffer (keeps TS + BlobPart typing happy).
  const ab = bytes.slice().buffer as ArrayBuffer;
  const blob = new Blob([ab], { type: "application/octet-stream" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  a.remove();
  URL.revokeObjectURL(url);
}

export function FileCompressionTool() {
  const [mode, setMode] = useState<Mode>("compress");
  const [file, setFile] = useState<File | null>(null);
  const [inputBytes, setInputBytes] = useState<Uint8Array | null>(null);
  const [outputBytes, setOutputBytes] = useState<Uint8Array | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [busy, setBusy] = useState(false);

  const stats = useMemo(() => {
    const inLen = inputBytes?.length ?? 0;
    const outLen = outputBytes?.length ?? 0;
    const ratio = inLen > 0 ? outLen / inLen : 0;
    return { inLen, outLen, ratio };
  }, [inputBytes, outputBytes]);

  async function onPickFile(f: File | null) {
    setError(null);
    setOutputBytes(null);
    setFile(f);
    setInputBytes(null);

    if (!f) return;
    const buf = await f.arrayBuffer();
    setInputBytes(new Uint8Array(buf));
  }

  async function run() {
    setError(null);
    setOutputBytes(null);

    if (!inputBytes) {
      setError("Please choose a file first.");
      return;
    }

    setBusy(true);
    try {
      const out =
        mode === "compress"
          ? await apiCompress(inputBytes)
          : await apiDecompress(inputBytes);
      setOutputBytes(out);
    } catch (e) {
      setError(e instanceof Error ? e.message : "Unexpected error");
    } finally {
      setBusy(false);
    }
  }

  const suggestedName = useMemo(() => {
    const base =
      file?.name ?? (mode === "compress" ? "input.bin" : "compressed.bin");
    if (mode === "compress") return `${base}.adaptive`;
    // best-effort: strip our suffix if present
    return base.endsWith(".adaptive")
      ? base.replace(/\.adaptive$/, "")
      : `decompressed_${base}`;
  }, [file, mode]);

  return (
    <section className="mx-auto w-full max-w-2xl rounded-2xl border border-zinc-200 bg-white p-6 shadow-sm">
      <div className="flex items-start justify-between gap-4">
        <div>
          <h1 className="text-xl font-semibold tracking-tight text-zinc-900">
            File {mode === "compress" ? "Compression" : "Decompression"}
          </h1>
        </div>
        <div className="inline-flex overflow-hidden rounded-xl border border-zinc-200">
          <button
            type="button"
            className={[
              "cursor-pointer px-3 py-2 text-sm",
              mode === "compress"
                ? "bg-zinc-900 text-white"
                : "bg-white text-zinc-800",
            ].join(" ")}
            onClick={() => {
              setMode("compress");
              setOutputBytes(null);
              setError(null);
            }}
          >
            Compress
          </button>
          <button
            type="button"
            className={[
              "cursor-pointer px-3 py-2 text-sm",
              mode === "decompress"
                ? "bg-zinc-900 text-white"
                : "bg-white text-zinc-800",
            ].join(" ")}
            onClick={() => {
              setMode("decompress");
              setOutputBytes(null);
              setError(null);
            }}
          >
            Decompress
          </button>
        </div>
      </div>

      <div className="mt-6 grid gap-4">
        <div className="rounded-xl border border-zinc-200 p-4">
          <label className="block text-sm font-medium text-zinc-900">
            Choose file
          </label>
          <input
            className="mt-2 block w-full text-sm text-zinc-700 file:mr-4 file:rounded-lg file:border-0 file:bg-zinc-100 file:px-4 file:py-2 file:text-sm file:font-medium file:text-zinc-900 hover:file:bg-zinc-200"
            type="file"
            onChange={(e) => void onPickFile(e.target.files?.item(0) ?? null)}
          />
          <div className="mt-3 text-sm text-zinc-600">
            <div>
              <span className="font-medium text-zinc-800">Selected:</span>{" "}
              {file ? file.name : "—"}
            </div>
            <div>
              <span className="font-medium text-zinc-800">Size:</span>{" "}
              {inputBytes ? formatBytes(inputBytes.length) : "—"}
            </div>
          </div>
        </div>

        <div className="flex flex-wrap items-center gap-3">
          <button
            type="button"
            onClick={() => void run()}
            disabled={busy || !inputBytes}
            className="cursor-pointer rounded-xl bg-zinc-900 px-4 py-2 text-sm font-medium text-white disabled:cursor-not-allowed disabled:opacity-50"
          >
            {busy
              ? "Working…"
              : mode === "compress"
              ? "Compress"
              : "Decompress"}
          </button>

          {outputBytes && (
            <button
              type="button"
              onClick={() => downloadBytes(outputBytes, suggestedName)}
              className="cursor-pointer rounded-xl border border-zinc-200 bg-white px-4 py-2 text-sm font-medium text-zinc-900 hover:bg-zinc-50"
            >
              Download result
            </button>
          )}

          <div className="ml-auto text-sm text-zinc-600">
            <span className="font-medium text-zinc-800">Output:</span>{" "}
            {outputBytes ? formatBytes(outputBytes.length) : "—"}
            {outputBytes && inputBytes && (
              <>
                {" "}
                <span className="text-zinc-400">/</span>{" "}
                <span className="font-medium text-zinc-800">
                  {(stats.ratio * 100).toFixed(1)}%
                </span>
              </>
            )}
          </div>
        </div>

        {error && (
          <div className="rounded-xl border border-red-200 bg-red-50 p-3 text-sm text-red-800">
            {error}
          </div>
        )}

        {outputBytes && (
          <div className="rounded-xl border border-zinc-200 bg-zinc-50 p-4 overflow-hidden">
            <div className="text-sm font-medium text-zinc-900">
              Preview (hex)
            </div>
            <pre className="mt-2 max-w-full overflow-x-auto rounded-lg bg-white p-3 text-xs text-zinc-800 whitespace-pre-wrap break-all">
              {toHexPreview(outputBytes)}
            </pre>
          </div>
        )}
      </div>
    </section>
  );
}
