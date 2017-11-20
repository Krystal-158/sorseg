//
//  HoughLine.h
//  segment


#ifndef HoughLine_h
#define HoughLine_h

#include "disjoint-set.h"
#include <vector>
#include "image.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

void Houghline(vector<coordinate> *Locat,vector<vector<coordinate>> *StraightLine,int imwidth,int imheight)
{
    double LineThreshold = 20;
    //[1]创建mat图像
    Mat midImage(imheight,imwidth,CV_8UC1,Scalar(0));
    for (int i=0; i<Locat->size(); i++)
    {
        midImage.Mat::at<unsigned char>((*Locat)[i].x,(*Locat)[i].y) = 255;
    }
    Mat dstImage = midImage.clone();//临时变量和目标图的定义

    //[2]进行霍夫变换
    vector<Vec4i> lines;//定义一个矢量结构lines用于存放得到的线段矢量集合
    HoughLinesP(midImage, lines, 2, CV_PI/180, 10, 0, 100);
    
    //[3]依次在图中绘制出每条线段
    /*
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line( dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186,88,255), 1, CV_AA);
    }
    */
    
    //[4]依次找出距离这些直线比较近的中心点位置:位于一条直线且范围就在直线附近的区域
    //for( size_t i = 0; i < lines.size(); i++ )
	for( unsigned int i = 0; i < lines.size(); i++ )
    {
        double x1,x2,y1,y2;
        //x是真横坐标，范围1-14000
        x1 = (double)lines[i][0];
        y1 = (double)lines[i][1];
        x2 = (double)lines[i][2];
        y2 = (double)lines[i][3];
        if ((pow(x1-x2,2)+pow(y1-y2,2)) > 1){
            vector<coordinate> OneLine;   //属于某一条线上的点的集合
            for (int j = 0; j < Locat->size();j++)
            {
                double Point2lineDistance = fabs((x2-x1)*((*Locat)[j].x) + (y1-y2)*((*Locat)[j].y)+x1*y2-y1*x2)/sqrt(pow(x1-x2,2)+pow(y1-y2,2));
                if (Point2lineDistance < LineThreshold)
                {
                    if(((*Locat)[j].y < max(x1,x2)+LineThreshold)&&((*Locat)[j].y > min(x1,x2)-LineThreshold))
                    {
                        if(((*Locat)[j].x < max(y1,y2)+LineThreshold)&&((*Locat)[j].x > min(y1,y2)-LineThreshold))
                            OneLine.push_back((*Locat)[j]);
                    }
                    
                }
            }
            StraightLine->push_back(OneLine);
            
        }
    }

    //[5]显示效果图
    //imwrite("/Users/Krystal/Documents/Segmentation/results/JPG13_Hough2.jpeg",dstImage);
    return;
}

#endif /* HoughLine_h */
