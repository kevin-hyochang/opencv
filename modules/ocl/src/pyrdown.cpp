/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2010-2012, Multicoreware, Inc., all rights reserved.
// Copyright (C) 2010-2012, Advanced Micro Devices, Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// @Authors
//		Dachuan Zhao, dachuan@multicorewareinc.com
//		Yao Wang, yao@multicorewareinc.com
//    
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other oclMaterials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
#include "precomp.hpp"

using namespace cv;
using namespace cv::ocl;
using namespace std;

using std::cout;
using std::endl;

namespace cv
{
    namespace ocl
    {
        ///////////////////////////OpenCL kernel strings///////////////////////////
        extern const char *pyr_down;

    }
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////// add subtract multiply divide /////////////////////////
//////////////////////////////////////////////////////////////////////////////
template<typename T>
void pyrdown_run(const oclMat &src, const oclMat &dst)
{

    CV_Assert(src.type() == dst.type());
    CV_Assert(src.depth() != CV_8S);

    Context  *clCxt = src.clCxt;
    //int channels = dst.channels();
    //int depth = dst.depth();

    string kernelName = "pyrDown";

    //int vector_lengths[4][7] = {{4, 0, 4, 4, 1, 1, 1},
    //    {4, 0, 4, 4, 1, 1, 1},
    //    {4, 0, 4, 4, 1, 1, 1},
    //    {4, 0, 4, 4, 1, 1, 1}
    //};

    //size_t vector_length = vector_lengths[channels-1][depth];
    //int offset_cols = (dst.offset / dst.elemSize1()) & (vector_length - 1);

    size_t localThreads[3]  = { 256, 1, 1 };
    size_t globalThreads[3] = { src.cols, dst.rows, 1};

    //int dst_step1 = dst.cols * dst.elemSize();
    vector<pair<size_t , const void *> > args;
    args.push_back( make_pair( sizeof(cl_mem), (void *)&src.data ));
    args.push_back( make_pair( sizeof(cl_int), (void *)&src.step ));
    args.push_back( make_pair( sizeof(cl_int), (void *)&src.offset ));
    args.push_back( make_pair( sizeof(cl_int), (void *)&src.rows));
    args.push_back( make_pair( sizeof(cl_int), (void *)&src.cols));
    args.push_back( make_pair( sizeof(cl_mem), (void *)&dst.data ));
    args.push_back( make_pair( sizeof(cl_int), (void *)&dst.step ));
    args.push_back( make_pair( sizeof(cl_int), (void *)&dst.offset ));
    args.push_back( make_pair( sizeof(cl_int), (void *)&dst.cols));

    openCLExecuteKernel(clCxt, &pyr_down, kernelName, globalThreads, localThreads, args, src.channels(), src.depth());
}
void pyrdown_run(const oclMat &src, const oclMat &dst)
{
	switch(src.depth())
	{
	case 0:
	    pyrdown_run<unsigned char>(src, dst);
		break;

	case 1:
	    pyrdown_run<char>(src, dst);
		break;

	case 2:
	    pyrdown_run<unsigned short>(src, dst);
		break;

	case 3:
	    pyrdown_run<short>(src, dst);
		break;

	case 4:
	    pyrdown_run<int>(src, dst);
		break;

	case 5:
	    pyrdown_run<float>(src, dst);
		break;

	case 6:
	    pyrdown_run<double>(src, dst);
		break;

	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////////
// pyrDown

void cv::ocl::pyrDown(const oclMat& src, oclMat& dst)
{
    CV_Assert(src.depth() <= CV_32F && src.channels() <= 4);

	//src.step = src.rows;

    dst.create((src.rows + 1) / 2, (src.cols + 1) / 2, src.type());

	dst.download_channels = src.download_channels;

    pyrdown_run(src, dst);
}

