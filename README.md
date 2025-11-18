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

