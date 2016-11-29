#include <mlopen/convolution.hpp>
#include <mlopen/util.hpp>
#include <mlopen/mlo_internal.hpp>

#include "tinygemm.hpp"
namespace mlopen {

void ConvolutionDescriptor::FindConvFwdAlgorithm(Handle& handle,
		const TensorDescriptor&		xDesc,
		const cl_mem				x,
		const TensorDescriptor&		wDesc,
		const cl_mem				w,
		const TensorDescriptor&		yDesc,
		const cl_mem				y,
		const int					 /*requestAlgoCount*/,
		int							* /*returnedAlgoCount*/,
		mlopenConvAlgoPerf_t		* /*perfResults*/,
		mlopenConvPreference_t		 /*preference*/,
		void						* /*workSpace*/,
		size_t						 /*workSpaceSize*/,
		bool						exhaustiveSearch) const {
	
	if(x == nullptr || w == nullptr || y == nullptr) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
#if 0
	if(returnedAlgoCount == nullptr || perfResults == nullptr) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(requestAlgoCount < 1) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
#endif 

	// Generate kernels if OpenCL
	// Compile, cache kernels, etc.
	// Launch all kernels and store the perf, workspace limits, etc.
	mlo_construct_direct2D construct_params(1); // forward
	{
		construct_params.doSearch(exhaustiveSearch);
		construct_params.saveSearchRequest(true);

		construct_params.setGeneralCompOptions("");

		construct_params.setStream(handle.GetStream());

		construct_params.setOutputDescFromMLDesc(yDesc);
		construct_params.setInputDescFromMLDesc(xDesc);
		construct_params.setWeightDescFromMLDesc(wDesc);

		construct_params.setConvDescr(pad_h, pad_w, u, v, upscalex, upscaley);

		construct_params.mloConstruct();
	}

	std::string program_name = construct_params.getKernelFile();  //"../src/Hello.cl"; // CL kernel filename
	std::string kernel_name = construct_params.getKernelName(); // "hello_world_kernel"; // kernel name
	std::string parms = construct_params.getCompilerOptions(); // kernel parameters

	std::string network_config;
	construct_params.mloBuildConf_Key(network_config);

	const std::vector<size_t> & vld = construct_params.getLocalWkSize();
	const std::vector<size_t> & vgd = construct_params.getGlobalWkSize();

	float padding_val = 0;
	handle.GetKernel("mlopenConvolutionFwdAlgoDirect",
			network_config,
			program_name, 
			kernel_name,
			vld,
			vgd,
			parms)(x, w, y, padding_val);
}

void ConvolutionDescriptor::ConvolutionForward(Handle& handle,
		const void					* /*alpha*/,
		const TensorDescriptor&		xDesc,
		const cl_mem				x,
		const TensorDescriptor&		wDesc,
		const cl_mem				w,
		mlopenConvFwdAlgorithm_t	algo,
		const void					* /*beta*/,
		const TensorDescriptor&		yDesc,
		cl_mem						y, 
		void						* /*workSpace*/,
		size_t						 /*workSpaceSize*/) const {

	if(x == nullptr || w == nullptr || y == nullptr) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(xDesc.GetSize() != yDesc.GetSize() || xDesc.GetSize() != wDesc.GetSize()) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(xDesc.GetType() != yDesc.GetType() || xDesc.GetType() != wDesc.GetType()) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(xDesc.GetLengths()[1] != wDesc.GetLengths()[1]) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(xDesc.GetSize() < 3) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	
	// TODO(paul): Replicating code for now.
	mlo_construct_direct2D construct_params(1); // forward
	{
		construct_params.setOutputDescFromMLDesc(yDesc);
		construct_params.setInputDescFromMLDesc(xDesc);
		construct_params.setWeightDescFromMLDesc(wDesc);
	}

	std::string network_config;
	construct_params.mloBuildConf_Key(network_config);

	std::string algorithm_name;
	switch(algo) {
		case mlopenConvolutionFwdAlgoDirect:
			algorithm_name = "mlopenConvolutionFwdAlgoDirect";
			break;
		case mlopenConvolutionFwdAlgoGEMM:
			algorithm_name = "mlopenConvolutionFwdAlgoGEMM";
			break;
		case mlopenConvolutionFwdAlgoFFT:
			algorithm_name = "mlopenConvolutionFwdAlgoFFT";
			break;
		case mlopenConvolutionFwdAlgoWinograd:
			algorithm_name = "mlopenConvolutionFwdAlgoWinograd";
			break;
	}

	float padding_val = 0;
	handle.GetKernel(algorithm_name, network_config)(x, w, y, padding_val);
}

// FindBackwardDataAlgorithm()
//
void ConvolutionDescriptor::FindConvBwdDataAlgorithm(Handle& handle,
		const TensorDescriptor&		dyDesc,
		const cl_mem				dy,
		const TensorDescriptor&		wDesc,
		const cl_mem				w,
		const TensorDescriptor&		dxDesc,
		const cl_mem				dx,
		const int					 /*requestAlgoCount*/,
		int							* /*returnedAlgoCount*/,
		mlopenConvAlgoPerf_t		* /*perfResults*/,
		mlopenConvPreference_t		 /*preference*/,
		void						* /*workSpace*/,
		size_t						 /*workSpaceSize*/,
		bool						exhaustiveSearch) const {
	
	if(dx == nullptr || w == nullptr || dy == nullptr) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
#if 0
	if(returnedAlgoCount == nullptr || perfResults == nullptr) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(requestAlgoCount < 1) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
#endif 

	// Generate kernels if OpenCL
	// Compile, cache kernels, etc.
	// Launch all kernels and store the perf, workspace limits, etc.
	mlo_construct_direct2D construct_params(0); // backward
	{
		construct_params.doSearch(exhaustiveSearch);
		construct_params.saveSearchRequest(true);

		construct_params.setGeneralCompOptions("");

		construct_params.setStream(handle.GetStream());

		construct_params.setOutputDescFromMLDesc(dyDesc);
		construct_params.setInputDescFromMLDesc(dxDesc);
		construct_params.setWeightDescFromMLDesc(wDesc);

		construct_params.setConvDescr(pad_h, pad_w, u, v, upscalex, upscaley);

		construct_params.mloConstruct();
	}

	std::string program_name = construct_params.getKernelFile();
	std::string kernel_name = construct_params.getKernelName(); // kernel name
	std::string parms = construct_params.getCompilerOptions(); // kernel parameters

	std::string network_config;
	construct_params.mloBuildConf_Key(network_config);

	const std::vector<size_t> & vld = construct_params.getLocalWkSize();
	const std::vector<size_t> & vgd = construct_params.getGlobalWkSize();

	float padding_val = 0;
	handle.GetKernel("mlopenConvolutionBwdDataAlgo_0",
			network_config,
			program_name,
			kernel_name,
			vld,
			vgd,
			parms)(dy, w, dx, padding_val);
}

// BackwardDataAlgorithm()
void ConvolutionDescriptor::ConvolutionBackwardData(Handle& handle,
		const void						* /*alpha*/,
		const TensorDescriptor&			dyDesc,
		const cl_mem					dy,
		const TensorDescriptor&			wDesc,
		const cl_mem					w,
		mlopenConvBwdDataAlgorithm_t	/* algo */,
		const void						* /*beta*/,
		const TensorDescriptor&			dxDesc,
		cl_mem							dx, 
		void							* /*workSpace*/,
		size_t							 /*workSpaceSize*/) const {

	if(dx == nullptr || w == nullptr || dy == nullptr) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(dyDesc.GetSize() != dxDesc.GetSize() || dyDesc.GetSize() != wDesc.GetSize()) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(dyDesc.GetType() != dxDesc.GetType() || dyDesc.GetType() != wDesc.GetType()) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(dyDesc.GetLengths()[1] != wDesc.GetLengths()[0]) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(dyDesc.GetSize() < 3) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}

