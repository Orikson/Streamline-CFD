#ifndef LBM_2D_H
#define LBM_2D_H

#include <defines.h>
#include "../fluid.h"
#include <clw.h>
#include <cstdlib>

#ifdef D2Q9

class LBM_2D : public Fluid {
	public:
		// init contains scene borders (and eventually emitters, tag values [for free surface / multiphase flow], etc.)
		// constructed by SceneBuilder class
		LBM_2D(float parameterize, size_t w, size_t h, cl::Image2D& init, CLContext* context, cl::CommandQueue& queue);

		cl::Image2D getVD();

		void step() override;
		void render() override;

	private:
		CLContext* context;

		// 0th directional df
		DoubleImage2D df_0;
		// 1-4 directional df (cartesian)
		DoubleImage2D df_14;
		// 5-8 directional df (diagonals)
		DoubleImage2D df_58;
		// velocity and density
		cl::Image2D v_d;
		// boundaries
		cl::Image2D boundaries;

		// Kernels
		Kernel streamAndCollide;
};

#endif

#endif