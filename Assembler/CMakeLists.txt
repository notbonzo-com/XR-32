project(Assembler LANGUAGES CXX)

file(GLOB_RECURSE ASSEMBLER_SOURCES Source/*.cpp)
file(GLOB_RECURSE ASSEMBLER_HEADERS Include/*.hpp)

add_executable(xr32as ${ASSEMBLER_SOURCES} ${ASSEMBLER_HEADERS})

target_include_directories(xr32as PRIVATE Include)

target_compile_options(xr32as PRIVATE -Wall -Wextra -Werror)
