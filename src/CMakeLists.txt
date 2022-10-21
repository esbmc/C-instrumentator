add_subdirectory(frontend-action)

add_executable(instrumentator main.cpp)
target_include_directories(instrumentator
        PRIVATE ${CLANG_INCLUDE_DIRS}
        )
target_link_libraries(instrumentator frontend)