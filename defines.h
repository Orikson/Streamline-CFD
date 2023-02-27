/**
Modify this file to set options before the program compiles
*/

#ifndef DEFINES_H
#define DEFINES_H

//***********************************************************************************
// Generic constants
#define GRAVITY 9.8f		// m/s
#define PI 3.14159f			// constant

//***********************************************************************************
// Fluid properties
//#define LBM				// Lattice-Boltzmann Method (see line __ for specific options)
//#define MAC				// MAC Grid (see line __ for specific options)
//#define SSF				// Stam's Stable Fluids (see line __ for specific options)
//#define VPH				// Volume Preserving Hyperbolic Mean Curvature Flow (see line __ for specific options)
//#define SPH				// Smoothed-particle Hydrodynamics (see line __ for specific options)
#define WAV					// 2D Heightfield with Wave Equations (see line __ for specific options)

// LBM Properties
#ifdef LBM
#define D2Q9				// 2 dimensions, 9 directions (__ bytes per cell)
//#define D2Q9T				// 2.5 dimensions, 9 directions (__ bytes per cell) (thin film fluid)
//#define D3Q19				// 3 dimensions, 19 directions (__ bytes per cell)
//#define D3Q27				// 3 dimensions, 27 directions (__ bytes per cell)

#define FS					// Enables volume-of-fluid free surface tracking and boundaries (__ bytes per cell)
//#define TT  				// Enables temperature tracking and diffusion (__ bytes per cell)
#endif

// MAC Properties
#ifdef MAC
// Choose one advection method
#define MAC_ADV_STAM		// Stam's Stable Fluids forward Euler backtracing (Semi-Lagrangian)
//#define MAC_ADV_MCOR		// McCormack advection (more accurate than forward Euler, also Semi-Lagrangian)

// Choose one solid boundary condition
#define MAC_STRICT			// Boundaries occupy a single cell exactly
//#define MAC_CURVED		// Curved solid boundaries

// Choose one free surface boundary condition
#define MAC_FS				// Whether or not to track the free surface (enables fluid/empty tags)
#ifdef MAC_FS
#define MAC_NO_PRESSURE		// Zeroes pressure in empty areas
//#define MAC_VOF			// Stores fractional fluid in grid cells, and interpolates pressure in partially filled cells
#endif

// Miscellaneous options
//#define MAC_TEMP			// Tracks temperature
//#define MAC_BOUY			// Tracks bouyancy

#endif 

// SSF Properties
#ifdef SSF

#endif

// VPH Properties
#ifdef VPH
#define VPH_2D				// 2 dimensions
//#define VPH_3D			// 3 dimensions
//#define VPH_SF_LBM		// Enables surface flow tracking using the Lattice Boltzmann Method
//#define VPH_SF_SSF		// Enables surface flow tracking using Stam's Stable Fluids
#endif

// SPH Properties
#ifdef SPH
#define SPH_2D				// 2 dimensions
//#define SPH_3D			// 3 dimensions
#define SPH_FS				// Free surface flow
#endif

// WAV Properties
#ifdef WAV

#endif

//***********************************************************************************
// Render properties
#define RENDER				// Enables rendering to a Qt window using OpenGL

#ifdef RENDER
#define FRAME_WIDTH	900		// Render frame width
#define FRAME_HEIGHT 630	// Render frame height
#endif

//***********************************************************************************
// Miscellaneous options
//#define SAVE_VELOCITY		// Serializes the velocity at each time step over PCIe
//#define SAVE_TEMPERATURE	// Serializes the temperature at each time step over PCIe

#endif