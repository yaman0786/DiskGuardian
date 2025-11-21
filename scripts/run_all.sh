#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
BIN="$BUILD_DIR/src/dg_benchmark"

echo "[run_all] Root: $ROOT_DIR"

# Build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja

# Run memory benchmark
echo "\n[run_all] Running memory benchmark (50MB, threads=4, runs=3)"
$BIN 52428800 4 3 | tee mem_benchmark.txt

# Run files benchmark on test_data
TEST_DATA="$ROOT_DIR/test_data"
if [ -d "$TEST_DATA" ]; then
  echo "\n[run_all] Running files benchmark on $TEST_DATA (threads=4, runs=3)"
  $BIN files "$TEST_DATA" 4 3 | tee files_benchmark.txt
  # Validate with python zlib
  echo "\n[run_all] Validating Adler-32 with python zlib..."
  python3 - <<PY
import zlib,os
p='$TEST_DATA'
files=sorted([os.path.join(p,f) for f in os.listdir(p)])
val=1
for f in files:
    with open(f,'rb') as fh:
        while True:
            b=fh.read(65536)
            if not b: break
            val=zlib.adler32(b,val)
print('python adler', val & 0xffffffff)
PY
else
  echo "[run_all] No test_data directory found, skipping files benchmark"
fi

# Summary
echo "\n[run_all] Done. Results saved to mem_benchmark.txt and files_benchmark.txt (if run)."
