project(ASTROBEE_APP C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${ros_app_MISSION_DIR}/fsw/platform_inc)

# Create the app module
add_cfe_app(astrobee_app fsw/src/astrobee_app.c)
target_link_libraries(astrobee_app m)

target_include_directories(astrobee_app PUBLIC
  fsw/mission_inc
  fsw/platform_inc
)
