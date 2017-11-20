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

#ifndef SEGMENT_GRAPH
#define SEGMENT_GRAPH

#include <algorithm>
#include <cmath>
#include "disjoint-set.h"
#include <vector>
#include "image.h"
#include "HoughLine.h"

// threshold function
#define THRESHOLD(size, c) (c/size)

typedef struct {
  float w;
  int a, b;
} edge;
/*
typedef struct {
    long int x,y;
    int p;
} coordinate;
*/
bool operator<(const edge &a, const edge &b) {
  return a.w < b.w;
}

/*
 * Segment a graph
 *
 * Returns a disjoint-set forest representing the segmentation.
 *
 * num_vertices: number of vertices in graph.
 * num_edges: number of edges in graph
 * edges: array of edges.
 * c: constant for treshold function.
 */
universe *segment_graph(image<ushort> *im, int num_vertices, int num_edges, edge *edges,
			float c,int size_threshold, int SeabThreshold,int min_size,vector<coordinate> *Locat1) {
  // sort edges by weight
  std::sort(edges, edges + num_edges);

  // make a disjoint-set forest
  universe *u = new universe(num_vertices, im);
  vector<coordinate> Locat;
  // init thresholds
  float *threshold = new float[num_vertices];
  for (int i = 0; i < num_vertices; i++)
    threshold[i] = THRESHOLD(1,c);

    // 每一条边以降序方式开始遍历。将权重大于1000且pixel大于1000的区域合并。直到边的差小于1000后，不再合并。
    for (int i = num_edges - 1; i >= 0; i--)
    {
        edge *pedge = &edges[i];
        //components conected by this edge
        int a = u->find(pedge->a);
        int b = u->find(pedge->b);
        if (a != b){
            if ((pedge->w > 1000) ||((u->pixel(pedge->a)>=800)&&(u->pixel(pedge->b)>=800)))
            {
                //将a,b标记为目标点。
                u->setlabel(a,1);
                u->setlabel(b,1);
                //合并ab
                u->join(a, b);
                //重新设置区域阈值
                a = u->find(a);
                threshold[a] = pedge->w;
            }
        }
    }
    
  // for each edge, in non-decreasing weight order...
  for (int i = 0; i < num_edges; i++)
  {
    edge *pedge = &edges[i];
    
    // components conected by this edge
    int a = u->find(pedge->a);
    int b = u->find(pedge->b);
    if ((a != b)&&(u->label(pedge->a) != 1)&&(u->label(pedge->b) != 1))   //阻止被标签记为目标的部分与周围背景合并
    {
      if ((pedge->w <= threshold[a]) &&
          (pedge->w <= threshold[b]))
      {
          u->join(a, b);
          a = u->find(a);
          int test = pedge->w + THRESHOLD(u->size(a), c);
          threshold[a] = SeabThreshold + pedge->w + THRESHOLD(u->size(a), c);     //以海底的SeabThreshold作为打底
          
      }
    }
  }


    // 把属于背景的分割区域融合。依据：面积
/*
    for (int i = 0; i <num_edges; i++)
    {
        edge *pedge = &edges[i];
        //components conected by this edge
        int a = u->find(pedge->a);
        int b = u->find(pedge->b);
        if (a != b)   //阻止被标签记为目标的部分与周围背景合并
        {
            if ((u->size(a) > size_threshold)&&(u->size(b) > size_threshold)){
                u->setlabel(a,0);
                u->setlabel(b,0);
                u->join(a, b);
            }
 
        }
    }
*/
    // post process small components
    
    for (int i = 0; i < num_edges; i++) {
        int a = u->find(edges[i].a);
        int b = u->find(edges[i].b);
        if ((a != b) && (u->label(a) != 1)&&(u->label(b) != 1)){
            if((u->size(a) < min_size) || (u->size(b) < min_size))
                u->join(a, b);
        }
    }
    
    //整理得到分割区域的中心点序列，保存到 Locat数列
    coordinate origin = {0,0,0};
    vector<coordinate> location(num_vertices,origin);
    int width = im->width();
    for (int i = 0; i < num_vertices; i++)
    {
        location[u->find(i)].x = location[u->find(i)].x + int(i/width);
        location[u->find(i)].y = location[u->find(i)].y + i % width;
        location[u->find(i)].p = u->find(i);
        
    }
    for (int i = 0; i < num_vertices; i++)
    {
        if(u->find(i) == i)
        {
            int size = u->size(i);
            location[i].x = long((location[i].x)/size);
            location[i].y = long((location[i].y)/size);
            Locat.push_back(location[i]);
            //printf("p = %d, (x,y) = (%d,%d)",u->find(i),location[u->find(i)].x,location[u->find(i)].y);
        }
    }
    *Locat1 = Locat;
    

    //用霍夫变换查找连成一线的中心点位置
    vector<vector<coordinate>> StraightLine;
    Houghline(&Locat,&StraightLine,im->width(),im->height());


    int merge_distant = 10;
    //合并靠的太近的两个区域，距离少于10的要合并
    for (int i = 0;i<Locat.size()-1;i++)
    {
        if(u->size(Locat[i].p)<1500)
        {
            for(int j = i+1;j<Locat.size();j++)
            {
                if(u->size(Locat[j].p)<900)
                {
                    double distant = sqrt(pow(double(Locat[i].x-Locat[j].x),2)+pow(double(Locat[i].y-Locat[j].y),2));
                    int ip = u->find(Locat[i].p);
                    int jp = u->find(Locat[j].p);
                    if ((distant <= merge_distant)&&(ip != jp)){
                        u->setlabel(ip,1);
                        u->setlabel(jp,1);
                        u->join(ip,jp);
                    }
                }
            }
        }
        
    }

    
    //将之前霍夫变幻中成线性的中心点区域合并
    
    for(int i = 0;i < StraightLine.size();i++)
    {
        vector<coordinate> OneLine = StraightLine[i];
        if (OneLine.size() != 0)
        {
            for(int j = 1;j < OneLine.size();j++)
            {
                int ip = u->find(OneLine[0].p);
                int jp = u->find(OneLine[j].p);
                if (ip != jp)
                {
                    u->setlabel(ip,2);
                    u->setlabel(jp,2);
                    u->join(ip,jp);
                }
            }
        }
        
    }
    
    int min_size2 = 25;
    //将太小的区域和背景合并
    /*
    for (int i = 0; i < num_edges; i++) {
        int a = u->find(edges[i].a);
        int b = u->find(edges[i].b);
        if ((a != b)&&(u->label(a) != 2)&&(u->label(b) != 2)){
            if((u->size(a) < min_size2) && (u->size(b) > 1000))
                u->join(a, b);
            else if((u->size(b) < min_size2) && (u->size(a) > 1000))
                u->join(a, b);
        }
    }
    */

    // free up
  delete[] threshold;
  return u;
}
 

#endif
