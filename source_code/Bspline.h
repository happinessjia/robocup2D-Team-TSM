#include "Geometry.h"
#ifndef BSPLINE_H
#define BSPLINE_H

class CBpline  
{
public:
	int Def_k;
	int num_points;
	float *T;
public:
	CBpline();
	virtual ~CBpline();
	VecPosition Qu(double u,int n,VecPosition *CP,double *weight);
	float N(double u,int i,int k);
private:
	void GetT(int n);
};

#endif
