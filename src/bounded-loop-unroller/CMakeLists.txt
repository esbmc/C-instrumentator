add_library(bounded-loop-unroller frontend_action.cpp frontend_consumer.cpp frontend_visitor.cpp)
target_include_directories(bounded-loop-unroller
        PRIVATE ${CLANG_INCLUDE_DIRS}
        )
set_target_properties(bounded-loop-unroller PROPERTIES COMPILE_FLAGS "-fno-rtti")
target_link_libraries(bounded-loop-unroller clangTooling clangAST clangIndex fmt::fmt)

add_executable(loop-unroller main.cpp)
target_include_directories(instrumentator
        PRIVATE ${CLANG_INCLUDE_DIRS}
        )
target_link_libraries(loop-unroller PUBLIC bounded-loop-unroller clangTooling clangAST clangIndex)
