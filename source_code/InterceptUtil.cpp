#include "Geometry.h"
#include "InterceptUtil.h"
#include <math.h>
#include <fstream>
#include <iostream>

using namespace std;

InterceptUtil interUtil;

#define Min(x,y) ((x) < (y) ? (x) : (y))
#define Max(x,y) ((x) > (y) ? (x) : (y))

/*!
*求取反正弦
*如果输入不在反正弦函数定义域内[-1.0 1.0]
*则强制修改输入为定义域两个端点中的最近点
*
*\param: [in] double x ,正弦值
*\return: AngleDeg, 正弦值对应的角度
*/
static double ASin(double x)
{ 
	return ((x) >= 1.0f ? 90.0f : ((x) <= -1.0f ? -90.0f : (Rad2Deg(asin(x))))); 
}

//求平方
static double Sqr(double x) 
{ 	
	return x*x; 
}

int InterceptUtil::getInterceptCyc(const VecPosition& ballVel)
{
	int cyc = (int)getInterceptInfo(ballVel)->OurCycles;
	if (cyc>100)
		cyc = 100;
	return cyc;
}

InterceptInfo* InterceptUtil::getInterceptInfo(const VecPosition& ballVel)
{
	static InterceptInfo inf;
	if(angBody==UnknownAngleValue){	//身体角度未知
		getinterceptioninfo2( ballVel, inf);
		return &inf;
	}
	if(playerVel.getMagnitude() < 0.1 || fabs(VecPosition::normalizeAngle(playerVel.getDirection() -  angBody)) > 15 ){
		//Guess: he is not in a prepared state. (it's wrong in few chances)
		getinterceptioninfo2( ballVel, inf);
		return &inf;
	}

	double ballspeed = ballVel.getMagnitude();
	double ballangle = ballVel.getDirection();
	VecPosition playerpos = playerPos - ballPos;
	playerpos = playerpos.rotate(- ballangle);

	double dash_miss =(maxSpeed - playerVel.getMagnitude() - maxDashEffect)/maxDashEffect;
	dash_miss = Max(0.0f, Min(dash_miss, 0.7f));
	kickArea = kickableArea - dash_miss;
	VecPosition it_inf = calc_interception_solution(playerpos, ballspeed);

	it_inf.setY( (double)(int(it_inf.getY() + 0.99f)) );
	it_inf.setX( Geometry::getSumGeomSeries( ballspeed, 0.94, it_inf.getY()) );

	VecPosition rel_it_pos = VecPosition(it_inf.getX(), 0);
	inf.posEnd = rel_it_pos.rotate(ballangle) + ballPos;

	double angle = VecPosition::normalizeAngle((rel_it_pos - playerpos).getDirection() + ballangle);
	double dist = Max(it_inf.getY(), kickArea);
	double allow_angle_diff = ASin(Min(kickArea, 0.9f)/dist);
	double IT_angle = (inf.posEnd - playerPos).getDirection();

	if(fabs(VecPosition::normalizeAngle(IT_angle - angBody)) < allow_angle_diff){
		//as you see, he doesn't need to turn more.
		inf.OurCycles = it_inf.getY();
	}else{
		//so he has to turn now
		getinterceptioninfo2( ballVel, inf);
	}
	return &inf;
}

void InterceptUtil::getinterceptioninfo2(const VecPosition& ballVel, InterceptInfo& inf){
	//should turn to intercept
	//so calculate directly from next cycle.
	static double ballDecay = 0.94;
	VecPosition n_ballpos = ballPos + ballVel;
	VecPosition n_ballvel = ballVel;n_ballvel=n_ballvel*ballDecay;
	VecPosition n_playerpos = playerPos + playerVel;

	double ballspeed = n_ballvel.getMagnitude();
	double ballangle = n_ballvel.getDirection();
	n_playerpos -= n_ballpos;
	n_playerpos = n_playerpos.rotate(- ballangle);

	double dashmiss = (1-maxDashEffect)/maxDashEffect;
	if(n_playerpos.getMagnitude() < kickableArea){
		dashmiss = 0;
	}

	kickArea = kickableArea - dashmiss;
	VecPosition it_inf = calc_interception_solution(n_playerpos, ballspeed);

	inf.OurCycles = (double)(int(it_inf.getY() + 0.99f));	//1 - turn cycle
	if (fabs(inf.OurCycles) > 1000 || inf.OurCycles < 0)
		inf.OurCycles = 1000;
	it_inf.setX( Geometry::getSumGeomSeries( ballspeed, ballDecay, inf.OurCycles) );
	VecPosition rel_it_pos = VecPosition(it_inf.getX(), 0);
	inf.posEnd = rel_it_pos.rotate(ballangle) + n_ballpos;
	inf.OurCycles += 1.0f;
}

