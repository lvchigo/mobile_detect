//#pragma once
//#include <cuda_runtime.h>
//#include <google/protobuf/text_format.h>
#include <queue>  // for std::priority_queue
#include <utility>  // for pair

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <time.h>
#include <sys/mman.h> /* for mmap and munmap */
#include <sys/types.h> /* for open */
#include <sys/stat.h> /* for open */
#include <fcntl.h>     /* for open */
#include <pthread.h>

#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include "API_mutilabel/API_caffe_pvanet.h"


using namespace caffe;  // NOLINT(build/namespaces)
using namespace std;

struct NmsInfo
{
	int 	label;
    float 	score;
    float 	bbox[4];	//output bbox-bias
};

struct IoUInfo {
	int 	index;
	int 	label;		
	float 	value;
	float 	area;
	float 	x1;
	float 	x2;
	float 	y1;
	float 	y2;
};

static bool SortComp(const NmsInfo& input1, const NmsInfo& input2)
{
    return (input1.score > input2.score);
}

static bool Sort_FasterRCNNInfo(const FasterRCNNInfo_MULTILABEL& input1, const FasterRCNNInfo_MULTILABEL& input2)
{
    return (input1.score > input2.score);
}

static bool rindcom(const IoUInfo& t1, const IoUInfo& t2) {
	return t1.value < t2.value;
}

/***********************************Init*************************************/
/// construct function 
API_CAFFE_FasterRCNN_MULTILABEL::API_CAFFE_FasterRCNN_MULTILABEL()
{
}

/// destruct function 
API_CAFFE_FasterRCNN_MULTILABEL::~API_CAFFE_FasterRCNN_MULTILABEL(void)
{
}

/***********************************Init*************************************/
int API_CAFFE_FasterRCNN_MULTILABEL::Init( 
		const char* DL_DeployFile,							//[In]:DL_DeployFile
		const char* DL_ModelFile,							//[In]:DL_ModelFile
		const char* layerName,								//[In]:layerName:"fc7"
		const int binGPU,									//[In]:USE GPU(1) or not(0)
		const int deviceID )								//[In]:GPU ID
{
	int i, device_id;
	string strLayerName = layerName;
	nRet = 0;

	if ( (!DL_DeployFile) || (!DL_ModelFile) )
		return -1;
		
	/***********************************Init GPU**********************************/
	if ( 1 == binGPU ) 
	{
		if ( deviceID != 0 ) 
			device_id = deviceID;
		else
			device_id = 0;
		Caffe::SetDevice(device_id);
		Caffe::set_mode(Caffe::GPU);
		cout << "Using GPU:" << device_id << "!!" << endl;
	} 
	else 
	{
		Caffe::set_mode(Caffe::CPU);
		cout << "Using CPU!!" << endl;
	}

	/***********************************Init**********************************/
	net_dl.reset(new caffe::Net<float>(DL_DeployFile, caffe::TEST));
	net_dl->CopyTrainedLayersFrom( DL_ModelFile );			// load model
	if ( !net_dl->has_blob( strLayerName ) ) {
		//LOOGE<<"[Unknown label name:%s in the network!!]";
		cout << "Unknown label name:%s in the network!!" << endl;
		return -1;
	}

	return nRet;
}

/***********************************Init*************************************/
int API_CAFFE_FasterRCNN_MULTILABEL::Init( 
		const char* DL_CompressFile,						//[In]:DL_DeployFile
		const char* layerName,								//[In]:layerName:"fc7"
		const int type,										//[In]:type:1-Uint8,0-Dtype;
		const int binGPU,									//[In]:USE GPU(1) or not(0)
		const int deviceID )								//[In]:GPU ID
{
	int i, device_id;
	string strLayerName = layerName;
	nRet = 0;

	if (!DL_CompressFile)
		return -1;
		
	/***********************************Init GPU**********************************/
	if ( 1 == binGPU ) 
	{
		if ( deviceID != 0 ) 
			device_id = deviceID;
		else
			device_id = 0;
		Caffe::SetDevice(device_id);
		Caffe::set_mode(Caffe::GPU);
		cout << "Using GPU:" << device_id << "!!" << endl;
	} 
	else 
	{
		Caffe::set_mode(Caffe::CPU);
		cout << "Using CPU!!" << endl;
	}

	/***********************************Init**********************************/
	printf("load compress_model:%s\n",DL_CompressFile);
	net_dl.reset( new caffe::Net<float>(DL_CompressFile, type) );
 	printf("load compress_model end!!\n");
	
	if ( !net_dl->has_blob( strLayerName ) ) {
		//LOOGE<<"[Unknown label name:%s in the network!!]";
		cout << "Unknown label name:%s in the network!!" << endl;
		return -1;
	}

	return nRet;
}

