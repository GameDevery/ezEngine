ez_cmake_init()

ez_requires_d3d()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(APPLICATION ${PROJECT_NAME})

ez_make_winmain_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  GameEngine
  RendererDX11
)

add_dependencies(${PROJECT_NAME}
  ShaderCompilerHLSL
)
