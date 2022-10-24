include(CTest)
find_package(Catch2 3 REQUIRED)
add_executable(GoalInjector goal-injector.test.cpp)
target_link_libraries(GoalInjector frontend Catch2::Catch2WithMain)

