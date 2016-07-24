
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ciostack")

add_executable(coffeeio_demo main.c ciostack/ciostack.c)
