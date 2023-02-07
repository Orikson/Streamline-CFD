#ifndef INTEROP_H
#define INTEROP_H

#include <string>
#include <vector>
using std::string;
using std::vector;

#ifdef _WIN32
#include <windows.h>
#endif

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <CL/opencl.h>
#include <CL/cl_gl.h>

#include <QOpenGLFunctions>

// Creates a set of cl_context_properties for CL context construction
cl_context_properties* createCLGLInteropContext(const cl::Platform& platform, cl_context_properties openGLContext, cl_context_properties display);

struct GLContext {
	cl_context_properties openGLContext;
	cl_context_properties display;

	inline GLContext() {
#if defined(__APPLE__) || defined(__MACOSX)
		openGLContext = (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext());
		display = NULL;
#elif _WIN32
		openGLContext = (cl_context_properties)wglGetCurrentContext();
		display = (cl_context_properties)wglGetCurrentDC();
#else
		openGLContext = (cl_context_properties)glXGetCurrentContext();
		display = (cl_context_properties)glXGetCurrentDisplay();
#endif // OS
	}
};

// CL-GL shared 2D image buffer
class InteropTexture2D {
	public:
		InteropTexture2D(int w, int h, QOpenGLFunctions* f, cl::Context &context) : w(w), h(h), f(f), context(context) {
			// Create GL texture
			f->glEnable(GL_TEXTURE_2D);
			f->glGenTextures(1, &glTexture);
			f->glBindTexture(GL_TEXTURE_2D, glTexture);
			f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);
			f->glBindTexture(GL_TEXTURE_2D, 0);
			f->glFinish();

			// Create CL image
			clImage = cl::ImageGL(
				context,
				CL_MEM_READ_WRITE,
				GL_TEXTURE_2D,
				0,
				glTexture
			);
		}
		~InteropTexture2D() {

		}

		GLuint getGL() { return glTexture; }
		cl::ImageGL getCL() { return clImage; }
		int width() { return w; }
		int height() { return h; }

	private:
		cl::Context context;
		QOpenGLFunctions* f;

		int w, h;
		GLuint glTexture;
		cl::ImageGL clImage;
};

inline cl_context_properties* createCLGLInteropContext(const cl::Platform& platform, cl_context_properties openGLContext, cl_context_properties display) {
#if defined(__APPLE__) || defined(__MACOSX)
	CGLSetCurrentContext((CGLContextObj)openGLContext);
	CGLShareGroupObj shareGroup = CGLGetShareGroup((CGLContextObj)openGLContext);
	if (shareGroup == NULL) throw Exception("Not able to get sharegroup");
	cl_context_properties* cps = new cl_context_properties[3];
	cps[0] = CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE;
	cps[1] = (cl_context_properties)shareGroup;
	cps[2] = 0;
#else
#ifdef _WIN32
	// Windows
	cl_context_properties* cps = new cl_context_properties[7];
	cps[0] = CL_GL_CONTEXT_KHR;
	cps[1] = openGLContext;
	cps[2] = CL_WGL_HDC_KHR;
	cps[3] = display;
	cps[4] = CL_CONTEXT_PLATFORM;
	cps[5] = (cl_context_properties)(platform)();
	cps[6] = 0;
#else
	cl_context_properties* cps = new cl_context_properties[7];
	cps[0] = CL_GL_CONTEXT_KHR;
	cps[1] = openGLContext;
	cps[2] = CL_GLX_DISPLAY_KHR;
	cps[3] = display;
	cps[4] = CL_CONTEXT_PLATFORM;
	cps[5] = (cl_context_properties)(platform)();
	cps[6] = 0;
#endif
#endif
	return cps;
}


#endif