	// Launch all kernels and store the perf, workspace limits, etc.
	mlo_construct_direct2D construct_params(0); // backward
	{
		construct_params.setOutputDescFromMLDesc(dyDesc);
		construct_params.setInputDescFromMLDesc(dxDesc);
		construct_params.setWeightDescFromMLDesc(wDesc);
	}

	std::string network_config;
	construct_params.mloBuildConf_Key(network_config);

	float padding_val = 0;
	handle.GetKernel("mlopenConvolutionBwdDataAlgo_0", network_config) (dy, w, dx, padding_val);
}

// ConvolutionBackwardWeightsGetWorkSpaceSize
void ConvolutionDescriptor::ConvolutionBackwardWeightsGetWorkSpaceSize(
	const TensorDescriptor&		 dyDesc,
	const TensorDescriptor&		 xDesc,
	const TensorDescriptor&		 dwDesc,
	size_t						*workSpaceSize)
{
	mlo_construct_BwdWrW2D construct_params(0); // backward with regards to weights
	construct_params.doSearch(false);
	construct_params.setOutputDescFromMLDesc(dyDesc);
	construct_params.setInputDescFromMLDesc(xDesc);
	construct_params.setWeightDescFromMLDesc(dwDesc);
	construct_params.setConvDescr(pad_h, pad_w, u, v, upscalex, upscaley);
	construct_params.mloConstruct();
	
	*workSpaceSize = construct_params.getWorkSpaceSzBytes();
}