/***********************************Init*************************************/
int API_CAFFE_FasterRCNN_MULTILABEL::init_release( 
		const char* DL_DeployFile,							//[In]:DL_DeployFile
		const char* DL_ModelFile,							//[In]:DL_ModelFile
		const char* layerName,								//[In]:layerName:"fc7"
		const int binGPU,									//[In]:USE GPU(1) or not(0)
		const int deviceID )								//[In]:GPU ID
{
	int i, device_id;
	string strLayerName = layerName;
	nRet = 0;

	if ( (!DL_DeployFile) || (!DL_ModelFile) )
		return -1;
		
	/***********************************Init GPU**********************************/
	if ( 1 == binGPU ) 
	{
		if ( deviceID != 0 ) 
			device_id = deviceID;
		else
			device_id = 0;
		Caffe::SetDevice(device_id);
		Caffe::set_mode(Caffe::GPU);
		cout << "Using GPU:" << device_id << "!!" << endl;
	} 
	else 
	{
		Caffe::set_mode(Caffe::CPU);
		cout << "Using CPU!!" << endl;
	}

	/***********************************Init**********************************/
	std::cout<<"reset 1:"<<std::endl;
	net_dl_2.reset(new caffe::Net<float>(DL_DeployFile, caffe::TEST));
	net_dl_2->CopyTrainedLayersFrom( DL_ModelFile );			// load model
	if ( !net_dl_2->has_blob( strLayerName ) ) {
		cout << "Unknown label name:%s in the network!!" << endl;
		return -1;
	}
    std::cout<<"init net_dl_2 end!!"<<std::endl;

	/***********************************data copy**********************************/
    std::cout<<"reset 2:"<<std::endl;
    net_dl.reset(new caffe::Net<float>());
	//net_dl->CopyFrom(net_dl_2);
	net_dl->CopyFrom_detect(net_dl_2);
	std::cout<<"init _net_dl end!!"<<std::endl;

	return nRet;
}

