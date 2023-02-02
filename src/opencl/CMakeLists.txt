add_library(clwrapper STATIC "clw.h" "CLDevice.h" "CLContext.h" "interop.h")

# for CL errors
target_compile_definitions(clwrapper PRIVATE __CL_ENABLE_EXCEPTIONS)

target_link_libraries(clwrapper PRIVATE Qt6::Core OpenCL::OpenCL glm::glm Qt6::OpenGLWidgets ${OpenCL_LIBRARY})
target_include_directories(clwrapper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCL_INCLUDE_DIR})
