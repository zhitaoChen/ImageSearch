#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <process.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;
using namespace cv;

struct k   
{
	string name;
	double result;

	bool operator < (const k &c) const
	{
		return result < c.result;
	}
};
typedef k picture;


int top;
int cmp(picture a,picture b);
void remove(string xsrc,string xdes,string rec,picture x,int rank);
void insert(vector<picture> &data,int p,int top,picture temp,int flag);

int main(void)
{
	vector<picture> data0,data1,data2,data3;
	string add,des,sample,real_address;
	string des0,des1,des2,des3;
	string arrayrec0,arrayrec1,arrayrec2,arrayrec3;

	cout<<"How many pictures do you wanna save?"<<endl;
	cin>>top;

	cout<<"Target:"<<endl;
	cin>>add;//目标图片

	cout<<"Sample:"<<endl;
	cin>>sample;//样本

	cout<<"Extract Address:"<<endl;
	cin>>real_address;//样本对应存放地址

	cout<<"Destination:"<<endl;//存放地址
	cin>>des0;
	cin>>des1;
	cin>>des2;
	cin>>des3;

	cout<<"Your result record:"<<endl;
	cin>>arrayrec0;
	cin>>arrayrec1;
	cin>>arrayrec2;
	cin>>arrayrec3;

	int HistogramBins[3] ={ 256,256,256};
	float HistogramRange1[2]={0,255};
	float HistogramRange2[2]={0,255};
	float HistogramRange3[2]={0,255};
	float *HistogramRange[3]={&HistogramRange1[0],&HistogramRange2[0],&HistogramRange3[0]};

	IplImage* tar=cvLoadImage(add.c_str());
	IplImage *RedImage=cvCreateImage(cvGetSize(tar),8,1);
	IplImage *GreenImage=cvCreateImage(cvGetSize(tar),8,1);
	IplImage *BlueImage=cvCreateImage(cvGetSize(tar),8,1);
	IplImage *ImageArray[3]={RedImage,GreenImage,BlueImage};

	cvSplit(tar,BlueImage,GreenImage,RedImage,0);
	CvHistogram *Histogram1=cvCreateHist(3,HistogramBins,CV_HIST_SPARSE,HistogramRange);
	cvCalcHist(ImageArray,Histogram1);
	cvNormalizeHist(Histogram1,1);

	cout<<"Search..."<<endl;
	string s=sample+"*.jpg";

	long handle;                                              
	struct _finddata_t fileinfo;                          
	handle=_findfirst(s.c_str(),&fileinfo);      
	if(-1== handle)
		return -1;

	int p=0;
	cout<<"Running..."<<endl;
	do 
	{
		string temppic=sample+fileinfo.name;
		//cout<<"Now handling "<<fileinfo.name<<endl;
		IplImage* gettemp=cvLoadImage(temppic.c_str());

		IplImage *tempRedImage=cvCreateImage(cvGetSize(gettemp),8,1);
		IplImage *tempGreenImage=cvCreateImage(cvGetSize(gettemp),8,1);
		IplImage *tempBlueImage=cvCreateImage(cvGetSize(gettemp),8,1);
		IplImage *tempImageArray[3]={tempRedImage,tempGreenImage,tempBlueImage};
		cvSplit(gettemp,tempBlueImage,tempGreenImage,tempRedImage,0);

		CvHistogram *Histogram2=cvCreateHist(3,HistogramBins,CV_HIST_SPARSE,HistogramRange);
		cvCalcHist(tempImageArray,Histogram2);
		cvNormalizeHist(Histogram2,1);

		for(int i=0;i<4;i++)
		{
			int method=i;

			double base_test = cvCompareHist(Histogram1,Histogram2,method);
			picture temp;
			temp.name=fileinfo.name;
			temp.result=base_test;

			switch(method)
			{
			case 0:insert(data0,p,top,temp,1);break;
			case 1:insert(data1,p,top,temp,0);break;
			case 2:insert(data2,p,top,temp,1);break;
			case 3:insert(data3,p,top,temp,0);break;
			}
		}
		p++;

		cvReleaseImage(&gettemp);
		cvReleaseImage(&tempGreenImage);
		cvReleaseImage(&tempBlueImage);
		cvReleaseImage(&tempRedImage);
		cvReleaseHist(&Histogram2);
	} while (!_findnext(handle,&fileinfo));
	_findclose(handle); 

	cout<<"Compute Finish,Moving..."<<endl;
	for(int i=0;i<top;i++)
	{
		remove(real_address,des0,arrayrec0,data0[i],i);
		remove(real_address,des1,arrayrec1,data1[i],i);
		remove(real_address,des2,arrayrec2,data2[i],i);
		remove(real_address,des3,arrayrec3,data3[i],i);
	}
	cout<<"Done"<<endl;

	cvReleaseImage(&tar);
	cvReleaseImage(&RedImage);
	cvReleaseImage(&GreenImage);
	cvReleaseImage(&BlueImage);
	cvReleaseHist(&Histogram1);

	return 0;
}

int cmp(picture a,picture b)
{
	return a.result>b.result;
}

void insert(vector<picture> &data,int p,int top,picture temp,int flag)
{
	if(p<top)
		data.push_back(temp);
	else if(p==top)
	{
		data.push_back(temp);
		if(flag)
			std::sort(data.begin(),data.end(),cmp);
		else
			std::sort(data.begin(),data.end());
	}
	else
	{
		data[top]=temp;
		if(flag)
			std::sort(data.begin(),data.end(),cmp);
		else
			std::sort(data.begin(),data.end());
	}
}

void remove(string xsrc,string xdes,string rec,picture x,int rank)
{
	string src=xsrc+x.name;
	string des=xdes+x.name;

	int c;
	FILE *fpSrc, *fpDest,*fpRec;  
	fpSrc = fopen(src.c_str(), "rb");   
	if(fpSrc==NULL)
	{
		printf( "Source file open failure!\n");  
		return;
	}

	fpDest = fopen(des.c_str(), "wb");  
	if(fpDest==NULL)
	{
		printf("Destination file open failure!\n");
		return;
	}

	fpRec=fopen(rec.c_str(),"a");

	fprintf(fpRec,"%d %s %lf\n",rank,x.name.c_str(),x.result);
	while((c=fgetc(fpSrc))!=EOF)
	{   
		fputc(c, fpDest);
	}

	fclose(fpSrc);  
	fclose(fpDest);
	fclose(fpRec);
}