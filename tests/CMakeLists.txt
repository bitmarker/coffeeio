add_library(imp_cpputest STATIC IMPORTED)

set_property(TARGET imp_cpputest PROPERTY
             IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libCppUTest.a)


include_directories(${PROJECT_SOURCE_DIR}/src/)


add_library(ciostackTest ./ciostack/ciostackTest.cpp "${PROJECT_SOURCE_DIR}/src/ciostack/ciostack.c")


add_executable(RunAllTests RunAllTests.cpp)


target_link_libraries(RunAllTests imp_cpputest ciostackTest)

