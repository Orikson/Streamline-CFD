﻿add_executable(streamline 
    "main.cpp" 
    "application.h" "application.cpp" 
    "glwidget.h" "glwidget.cpp" 
    "infoWidget.h" "infoWidget.cpp" 
    "scenes/scene.h" 
    "scenes/sceneBuilder.h"
    "scenes/VPHMCF_2D/bubbleScene.h" 
    "scenes/LBM_2D/turbulentScene.h"
    "scenes/SPH_2D/damBreakScene.h" "scenes/WAV/boatScene.h")

target_link_libraries(streamline PRIVATE
    defines fluids clwrapper glwrapper
    Qt6::Widgets Qt6::Gui Qt6::OpenGLWidgets 
    OpenCL::OpenCL 
    glm::glm
    ${OPENGL_LIBRARIES} ${GLUT_LIBRARY}
)

set_target_properties(streamline PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)