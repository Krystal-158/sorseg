//
//  save_jpg.h
//  graph-based-segmentation
//
//  Created by 夏虫 on 17/6/19.
//  Copyright © 2017年 夏虫. All rights reserved.
//

#ifndef image_analysis_h
#define image_analysis_h

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv/highgui.h>
#include <fstream>
#include "image.h"
#include <vector>
using namespace std;
using namespace cv;

void saveJPG(image<rgb> *im,const String &name)
{
    int width = im->width();
    int height = im->height();
    Mat jpg(height,width,CV_8UC3);
    for (int i = 0;i<height;i++)
    {
        for (int j=0; j<width; j++)
        {
            jpg.Mat::at<Vec3b>(i,j)[0] = imRef(im,j,i).r;
            jpg.Mat::at<Vec3b>(i,j)[1] = imRef(im,j,i).g;
            jpg.Mat::at<Vec3b>(i,j)[2] = imRef(im,j,i).b;
        }
    }
    vector<int> compression;
    compression.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression.push_back(100);
    imwrite(name,jpg,compression);
}

  /*
static void Analysis(image<ushort> *im,const string *name)
{
    int width = im->width();
    int height = im->height();
    ushort maximum = 0;
    Mat jpg(height,width,CV_16UC1);
    for (int i = 0;i<height;i++){
        for (int j=0; j<width; j++){
            jpg.Mat::at<ushort>(i,j) = imRef(im,j,i);
            maximum = (maximum >= imRef(im,j,i))?maximum:imRef(im,j,i);
        }

    }
    
}

*/
#endif /* image_analysis.h */