/***********************************Predict**********************************/
int API_CAFFE_FasterRCNN_MULTILABEL::Predict(
		const unsigned char							*image, 			//[In]:image
		const int 									width, 				//[In]:width
		const int 									height, 			//[In]:height
		const int 									nChannel,			//[In]:nChannel
		const float									Min_T,				//[In]:Min_T
		vector<FasterRCNNInfo_MULTILABEL> 			&vecResInfo)		//[Out]:Res
{
	if(!image || (width<16) || (height<16) || (nChannel<1) ) 
	{	
		//LOOGE<<"[Predict]:input err!!";
		cout << "[Predict]:input err!!" << endl;
		return 201;
	}

	//init
	float tmpScore;
	float MaxLabel_THRESH = 0.5;
    float NMS_THRESH = 0.3;			//[0.2,0.3,0.4]
	float CONF_THRESH = Min_T;		//0.8
	float iter_loss = 0.0;

	float im_info[4] = {0};
    int i,j,k,w,h,DataSize,tmp;
	nRet = 0;

	vector<float> inFeat;

	vector< NmsInfo > vecNMSInfo;
	vector< RectInfo_MULTILABEL > vecRectInfo;
	vector<FasterRCNNInfo_MULTILABEL> vecRectRegressionInfo;
	vector<FasterRCNNInfo_MULTILABEL> vecBBoxNMSInput;
	vector<FasterRCNNInfo_MULTILABEL> vecBBoxNMSOutput;
	vecResInfo.clear();

	/*****************************Change Data****************************/
	printf("API_CAFFE_FasterRCNN_MULTILABEL::Predict Change Data\n");
	DataSize = height*width*3;
	float* data_buf = new float[DataSize];
    for ( h = 0; h < height; ++h) {
        for ( w = 0; w < width; ++w) {
			data_buf[(         h)*width+w] = (float)(image[h*width*nChannel+w*nChannel]) - float(102.9801);
            data_buf[(1*height+h)*width+w] = (float)(image[h*width*nChannel+w*nChannel+1])-float(115.9465);
            data_buf[(2*height+h)*width+w] = (float)(image[h*width*nChannel+w*nChannel+2])-float(122.7717);
        }
    }
    im_info[0] = height;
    im_info[1] = width;
    im_info[2] = 1.0;	//scale-height
    im_info[3] = 1.0;	//scale-width
    printf("im_info:%.2f,%.2f\n",im_info[0],im_info[1]);

	std::vector<Blob<float>*> input_blobs; 
	input_blobs = net_dl->input_blobs();
	printf("input_blobs.size:%ld\n",input_blobs.size());
	if (input_blobs.size()<1 )
	{
		printf("input_blobs err!!\n");
		if(data_buf != NULL){ delete[] data_buf; data_buf = NULL;}
		return 202;
	}

	//copy data
	printf("API_CAFFE_FasterRCNN_MULTILABEL::Predict copy data\n");
	if ( input_blobs[0]->count() != DataSize )
	{
		input_blobs[0]->Reshape(1, 3, height, width);
	}
	caffe_copy(input_blobs[0]->count(), data_buf, input_blobs[0]->mutable_cpu_data());
	caffe_copy(input_blobs[1]->count(), im_info, input_blobs[1]->mutable_cpu_data());

	/*****************************Forward Model****************************/
	printf("API_CAFFE_FasterRCNN_MULTILABEL::Predict Forward\n");
	std::vector<Blob<float>*> output_blobs;
	try
	{
		output_blobs = net_dl->Forward(input_blobs,&iter_loss);
		if (output_blobs.size()<1)
		{
			//LOOGE<<"[net_dl->ForwardPrefilled() err~~!!]";
			cout << "[net_dl->ForwardPrefilled() err~~!!]" << endl;
			throw 1;
		}
	}
	catch(...)
	{
		if(data_buf != NULL){ delete[] data_buf; data_buf = NULL;}
		return 203;
	}

	//delete
	if(data_buf != NULL){ delete[] data_buf; data_buf = NULL;}

	/*****************************Get Output Data*****************************/
	printf("API_CAFFE_FasterRCNN_MULTILABEL::Predict Get Output Data\n");
	caffe::shared_ptr<caffe::Blob<float> > cls_blob;
	cls_blob = net_dl->blob_by_name("cls_prob");

	caffe::shared_ptr<caffe::Blob<float> > bbox_blob;
	bbox_blob = net_dl->blob_by_name("bbox_pred");

	caffe::shared_ptr<caffe::Blob<float> > roi_blob;
	roi_blob = net_dl->blob_by_name("rois");

	int cls_batch_size = cls_blob->num();
	int bbox_batch_size = bbox_blob->num();
	int roi_batch_size = roi_blob->num();
	if( ( cls_batch_size<1 )||( bbox_batch_size<1 )||( roi_batch_size<1 )||
		( cls_batch_size!=bbox_batch_size )||( cls_batch_size!=roi_batch_size ) )
	{
		printf("[Predict]:cls_batch_size = %d, bbox_batch_size = %d, roi_batch_size = %d, err!!\n", 
				cls_batch_size, bbox_batch_size, roi_batch_size );	
		return 204;
	}
	
	int cls_dim_features = cls_blob->count() / cls_batch_size;
	int bbox_dim_features = bbox_blob->count() / bbox_batch_size;
	int roi_dim_features = roi_blob->count() / roi_batch_size;
	if( ( cls_dim_features<1 )||( bbox_dim_features<1 ) )
	{
		printf("[Predict]:cls_dim_features = %d, bbox_dim_features = %d, roi_dim_features = %d, err!!\n", 
				cls_dim_features, bbox_dim_features,roi_dim_features );	
		return 205;
	}

	printf("[Predict]:cls_batch_size = %d, cls_dim_features = %d!!\n", cls_batch_size, cls_dim_features);
	printf("[Predict]:bbox_batch_size = %d, bbox_dim_features = %d!!\n", bbox_batch_size, bbox_dim_features);
	printf("[Predict]:roi_batch_size = %d, roi_dim_features = %d!!\n", roi_batch_size, roi_dim_features);

	/*****************************Get Label && Score && Rect && Bias*****************************/
	const float* cls_blob_data;
	const float* bbox_blob_data;
	const float* roi_blob_data;
	
	vecRectInfo.clear();
	for (i = 0; i < cls_batch_size; ++i) {
		cls_blob_data = cls_blob->cpu_data() + cls_blob->offset(i);
		bbox_blob_data = bbox_blob->cpu_data() + bbox_blob->offset(i);
		roi_blob_data = roi_blob->cpu_data() + roi_blob->offset(i);

		vecNMSInfo.clear();
		tmpScore = 0;
		for (j = 1; j < cls_dim_features; ++j) {
		  	tmpScore = cls_blob_data[j];

			//Get Max Label
			if ( tmpScore<MaxLabel_THRESH )
				continue;

			//add data
			NmsInfo nmsInfo;
			nmsInfo.label = j-1;	//remove background
		  	nmsInfo.score = tmpScore;
			for(k=0;k<4;++k)
				nmsInfo.bbox[k] = bbox_blob_data[4+k];	//cfg.TEST.AGONISTIC
				//nmsInfo.bbox[k] = bbox_blob_data[k];	//cfg.TEST.normal
				//nmsInfo.bbox[k] = bbox_blob_data[k+j*4];
			vecNMSInfo.push_back( nmsInfo );
		}
		
		if (vecNMSInfo.size()<1)
			continue;

		//sort
		sort(vecNMSInfo.begin(), vecNMSInfo.end(),SortComp);	

		//T<CONF_THRESH
		if (vecNMSInfo[0].score<CONF_THRESH)
			continue;

		//add data
		RectInfo_MULTILABEL rectInfo;
		rectInfo.feat.clear();
		rectInfo.label = vecNMSInfo[0].label;
	  	rectInfo.score = vecNMSInfo[0].score;
		for(k=0;k<4;++k)
			rectInfo.bbox[k] = vecNMSInfo[0].bbox[k];
		for(k=0;k<4;++k)
			rectInfo.rect[k] = roi_blob_data[1+k];

		//if (i<3)
		//	printf("roi_feat:%.2f_%.2f_%.2f!!\n",rectInfo.feat[0],rectInfo.feat[1],rectInfo.feat[2]);

		//push nms data
		vecRectInfo.push_back( rectInfo );
	}

	//judge
	if (vecRectInfo.size()<1)
	{
		vecResInfo.clear();
		FasterRCNNInfo_MULTILABEL info;
		info.label = 51;	//other,11-6,12-7
		info.score = 0;
		info.rect[0] = 0;
		info.rect[1] = 0;
		info.rect[2] = width-1;
		info.rect[3] = height-1;
		info.feat.clear();
		vecResInfo.push_back( info );
		
		//LOOGE<<"[Predict]:vecRectInfo.size()<1!!";
		return 0;
	}

	/*****************************BBox Regression*****************************/
	vecRectRegressionInfo.clear();
	nRet = bbox_regression( vecRectInfo, width, height, vecRectRegressionInfo );
	if ( (nRet!=0) || (vecRectRegressionInfo.size()<1) )
	{
		//LOOGE<<"[Predict]:bbox_regression Err!!";
		cout << "[Predict]:bbox_regression Err!!" << endl;
		return 207;
	}

	/*****************************BBox NMS*****************************/
	vector<int> suppress;
	vecResInfo.clear();
	for(i=0;i<cls_dim_features-1;i++)	//class-label
	{
		//input data
		vecBBoxNMSInput.clear();
		suppress.clear();
		for(j=0;j<vecRectRegressionInfo.size();j++)
		{
			if( vecRectRegressionInfo[j].label == i )
			{
				vecBBoxNMSInput.push_back( vecRectRegressionInfo[j] );
				suppress.push_back(j);	//index
			}
		}
		
		if ( vecBBoxNMSInput.size()<1 )
			continue;
		else if ( vecBBoxNMSInput.size()==1 )
		{
			//send data
			vecResInfo.push_back( vecBBoxNMSInput[0] );
		}
		else
		{
			//bbox_NMS
			vecBBoxNMSOutput.clear();
			nRet = bbox_NMS( vecBBoxNMSInput, vecBBoxNMSOutput, NMS_THRESH );
			if ( (nRet!=0) || (vecBBoxNMSOutput.size()<1) )
			{
				//LOOGE<<"[Predict]:bbox_NMS Err!!";
				cout << "[Predict]:bbox_NMS Err!!" << endl;
				return -1;
			}

			//send data
			for (j=0;j<vecBBoxNMSOutput.size();j++)
				vecResInfo.push_back( vecBBoxNMSOutput[j] );
		}

		//remove rect
		for (j=suppress.size()-1;j>=0;j--)
		{
			vecRectRegressionInfo.erase(vecRectRegressionInfo.begin()+suppress[j]);
		}
	}

	return nRet;
}

