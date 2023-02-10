#include "glWidget.h"

// Screen plane vertices
const float GLWidget::vertices[12] = {
	-1.0f,-1.0f, 0.0,
	 1.0f,-1.0f, 0.0,
	 1.0f, 1.0f, 0.0,
	-1.0f, 1.0f, 0.0
};
const unsigned int GLWidget::indices[6] = { 0, 1, 2, 0, 2, 3 };

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
	clContext = NULL;
	screenTexture = NULL;
	VAO = NULL; EBO = 0; VBO = 0;
	pTimer = NULL;
	shader = NULL;
	
	scene = NULL;
	
	frame = 0;
}
GLWidget::~GLWidget() {

}

void GLWidget::initializeGL() {
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Initialize CL context now that GL context has been initialized
	vector<CLDevice> clDevices;
	clDevices.push_back(getBestDevice());
	qDebug() << "Using device: " << clDevices.at(0).name.c_str();
	clContext = new CLContext(clDevices, true);

	// Create CL-GL texture
	screenTexture = new InteropTexture2D(size().width(), size().height(), f, clContext->context);

	// Create CL kernel (to compute Mandelbrot)
	clContext->createProgram("mandelbrot", "../../../src/shaders/opencl/mandelbrot.cl", "");

	// Setup kernel parameters
	cl::CommandQueue queue = clContext->getQueue(0);
	cl::NDRange globalOffset = cl::NullRange;
	cl::NDRange globalRange = cl::NDRange(screenTexture->width(), screenTexture->height());
	cl::NDRange localRange = cl::NullRange;
	vector<cl::Memory> glObjects; 
	glObjects.push_back(screenTexture->getCL());
	mandelbrot = Kernel(queue, globalOffset, globalRange, localRange, glObjects);

	// Create GL shader (to render texture to plane)
	shader = new Shader(f, "../../../src/shaders/opengl/render.vs", "../../../src/shaders/opengl/render.fs");

	// Create screen plane
	VAO = new QOpenGLVertexArrayObject(this);
	VAO->create();
	VAO->bind();

	f->glGenBuffers(1, &VBO);
	f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
	f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	f->glGenBuffers(1, &EBO);
	f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	f->glEnableVertexAttribArray(0);

	VAO->release();

	// Set timer for render updates
	pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(update()));
	pTimer->start(1000 / 60.0);

	// Create scene
#if defined(VPH_2D) // VPH
	scene = new BubbleScene(f);
#elif defined(D2Q9)
	cl::ImageGL imageGL = screenTexture->getCL();
	scene = new TurbulentScene(f, clContext, queue, imageGL, screenTexture->width(), screenTexture->height());
#else // Default

#endif // Default
}

void GLWidget::paintGL() {
	frame ++;

	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	f->glClear(GL_COLOR_BUFFER_BIT);

	// Compute CL kernel
	//clContext->runProgram("mandelbrot", mandelbrot, float(frame), screenTexture->getCL());

	// Render screen-plane
	scene->step();
	scene->render();
	if (scene->useScreenPlane()) {
		VAO->bind();

		f->glActiveTexture(GL_TEXTURE0);
		f->glBindTexture(GL_TEXTURE_2D, screenTexture->getGL());

		f->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		VAO->release();
	}
}

void GLWidget::resizeGL(int w, int h) {

}