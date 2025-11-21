#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
BIN="$BUILD_DIR/src/dg_benchmark"

# 1) Generate profile build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DPGO_MODE=generate ..
ninja -v

# 2) Run workload to generate profiles (adjust args as needed)
echo "Running workload to generate profiles..."
$BIN 52428800 4 3 > /dev/null

# Merge/convert instrumentation data if using clang/llvm
if command -v llvm-profdata >/dev/null 2>&1; then
	echo "Found llvm-profdata, attempting to merge .profraw files into default.profdata"
	# find profraw files
	shopt -s nullglob || true
	profs=(""*.profraw)
	if [ -e "*.profraw" ] || [ ${#profs[@]} -gt 0 ]; then
		llvm-profdata merge -o default.profdata *.profraw || true
	fi
fi

# 3) Rebuild with profile use if default.profdata exists
if [ -f default.profdata ]; then
	echo "default.profdata found, rebuilding with PGO use"
	cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DPGO_MODE=use ..
	ninja -v
else
	echo "default.profdata not found; skipping PGO 'use' rebuild. Your toolchain may require llvm-profdata or different instrumentation flags."
	exit 0
fi

# 4) Run benchmark and report
echo "Running post-PGO benchmark"
$BIN 52428800 4 3 | tee pgo_benchmark.txt

echo "PGO run completed. Results in pgo_benchmark.txt"