/***********************************Predict**********************************/
int API_CAFFE_FasterRCNN_MULTILABEL::Predict_release(
		const unsigned char							*image, 			//[In]:image
		const int 									width, 				//[In]:width
		const int 									height, 			//[In]:height
		const int 									nChannel,			//[In]:nChannel
		const float									Min_T,				//[In]:Min_T
		vector<FasterRCNNInfo_MULTILABEL> 			&vecResInfo)		//[Out]:Res
{
	if(!image || (width<16) || (height<16) || (nChannel<1) ) 
	{	
		//LOOGE<<"[Predict]:input err!!";
		cout << "[Predict]:input err!!" << endl;
		return 201;
	}

	//init
	float tmpScore;
	float MaxLabel_THRESH = 0.5;
    float NMS_THRESH = 0.3;			//[0.2,0.3,0.4]
	float CONF_THRESH = Min_T;		//0.8
	float iter_loss = 0.0;

	float im_info[4] = {0};
    int i,j,k,w,h,tmp,DataSize,tIndex;
	nRet = 0;

	vector<float> inFeat;

	vector< NmsInfo > vecNMSInfo;
	vector< RectInfo_MULTILABEL > vecRectInfo;
	vector<FasterRCNNInfo_MULTILABEL> vecRectRegressionInfo;
	vector<FasterRCNNInfo_MULTILABEL> vecBBoxNMSInput;
	vector<FasterRCNNInfo_MULTILABEL> vecBBoxNMSOutput;
	vecResInfo.clear();

	/*****************************Change Data****************************/
	printf("API_CAFFE_FasterRCNN_MULTILABEL::Predict Change Data\n");
	DataSize = height*width*3;
	float* data_buf = new float[DataSize];
    for ( h = 0; h < height; ++h) {
        for ( w = 0; w < width; ++w) {
			data_buf[(         h)*width+w] = (float)(image[h*width*nChannel+w*nChannel]) - float(102.9801);
            data_buf[(1*height+h)*width+w] = (float)(image[h*width*nChannel+w*nChannel+1])-float(115.9465);
            data_buf[(2*height+h)*width+w] = (float)(image[h*width*nChannel+w*nChannel+2])-float(122.7717);
        }
    }
    im_info[0] = height;
    im_info[1] = width;
    im_info[2] = 1.0;	//scale-height
    im_info[3] = 1.0;	//scale-width
    printf("im_info:%.2f,%.2f\n",im_info[0],im_info[1]);

	std::vector<Blob<float>*> input_blobs; 
	input_blobs = net_dl->input_blobs();
	printf("input_blobs.size:%ld\n",input_blobs.size());
	if (input_blobs.size()<1 )
	{
		printf("input_blobs err!!\n");
		if(data_buf != NULL){ delete[] data_buf; data_buf = NULL;}
		return 202;
	}

	//copy data
	printf("API_CAFFE_FasterRCNN_MULTILABEL::Predict copy data\n");
	printf("API_CAFFE_FasterRCNN_MULTILABEL::input_blobs[0]->count():%d,DataSize:%d\n",
		input_blobs[0]->count(), DataSize );
	printf("API_CAFFE_FasterRCNN_MULTILABEL::input_blobs[1]->count():%d\n", input_blobs[1]->count() );
	if ( input_blobs[0]->count() != DataSize )
	{
		input_blobs[0]->Reshape(1, 3, height, width);
	}
	if ( input_blobs[1]->count() != 4 )
	{
		//input_blobs[1]->Reshape(1, 4, 1, 1);
		cout << "input_blobs[1]->count() != 4" << endl;
		return 202;
	}
	printf("API_CAFFE_FasterRCNN_MULTILABEL::input_blobs[0]->count():%d,DataSize:%d\n",
		input_blobs[0]->count(), DataSize );
	printf("API_CAFFE_FasterRCNN_MULTILABEL::input_blobs[1]->count():%d\n", input_blobs[1]->count() );
	caffe_copy(input_blobs[0]->count(), data_buf, input_blobs[0]->mutable_cpu_data());
	printf("API_CAFFE_FasterRCNN_MULTILABEL::caffe_copy data_buf\n");
	caffe_copy(input_blobs[1]->count(), im_info, input_blobs[1]->mutable_cpu_data());
	printf("API_CAFFE_FasterRCNN_MULTILABEL::caffe_copy im_info\n");

	/*****************************Forward Model****************************/
	printf("API_CAFFE_FasterRCNN_MULTILABEL::Predict Forward\n");
	std::vector<Blob<float>*> output_blobs;
	try
	{
		output_blobs = net_dl->Forward(input_blobs,&iter_loss);
		if (output_blobs.size()<1)
		{
			//LOOGE<<"[net_dl->ForwardPrefilled() err~~!!]";
			cout << "[net_dl->ForwardPrefilled() err~~!!]" << endl;
			throw 1;
		}
	}
	catch(...)
	{
		if(data_buf != NULL){ delete[] data_buf; data_buf = NULL;}
		return 203;
	}

	//delete
	if(data_buf != NULL){ delete[] data_buf; data_buf = NULL;}

	/*****************************Get Output Data*****************************/
	printf("API_CAFFE_FasterRCNN_MULTILABEL::Predict Get Output Data\n");

	vector<float> vec_cls_blob;	//top: "cls_prob",Softmax
	for ( k=0; k < output_blobs[0]->count(); ++k )
		vec_cls_blob.push_back( output_blobs[0]->cpu_data()[k] );
	printf("vec_cls_blob:0_%.4f,1_%.4f,2_%.4f\n",vec_cls_blob[0],vec_cls_blob[1],vec_cls_blob[2]);

	vector<float> vec_bbox_blob;	//top: "bbox_pred",
	for ( k=0; k < output_blobs[1]->count(); ++k )
		vec_bbox_blob.push_back( output_blobs[1]->cpu_data()[k] );
	printf("vec_bbox_blob:0_%.4f,1_%.4f,2_%.4f\n",vec_bbox_blob[0],vec_bbox_blob[1],vec_bbox_blob[2]);

	vector<float> vec_roi_blob;	//top: "cls_score",ProposalLayer
	for ( k=0; k < output_blobs[2]->count(); ++k )
		vec_roi_blob.push_back( output_blobs[2]->cpu_data()[k] );
	printf("vec_roi_blob:0_%.4f,1_%.4f,2_%.4f\n",vec_roi_blob[0],vec_roi_blob[1],vec_roi_blob[2]);

	int cls_dim = vec_cls_blob.size();
	int bbox_dim = vec_bbox_blob.size();
	int roi_dim = vec_roi_blob.size();
	if( ( cls_dim<1 )||( bbox_dim<1 )||( roi_dim<1 )||
		( cls_dim%PATCH_SIZE!=0 )||( bbox_dim%PATCH_SIZE!=0 )||( roi_dim%PATCH_SIZE!=0 ) )
	{
		printf("[Predict]:cls_dim = %d, bbox_dim = %d, roi_dim = %d, err!!\n", cls_dim, bbox_dim, roi_dim );	
		return 204;
	}
	
	int cls_dim_features = cls_dim / PATCH_SIZE;
	int bbox_dim_features = bbox_dim / PATCH_SIZE;
	int roi_dim_features = roi_dim / PATCH_SIZE;
	if( ( cls_dim_features<1 )||( bbox_dim_features<1 ) )
	{
		printf("[Predict]:cls_dim_features = %d, bbox_dim_features = %d, roi_dim_features = %d, err!!\n", 
				cls_dim_features, bbox_dim_features,roi_dim_features );	
		return 205;
	}

	printf("[Predict]:cls_dim = %d, cls_dim_features = %d!!\n", cls_dim, cls_dim_features);
	printf("[Predict]:bbox_dim = %d, bbox_dim_features = %d!!\n", bbox_dim, bbox_dim_features);
	printf("[Predict]:roi_dim = %d, roi_dim_features = %d!!\n", roi_dim, roi_dim_features);

	/*****************************Get Label && Score && Rect && Bias*****************************/
	vecRectInfo.clear();
	for (i = 0; i < PATCH_SIZE; ++i) {
		vecNMSInfo.clear();
		tmpScore = 0;
		tIndex = i*cls_dim_features;
		for (j = 1; j < cls_dim_features; ++j) {
		  	tmpScore = vec_cls_blob[j+i*cls_dim_features];

			//Get Max Label
			if ( tmpScore<MaxLabel_THRESH )
				continue;

			//add data
			NmsInfo nmsInfo;
			nmsInfo.label = j-1;	//remove background
		  	nmsInfo.score = tmpScore;
			for(k=0;k<4;++k)
				nmsInfo.bbox[k] = vec_bbox_blob[4+k+i*cls_dim_features*4];	//cfg.TEST.AGONISTIC
				//nmsInfo.bbox[k] = bbox_blob_data[k];	//cfg.TEST.normal
				//nmsInfo.bbox[k] = bbox_blob_data[k+j*4];
			vecNMSInfo.push_back( nmsInfo );
		}
		
		if (vecNMSInfo.size()<1)
			continue;

		//sort
		sort(vecNMSInfo.begin(), vecNMSInfo.end(),SortComp);	

		//T<CONF_THRESH
		if (vecNMSInfo[0].score<CONF_THRESH)
			continue;

		//add data
		RectInfo_MULTILABEL rectInfo;
		rectInfo.feat.clear();
		rectInfo.label = vecNMSInfo[0].label;
	  	rectInfo.score = vecNMSInfo[0].score;
		for(k=0;k<4;++k)
			rectInfo.bbox[k] = vecNMSInfo[0].bbox[k];
		for(k=0;k<4;++k)
			rectInfo.rect[k] = vec_roi_blob[1+k+i*5];

		//if (i<3)
		//	printf("roi_feat:%.2f_%.2f_%.2f!!\n",rectInfo.feat[0],rectInfo.feat[1],rectInfo.feat[2]);

		//push nms data
		vecRectInfo.push_back( rectInfo );
	}

	//judge
	if (vecRectInfo.size()<1)
	{
		vecResInfo.clear();
		FasterRCNNInfo_MULTILABEL info;
		info.label = 51;	//other,11-6,12-7
		info.score = 0;
		info.rect[0] = 0;
		info.rect[1] = 0;
		info.rect[2] = width-1;
		info.rect[3] = height-1;
		info.feat.clear();
		vecResInfo.push_back( info );
		
		//LOOGE<<"[Predict]:vecRectInfo.size()<1!!";
		return 0;
	}

	/*****************************BBox Regression*****************************/
	vecRectRegressionInfo.clear();
	nRet = bbox_regression( vecRectInfo, width, height, vecRectRegressionInfo );
	if ( (nRet!=0) || (vecRectRegressionInfo.size()<1) )
	{
		//LOOGE<<"[Predict]:bbox_regression Err!!";
		cout << "[Predict]:bbox_regression Err!!" << endl;
		return 207;
	}

	/*****************************BBox NMS*****************************/
	vector<int> suppress;
	vecResInfo.clear();
	for(i=0;i<cls_dim_features-1;i++)	//class-label
	{
		//input data
		vecBBoxNMSInput.clear();
		suppress.clear();
		for(j=0;j<vecRectRegressionInfo.size();j++)
		{
			if( vecRectRegressionInfo[j].label == i )
			{
				vecBBoxNMSInput.push_back( vecRectRegressionInfo[j] );
				suppress.push_back(j);	//index
			}
		}
		
		if ( vecBBoxNMSInput.size()<1 )
			continue;
		else if ( vecBBoxNMSInput.size()==1 )
		{
			//send data
			vecResInfo.push_back( vecBBoxNMSInput[0] );
		}
		else
		{
			//bbox_NMS
			vecBBoxNMSOutput.clear();
			nRet = bbox_NMS( vecBBoxNMSInput, vecBBoxNMSOutput, NMS_THRESH );
			if ( (nRet!=0) || (vecBBoxNMSOutput.size()<1) )
			{
				//LOOGE<<"[Predict]:bbox_NMS Err!!";
				cout << "[Predict]:bbox_NMS Err!!" << endl;
				return -1;
			}

			//send data
			for (j=0;j<vecBBoxNMSOutput.size();j++)
				vecResInfo.push_back( vecBBoxNMSOutput[j] );
		}

		//remove rect
		for (j=suppress.size()-1;j>=0;j--)
		{
			vecRectRegressionInfo.erase(vecRectRegressionInfo.begin()+suppress[j]);
		}
	}

	return nRet;
}