// FindBackwardWeightsAlgorithm()
//
void ConvolutionDescriptor::FindConvBwdWeightsAlgorithm(Handle& handle,
		const TensorDescriptor&		dyDesc,
		const cl_mem				dy,
		const TensorDescriptor&		xDesc,
		const cl_mem				x,
		const TensorDescriptor&		dwDesc,
		const cl_mem				dw,
		const int					 /*requestAlgoCount*/,
		int							* /*returnedAlgoCount*/,
		mlopenConvAlgoPerf_t		* /*perfResults*/,
		mlopenConvPreference_t		 /*preference*/,
		cl_mem						workSpace,
		size_t						/*workSpaceSize*/,
		bool						/*exhaustiveSearch*/) const {
	
	if(x == nullptr || dw == nullptr || dy == nullptr) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}

	int in_n, in_c, in_h, in_w;
	std::tie(in_n, in_c, in_h, in_w) = tie4(xDesc.GetLengths());

	int wei_n, wei_h, wei_w;
	std::tie(wei_n, std::ignore, wei_h, wei_w) = tie4(dwDesc.GetLengths());

	int out_h, out_w;
	std::tie(std::ignore, std::ignore, out_h, out_w) = tie4(dyDesc.GetLengths());

	// GEMM
	int M = in_c * wei_h * wei_w;
	int N = wei_n;
	int K = out_h * out_w;
	float alpha = 1.0;
	float beta = 1.0;
	bool tA = true;
	bool tB = false;
	bool tC = false;
	unsigned int lda = K;
	unsigned int ldb = K;
	unsigned int ldc = M;

	size_t in_offset = 0;
	Im2ColGPU(handle, x, in_offset, in_c, in_h, in_w, wei_h, wei_w, out_h, out_w, pad_h, pad_w, v, u, workSpace);

	// bool isColMajor, bool tA, bool tB, bool tC, lda, ldb, ldc, m, n, k, a_offset, b_offset, c_offset
	tinygemm::TinyGemmGeometry tgg( true, tA, tB, tC, lda, ldb, ldc, M, N, K, 0, 0, 0);
	// alloted_time, queue, a, b, c, enforce_determinism, float_type, geometry, alpha, beta, verbose 
	tinygemm::TinyGemmSolution soln = tinygemm::find(15, handle.GetStream(), workSpace, dy, dw, false, 'f', tgg, alpha, beta, true);

	std::string program_name = soln.main_kernel;
	std::string kernel_name = soln.main_kernel_function_name;
	std::string network_config = tgg.get_networkconfig_string();

	auto main_kernel_worksize_params =  soln.get_main_kernel_worksize_params(M, N);
	size_t local_work_size = main_kernel_worksize_params.at("local_work_size");
	size_t global_work_size = main_kernel_worksize_params.at("global_work_size");

	std::vector<size_t> vld (1, local_work_size);
	std::vector<size_t> vgd (1, global_work_size);

	handle.GetKernel("mlopenConvolutionBwdWeightsAlgoGEMM",
			network_config,
			program_name,
			kernel_name,
			vld,
			vgd,
			"");

	// Direct Kernel
	mlo_construct_BwdWrW2D construct_params(0); // backward with regards to weights
	{
		construct_params.doSearch(false);
		construct_params.setStream(handle.GetStream());
		construct_params.setOutputDescFromMLDesc(dyDesc);
		construct_params.setInputDescFromMLDesc(xDesc);
		construct_params.setWeightDescFromMLDesc(dwDesc);
		construct_params.setConvDescr(pad_h, pad_w, u, v, upscalex, upscaley);
		construct_params.mloConstruct();
	}

	construct_params.mloBuildConf_Key(network_config);

	const std::vector<mlo_kernel_info> & bwd_wrw_info = construct_params.getKernelsInfo();
	/*
	* get info for all kernels of the layer
	* std::string _kernel_name;
	* std::string _kernel_file;
	* std::string _comp_options;
	* std::vector<size_t> _g_wk;
	* std::vector<size_t> _l_wk;
	*/
	// reset profiler
	handle.ResetKernelTime();
	
	// main kernel
	const mlo_kernel_info &bwd_wrw = bwd_wrw_info[0];
	float padding_val = 0;

	handle.GetKernel("mlopenConvolutionBwdWeightsAlgoDirect_Main",
			network_config,
			std::get<1>(bwd_wrw),
			std::get<0>(bwd_wrw),
			std::get<4>(bwd_wrw),
			std::get<3>(bwd_wrw),
			std::get<2>(bwd_wrw)) (dy, x, workSpace, padding_val);

	float time0 = handle.GetKernelTime(); 
	
	// second kernel hash
	network_config += "x1";
	// reduction  kernel
	const mlo_kernel_info &bwd_wrw = bwd_wrw_info[1];

	handle.GetKernel("mlopenConvolutionBwdWeightsAlgoDirect_Red",
			network_config,
			std::get<1>(bwd_wrw),
			std::get<0>(bwd_wrw),
			std::get<4>(bwd_wrw),
			std::get<3>(bwd_wrw),
			std::get<2>(bwd_wrw)) (workSpace, dw);

	handle.AccumKernelTime(time0);
}

