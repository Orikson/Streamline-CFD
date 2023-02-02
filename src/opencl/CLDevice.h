#ifndef CLDEVICE_H
#define CLDEVICE_H

#include <string>
#include <vector>
using std::string; 
using std::vector;

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <stdio.h>

#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <CL/opencl.h>
#include <CL/cl_gl.h>

inline string to_lower(const string& s);
inline bool contains(const string& s, const string& match);
inline bool contains_any(const string& s, const vector<string>& matches);

// Adapted from https://github.com/ProjectPhysX/FluidX3D/blob/master/src/opencl.hpp
// Rewritten for understanding and minimization
struct CLDevice {
	cl::Device cl_device;
	cl_platform_id platform;
	string name, vendor;								// device name and vendor
	string driver_version, opencl_c_version;			// device driver and OpenCL C version

	bool is_cpu = false, is_gpu = false;

	cl::STRING_CLASS extensions = "";					// set of extensions supported by device

	int cores = 0, compute_units = 0, clock_frequency = 0;	// number of cores, maximum compute units, and clock frequency
	float tflops = 0;										// estimated floating point efficiency

	inline CLDevice(const cl::Device& device) {
		cl_device = device;
		platform = device.getInfo<CL_DEVICE_PLATFORM>();
		name = device.getInfo<CL_DEVICE_NAME>();
		vendor = device.getInfo<CL_DEVICE_VENDOR>();
		driver_version = device.getInfo<CL_DRIVER_VERSION>();
		opencl_c_version = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>();

		is_cpu = device.getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_CPU;
		is_gpu = device.getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_GPU;

		extensions = device.getInfo<CL_DEVICE_EXTENSIONS>();

		compute_units = device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
		clock_frequency = cl_device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();

		const int ipc = is_gpu ? 2 : 32; // IPC (instructions per cycle) is 2 for GPUs and 32 for most modern CPUs
		const bool nvidia_192_cores_per_cu = contains_any(to_lower(name), { "gt 6", "gt 7", "gtx 6", "gtx 7", "quadro k", "tesla k" }) || (clock_frequency < 1000u && contains(to_lower(name), "titan")); // identify Kepler GPUs
		const bool nvidia_64_cores_per_cu = contains_any(to_lower(name), { "p100", "v100", "a100", "a30", " 16", " 20", "titan v", "titan rtx", "quadro t", "tesla t", "quadro rtx" }) && !contains(to_lower(name), "rtx a"); // identify P100, Volta, Turing, A100, A30
		const bool amd_128_cores_per_dualcu = contains(to_lower(name), "gfx10"); // identify RDNA/RDNA2 GPUs where dual CUs are reported
		const float nvidia = (float)(contains(to_lower(vendor), "nvidia")) * (nvidia_64_cores_per_cu ? 64.0f : nvidia_192_cores_per_cu ? 192.0f : 128.0f); // Nvidia GPUs have 192 cores/CU (Kepler), 128 cores/CU (Maxwell, Pascal, Ampere, Hopper, Ada) or 64 cores/CU (P100, Volta, Turing, A100, A30)
		const float amd = (float)(contains_any(to_lower(vendor), { "amd", "advanced" })) * (is_gpu ? (amd_128_cores_per_dualcu ? 128.0f : 64.0f) : 0.5f); // AMD GPUs have 64 cores/CU (GCN, CDNA) or 128 cores/dualCU (RDNA, RDNA2), AMD CPUs (with SMT) have 1/2 core/CU
		const float intel = (float)(contains(to_lower(vendor), "intel")) * (is_gpu ? 8.0f : 0.5f); // Intel integrated GPUs usually have 8 cores/CU, Intel CPUs (with HT) have 1/2 core/CU
		const float apple = (float)(contains(to_lower(vendor), "apple")) * (128.0f); // Apple ARM GPUs usually have 128 cores/CU
		const float arm = (float)(contains(to_lower(vendor), "arm")) * (is_gpu ? 8.0f : 1.0f); // ARM GPUs usually have 8 cores/CU, ARM CPUs have 1 core/CU
		cores = (int)((float)compute_units * (nvidia + amd + intel + apple + arm)); // for CPUs, compute_units is the number of threads (twice the number of cores with hyperthreading)
		tflops = 1E-6f * (float)cores * (float)ipc * (float)clock_frequency; // estimated device floating point performance in TeraFLOPs/s
	}

	inline CLDevice() {}
};

// Returns all OpenCL enabled devices in the system
inline vector<CLDevice> getDevices() {
	vector<CLDevice> devices;
	vector<cl::Platform> cl_platforms;
	cl::Platform::get(&cl_platforms);

	for (unsigned int i = 0; i < (unsigned int)cl_platforms.size(); i++) {
		vector<cl::Device> cl_devices;
		cl_platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &cl_devices);
		for (unsigned int j = 0; j < (unsigned int)cl_devices.size(); j++) {
			devices.push_back(CLDevice(cl_devices[j]));
		}
	}
	if (cl_platforms.size() == 0 || devices.size() == 0) {
		throw std::runtime_error("No OpenCL devices detected");
	}
	return devices;
}

// Gets the best device in the system (by floating point performance)
inline CLDevice getBestDevice() {
	vector<CLDevice> devices = getDevices();
	float best = 0;
	int bestIndex = 0;
	for (int i = 0; i < devices.size(); i++) {
		if (devices.at(i).tflops > best) {
			best = devices.at(i).tflops;
			bestIndex = i;
		}
	}
	return devices.at(bestIndex);
}

inline string to_lower(const string& s) {
	string r = "";
	for (int i = 0; i < s.length(); i++) {
		const char c = s.at(i);
		r += c > 64 && c < 91 ? c + 32 : c;
	}
	return r;
}
inline bool contains(const string& s, const string& match) {
	return s.find(match) != string::npos;
}
inline bool contains_any(const string& s, const vector<string>& matches) {
	for (int i = 0; i < matches.size(); i++) if (contains(s, matches[i])) return true;
	return false;
}

#endif