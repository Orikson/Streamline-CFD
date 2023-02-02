#ifndef CLCONTEXT_H
#define CLCONTEXT_H

#include <string>
#include <vector>
using std::string;
using std::vector;
using std::map;

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <CL/opencl.h>
#include <CL/cl_gl.h>

#include "CLDevice.h"
#include "interop.h"

inline string readFile(string filename);
inline void CL_CALLBACK contextCallback(const char* errinfo, const void* private_info, size_t cb, void* user_data) {
	qDebug() << errinfo;
}

class CLContext {
	public:
		CLContext(vector<CLDevice> devices, bool usingOpenGL) : devices(devices), usingOpenGL(usingOpenGL) {
			platform = devices.at(0).platform;

			if (usingOpenGL) {
				glVars = GLContext();
				cps = createCLGLInteropContext(platform, glVars.openGLContext, glVars.display);
			}
			else {
				cps = new cl_context_properties[3];
				cps[0] = CL_CONTEXT_PLATFORM;
				cps[1] = (cl_context_properties)(platform)();
				cps[2] = 0;
			}

			for (int i = 0; i < devices.size(); i++) {
				cl_devices.push_back(devices.at(i).cl_device);
			}

			context = cl::Context(cl_devices, cps, contextCallback);
			delete[] cps;

			for (int i = 0; i < devices.size(); i++) {
				queues.push_back(cl::CommandQueue(context, devices.at(i).cl_device));
			}
		}
		~CLContext() {}

		int createProgram(string fname, string buildOptions) {
			string sourceCode = readFile(fname);

			cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()));
			cl::Program program = buildSources(source, buildOptions);
			programs.push_back(program);
			return programs.size() - 1;
		}
		int createProgram(vector<string> fnames, string buildOptions) {
			string sourceCode = readFile(fnames[0]);
			cl::Program::Sources sources(fnames.size(), std::make_pair(sourceCode.c_str(), sourceCode.length()));
			for (int i = 1; i < fnames.size(); i++) {
				string sourceCode2 = readFile(fnames[i]);
				sources[i] = std::make_pair(sourceCode2.c_str(), sourceCode2.length());
			}

			cl::Program program = buildSources(sources, buildOptions);
			programs.push_back(program);
			return programs.size() - 1;
		}
		int createProgram(string rname, string fname, string buildOptions) {
			programNames[rname] = createProgram(fname, buildOptions);
			return programNames[rname];
		}
		int createProgram(string rname, vector<string> fnames, string buildOptions) {
			programNames[rname] = createProgram(fnames, buildOptions);
			return programNames[rname];
		}
		cl::Program buildSources(cl::Program::Sources source, string buildOptions) {
			// Make program of the source code in the context
			cl::Program program = cl::Program(context, source);

			// Build program for the context devices
			try {
				program.build(cl_devices, buildOptions.c_str());
				programs.push_back(program);
			}
			catch (cl::Error& error) {
				if (error.err() == CL_BUILD_PROGRAM_FAILURE) {
					for (unsigned int i = 0; i < devices.size(); i++) {
						qDebug() << "Build log, device " << i << ":\n" << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl_devices[i]).c_str();
					}
				}
				qDebug() << "[ERROR] " << error.err();

				throw error;
			}
			return program;
		}

		cl::Program getProgram(unsigned int i) {
			return programs.at(i);
		}
		cl::Program getProgram(string rname) {
			return programs.at(programNames[rname]);
		}
		cl::Context getContext() {
			return context;
		}
		cl::CommandQueue getQueue(unsigned int i) {
			return queues.at(i);
		}

		// Run the kernel with the given name, range, and arguments
		// For interop, a glObjects vector is provided that must contain all relevant gl memory to the kernel
		template <class ... Ts>
		void runProgram(string rname, const cl::CommandQueue &queue, const cl::NDRange &globalOffset, const cl::NDRange &globalRange, const cl::NDRange &localRange, const vector<cl::Memory> &glObjects, Ts &&... args) {
			queue.enqueueAcquireGLObjects(&glObjects);

			int i = 0;
			cl::Kernel kernel(getProgram(rname), rname);
			([&] {
				// Set all kernel arguments
				kernel.setArg(i, args);
				i++;
			}(), ...);

			queue.enqueueNDRangeKernel(
				kernel,
				globalOffset,
				globalRange,
				localRange
			);
			queue.enqueueReleaseGLObjects(&glObjects);
			queue.finish();
		}
		// Run the kernel with the given name, range, and arguments
		template <class ... Ts>
		void runProgram(string rname, const cl::CommandQueue& queue, const cl::NDRange& globalOffset, const cl::NDRange& globalRange, const cl::NDRange& localRange, Ts &&... args) {
			int i = 0;
			cl::Kernel kernel(getProgram(rname), rname);
			([&] {
				// Set all kernel arguments
				kernel.setArg(i, args);
				i++;
			}(), ...);

			queue.enqueueNDRangeKernel(
				kernel,
				globalOffset,
				globalRange,
				localRange
			);
			queue.finish();
		}

	private:
		cl::Context context;
		cl::Platform platform;
		vector<cl::CommandQueue> queues;
		vector<cl::Program> programs;
		map<string, int> programNames;
		
		GLContext glVars;
		bool usingOpenGL;

		vector<CLDevice> devices;
		vector<cl::Device> cl_devices;

		cl_context_properties* cps;
};

inline string readFile(string filename) {
	string retval = "";

	std::ifstream sourceFile(filename.c_str(), std::fstream::in);

	if (sourceFile.fail())
		throw std::runtime_error("Failed to open OpenCL source file.");

	std::stringstream stringStream;
	stringStream.str("");
	stringStream << sourceFile.rdbuf();

	string sourceCode = stringStream.str();
	retval = sourceCode;

	return retval;
}

#endif