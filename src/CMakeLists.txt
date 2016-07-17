
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ciostack")

add_executable(coffeeio_test main.c ciostack/ciostack.c)