// BackwardWeightsAlgorithm()
void ConvolutionDescriptor::ConvolutionBackwardWeights(Handle& handle,
		const void						* /*alpha*/,
		const TensorDescriptor&			dyDesc,
		const cl_mem					dy,
		const TensorDescriptor&			xDesc,
		const cl_mem					x,
		mlopenConvBwdWeightsAlgorithm_t	algo,
		const void						* /*beta*/,
		const TensorDescriptor&			dwDesc,
		cl_mem							dw, 
		cl_mem							workSpace,
		size_t							/*workSpaceSize*/) const {

	if(x == nullptr || dw == nullptr || dy == nullptr) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(dyDesc.GetSize() != dwDesc.GetSize() || dyDesc.GetSize() != xDesc.GetSize()) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(dyDesc.GetType() != dwDesc.GetType() || dyDesc.GetType() != xDesc.GetType()) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(dyDesc.GetLengths()[0] != xDesc.GetLengths()[0]) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}
	if(dyDesc.GetSize() < 3) {
		MLOPEN_THROW(mlopenStatusBadParm);
	}

	switch (algo)
	{
		case mlopenConvolutionBwdWeightsAlgoGEMM:
		{
			int in_n, in_c, in_h, in_w;
			std::tie(in_n, in_c, in_h, in_w) = tie4(xDesc.GetLengths());

			int wei_n, wei_h, wei_w;
			std::tie(wei_n, std::ignore, wei_h, wei_w) = tie4(dwDesc.GetLengths());

			int out_h, out_w;
			std::tie(std::ignore, std::ignore, out_h, out_w) = tie4(dyDesc.GetLengths());

			int M = in_c * wei_h * wei_w;
			int N = wei_n;
			int K = out_h * out_w;
			float alpha = 1.0;
			float beta = 1.0;
			bool tA = true;
			bool tB = false;
			bool tC = false;
			unsigned int lda = K;
			unsigned int ldb = K;
			unsigned int ldc = M;

			tinygemm::TinyGemmGeometry tgg( true, tA, tB, tC, lda, ldb, ldc, M, N, K, 0, 0, 0);
			std::string network_config = tgg.get_networkconfig_string();

			std::string algorithm_name;
			algorithm_name = "mlopenConvolutionBwdWeightsAlgoGEMM";
			for(int i = 0; i < in_n; i++) {
				unsigned int out_offset = i * wei_n * out_h * out_w;
				if(wei_h != 1 && wei_w != 1) {
					size_t in_offset = i * in_c * in_h * in_w;
					Im2ColGPU(handle, x, in_offset, in_c, in_h, in_w, wei_h, wei_w, out_h, out_w, pad_h, pad_w, v, u, workSpace);

					handle.GetKernel(algorithm_name, network_config)(dw, workSpace, dy, alpha, beta, lda, ldb, ldc, M, N, K, 0, out_offset, 0);
				}
				else if(wei_h == 1 && wei_w == 1) {
					unsigned int in_offset = i * in_c * in_h * in_w;
					handle.GetKernel(algorithm_name, network_config)(dw, workSpace, dy, alpha, beta, lda, ldb, ldc, M, N, K, in_offset, out_offset, 0);
				}
			}
		}
		break;

		case mlopenConvolutionBwdWeightsAlgoDirect:
		{
			mlo_construct_BwdWrW2D construct_params(0); // backward with regards to weights
			construct_params.setOutputDescFromMLDesc(dyDesc);
			construct_params.setInputDescFromMLDesc(xDesc);
			construct_params.setWeightDescFromMLDesc(dwDesc);

			std::string network_config;
			construct_params.mloBuildConf_Key(network_config);

			handle.ResetKernelTime();

			// main kernel
			float padding_val = 0;
			handle.GetKernel("mlopenConvolutionBwdWeightsAlgoDirect_Main", network_config) (dy, x, workSpace, padding_val);
			float time0 = handle.GetKernelTime();
			
			// second kernel hash
			network_config += "x1";
			
			// reduction  kernel
			handle.GetKernel("mlopenConvolutionBwdWeightsAlgoDirect_Red", network_config) (workSpace, dw);
			handle.AccumKernelTime(time0);

		}
		break;
	};
}

}  // namespace mlopen
