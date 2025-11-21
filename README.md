# Disk‑Guardian

macOS + Windows + iOS için **disk kurtarma + AI‑asistan** prototipi.

- **Core**: C++17, CMake, libcurl, nlohmann‑json.
- **macOS UI**: SwiftUI (tek pencere, “Kurtarma” & “AI‑Chat” sekmeleri).
- **Windows UI**: WPF (C#) + C++ core.
- **iOS UI**: SwiftUI + OpenAI Chat (sınırlı disk erişimi).

## Build (macOS / Linux)

```bash
mkdir -p build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

## Benchmark

Bu repo örnek bir benchmark içerir: `src/dg_benchmark` — Adler-32 tabanlı, hem seri hem paralel (thread bölmeli) çalışma modları içerir.

Derleme ve çalıştırma:

```bash
mkdir -p build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
./src/dg_benchmark
```

CI: GitHub Actions workflow eklendi (`.github/workflows/benchmark.yml`) — push veya workflow_dispatch ile çalıştırılabilir ve `benchmark-output.txt` artifact'i üretir.

Not: Paralel sonuçlar Adler-32 için seri karşılığıyla eşleşecek şekilde birleştiriliyor. Bu örnek gerçek disk I/O optimizasyonu için temel bir iskelet sağlar.

## CLI (kullanım)

- Bellek modu (varsayılan): rastgele veri üzerinde çalışır
  - `./src/dg_benchmark [bytes] [threads] [runs]`
  # DiskGuardian

  Small, self-contained benchmark demonstrating a deterministic, parallel
  Adler-32 implementation with file-mode, mmap fallback, and build scaffolding
  for LTO/PGO/SIMD. Includes helper scripts for local benchmarking and an
  optional PGO workflow.

  ## Quick start (macOS)

  Install build tools (Homebrew):

  ```bash
  # install Homebrew first if you don't have it: https://brew.sh/
  brew update
  brew install cmake ninja python
  ```

  Build and run (Release):

  ```bash
  mkdir -p build && cd build
  cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
  ninja
  ./src/dg_benchmark --bytes 50000000 --threads 4 --runs 3
  ```

  Files mode (deterministic Adler-32 over sorted files in a directory):

  ```bash
  ./src/dg_benchmark --mode files --path /path/to/dir --threads 4 --runs 2
  ```

  ## PGO (optional)

  To use PGO with Clang you will typically need `llvm-profdata` available. On
  macOS you can get it by installing LLVM via Homebrew:

  ```bash
  brew install llvm
  # make sure llvm is on your PATH; brew will print instructions such as:
  #   export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
  ```

  Then run the helper script which attempts a generate->run->merge->use flow:

  ```bash
  bash scripts/run_pgo.sh
  ```

  ## Notes

  - `--simd` CLI flag is a compile/run-time scaffolding option. SIMD intrinsics
    are not fully implemented in this branch; enabling the flag may be a future
    optimization path.
  - The `scripts/run_pgo.sh` script will gracefully skip the PGO `use` stage if
    `default.profdata` is not available; this occurs on toolchains that don't
    auto-merge profile data. In that case install `llvm-profdata` and re-run the
    script.

  ## Validation (example run performed locally)

  I ran the following locally on macOS to validate the code in this branch:

  ```bash
  # build
  rm -rf build && mkdir build && cd build && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && ninja

  # memory benchmark
  /Users/ozcanyaman/DiskGuardian-1/build/src/dg_benchmark --bytes 10000000 --threads 4 --runs 3

  # files benchmark (uses test_data/ included in repo)
  /Users/ozcanyaman/DiskGuardian-1/build/src/dg_benchmark --mode files --path /Users/ozcanyaman/DiskGuardian-1/test_data --threads 4 --runs 2

  # validate files-mode result with Python zlib.adler32
  python3 - <<'PY'
  import zlib, os
  root='/Users/ozcanyaman/DiskGuardian-1/test_data'
  files=sorted(os.listdir(root))
  acc=1
  for fn in files:
      with open(os.path.join(root,fn),'rb') as f:
          acc = zlib.adler32(f.read(), acc)
  print(acc)
  PY
  ```

  ## Deliverables

  - `src/dg_benchmark` — benchmark binary produced by CMake/Ninja
  - `scripts/run_all.sh` — builds and runs mem & files benchmarks and validates
    files-mode with Python
  - `scripts/run_pgo.sh` — scaffolding for PGO generate->run->merge->use
  - `test_data/` — small files used by files-mode smoke test

  ## Next steps (optional)

  If you want, I can now:
  - implement SIMD intrinsics for a chosen target (x86_64 AVX2 or ARM64 NEON) and
    run microbenchmarks, or
  - attempt to install LLVM (`llvm-profdata`) on your machine (requires Homebrew)
    and finish the PGO `use` step automatically.

  If you'd like me to proceed with either of those, tell me which one.
## CLI (kullanım)


