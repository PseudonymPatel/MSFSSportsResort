cmake_minimum_required(VERSION 3.13)
project(MSFSSportsResort)

set(CMAKE_CXX_STANDARD 14)

# where the #include stuff will be located.
include_directories(includes)
include_directories(src)

# create the executable
add_executable(MSFSSportsResort "")

add_subdirectory(src)

# find and link Simconnect and wiiuse
find_library(WIIUSE_LIB wiiuse lib)
find_library(SIMCONNECT_LIB SimConnect SimConnect)
target_link_libraries(MSFSSportsResort LINK_PUBLIC ${WIIUSE_LIB})
target_link_libraries(MSFSSportsResort LINK_PUBLIC ${SIMCONNECT_LIB})

# move the dlls into the running folder
add_custom_command(TARGET MSFSSportsResort POST_BUILD        # Adds a post-build event to uselib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/lib/wiiuse.dll"      # this is in-file
        $<TARGET_FILE_DIR:MSFSSportsResort>)                 # this is out-file path
add_custom_command(TARGET MSFSSportsResort POST_BUILD        # Adds a post-build event to uselib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/SimConnect/SimConnect.dll"      # this is in-file
        $<TARGET_FILE_DIR:MSFSSportsResort>)                 # this is out-file path