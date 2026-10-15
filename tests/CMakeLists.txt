include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
    test_grafo.cpp
    test_dijkstra.cpp
    test_cola_prioridad.cpp
    test_incidente.cpp
    test_unidad.cpp
    test_estrategias.cpp
)

add_executable(SIDEA_tests ${TEST_SOURCES})

target_link_libraries(SIDEA_tests
    PRIVATE
    GTest::gtest_main
    sidea_core
)

target_include_directories(SIDEA_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(GoogleTest)
gtest_discover_tests(SIDEA_tests)