//see python-faster-rcnn:bbox_transform_inv
int API_CAFFE_FasterRCNN_MULTILABEL::bbox_regression( 
	vector<RectInfo_MULTILABEL> 			vecRectInfo, 
	int 						width, 
	int 						height, 
	vector<FasterRCNNInfo_MULTILABEL> 		&vecResInfo )
{
	if( ( vecRectInfo.size()<1 ) || (width<32) || (height<32) )
    {
        //LOOGE<<"[bbox_regression]:input err!!";
		cout << "[bbox_regression]:input err!!" << endl;
		return -1;
    }

	nRet = 0;
	int i,k,x1,x2,y1,y2;
	float w, h, ctr_x, ctr_y, dx, dy, dw, dh, pred_ctr_x, pred_ctr_y, pred_w, pred_h;

	vecResInfo.clear();
	for( i=0; i< vecRectInfo.size(); i++)
    {
        w = vecRectInfo[i].rect[2] - vecRectInfo[i].rect[0] + 1.0;
        h = vecRectInfo[i].rect[3] - vecRectInfo[i].rect[1] + 1.0;
        ctr_x = vecRectInfo[i].rect[0] + 0.5 * w;
        ctr_y = vecRectInfo[i].rect[1] + 0.5 * h;

        dx = vecRectInfo[i].bbox[0];
        dy = vecRectInfo[i].bbox[1];
        dw = vecRectInfo[i].bbox[2];
        dh = vecRectInfo[i].bbox[3];
        pred_ctr_x = ctr_x + w*dx;
        pred_ctr_y = ctr_y + h*dy;
        pred_w = w * exp(dw);
        pred_h = h * exp(dh);

        x1 = int(std::max(std::min((pred_ctr_x - 0.5* pred_w), (width-1)* 1.0), 0.0));
        y1 = int(std::max(std::min((pred_ctr_y - 0.5* pred_h), (height-1)*1.0), 0.0));
        x2 = int(std::max(std::min((pred_ctr_x + 0.5* pred_w), (width-1)* 1.0), 0.0));
        y2 = int(std::max(std::min((pred_ctr_y + 0.5* pred_h), (height-1)*1.0), 0.0));

		if ( (x1>=x2) || (y1>=y2) )
			continue;
		
		FasterRCNNInfo_MULTILABEL fasterRCNNInfo;
		fasterRCNNInfo.label = vecRectInfo[i].label;
		fasterRCNNInfo.score= vecRectInfo[i].score;
		fasterRCNNInfo.rect[0] = x1;
		fasterRCNNInfo.rect[1] = y1;
		fasterRCNNInfo.rect[2] = x2;
		fasterRCNNInfo.rect[3] = y2;
		std::copy(vecRectInfo[i].feat.begin(),vecRectInfo[i].feat.end(), std::back_inserter(fasterRCNNInfo.feat)); 

		vecResInfo.push_back(fasterRCNNInfo);
    }

	if (vecResInfo.size()<1)
	{
		//LOOGE<<"[bbox_regression]:vecResInfo.size()<1!!";
		cout << "[bbox_regression]:vecResInfo.size()<1!!" << endl;
		return -1;
	}

	return nRet;
}

