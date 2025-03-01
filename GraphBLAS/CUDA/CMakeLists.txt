cmake_minimum_required(VERSION 3.20.1)

# CMake build for generating googletest c++ files that can be compiled and executed in parallel.
# Build can be customized to speed up development by allowing the targeting of specific
# specific parameters. The output of this build is an executable that can be used to
# run the gtests.

project(GRAPHBLAS_CUDA VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_CUDA_FLAGS "-cudart static -lineinfo -G")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -fPIC ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGBNCPUFEAT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGBNCPUFEAT")
set(CMAKE_C_STANDARD 11)

message(STATUS "${CMAKE_CXX_FLAGS}")

file(GLOB GRAPHBLAS_CUDA_SOURCES "*.cu" "*.c")

add_library(graphblascuda SHARED
            ${GRAPHBLAS_CUDA_SOURCES}
)

set(RMM_WRAP_INCLUDES "../rmm_wrap")

message(STATUS "RMM_WRAP_INCLUDES: ${RMM_WRAP_INCLUDES}")
set(GRAPHBLAS_CUDA_INCLUDES
        ${RMM_WRAP_INCLUDES}
        ../Source
        ../Source/Template
        ../Include
        ../CUDA)

message(STATUS "${GRAPHBLAS_CUDA_INCLUDES}")

target_include_directories(graphblascuda PUBLIC  ${CUDA_INCLUDE_DIRECTORIES} ${GRAPHBLAS_CUDA_INCLUDES})
set_target_properties(graphblascuda PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(graphblascuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(graphblascuda PROPERTIES CUDA_ARCHITECTURES "75")

target_link_libraries(graphblascuda nvrtc cuda)

# 1. Execute enumify/stringify/jitify logic to compile ptx kernels and compile/link w/ relevant *.cu files.

# TODO: Need to do this piece in cmake

# 2. Generate test .cu files named "{semiring_operation}_test_instances.hpp"
set(CUDA_TEST_SUITES
    AxB_dot3
#    reduce_to_scalar
)

#
set(CUDA_TEST_MONOIDS PLUS) #MIN MAX TIMES ANY)
set(CUDA_TEST_BINOPS TIMES) #PLUS MIN MAX DIV MINUS RDIV RMINUS FIRST SECOND PAIR)
set(CUDA_TEST_SEMIRINGS PLUS_TIMES) # MIN_PLUS MAX_PLUS)
set(CUDA_TEST_DATATYPES int32_t ) #int64_t uint32_t uint64_t float double)
set(CUDA_TEST_KERNELS vsvs) # mp vsvs dndn spdn vssp)


# TODO: Update testGen.py to accept the above CUDA_TEST_* params as arguments

# Note: I don't believe there's a way to do this particular piece in parallel but
# once all the files are written, we should be able to compile them in parallel

# Separate individual kernels from larger "overview" test (e.g. 2-level testing structure)
# We want to test all the *_cuda versions

set(CUDA_TEST_CPP_FILES "")
foreach(var ${CUDA_TEST_SUITES})
    foreach(semiring ${CUDA_TEST_SEMIRINGS})
        foreach(kernel ${CUDA_TEST_KERNELS})

            # TODO: Have Python script also build separate cudaTest.cpp (named something
            # like AxB_dot3_cuda_tests.cpp) for each suite. This way we should be able to
            # easily ignore them from the build
            add_custom_command(
                    OUTPUT
                    ${CMAKE_CURRENT_BINARY_DIR}/${var}_${semiring}_${kernel}_test_instances.hpp
                    ${CMAKE_CURRENT_BINARY_DIR}/${var}_${semiring}_${kernel}_cuda_tests.cu
                    DEPENDS
                    jitFactory.hpp
                    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test/testGen_cmake.py "\"${CMAKE_CURRENT_SOURCE_DIR}\"" "\"${var}\"" "\"${CUDA_TEST_MONOIDS}\""
                        "\"${CUDA_TEST_BINOPS}\"" "\"${semiring}\"" "\"${CUDA_TEST_DATATYPES}\""
                        "\"${kernel}\""
            )

            # Construct final list of files to compile (in parallel)
            list(APPEND CUDA_TEST_CPP_FILES ${CMAKE_CURRENT_BINARY_DIR}/${var}_${semiring}_${kernel}_cuda_tests.cu)
        endforeach()
    endforeach()
endforeach()

include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


#file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external_includes)
#execute_process(
#        COMMAND git clone "https://github.com/google/googletest.git" googletest
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external_includes)
#
#include_directories(${CMAKE_CURRENT_BINARY_DIR}/external_includes/googletest/googletest/include)

#add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/external_includes/googletest/googletest/)

# 3. Compile/link individual {test_suite_name}_cuda_tests.cpp files into a gtest executable
set(GRAPHBLAS_CUDA_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/test)

message(STATUS "${CUDA_TEST_CPP_FILES}")

add_executable(graphblascuda_test ${CUDA_TEST_CPP_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/test/run_tests.cpp)

set_target_properties(graphblascuda_test PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(graphblascuda_test PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(graphblascuda_test PROPERTIES CUDA_ARCHITECTURES "70")

include(GoogleTest)

add_dependencies(graphblascuda_test graphblas)
add_dependencies(graphblascuda_test graphblascuda)
add_dependencies(graphblascuda_test gtest_main)
add_dependencies(graphblascuda_test rmm_wrap)

target_link_libraries(graphblascuda_test
        graphblas
        graphblascuda
        gtest_main
        nvrtc
        cuda
        ${ADDITIONAL_DEPS})

target_include_directories(graphblascuda_test PUBLIC
        ${ADDITIONAL_INCLUDES}
        ${CUDA_INCLUDE_DIRECTORIES}
        ${GRAPHBLAS_CUDA_INCLUDES})
