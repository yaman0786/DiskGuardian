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
  - örn: `./src/dg_benchmark 52428800 8 3` (50MB, 8 thread, 3 run)

- Dosya/dizin modu: klasördeki dosyaları sıralı şekilde birleştirip checksum alır
  - `./src/dg_benchmark files <path> [threads] [runs]`
  - örn: `./src/dg_benchmark files /path/to/dir 4 3`

## Gelişmiş seçenekler ve optimizasyonlar

- PGO (Profile Guided Optimization): `scripts/run_pgo.sh` ile yerel PGO akışı çalıştırılabilir. Not: macOS/clang ile farklı profilleme bayrakları gerekebilir; script, `llvm-profdata` ile `.profraw` dosyalarını `default.profdata`'ya dönüştürmeye çalışır. Eğer PGO 'use' aşaması atlanırsa, toolchain/llvm kurulumunu kontrol edin.
- SIMD: CMake'de `-DENABLE_SIMD=ON` ile derleme denenebilir (x86 Linux için `-mavx2` gibi bayraklar eklenecektir). ARM/AppleSilicon için farklı bayraklar gerekebilir.
- Mmap: Dosya modu, mümkünse `mmap` kullanır (zero-copy) ve fallback olarak stdio kullanır.

## Scriptler

- `scripts/run_all.sh` — derleme, mem benchmark, files benchmark ve python ile doğrulama.
- `scripts/run_pgo.sh` — PGO generate & (şartlı) use akışı.
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
  - örn: `./src/dg_benchmark 52428800 8 3` (50MB, 8 thread, 3 run)

- Dosya/dizin modu: klasördeki dosyaları sıralı şekilde birleştirip checksum alır
  - `./src/dg_benchmark files <path> [threads] [runs]`
  - örn: `./src/dg_benchmark files /path/to/dir 4 3`

Gelişmiş seçenekler ve optimizasyonlar
- PGO (Profile Guided Optimization): `scripts/run_pgo.sh` ile yerel PGO akışı çalıştırılabilir. Not: macOS/clang ile farklı profilleme bayrakları gerekebilir; script, `llvm-profdata` ile `.profraw` dosyalarını `default.profdata`'ya dönüştürmeye çalışır. Eğer PGO 'use' aşaması atlanırsa, toolchain/llvm kurulumunu kontrol edin.
- SIMD: CMake'de `-DENABLE_SIMD=ON` ile derleme denenebilir (x86 Linux için `-mavx2` gibi bayraklar eklenecektir). ARM/AppleSilicon için farklı bayraklar gerekebilir.
- Mmap: Dosya modu, mümkünse `mmap` kullanır (zero-copy) ve fallback olarak stdio kullanır.

Scriptler:
- `scripts/run_all.sh` — derleme, mem benchmark, files benchmark ve python ile doğrulama.
- `scripts/run_pgo.sh` — PGO generate & (şartlı) use akışı.

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
  - örn: `./src/dg_benchmark 52428800 8 3` (50MB, 8 thread, 3 run)

- Dosya/dizin modu: klasördeki dosyaları sıralı şekilde birleştirip checksum alır
  - `./src/dg_benchmark files <path> [threads] [runs]`
  - örn: `./src/dg_benchmark files /path/to/dir 4 3`
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
# Disk‑Guardian

macOS + Windows + iOS için **disk kurtarma + AI‑asistan** prototipi.

- **Core**: C++17, CMake, libcurl, nlohmann‑json.
- **macOS UI**: SwiftUI (tek pencere, “Kurtarma” & “AI‑Chat” sekmeleri).
- **Windows UI**: WPF (C#) + C++ core.
- **iOS UI**: SwiftUI + OpenAI Chat (sınırlı disk erişimi).

## Build (macOS / Linux)

```bash
mkdir -p build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja

