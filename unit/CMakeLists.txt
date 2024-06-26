include(CTest)
find_package(Catch2 3 REQUIRED)

add_executable(GoalInjector goal-injector.test.cpp)
target_link_libraries(GoalInjector frontend Catch2::Catch2WithMain)
add_test(NAME GoalInjectorTest COMMAND GoalInjector)

add_executable(LoopUnroller bounded-loop-unroller.test.cpp)
target_link_libraries(LoopUnroller bounded-loop-unroller Catch2::Catch2WithMain)
add_test(NAME LoopUnrollerTest COMMAND LoopUnroller)
