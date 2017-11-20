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

#ifndef DISJOINT_SET
#define DISJOINT_SET

#include "image.h"
using namespace std;
// disjoint-set forests using union-by-rank and path compression (sort of).

typedef struct {
  int rank;
  int p;
  int size;
  ushort pixel;
  int label;      //标记可能成为的物体.   阴影－1；海底0；物体1；
} uni_elt;

class universe {
public:
  universe(int elements,image<ushort> *im);
  ~universe();
  int find(int x);  
  void join(int x, int y);
  int size(int x) const { return elts[x].size; }
  ushort pixel(int x) const { return elts[x].pixel;}
  int num_sets() const { return num; }
  int label(int x);
  void setlabel(int x, int lb);

private:
  uni_elt *elts;
  int num;
};

universe::universe(int elements,image<ushort> *im) {
  elts = new uni_elt[elements];
  num = elements;
  int width = im ->width();
  int height = im ->height();
    int i = 0;
    for(int h = 0; h < height; h++){
        for (int w = 0; w < width; w++){
            if (i < elements){
                elts[i].rank = 0;
                elts[i].size = 1;
                elts[i].pixel = imRef(im,w,h);
                elts[i].p = i;
                elts[i].label = 0;
                //label = 0背景   1目标   3 线形   4其他
                i++;
            }
        }
    }
}
  
universe::~universe() {
  delete [] elts;
}

int universe::find(int x) {
  int y = x;
  while (y != elts[y].p)
    y = elts[y].p;
  elts[x].p = y;
  return y;
}

void universe::join(int x, int y) {
  if (elts[x].label != elts[y].label)
  {
      printf("元素%d(label分类%d)和元素%d(label分类%d)在合并过程中，label属性发生冲突！",x,elts[x].label,y,elts[y].label);
  }
    
  if (elts[x].rank > elts[y].rank) {
    elts[y].p = x;
    elts[x].size += elts[y].size;
  } else {
    elts[x].p = y;
    elts[y].size += elts[x].size;
    if (elts[x].rank == elts[y].rank)
      elts[y].rank++;
  }
  num--;
}
int universe::label(int x){
    int core = this->find(x);
    return elts[core].label;
}

//设置x的label，同时设置x以及x的中心点的label
void universe::setlabel(int x, int lb){
    //   0海底  1目标   2线条   3其他亮区
    if (lb == 0 || lb == 1  || lb == 2 || lb == 3)
    {
        elts[x].label = lb;
        elts[this->find(x)].label = lb;
    }
    
    else
        printf("error：label定义值超出范围！");
}



#endif
