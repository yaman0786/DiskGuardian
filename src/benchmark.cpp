#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <thread>
#include <vector>
#include <filesystem>

using highres_clock = std::chrono::high_resolution_clock;

static const uint32_t ADLER_MOD = 65521u;

// Adler-32: per-chunk compute starting with A=1,B=0
struct AdlerPart { uint32_t A; uint32_t B; size_t len; };

AdlerPart adler32_chunk(const uint8_t* data, size_t len) {
    uint32_t A = 1;
    uint32_t B = 0;
    for (size_t i = 0; i < len; ++i) {
        A += data[i];
        if (A >= ADLER_MOD) A -= ADLER_MOD;
        B += A;
        if (B >= ADLER_MOD) B -= ADLER_MOD;
    }
    return {A, B, len};
}

// Combine two AdlerPart values where p1 is the prefix and p2 follows it.
AdlerPart adler32_combine(const AdlerPart& p1, const AdlerPart& p2) {
    // Given p1 computed from initial A=1,B=0 over len1 bytes,
    // and p2 similarly for len2, the combined A = (p1.A + p2.A - 1) % M
    uint32_t A = p1.A + p2.A;
    if (A > 1) A = (A - 1) % ADLER_MOD; else A = (ADLER_MOD + A - 1) % ADLER_MOD;
    // Combined B = p1.B + p2.B + len2*(p1.A - 1)
    uint64_t tmp = p1.B + p2.B + (uint64_t)p2.len * (uint64_t)(p1.A - 1);
    uint32_t B = static_cast<uint32_t>(tmp % ADLER_MOD);
    return {A, B, p1.len + p2.len};
}

uint32_t adler32_final(const AdlerPart& p) {
    return (p.B << 16) | p.A;
}

std::vector<uint8_t> generate_data(size_t bytes) {
    std::vector<uint8_t> data(bytes);
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < bytes; ++i) data[i] = static_cast<uint8_t>(dist(rng));
    return data;
}

// Baseline: single-threaded checksum-like work
// Baseline: Adler-32 single-threaded
uint32_t baseline_work(const std::vector<uint8_t>& data) {
    AdlerPart p = adler32_chunk(data.data(), data.size());
    return adler32_final(p);
}

// Optimized: divide data across threads, compute AdlerPart per-chunk and combine in order
uint32_t optimized_work(const std::vector<uint8_t>& data, unsigned threads) {
    if (threads <= 1) return baseline_work(data);
    std::vector<std::thread> workers;
    std::vector<AdlerPart> parts(threads);
    size_t n = data.size();
    size_t chunk = n / threads;

    for (unsigned t = 0; t < threads; ++t) {
        size_t start = t * chunk;
        size_t end = (t + 1 == threads) ? n : (start + chunk);
        workers.emplace_back([&data, start, end, &parts, t]() {
            parts[t] = adler32_chunk(data.data() + start, end - start);
        });
    }
    for (auto &th : workers) th.join();
    // combine sequentially to match serial Adler-32
    AdlerPart combined = parts[0];
    for (unsigned t = 1; t < threads; ++t) combined = adler32_combine(combined, parts[t]);
    return adler32_final(combined);
}

// File-based mode: process files under a directory (recursively) and compute checksum
// Returns adler32 of concatenated file bytes
uint32_t files_work(const std::string& root, unsigned threads) {
    namespace fs = std::filesystem;
    std::vector<fs::path> files;
    if (fs::is_regular_file(root)) {
        files.push_back(fs::path(root));
    } else if (fs::is_directory(root)) {
        for (auto &p : fs::recursive_directory_iterator(root)) {
            if (p.is_regular_file()) files.push_back(p.path());
        }
    } else {
        std::cerr << "Path is not a file or directory: " << root << "\n";
        return 0;
    }
    if (files.empty()) return 0;
    // deterministic order
    std::sort(files.begin(), files.end());

    // compute AdlerPart per file in parallel (partition files among threads)
    size_t nf = files.size();
    std::vector<AdlerPart> parts(nf);
    unsigned workers = threads == 0 ? 1 : threads;
    size_t per = (nf + workers - 1) / workers;
    std::vector<std::thread> ths;
    for (unsigned t = 0; t < workers; ++t) {
        size_t start = t * per;
        size_t end = std::min(start + per, nf);
        if (start >= end) break;
        ths.emplace_back([start, end, &files, &parts]() {
            for (size_t i = start; i < end; ++i) {
                // compute AdlerPart for files[i]
                FILE* f = fopen(files[i].c_str(), "rb");
                if (!f) { std::cerr << "Could not open file: " << files[i] << "\n"; parts[i] = {1,0,0}; continue; }
                uint32_t A = 1, B = 0;
                constexpr size_t CHUNK = 1 << 20;
                std::vector<uint8_t> buf(CHUNK);
                size_t r;
                size_t total = 0;
                while ((r = fread(buf.data(), 1, CHUNK, f)) > 0) {
                    for (size_t k = 0; k < r; ++k) {
                        A += buf[k]; if (A >= ADLER_MOD) A -= ADLER_MOD;
                        B += A; if (B >= ADLER_MOD) B -= ADLER_MOD;
                    }
                    total += r;
                }
                fclose(f);
                parts[i] = {A, B, total};
            }
        });
    }
    for (auto &t : ths) t.join();

    // combine in sorted order
    AdlerPart combined = parts[0];
    for (size_t i = 1; i < nf; ++i) combined = adler32_combine(combined, parts[i]);
    return adler32_final(combined);
}

