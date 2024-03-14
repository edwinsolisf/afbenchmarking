cmake_minimum_required(VERSION 3.20)

project(aftesting
    LANGUAGES CXX)
    
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(ArrayFire REQUIRED)
find_package(benchmark REQUIRED)

add_executable(afMatmulTest afMatmulTest.cpp)
target_link_libraries(afMatmulTest ArrayFire::af benchmark::benchmark)
target_compile_features(afMatmulTest PUBLIC cxx_std_17)

file(GLOB ASSETS ${CMAKE_SOURCE_DIR}/assets)
foreach(ASSET ${ASSETS})
    file(COPY ${ASSET} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()