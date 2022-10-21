add_library(frontend frontend_action.cpp frontend_consumer.cpp frontend_visitor.cpp)
target_include_directories(frontend
        PRIVATE ${CLANG_INCLUDE_DIRS}
        )
target_link_libraries(frontend clangTooling clangAST clangIndex)