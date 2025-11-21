add_executable(dg_benchmark benchmark.cpp)
target_compile_features(dg_benchmark PRIVATE cxx_std_17)

# Optimize and enable warnings
option(ENABLE_LTO "Enable LTO when supported" ON)
option(ENABLE_SIMD "Enable SIMD optimizations (platform dependent)" OFF)
set(PGO_MODE "none" CACHE STRING "PGO mode: none|generate|use")

if(MSVC)
  target_compile_options(dg_benchmark PRIVATE /O2 /W4)
else()
  target_compile_options(dg_benchmark PRIVATE -O3 -march=native -Wall -Wextra -pthread)
  if(ENABLE_LTO)
    target_compile_options(dg_benchmark PRIVATE -flto)
    target_link_options(dg_benchmark PRIVATE -flto)
  endif()

  if(ENABLE_SIMD)
    # Try enabling common SIMD flags for x86; users on other archs can adjust
    target_compile_options(dg_benchmark PRIVATE -mavx2 -mbmi2)
  endif()

  if(PGO_MODE STREQUAL "generate")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      target_compile_options(dg_benchmark PRIVATE -fprofile-instr-generate)
      target_link_libraries(dg_benchmark PRIVATE -fprofile-instr-generate)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(dg_benchmark PRIVATE -fprofile-generate)
      target_link_libraries(dg_benchmark PRIVATE -fprofile-generate)
    endif()
  elseif(PGO_MODE STREQUAL "use")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      # expects a default.profdata produced by llvm-profdata
      target_compile_options(dg_benchmark PRIVATE -fprofile-instr-use=default.profdata)
      target_link_libraries(dg_benchmark PRIVATE -fprofile-instr-use=default.profdata)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(dg_benchmark PRIVATE -fprofile-use -fprofile-correction)
      target_link_libraries(dg_benchmark PRIVATE -fprofile-use)
    endif()
  endif()
endif()

target_link_libraries(dg_benchmark PRIVATE pthread)
