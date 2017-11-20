

#ifndef SOANR_FILE_H
#define SOANR_FILE_H

#include <cstdlib>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
#include "image.h"
#include "misc.h"
#include "datapacket.h"
using namespace std;
#define BUF_SIZE 256
#define HEAD_SIZE 516


static void sonar_read(image<ushort>* im, int height,ifstream &file, streampos datapoint,int mode) {
	int width = im->width();
    if (mode == 1)   //
    {
        file.seekg(datapoint);
		//
        for (int k = width/2;k < width-1;k++)
        {
            
            file.read((char*)(imPtr(im, k, height)),2);
        }
    }
    else if (mode == -1)
    {
        file.seekg(datapoint);
        for (int k = 1;k <= width/2;k++)
        {
            ushort *pr = imPtr(im, width/2 - k, height);
            file.read((char*)pr,2);
        }
    }
    else{
        printf("error: defination of function sonar_read in sonar-format.h!");
    }
}

static image<ushort> *loadSONAR(const char *name,unsigned int BeginningPing,bool IfMultiBeam) {
    vector<HeaderData> Info;
    HeaderData Info_pre;
    vector<streampos> datapoint;
    unsigned long c;
	
	//
	ifstream file(name, ios::in | ios::binary);
	
	//
	while (!file.eof())
    {
        file.read((char*)&c,8);
        if (c == 0xFFFFFFFF)
        {
            file.seekg(-4,ios::cur);
            file.read((char*)&Info_pre,sizeof(Info_pre));
			//
            if (Info_pre.PingNumber < BeginningPing)
            {
				//
				//
				streamoff PingVolume;
                if (IfMultiBeam == true)
                {
                    PingVolume = (streamoff)((2*5+2*2*12)*2*Info_pre.NumSamples);
                }
                else
                {
                    PingVolume = (streamoff)(2*5*2*Info_pre.NumSamples);
                }
                file.seekg(PingVolume,ios::cur);
            }
            else if(Info_pre.PingNumber == BeginningPing)
            {
                file.seekg(-(streamoff)(sizeof(Info_pre)+4),ios::cur);
                break;
            }
            else
			{
                printf("该图像不存在该起始帧!");
			}
		}
	}
	
	//
	/**/
	while (!file.eof())
    {
        file.read((char*)&c,8);
        if (c == 0xFFFFFFFF)
        {
            file.seekg(-4,ios::cur);
            file.read((char*)&Info_pre,sizeof(Info_pre));
			//
            datapoint.push_back(file.tellg());
			//
            if (IfMultiBeam == true)
            {
                streamoff PingVolume = (streamoff)((2*5+2*2*12)*2*Info_pre.NumSamples);
                file.seekg(PingVolume,ios::cur);
            }
			else
            {
				streamoff PingVolume = (streamoff)(2*5*2*Info_pre.NumSamples);
                file.seekg(PingVolume,ios::cur);
            }
        }
    }
    file.clear();
	/**/
    unsigned int width;
    unsigned int height;
    width = 2 * Info[0].NumSamples;
    height = 5 * Info.size();
    height = 80;                       //
	image<ushort> *im = new image<ushort>(width, height);
    for (int i = 0;i < height;i ++)
	{
		/**/
        unsigned int num1 = (unsigned int)floor(i/5);
        unsigned int num2 = i%5;
		/**/
		file.seekg(datapoint[num1]);
        file.seekg((streamoff)(2*num2*Info[num1].NumSamples),ios::cur);
        streampos t_pos = file.tellg();
        file.seekg(2*5*Info[num1].NumSamples,ios::cur);
        streampos t_pos2 = file.tellg();
		
		sonar_read(im,i,file,t_pos,-1);
        sonar_read(im,i,file,t_pos2,1);
	
	
	}
	
	return im;
}

static void saveSONAR(image<ushort> *im, const char *name) {
  int width = im->width();
  int height = im->height();
  std::ofstream file(name, std::ios::out | std::ios::binary);
  
  
  file << "P5\n" << width << " " << height << "\n" << 0xFFFF << "\n";
  file.write((char *)imPtr(im, 0, 0), width * height * 2);
}

#endif
