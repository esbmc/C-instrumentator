add_library(frontend frontend_action.cpp frontend_consumer.cpp frontend_visitor.cpp)
target_include_directories(frontend
        PRIVATE ${CLANG_INCLUDE_DIRS}
        )
set_target_properties(frontend PROPERTIES COMPILE_FLAGS "-fno-rtti")
target_link_libraries(frontend clangTooling clangAST clangIndex fmt::fmt)

add_executable(instrumentator main.cpp)
target_include_directories(instrumentator
        PRIVATE ${CLANG_INCLUDE_DIRS}
        )
target_link_libraries(instrumentator PUBLIC frontend clangTooling clangAST clangIndex)
