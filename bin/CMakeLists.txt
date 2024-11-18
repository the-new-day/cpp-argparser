add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(uint239_t)

target_link_libraries(${PROJECT_NAME} PRIVATE argparser uint239_t)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})