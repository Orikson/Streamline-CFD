add_library(fluids STATIC 
	"fluid.h" "fluid.cpp" 
	"VPHMCF/vphmcf.h" "VPHMCF/vphmcf.cpp"
	"LBM/lbm2d.h" "LBM/lbm2d.cpp" 
	"SPH/sph2d.h" "SPH/sph2d.cpp" 
	"shapes.h" "WAV/wav.h" "WAV/wav.cpp")

target_link_libraries(fluids PRIVATE defines clwrapper glm::glm Qt6::Widgets Qt6::OpenGLWidgets OpenCL::OpenCL ${OpenCL_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
target_include_directories(fluids INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCL_INCLUDE_DIR})
