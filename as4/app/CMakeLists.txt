# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(as4 ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(as4 LINK_PRIVATE hal)
target_link_libraries(as4 LINK_PRIVATE Threads::Threads)
target_link_libraries(as4 LINK_PRIVATE m)

include_directories(../pru-as4/)

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET as4 POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:as4>"
     "~/cmpt433/public/myApps/as4" 
  COMMENT "Copying ARM executable to public NFS directory")
