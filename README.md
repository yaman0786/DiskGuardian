# Disk‑Guardian

macOS + Windows + iOS için **disk kurtarma + AI‑asistan** prototipi.


## Build (macOS / Linux)

```bash
mkdir -p build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

## Benchmark

Bu repo örnek bir benchmark içerir: `src/dg_benchmark` — Adler-32 tabanlı, hem seri hem paralel (thread bölmeli) çalışma modları içerir.

# Disk-Guardian

Basit bir prototip: disk kurtarma + AI-asistan örneği ve küçük bir benchmark
örneği içerir (`src/dg_benchmark`). Benchmark deterministik paralel Adler-32,
dosya modu (mmap fallback) ve LTO/PGO/SIMD için yapı iskeleti sağlar.

Öne çıkan teknolojiler: C++17, CMake, Ninja, SwiftUI (macOS/iOS), WPF (Windows),
Python (doğrulama amaçlı).

## Hızlı başlat (macOS)

Gerekli araçları Homebrew ile yükleyin:

```bash
# Homebrew yoksa önce https://brew.sh/ adresinden kurun
brew update
brew install cmake ninja python
```

Derleme ve çalıştırma (Release):

```bash
mkdir -p build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
./src/dg_benchmark --bytes 50000000 --threads 4 --runs 3
```

Dosya modu (dizin içindeki dosyaların sıralı hale getirilmiş Adler-32'si):

```bash
./src/dg_benchmark --mode files --path /path/to/dir --threads 4 --runs 2
```
# Disk-Guardian

Basit bir prototip: disk kurtarma + AI-asistan örneği ve küçük bir benchmark
örneği içerir (`src/dg_benchmark`). Benchmark deterministik paralel Adler-32,
dosya modu (mmap fallback) ve LTO/PGO/SIMD için yapı iskeleti sağlar.

Öne çıkan teknolojiler: C++17, CMake, Ninja, SwiftUI (macOS/iOS), WPF (Windows),
Python (doğrulama amaçlı).

## Hızlı başlat (macOS)

Gerekli araçları Homebrew ile yükleyin:

```bash
# Homebrew yoksa önce https://brew.sh/ adresinden kurun
brew update
brew install cmake ninja python
```

Derleme ve çalıştırma (Release):

```bash
mkdir -p build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
./src/dg_benchmark --bytes 50000000 --threads 4 --runs 3
```

Dosya modu (dizin içindeki dosyaların sıralı hale getirilmiş Adler-32'si):

```bash
./src/dg_benchmark --mode files --path /path/to/dir --threads 4 --runs 2
```

## PGO (isteğe bağlı)

Clang ile PGO kullanmak için `llvm-profdata` gereklidir. macOS'ta Homebrew ile
şu şekilde yükleyebilirsiniz:

```bash
brew install llvm
# Kurulum sonrası PATH talimatlarını uygulayın, ör.:
# export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
```

PGO akışı için yardımcı betik:

```bash
bash scripts/run_pgo.sh
```

Notlar:

- `--simd` CLI bayrağı SIMD için yapı/çalıştırma iskeleti sağlar; mimariye özel
  intrinsics bu dalda henüz uygulanmadı.
- `scripts/run_pgo.sh`, profilleri otomatik birleştirmeyen sistemlerde `use`
  aşamasını atlar; `llvm-profdata` kurulduktan sonra tekrar çalıştırın.

## Doğrulama (örnek)

Yerel doğrulama örneği:

```bash
# temiz derleme ve build
rm -rf build && mkdir build && cd build && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && ninja

# bellek benchmark
./src/dg_benchmark --bytes 10000000 --threads 4 --runs 3

# dosya benchmark (repo içindeki test_data/ klasörü kullanılır)
./src/dg_benchmark --mode files --path ../test_data --threads 4 --runs 2

# files-mode sonucunu Python zlib.adler32 ile doğrulama
python3 - <<'PY'
import zlib, os
root='test_data'
files=sorted(os.listdir(root))
acc=1
for fn in files:
    with open(os.path.join(root,fn),'rb') as f:
        acc = zlib.adler32(f.read(), acc)
print(acc)
PY
```

## Teslim edilenler

- `src/dg_benchmark` — derlenmiş benchmark ikili dosyası
- `scripts/run_all.sh` — benchmarkları çalıştırıp doğrulayan betik
- `scripts/run_pgo.sh` — PGO generate->run->merge->use iskeleti
- `test_data/` — files-mode smoke test için küçük dosyalar

---

Eğer isterseniz SIMD intrinsics veya PGO kullanımını otomatikleştirip çalıştırabilirim; hangi adımı istediğinizi söyleyin.
- `test_data/` — files-mode smoke test için küçük dosyalar

