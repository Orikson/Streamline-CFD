add_library(fluids STATIC "VPHMCF/vphmcf.h" "fluid.h" "fluid.cpp" "VPHMCF/vphmcf.cpp" "LBM/lbm2d.h" "LBM/lbm2d.cpp")

target_link_libraries(fluids PRIVATE defines glm::glm Qt6::Widgets Qt6::OpenGLWidgets OpenCL::OpenCL ${OpenCL_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
target_include_directories(fluids INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCL_INCLUDE_DIR})
