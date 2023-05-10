set(
  RED4EXT_TESTS_SRC_FILES
    Test.cpp
)

source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  FILES ${RED4EXT_TESTS_SRC_FILES}
)
add_executable(Tests ${RED4EXT_TESTS_SRC_FILES})

target_include_directories(Tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(
  Tests
    PUBLIC
      RED4ext.Lib
    PRIVATE
      Catch2::Catch2WithMain
)

target_compile_definitions(Catch2WithMain PUBLIC DO_NOT_USE_WMAIN=TRUE)

# enable_testing()

# include(CTest)
# include(Catch)
# catch_discover_tests(Tests)

# add_test(NAME ScriptCompilationSystem COMMAND Tests)