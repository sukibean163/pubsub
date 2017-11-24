cmake_minimum_required(VERSION 2.8)
project(pubsub)

add_compile_options(-std=c++11)
add_compile_options(-pthread)



file(GLOB_RECURSE src_files src/*.cpp)
file(GLOB_RECURSE examples_files examples/*.cpp)


include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../${PROJECT_NAME}/include   
)

add_library(${PROJECT_NAME}
  ${src_files}
  ${header_files}
)

target_link_libraries(${PROJECT_NAME}
pthread
)

add_executable(${PROJECT_NAME}_main
  ${examples_files}
)


target_link_libraries(${PROJECT_NAME}_main
${PROJECT_NAME}
    pubsub
)

install(TARGETS ${PROJECT_NAME}
        DESTINATION lib
        )
install(TARGETS ${PROJECT_NAME}_main
        DESTINATION lib
        )
install(DIRECTORY include/
        DESTINATION include)