int API_CAFFE_FasterRCNN_MULTILABEL::bbox_NMS(vector<FasterRCNNInfo_MULTILABEL> src, vector<FasterRCNNInfo_MULTILABEL> &dst, float overlap) 
{ 
// pick out the most probable boxes
	if (src.empty())
		return -1;
	
	int tmp, i, pos, last, srcnum = src.size();
	float xx1,yy1,xx2,yy2,w,h,tmpArea, o;
	vector<IoUInfo> s(srcnum);
	vector<int> pick;
	vector<int> suppress;
	
	for (i=0;i<srcnum;i++)
	{
		s[i].index=i;
		s[i].label=src[i].label;
		s[i].value=src[i].score;
		s[i].x1=src[i].rect[0];
		s[i].y1=src[i].rect[1];
		s[i].x2=src[i].rect[2];
		s[i].y2=src[i].rect[3];
		s[i].area=(src[i].rect[2]-src[i].rect[0]+1)*(src[i].rect[3]-src[i].rect[1]+1);
	}
	
	if (s.size()<1)
		return -1;
	
	std::sort(s.begin(),s.end(),rindcom);

	tmp = 0;
	pick.clear();
	while (!s.empty())
	{
		suppress.clear();
		last=s.size()-1;
		pick.push_back(s[last].index);
		suppress.push_back(last);
		for (pos=0;pos<last;pos++)
		{          
			xx1=max(s[last].x1,s[pos].x1);
			yy1=max(s[last].y1,s[pos].y1);
			xx2=min(s[last].x2,s[pos].x2);
			yy2=min(s[last].y2,s[pos].y2);
			w=xx2-xx1+1;
			h=yy2-yy1+1;
			tmpArea = w*h;
			if ( (w>0)&&(h>0)&&(s[pos].area+s[last].area>tmpArea) )
			{
				//o = tmpArea*1.0/s[pos].area;	//pos.area
				o = tmpArea*1.0/(s[pos].area+s[last].area-tmpArea);	//pos.area
				if (o>overlap)
				{
					suppress.push_back(pos);
				}
			}
		}
		sort(suppress.begin(),suppress.end());

		//remove iou rect
		for (i=suppress.size()-1;i>=0;i--)
		{
			s.erase(s.begin()+suppress[i]);
		}
	}

	if (!dst.empty())
		dst.clear();

	if (pick.size()<1)
		return -1;

	for (i=0;i<pick.size();i++) {
		dst.push_back(src[pick[i]]);
	}

	if (dst.size()<1)
		return -1;

	std::sort(dst.begin(),dst.end(),Sort_FasterRCNNInfo);

	return 0;
}

/***********************************Release**********************************/
void API_CAFFE_FasterRCNN_MULTILABEL::Release()
{
	if (net_dl) {
    	net_dl.reset();
  	}
}

/***********************************Release**********************************/
void API_CAFFE_FasterRCNN_MULTILABEL::Release_release()
{
	if (net_dl) {
    	net_dl.reset();
  	}

	if (net_dl_2) {
    	net_dl_2.reset();
  	}
}



