#ifndef DATA_PACKET_H
#define DATA_PACKET_H
//定义了包头结构DataPacketHeader, 重要信息结构SonarIMdata,以及坐标结构Pixel_loc

#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;


//帧信息结构，大小为512字节
    #pragma pack(push)
    #pragma pack(2)
typedef struct SonarDataPacketHeader{
	unsigned int NumberBytes;
	unsigned int PageVersion;
	unsigned int Configuration;
	unsigned int PingNumber;			//当前数据的ping数值    17
	unsigned int NumSamples;			//每个通道数据的采点样数   21
	unsigned int BeamsToDisplay;
	unsigned int ErrorFlags;
	unsigned int Range;					//潜航体显示距离        33
	unsigned int SpeedFish;				//潜航体速度            37
	unsigned int SpeedSound;			//声速                  41
	unsigned int ResMode;
	unsigned int TxWaveFrom;
	unsigned int RespDiv;
	unsigned int RespFreq;
	unsigned int ManualSpeedSwitch;
	unsigned int DespeckleSwitch;
	unsigned int SpeedFilterSwitch;
	unsigned int Year;					//时间 年
	unsigned int Month;				//月
	unsigned int Day;					//日
	unsigned int Hour;					//时
	unsigned int Minute;				//分
	unsigned int Second;				//秒
	unsigned int hSecond;				//毫秒
	unsigned int FixTimeHour;			//…œ¥ŒGPS∏¸–¬æ‡œ÷‘⁄µƒ ±º‰£®–° ±£©
	unsigned int FixTimeMinute;			//…œ¥ŒGPS∏¸–¬æ‡œ÷‘⁄µƒ ±º‰£®∑÷÷”£©     26∏ˆ
	float FixTimeSecond;				//…œ¥ŒGPS∏¸–¬æ‡œ÷‘⁄µƒ ±º‰£®√Î£©      
	float  FishHeading;					//潜航体航向（度）             113
	float  Pitch;						//潜航体倾斜 度               117
	float  Roll;						//潜航体横滚角  度             121
	float  Depth;						//潜航体深度 米                125
	float  Altitude;					//潜航体离体高度                129
	float  Temperature;
	float  Speed;						//母船速度  m/s                137
	float  ShipHeading;				//母船航向 度
	float  MagneticVariation;			//GPSÀ˘≤‚µ√¥≈≥°±‰ªØ                10∏ˆ
	double ShipLat;					//母船纬度                    149
	double ShipLon;					//母船经度                    153
	double FishLat;					//潜航体经度  弧度
	double FishLon;					//潜航体纬度   弧度                 4∏ˆ
	unsigned int TvgPage;												    //181
	unsigned int HeaderSize;			// 数据值256                         185
	unsigned int FixTimeYear;			//…œ¥ŒGPSµº∫Ω–≈œ¢∏¸–¬¿Îœ÷‘⁄µƒƒÍ ˝
	unsigned int FixTimeMonth;			//…œ¥ŒGPSµº∫Ω–≈œ¢∏¸–¬¿Îœ÷‘⁄µƒ‘¬ ˝
	unsigned int FixTimeDay;			//…œ¥ŒGPSµº∫Ω–≈œ¢∏¸–¬¿Îœ÷‘⁄µƒÃÏ ˝   5个
	float  AuxPitch;
	float  AuxRoll;
	float  AuxDepth;
	float  AuxAlt;
	float  CableOut;					//Õ–¿¬≥§∂»£®√◊£©                      217
	float  FSeconds;					//Ping 时间  秒                    6∏ˆ
	unsigned int Altimeter;				
	unsigned int SampleFreq;			//采样频率 次/s
	unsigned int DepressorType;
	unsigned int CableType;                                                 // 4个
	float  ShieveXoff;
	float  ShieveYoff;
	float  ShieveZoff;
	float  GPSHeight;                                                      //4∏ˆ
	unsigned int RawDataConfig;                                             //257
	unsigned int Header3ExtensionSize;	//÷µŒ™256£¨¥À ˝æ›Õ∑µƒ¥Û–°
	unsigned int SbpTxWaveFrom;
	unsigned int SbpPreAmpGain;
	unsigned int SbpDataRaw;
	unsigned int sbpNumSamples;
	unsigned int SbpNumSampleFreq;
	unsigned int SbpTxWaveFromVersion;
	unsigned int WingAngle;
	unsigned int WingEmergencySwitchState;
	unsigned int LaybackMethod;                                      // 11∏ˆ
	double LaybackFishLat;
	double LaybackFishLon;                                           //2∏ˆ
	float  FishHeadingOffset;
	float  PressureSensorOffset;                                       // 2∏ˆ
	//上传的声呐参数
	int	   ConferTimeState;				// ⁄ ±◊¥Ã¨                  325
	int	   SynMode;						//Õ¨≤Ωƒ£ Ω                  329
	int	   Frequency;					//频率                         333
	int	   SampleFrequency;				//采样频率                   337
	int	   ReceiveBandWidth;			//Ω” ’¥¯øÌ                      341
	int	   TxPulseBandWidth;			//Tx¬ˆ≥ÂøÌ∂»                   345
	int	   TxPulseType;					//Tx¬ˆ≥Â¿‡–Õ               349
	int	   MaxPingFreq;					//◊Ó¥ÛPing¬                353
	int	   PingPeriod;					//Ping周期                 357
	int	   WorkRange;					//距离                     361
	int	   Power;						//功率                     365
	int	   GainSelect;					//‘ˆ“Ê—°‘Ò                  369
	int	   Attenuate;					//À•ºıœµ ˝                      373
	int	   Gain;						//πÃ∂®‘ˆ“Ê                      377
	int	   WorkState;					//π§◊˜◊¥Ã¨£¨0£∫Œ¥π§◊˜£¨1£∫π§◊˜    15个  381
	
	int    Reserved[33];				//保留位		                     33个
}HeaderData;

//包头结构，大小为516个字节
typedef struct DataPacketHeader{
	unsigned int MessageType;
	struct SonarDataPacketHeader DataHeader;
}Header;

struct SonarData
{
	unsigned short int pixel;
};

typedef struct SonarIMdata
{
	unsigned int PingNumber;			//当前数据的ping数值     17
	unsigned int NumSamples;			//采样点数               21
	unsigned int Range;					//«±∫ΩÃÂœ‘ ææ‡¿Î          33
    float  FishHeading;					//潜航体航向（度）             113
    float  Pitch;						//潜航体倾斜 度               117
    float  Roll;						//潜航体横滚角  度             121
    float  Depth;						//潜航体深度 米                125
    float  Altitude;					//潜航体离体高度                129
    float  Speed;						//母船速度  m/s                137
	float  ShipHeading;				//GPS所测得母船的航向       141
    double ShipLat;					//母船纬度                    149
    double ShipLon;					//母船经度                    153
}HeaderBrief;

struct Pixel_loc
{
	double x;					// x坐标
	double y;					// y坐标
};
#pragma pack(pop)
#endif
