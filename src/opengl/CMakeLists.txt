add_library(glwrapper STATIC "glshader.h" "OBJ_Loader.h" "camera.h" "mesh.h")

target_link_libraries(glwrapper PRIVATE Qt6::Core glm::glm Qt6::OpenGLWidgets assimp::assimp)
target_include_directories(glwrapper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCL_INCLUDE_DIR})
