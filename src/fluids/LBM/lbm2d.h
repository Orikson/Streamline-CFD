#ifndef LBM_2D_H
#define LBM_2D_H

#include <defines.h>
#include "../fluid.h"
#include <clw.h>

#ifdef D2Q9

class LBM_2D : public Fluid {
	public:
		// init contains scene borders (and eventually emitters, tag values [for free surface / multiphase flow], etc.)
		// constructed by SceneBuilder class
		LBM_2D(float parameterize, size_t w, size_t h, cl::Image2D& init, CLContext* context) : Fluid("lbm2D", parameterize * 0.01) {
			cl::Context clContext = context->getContext();

			float* data_0, * data_14, * data_58;
			// initialize df_0
			data_0 = new float[w * h];
			for (unsigned int i = 0; i < w; i ++) {
				for (unsigned int j = 0; j < h; j ++) {
					data_0[i + j * w] = 0;
				}
			}

			// initialize df_14
			data_14 = new float[4u * w * h];
			for (unsigned int i = 0; i < w; i++) {
				for (unsigned int j = 0; j < h; j++) {
					data_14[i + j * w] = 0;
				}
			}

			// initialize df_58
			data_58 = new float[w * h];
			for (unsigned int i = 0; i < w; i++) {
				for (unsigned int j = 0; j < h; j++) {
					data_58[i + j * w] = 0;
				}
			}

			// construct df textures
			cl_int err;
			df_0 = cl::Image2D(
				clContext,
				CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
				cl::ImageFormat(CL_R, CL_FLOAT),
				w, h,
				w * sizeof(float),
				(void*)data_0,
				&err
			);
			qDebug() << "Error: " << err;

			df_14 = cl::Image2D(
				clContext,
				CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
				cl::ImageFormat(CL_RGBA, CL_FLOAT),
				w, h,
				w * sizeof(float),
				(void*)data_14,
				&err
			);
			qDebug() << "Error: " << err;

			df_58 = cl::Image2D(
				clContext,
				CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
				cl::ImageFormat(CL_RGBA, CL_FLOAT),
				w, h,
				w * sizeof(float),
				(void*)data_58,
				&err
			);
			qDebug() << "Error: " << err;

			delete[] data_0;
			delete[] data_14;
			delete[] data_58;
		}

	private:
		// 0th directional df
		cl::Image2D df_0;
		// 1-4 directional df (cartesian)
		cl::Image2D df_14;
		// 5-8 directional df (diagonals
		cl::Image2D df_58;

		// Kernels
		Kernel streamAndCollide;


};

#endif

#endif