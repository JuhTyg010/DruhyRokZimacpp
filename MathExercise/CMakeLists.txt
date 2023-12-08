cmake_minimum_required (VERSION 3.20)

project("Dynamic Polymorphism"
    VERSION 0.1
    DESCRIPTION "Demonstration for dynamic polymorphism."
    LANGUAGES CXX
)

# Set the C++ standard to C++23 (globally)
set(CMAKE_CXX_STANDARD 23) # suggest using C++23
set(CMAKE_CXX_STANDARD_REQUIRED True) # Enforce C++23 (i.e., for GCC, -std=c++23)
set(CMAKE_CXX_EXTENSIONS OFF) # Disable compiler extensions (e.g. GNU extensions: -std=gnu++23)

include_directories(include)

add_executable(expressions1 expressions.cpp)
add_executable(expressions2 expressions.cpp)

target_compile_definitions(expressions1 PRIVATE EXPRESSIONS1) # -DEXPRESSIONS1
target_compile_definitions(expressions2 PRIVATE EXPRESSIONS2) # -DEXPRESSIONS2

if (MSVC) # Microsoft Visual C++
    target_compile_options(expressions1 PRIVATE /W4)
    target_compile_options(expressions2 PRIVATE /W4)
else() # Clang, GCC, Intel, ...
    target_compile_options(expressions1 PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(expressions2 PRIVATE -Wall -Wextra -pedantic)
endif()
