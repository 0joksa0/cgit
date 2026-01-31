cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
project(cGit C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE})

enable_testing()
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(cGit src/main.c)
target_include_directories(cGit PRIVATE ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/commands
    ${CMAKE_SOURCE_DIR}/include/core
${CMAKE_SOURCE_DIR}/include/util
)
target_link_libraries(cGit PRIVATE ZLIB::ZLIB)
target_link_libraries(cGit PRIVATE OpenSSL::Crypto)

file(GLOB EXTRA_SOURCES "src/*.c" "src/*/*.c")
list(REMOVE_ITEM EXTRA_SOURCES "${CMAKE_SOURCE_DIR}/src/main.c")
target_sources(cGit PRIVATE ${EXTRA_SOURCES})


option(BUILD_TESTS "Build Criterion tests" OFF)
if(BUILD_TESTS)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CRITERION REQUIRED criterion)

    add_executable(tests
        tests/test_main.c
        ${EXTRA_SOURCES})

    target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/commands
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/util
    )
    target_link_libraries(tests PRIVATE ZLIB::ZLIB)
    target_link_libraries(tests PRIVATE OpenSSL::Crypto)
    
    target_include_directories(tests PRIVATE ${CRITERION_INCLUDE_DIRS})
    target_link_libraries(tests PRIVATE ${CRITERION_LIBRARIES})

    add_test(NAME unit COMMAND tests)
endif()


# ───── coverage (only for GCC/Clang) ─────
option(COVERAGE "Enable coverage flags" OFF)
if(COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(cGit PRIVATE
        -O0 -g --coverage)
    target_link_options(cGit PRIVATE --coverage)
    if(TARGET tests)
        target_compile_options(tests PRIVATE -O0 -g --coverage)
        target_link_options(tests PRIVATE --coverage)
    endif()
endif()


install(TARGETS cGit 
        RUNTIME DESTINATION bin)

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")
