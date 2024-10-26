project(Emulator LANGUAGES CXX)

file(GLOB_RECURSE EMULATOR_SOURCES Source/*.cpp)
file(GLOB_RECURSE EMULATOR_HEADERS Include/*.hpp)

add_executable(xr32em ${EMULATOR_SOURCES} ${EMULATOR_HEADERS})

target_include_directories(xr32em PRIVATE Include)

target_compile_options(xr32em PRIVATE -Wall -Wextra -Werror)
