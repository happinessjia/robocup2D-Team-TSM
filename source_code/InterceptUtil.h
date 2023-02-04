#ifndef INTERCEPT_H
#define INTERCEPT_H


#include "WorldModel.h"
#include "Objects.h"      
#include "PlayerSettings.h" 

typedef struct TagInterceptInfo
{
//截球基本参数
	VecPosition Vel;

	ObjectT OppFastest;
	double OppCycles;

	ObjectT OurFastest;
	double OurCycles;
	VecPosition posEnd;
	
	bool isCaled;
} InterceptInfo;


class InterceptUtil	//using tsinghua method
{
public:
	//通常用于搜索:只是改变球速，其他如球员的参数不变
	//得到截球信息
	InterceptInfo* getInterceptInfo(const VecPosition& ballVel);
	int getInterceptCyc(const VecPosition& ballVel);
	//参数
	VecPosition ballPos;
	VecPosition playerPos, playerVel;
	double angBody;
	double maxSpeed;
	double maxDashEffect;
	double kickableArea;//kickArea是内部计算用的参数
private:
	double kickArea;//kickArea是内部计算用的参数
	void getinterceptioninfo2(const VecPosition& ballVel, InterceptInfo& inf);
	VecPosition calc_interception_solution(const VecPosition& startpoint, double ballspeed);
	double calc_peakpoint(const VecPosition& pos);
};

extern InterceptUtil interUtil;

#endif
