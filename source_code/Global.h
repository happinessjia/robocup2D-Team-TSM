#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "Geometry.h"
////////////////added by dxf

////////////////added by dxf
extern int g_cyc;	//当前周期

//========================
extern VecPosition	g_ballPos;
extern VecPosition	g_ballVel;

//========================
extern VecPosition	g_agentPos;
extern VecPosition	g_agentVel;
extern double		g_agentBodyAng;
extern VecPosition	g_strpos;
//================
extern ControlPoint position_cp_start;
extern ControlPoint position_cp_A;
extern ControlPoint position_cp_Attack;
extern ControlPoint position_cp_M;
extern ControlPoint position_cp_D;
extern ControlPoint position_cp;
//================

#endif

