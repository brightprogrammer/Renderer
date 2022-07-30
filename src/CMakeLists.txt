# get source files
file(GLOB_RECURSE GAME_SRCS ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_executable(dynamic ${GAME_SRCS})
target_link_libraries(dynamic ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES})
target_include_directories(dynamic PUBLIC ${SDL2_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
