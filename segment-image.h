/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#ifndef SEGMENT_IMAGE
#define SEGMENT_IMAGE
  
#include <cstdlib>
#include "image.h"
#include "misc.h"
#include "filter.h"
#include "segment-graph.h"
#include <vector>

//用于传递历史信息参数的数据结构
struct ConnectInfo
{
    int left;
    int right;
    rgb color;
    int type;     //type就是0 1 2 3代表分类的类型   0海底  1目标   2线条   3其他亮区
    int p;
};

// random color
rgb random_rgb(){ 
  rgb c;
  
  c.r = (uchar)rand();
  c.g = (uchar)rand();
  c.b = (uchar)rand();

  return c;
}

// dissimilarity measure between pixels
static inline float diff(image<float> *r,int x1, int y1, int x2, int y2) {
  //return sqrt(square(imRef(r, x1, y1)-imRef(r, x2, y2)) + square(imRef(g, x1, y1)-imRef(g, x2, y2)) + square(imRef(b, x1, y1)-imRef(b, x2, y2)));
    return fabs(imRef(r, x1, y1)-imRef(r, x2, y2));
}

/*
 * Segment an image
 *
 * Returns a color image representing the segmentation.
 *
 * im: image to segment.
 * sigma: to smooth the image.
 * c: constant for treshold function.
 * min_size: minimum component size (enforced by post-processing stage).
 * num_ccs: number of connected components in the segmentation.
 */

