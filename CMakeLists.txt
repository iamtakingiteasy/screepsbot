include(/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake)

cmake_minimum_required(VERSION 3.11)
project(screepsbot C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")
set(CMAKE_SHARED_LIBRARY_SUFFIX_C ".wasm")

set(PAGE_SIZE 65536)
set(TOTAL_PAGES 32) # 2048 kb
set(STACK_PAGES 8)

math(EXPR TOTAL_MEMORY "${PAGE_SIZE} * ${TOTAL_PAGES}")
math(EXPR TOTAL_STACK "${PAGE_SIZE} * ${STACK_PAGES}")
math(EXPR STACK_START "${PAGE_SIZE} * (${TOTAL_PAGES} - ${STACK_PAGES})")

add_definitions(-DTOTAL_MEMORY=${TOTAL_MEMORY})
add_definitions(-DTOTAL_STACK=${TOTAL_STACK})
add_definitions(-DSTACK_START=${STACK_START})
add_executable(screepsbot main.c)
set_target_properties(screepsbot PROPERTIES LINK_FLAGS " -s ONLY_MY_CODE=1 -s WASM=1 -s TOTAL_MEMORY=${TOTAL_MEMORY} -s TOTAL_STACK=${TOTAL_STACK} -s GLOBAL_BASE=0 -s STACK_START=${STACK_START}")
