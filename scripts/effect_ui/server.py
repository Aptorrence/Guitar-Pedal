"""Local web UI for auditioning effect chains.

Serves index.html and shells out to the native effect_runner build for each request.
Standard library only.

    cmake --preset native && cmake --build build/native --target effect_runner
    python scripts/effect_ui/server.py
"""

import argparse
import json
import subprocess
import tempfile
import webbrowser
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, urlparse

HERE = Path(__file__).resolve().parent
REPO_ROOT = HERE.parents[1]
DEFAULT_RUNNER = REPO_ROOT / "build" / "native" / "app" / "effect_test" / "effect_runner.exe"


class Handler(BaseHTTPRequestHandler):
    runner: Path

    def do_GET(self):
        path = urlparse(self.path).path
        if path == "/":
            self.reply(200, (HERE / "index.html").read_bytes(), "text/html; charset=utf-8")
        elif path == "/effects":
            result = subprocess.run([self.runner, "--list"], capture_output=True)
            self.reply(200 if result.returncode == 0 else 500, result.stdout or result.stderr,
                       "application/json")
        else:
            self.reply(404, b"not found", "text/plain")

    def do_POST(self):
        url = urlparse(self.path)
        if url.path != "/run":
            self.reply(404, b"not found", "text/plain")
            return

        chain = json.loads(parse_qs(url.query).get("chain", ["[]"])[0])
        wav = self.rfile.read(int(self.headers["Content-Length"]))

        args = []
        for effect in chain:
            args.append(effect["name"])
            args.extend(str(float(p)) for p in effect["params"])

        with tempfile.TemporaryDirectory() as tmp:
            in_path, out_path = Path(tmp) / "in.wav", Path(tmp) / "out.wav"
            in_path.write_bytes(wav)
            result = subprocess.run([self.runner, in_path, out_path, *args], capture_output=True,
                                    text=True)
            if result.returncode != 0:
                self.reply(400, (result.stderr or result.stdout).encode(), "text/plain")
                return
            self.reply(200, out_path.read_bytes(), "audio/wav")

    def reply(self, status, body, content_type):
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--runner", type=Path, default=DEFAULT_RUNNER)
    parser.add_argument("--no-browser", action="store_true")
    args = parser.parse_args()

    if not args.runner.exists():
        parser.error(f"effect_runner not found at {args.runner}; build it first (see top of file)")

    Handler.runner = args.runner
    server = ThreadingHTTPServer(("127.0.0.1", args.port), Handler)
    url = f"http://127.0.0.1:{args.port}/"
    print(f"Effect UI at {url}  (Ctrl+C to stop)")
    if not args.no_browser:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