// Single-threaded files baseline: read files in sorted order and compute Adler-32
uint32_t files_baseline(const std::string& root) {
    namespace fs = std::filesystem;
    std::vector<fs::path> files;
    if (fs::is_regular_file(root)) {
        files.push_back(fs::path(root));
    } else if (fs::is_directory(root)) {
        for (auto &p : fs::recursive_directory_iterator(root)) {
            if (p.is_regular_file()) files.push_back(p.path());
        }
    } else {
        std::cerr << "Path is not a file or directory: " << root << "\n";
        return 0;
    }
    if (files.empty()) return 0;
    std::sort(files.begin(), files.end());
    uint32_t A = 1, B = 0;
    for (auto &fp : files) {
        FILE* f = fopen(fp.c_str(), "rb");
        if (!f) { std::cerr << "Could not open file: " << fp << "\n"; continue; }
        constexpr size_t CHUNK = 1 << 20;
        std::vector<uint8_t> buf(CHUNK);
        size_t r;
        while ((r = fread(buf.data(), 1, CHUNK, f)) > 0) {
            for (size_t k = 0; k < r; ++k) {
                A += buf[k]; if (A >= ADLER_MOD) A -= ADLER_MOD;
                B += A; if (B >= ADLER_MOD) B -= ADLER_MOD;
            }
        }
        fclose(f);
    }
    return (B << 16) | A;
}

int main(int argc, char** argv) {
    size_t bytes = 50 * 1024 * 1024; // default 50 MB
    unsigned threads = std::thread::hardware_concurrency();
    int runs = 3;

    bool mode_files = false;
    std::string files_root;

    if (argc > 1) {
        std::string a1 = argv[1];
        if (a1 == "files") {
            mode_files = true;
            if (argc > 2) files_root = argv[2]; else { std::cerr << "Usage: dg_benchmark files <path> [threads] [runs]\n"; return 1; }
            if (argc > 3) threads = static_cast<unsigned>(std::stoul(argv[3]));
            if (argc > 4) runs = std::stoi(argv[4]);
        } else {
            bytes = static_cast<size_t>(std::stoull(a1));
            if (argc > 2) threads = static_cast<unsigned>(std::stoul(argv[2]));
            if (argc > 3) runs = std::stoi(argv[3]);
        }
    }

    if (mode_files) {
        std::cout << "Mode: files\n";
        std::cout << "Root: " << files_root << "\n";
        std::cout << "Threads: " << threads << "\n";
        std::cout << "Runs: " << runs << "\n";

        // warmup
        uint32_t ref = files_baseline(files_root);
        (void)ref;

        double total_baseline = 0.0, total_opt = 0.0;
        for (int i = 0; i < runs; ++i) {
            auto t0 = highres_clock::now();
            uint32_t r = files_baseline(files_root);
            auto t1 = highres_clock::now();
            double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            std::cout << "Baseline run " << i+1 << ": " << ms << " ms; result=" << r << "\n";
            total_baseline += ms;
        }
        for (int i = 0; i < runs; ++i) {
            auto t0 = highres_clock::now();
            uint32_t r = files_work(files_root, threads);
            auto t1 = highres_clock::now();
            double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            std::cout << "Optimized run " << i+1 << ": " << ms << " ms; result=" << r << "\n";
            total_opt += ms;
        }
        std::cout << "Average baseline: " << (total_baseline / runs) << " ms\n";
        std::cout << "Average optimized: " << (total_opt / runs) << " ms\n";
        std::cout << "Speedup: " << (total_baseline / total_opt) << "x\n";

        return 0;
    }

    std::cout << "Data size: " << bytes / (1024.0 * 1024.0) << " MB\n";
    std::cout << "Threads: " << threads << "\n";
    std::cout << "Runs: " << runs << "\n";

    auto data = generate_data(bytes);

    // Warmup
    uint64_t ref = baseline_work(data);
    (void)ref;

    // Baseline
    double total_baseline = 0.0;
    for (int i = 0; i < runs; ++i) {
        auto t0 = highres_clock::now();
        uint64_t r = baseline_work(data);
        auto t1 = highres_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        std::cout << "Baseline run " << i+1 << ": " << ms << " ms; result=" << r << "\n";
        total_baseline += ms;
    }

    // Optimized
    double total_opt = 0.0;
    for (int i = 0; i < runs; ++i) {
        auto t0 = highres_clock::now();
        uint64_t r = optimized_work(data, threads);
        auto t1 = highres_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        std::cout << "Optimized run " << i+1 << ": " << ms << " ms; result=" << r << "\n";
        total_opt += ms;
    }

    std::cout << "Average baseline: " << (total_baseline / runs) << " ms\n";
    std::cout << "Average optimized: " << (total_opt / runs) << " ms\n";
    std::cout << "Speedup: " << (total_baseline / total_opt) << "x\n";
    
    return 0;
}
