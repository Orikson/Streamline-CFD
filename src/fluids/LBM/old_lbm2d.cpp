#include "lbm2d.h"

#ifdef D2Q9
LBM_2D::LBM_2D(float parameterize, size_t w, size_t h, cl::Image2D& init, CLContext* context, cl::CommandQueue& queue) : boundaries(init), context(context), Fluid("lbm2D", parameterize * 0.01) {
	cl::Context clContext = context->getContext();

	srand(0);
	
	float* data_0, * data_14, * data_58, * data_vd;
	float lo = 0.01; float hi = 0.02;
	float* rho = new float[9u * w * h];
	// initialize df_0
	data_0 = new float[w * h];
	for (unsigned int i = 0; i < w; i++) {
		for (unsigned int j = 0; j < h; j++) {
			data_0[i + j * w] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
		}
	}

	// initialize df_14
	data_14 = new float[4u * w * h];
	for (unsigned int i = 0; i < w; i++) {
		for (unsigned int j = 0; j < h; j++) {
			data_14[i * 4 + j * 4 * w] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));;
			data_14[i * 4 + j * 4 * w + 1] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
			data_14[i * 4 + j * 4 * w + 2] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
			data_14[i * 4 + j * 4 * w + 3] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
		}
	}

	// initialize df_58
	data_58 = new float[4u * w * h];
	for (unsigned int i = 0; i < w; i++) {
		for (unsigned int j = 0; j < h; j++) {
			data_58[i * 4 + j * 4 * w] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
			data_58[i * 4 + j * 4 * w + 1] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
			data_58[i * 4 + j * 4 * w + 2] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
			data_58[i * 4 + j * 4 * w + 3] = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
		}
	}

	// divide by rho

	// initialize v_d
	data_vd = new float[4u * w * h];
	for (unsigned int i = 0; i < w; i++) {
		for (unsigned int j = 0; j < h; j++) {
			data_vd[i * 4 + j * 4 * w] = 0;
			data_vd[i * 4 + j * 4 * w + 1] = 0;
			data_vd[i * 4 + j * 4 * w + 2] = 0;
			data_vd[i * 4 + j * 4 * w + 3] = 0;
		}
	}

	// construct df textures
	cl_int err;
	df_0 = DoubleImage2D(
		clContext,
		CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE,
		cl::ImageFormat(CL_R, CL_FLOAT),
		w, h,
		w * sizeof(float),
		(void*)data_0,
		&err
	);
	qDebug() << "Error: " << err;

	df_14 = DoubleImage2D(
		clContext,
		CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE,
		cl::ImageFormat(CL_RGBA, CL_FLOAT),
		w, h,
		w * sizeof(float) * 4,
		(void*)data_14,
		&err
	);
	qDebug() << "Error: " << err;

	df_58 = DoubleImage2D(
		clContext,
		CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE,
		cl::ImageFormat(CL_RGBA, CL_FLOAT),
		w, h,
		w * sizeof(float) * 4,
		(void*)data_58,
		&err
	);
	qDebug() << "Error: " << err;

	v_d = cl::Image2D(
		clContext,
		CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE,
		cl::ImageFormat(CL_RGBA, CL_FLOAT),
		w, h,
		w * sizeof(float) * 4,
		(void*)data_vd,
		&err
	);
	qDebug() << "Error: " << err;

	delete[] data_0;
	delete[] data_14;
	delete[] data_58;

	// Initialize kernels
	context->createProgram("lbm", "../../../src/shaders/opencl/lbm.cl", "");
	cl::NDRange globalOffset = cl::NullRange;
	cl::NDRange globalRange = cl::NDRange(w, h);
	cl::NDRange localRange = cl::NullRange;
	vector<cl::Memory> glObjects;
	streamAndCollide = Kernel(queue, globalOffset, globalRange, localRange, glObjects);
}

void LBM_2D::step() {
	context->runProgram(
		"lbm", "lbm_2d_stream_and_collide",
		streamAndCollide,
		boundaries, df_0.getInactive(), df_14.getInactive(), df_58.getInactive(), deltaT,
		df_0.getActive(), df_14.getActive(), df_58.getActive(), v_d
	);

	df_0.swap();
	df_14.swap();
	df_58.swap();
}

void LBM_2D::render() {

}

cl::Image2D LBM_2D::getVD() {
	return v_d;
}

#endif