image<rgb> *segment_image(image<ushort> *im, vector<ConnectInfo> *Connection, float sigma, float c, int min_size,
			  int *num_ccs,int size_threshold, int SeabThreshold) {
  int width = im->width();
  int height = im->height();

  image<float> *r = new image<float>(width, height);

  // smooth each color channel

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      imRef(r, x, y) = imRef(im, x, y);

    }
  }

  //image<float> *smooth_r = smooth(r, sigma);           //需要加入高斯模糊吗
  image<float> *smooth_r = r;
 
  // 建图
  edge *edges = new edge[width*height*4];
  int num = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (x < width-1) {
	edges[num].a = y * width + x;
	edges[num].b = y * width + (x+1);
	edges[num].w = diff(smooth_r, x, y, x+1, y);
	num++;
      }

      if (y < height-1) {
	edges[num].a = y * width + x;
	edges[num].b = (y+1) * width + x;
	edges[num].w = diff(smooth_r, x, y, x, y+1);
	num++;
      }

      if ((x < width-1) && (y < height-1)) {
	edges[num].a = y * width + x;
	edges[num].b = (y+1) * width + (x+1);
	edges[num].w = diff(smooth_r, x, y, x+1, y+1);
	num++;
      }

      if ((x < width-1) && (y > 0)) {
	edges[num].a = y * width + x;
	edges[num].b = (y-1) * width + (x+1);
	edges[num].w = diff(smooth_r, x, y, x+1, y-1);
	num++;
      }
    }
  }
  delete smooth_r;

  // segment
  vector<coordinate> Locat;//////////////
  universe *u = segment_graph(im, width*height, num, edges, c,size_threshold,SeabThreshold,min_size,&Locat);
  
  
  delete [] edges;
  *num_ccs = u->num_sets();

  image<rgb> *output = new image<rgb>(width, height);

  // 为分割区域上色
  rgb *colors = new rgb[width*height];
  rgb black; black.r = 0; black.g = 0; black.b = 0;    //将目标设为黑色
  rgb white; white.r = 255; white.g = 255; white.b = 255;    //将背景设为白色
  for (int i = 0; i < width * height; i++)
    colors[i] = random_rgb();
 
  //对上色向量colors[i]进行修改，特定的p对应特定的颜色
       //首先统计开头的12帧中的物体，保存在iConnection中
    vector<ConnectInfo> iConnection;
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < width; x++) {
            rgb element = imRef(output,x,y);
            if (element != white)
            {
                bool ifrecord = false;
                int i = 0;
                while(i < iConnection.size() && ifrecord == false)     //如果已经有同颜色的分组，就将(x,y)加入该分组
                {
                    if ((iConnection)[i].color == element)
                    {
                        iConnection[i].left = iConnection[i].left<=x?iConnection[i].left:x;
                        iConnection[i].right = iConnection[i].right>=x?iConnection[i].right:x;
                        ifrecord = true;
                    }
                    i++;
                }
                if (ifrecord == false)                                  //如果没有该颜色的分组，就按横坐标x的大小顺序插入一个新的分组
                {
                    struct ConnectInfo addelement = {x,x,element,u->label(y * width + x),u->find(y * width + x)};
                    for (int j=0; j < iConnection.size();j++)
                    {
                        if(iConnection[j].left >= x)
                        {
                            iConnection.insert(iConnection.begin()+j,1,addelement);
                            break;
                        }
                        else if(j == iConnection.size()-1)
                        {
                            iConnection.push_back(addelement);
                            break;
                        }
                    }
                    if (iConnection.size() == 0)
                    {
                        iConnection.push_back(addelement);
                    }
                }
            }
        }
    }
      //2再逐次比对iConnection和Connection，也就是上一段末尾和这一段开头的可能合并的物体
    int i=0,j=0,k=0;
    while(i<iConnection.size() && k<Connection->size())
    {
        for(j=k;j<Connection->size();j++)
        {
            ConnectInfo a = iConnection[i],b=(*Connection)[j];
            if (a.left <= b.right && a.right >= b.left)
            {
                int Overlapping = min(a.right,b.right)-max(a.left, b.left);
                int OverlappingRate = Overlapping == 0?0:min(Overlapping/(a.right-a.left),Overlapping/(b.right-b.left));
                if ((OverlappingRate >= 0.5) || ((Overlapping >0) && iConnection[i].type==2 && (*Connection)[j].type == 2))
                    colors[iConnection[i].p] = (*Connection)[j].color;
            }
            else if (a.right <b.left){
                break;
            }
            else if (a.left >b.right){
                k++;
            }
            
        }
        i++;
    }

    
  //vector<int> test;               ///////////////////test
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
          int comp = u->find(y * width + x);
          imRef(output, x, y) = colors[comp];
        if((u->label(y * width + x) == 1)&&((u->size(u->find(y * width + x)) <= 100))&&(u->size(u->find(y * width + x)) >= 10)){
            //imRef(output, x, y) = black;
        }
        else if(u->size(u->find(y * width + x)) > 10000)
            imRef(output, x, y) = white;       
  //////////////////////test amount of color////////////////////////
       /*
        for(int i = int(test.size());i >= 0;i--){
            if(i == 0)
                test.push_back(comp);
            else if (test[i-1] == comp)
                break;
        }
        */
    }
  }
    
    //根据已上色结果建立历史信息：每个目标区域的末端宽度，颜色，判别类型
    vector<ConnectInfo>().swap(*Connection);     //清除原有vector
    for (int y = height-12; y < height; y++) {
        for (int x = 0; x < width; x++) {
            rgb element = imRef(output,x,y);
            if (element != white)
            {
                bool ifrecord = false;
                int i = 0;
                while(i < Connection->size() && ifrecord == false)
                {
                    if ((*Connection)[i].color == element)
                    {
                        (*Connection)[i].left = (*Connection)[i].left<=x?(*Connection)[i].left:x;
                        (*Connection)[i].right = (*Connection)[i].right>=x?(*Connection)[i].right:x;
                        ifrecord = true;
                    }
                    i++;
                }
                if (ifrecord == false)
                {
                    struct ConnectInfo addelement = {x,x,element,u->label(y * width + x)};
                    for (int j=0; j < Connection->size();j++)
                         {
                             if((*Connection)[j].left >= x)
                             {
                                 Connection->insert(Connection->begin()+j,1,addelement);
                                 break;
                             }
                             else if(j == Connection->size()-1)
                             {
                                 Connection->push_back(addelement);
                                 break;
                             }
                         }
                    if (Connection->size() == 0)
                    {
                        Connection->push_back(addelement);
                    }
                }
            }
        }
    }
  //////////////把每个区域的中心点Locat向量标出来 ///////////////////
    /*
    for (int i=0; i<Locat.size(); i++)
    {
        imRef(output,Locat[i].y,Locat[i].x) = black;
    }
     */
  delete [] colors;  
  delete u;

  return output;
}



#endif
