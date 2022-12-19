option(ENABLE_RUN_TESTS_AFTER_BUILD "Automatically run built artifacts. If disabled, the tests can be run with ctest instead" OFF)

add_executable(TestRayTracing)
target_compile_features(TestRayTracing PRIVATE cxx_std_20)
set_target_properties(TestRayTracing PROPERTIES CXX_EXTENSIONS OFF)

find_package(ut CONFIG REQUIRED)
target_compile_definitions(TestRayTracing PRIVATE BOOST_UT_DISABLE_MODULE)
target_link_libraries(TestRayTracing PRIVATE Boost::ut)

if(ENABLE_RUN_TESTS_AFTER_BUILD)
  add_custom_command(TARGET TestRayTracing COMMAND TestRayTracing)
else()
  add_test(NAME TestRayTracing COMMAND TestRayTracing)
endif()

target_sources(TestRayTracing PRIVATE
	tests.cc
)

target_link_libraries(TestRayTracing PRIVATE RayTracing)
