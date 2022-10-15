include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
add_library(strings_lib "${CMAKE_CURRENT_SOURCE_DIR}/../src/strings.c")


enable_testing()

include_directories(${gtest_SOURCE_DIR}/include)
add_executable(strings_test strings_test.cpp)

target_link_libraries(strings_test PUBLIC gtest gtest_main strings_lib)