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


#include <iostream>
#include "image.h"
#include "misc.h"
#include "pnmfile.h"
#include "segment-image.h"
#include "time.h"
#include "sonar_format.h"
#include "analysis.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

int main(){
    vector<ConnectInfo> Connection;
    for (int image5 = 0; image5 < 5;image5++)
    {
		clock_t start_time = clock();
		bool IfMultiBeam = true;
		unsigned int BeginningPing = 2540+image5*16;
		float sigma = 0.4;
		float k = 400;
		int min_size = 10;
		int size_threshold = 300;
		int SeabThreshold = 10;

		printf("loading input image.\n");
		image<ushort> *input = loadSONAR( "E:\声呐实验\UUV2017密云水库\密云水库数据\2017年09月21日14时21分53秒.rwd",BeginningPing,IfMultiBeam);
		printf("processing\n");
		int num_ccs;
		image<rgb> *seg = segment_image(input, &Connection,sigma, k, min_size, &num_ccs,size_threshold,SeabThreshold);
		printf("got %d components\n", num_ccs);
		char name[50];
		sprintf(name,"%s%d%s","E:\目标检测\coarse-segmentation-windows\处理结果\test_",BeginningPing,".jpeg");
		saveJPG(seg,name);
		double time = double((clock()-start_time))/CLOCKS_PER_SEC;
		printf("Time cost: %f",time);
    }
    return 0;
}