//计算最快截球点
VecPosition InterceptUtil::calc_interception_solution(const VecPosition& startpoint, double ballspeed){	
	bool testok;
	int cycles;
	unsigned char secs = 0;
	double fx, fx_;
	static double ballDecay = 0.94;
	static double logBallDecay = log(ballDecay);
	VecPosition inf[3];
	if(startpoint.getX() < 0){
		testok = 0;
	}else{
		double testz = (fabs(startpoint.getY())-kickArea)/maxSpeed;
		if(testz < 0){
			testok = 1;
		}else{
 			if(Geometry::getSumGeomSeries( ballspeed, ballDecay, testz ) < startpoint.getX())
				testok = 1;
			else
				testok = 0;
		}
	}

	double minz, z, tmp, x;
	minz = fabs(startpoint.getY()) - kickArea +0.2;
	minz = Max(0.0, minz);
	z = (minz + kickArea)/maxSpeed;
	//calculate the nearest intercept point
	if(sqrt(Sqr(startpoint.getX()) + Sqr(startpoint.getY())) < kickArea){
		//situation 1: the ball has been intercepted here.
		inf[0].setX( 0.0f ); inf[0].setY( 0.0f );
		secs |= 1;
	}else if(testok){
		cycles = 0;
		fx = 1;
		while(cycles < 5 && fabs(fx) > 0.05f){
			x = Geometry::getSumGeomSeries( ballspeed, ballDecay, z );
			tmp = sqrt(Sqr(maxSpeed*z+kickArea)-Sqr(startpoint.getY()));
			fx = startpoint.getX() - tmp - x;
			fx_ = - maxSpeed*(maxSpeed*z+kickArea)/tmp +  ballspeed*pow(ballDecay, z)*logBallDecay/(1-ballDecay);
			z = z - fx/fx_;
			if(z < minz) z = minz;        
			cycles ++;
		}
		inf[0].setX(x);	inf[0].setY(z);
		secs |= 1 ;
	}
	if(secs){
		return  inf[0];
	}

	//calculate fatherest intercept point 
	fx = 1;	cycles = 0;
 	z = Max(z, 30.0);
	//if no fore intercept point, we must find a back point
	int s_mincycles = testok ? 5 : 2 * 5;
	while(cycles < s_mincycles && fabs(fx) > 0.05f){
		x = Geometry::getSumGeomSeries( ballspeed, ballDecay, z );
		tmp = sqrt(Sqr(maxSpeed*z+kickArea)-Sqr(startpoint.getY()));
		fx = startpoint.getX() + tmp - x;
		fx_ = maxSpeed*(maxSpeed*z+kickArea)/tmp +  ballspeed*pow(ballDecay, z)*logBallDecay/(1-ballDecay);
		z = z - fx/fx_;
		if(z < minz){
			if(testok){
				break;
			}else{
				z = minz;
			}
		}
		cycles ++;
	}
	if(!testok || fabs(fx) < 0.05f){//exist a back point
		inf[2].setX(x);	inf[2].setY(z);
		secs |= 4;
		
		if(!testok){
			double p = calc_peakpoint(startpoint);
			if(p < 20.0f && p >= startpoint.getX() && p < inf[2].getX()){
				z = (sqrt(Sqr(startpoint.getX() - p) + Sqr(startpoint.getY())) - kickArea)/maxSpeed;
				if(z < 1) z=1;
				int int_z = int(z);
				x = Geometry::getSumGeomSeries( ballspeed, ballDecay, int_z );
				if(sqrt(Sqr(startpoint.getX() - x) + Sqr(startpoint.getY())) <= maxSpeed*int_z+kickArea){
					inf[1].setX(x); inf[1].setY(int_z);
					secs |= 2;
				}else{
					int_z ++;
					x = Geometry::getSumGeomSeries( ballspeed, ballDecay, int_z );
					if(sqrt(Sqr(startpoint.getX() - x) + Sqr(startpoint.getY())) <= maxSpeed*int_z+kickArea){
						inf[1].setX(x); inf[1].setY(int_z);
						secs |= 2;
					}
				}
			}
		}else{
			cycles = 0;
			z = minz;
			fx = 1;
			while(cycles < 5 && fx > 0.05f){
				x = Geometry::getSumGeomSeries( ballspeed, ballDecay, z );
				tmp = sqrt(Sqr(maxSpeed*z+kickArea) - Sqr(startpoint.getY()));
				fx = startpoint.getX() + tmp - x;
				fx_ = maxSpeed +  ballspeed*pow(ballDecay, z)*logBallDecay/(1-ballDecay);
				z = z - fx/fx_;
				if(z < minz) break;            
				cycles ++;
			}
			inf[1].setX(x); inf[1].setY(z);
			secs |= 2;
		}
		if(1){
			if(secs & 2){
				inf[0] = inf[1];
			}else{
				inf[0] = inf[2];
			}
		}
	}else{
		//shouldn't be here
	}
	return  inf[0];
}

double InterceptUtil::calc_peakpoint(const VecPosition& pos){
	if(pos.getX() <= 0) return 100.0f;	//peak point doesn't exist
//use Newton Method to calc the peak point
	double z;
	double sq, pw;
	double f, f_, delta = 1;
	static double ballDecay = 0.94;
	static double logBallDecay = log(ballDecay);
	
	int cycles = 0;
	double x=pos.getX();
	do{
		sq = sqrt( Sqr(x-pos.getX()) + Sqr(pos.getY()) );
		z = sq - 1;
		pw = pow(ballDecay, -z);
		f = (x - pos.getX()) * x * (1 - ballDecay) - sq * (pw - 1);
		f_ = (1-ballDecay) * (2*x-pos.getX()) + (x-pos.getX()) * (pw*logBallDecay - (pw-1)/sq);
		delta = f/f_;
		x = x - delta;
		cycles ++;
	}while(fabs(delta) > 0.05f && cycles < 5 && x > 0);

	if (fabs(delta)>0.05f || x<=0){
		return  125.1;
	}else{
		//additional fix to handle special situation.
		//sometimes, x is negative
		double sx;
		if(fabs(pos.getY()) < 0.7){
			sx = pos.getX() + sqrt(Sqr(0.7)-Sqr(pos.getY()));
		}else{
			sx = pos.getX();
		}
		return Max(x, sx);
	}
}

