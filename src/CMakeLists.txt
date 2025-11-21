add_executable(dg_benchmark benchmark.cpp)
target_compile_features(dg_benchmark PRIVATE cxx_std_17)

# Optimize and enable warnings
option(ENABLE_LTO "Enable LTO when supported" ON)
option(ENABLE_PGO "Enable PGO (profile guided) scaffolding" OFF)

if(MSVC)
  target_compile_options(dg_benchmark PRIVATE /O2 /W4)
else()
  target_compile_options(dg_benchmark PRIVATE -O3 -march=native -Wall -Wextra -pthread)
  if(ENABLE_LTO)
    target_compile_options(dg_benchmark PRIVATE -flto)
    target_link_options(dg_benchmark PRIVATE -flto)
  endif()
  if(ENABLE_PGO)
    # scaffolding: user can toggle to generate profiles
    target_compile_options(dg_benchmark PRIVATE -fprofile-generate)
    target_link_libraries(dg_benchmark PRIVATE -fprofile-generate)
  endif()
endif()

target_link_libraries(dg_benchmark PRIVATE pthread)
