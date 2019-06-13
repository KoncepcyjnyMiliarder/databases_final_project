cmake_minimum_required(VERSION 3.0)

project(databases_final_project)

set(CMAKE_CXX_STANDARD 14)

find_package(PostgreSQL REQUIRED)

set(SOURCES
  src/main.cpp
  src/database_executor.cpp
  src/query_result.cpp
  src/init_command_handler.cpp
  src/api_command_handler.cpp
  src/init_leader.cpp
  src/api_support.cpp
  src/api_upvote.cpp
  src/api_actions.cpp
  src/api_projects.cpp
  src/api_votes.cpp
  src/api_trolls.cpp
)

set(INCLUDES
  inc/database_executor.hpp
  inc/query_result.hpp
  inc/command_handler.hpp
  inc/init_command_handler.hpp
  inc/api_command_handler.hpp
  inc/abstract_api.hpp
  inc/init_leader.hpp
  inc/api_support.hpp
  inc/api_upvote.hpp
  inc/api_actions.hpp
  inc/api_projects.hpp
  inc/api_votes.hpp
  inc/api_trolls.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
  inc
  third_party
  ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${PostgreSQL_LIBRARIES}
)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/resources
  ${CMAKE_CURRENT_BINARY_DIR}/resources
)
