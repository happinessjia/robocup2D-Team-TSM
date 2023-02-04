#include "Bspline.h"

CBpline::CBpline()
{
	Def_k=2;
	num_points=50;
	GetT(20);
}
CBpline::~CBpline()
{
	delete []T;
}

float CBpline::N(double u,int i,int k)
{
	float a;
	
	if(k==0)
	{
		if(u>=T[i-1]&&u<=T[i])
			return 1;
		else 
			return 0;
	}
	else 
		return a=(float)(u-T[i-1])*N(u,i,k-1)/(T[i-1+k]-T[i-1])+(T[i+k]-u)*N(u,i+1,k-1)/(T[i+k]-T[i]);
  //N(u)=(u-t(i))*N(u,i,k-1,t)/(t(i+k)-t(i))+(t(i+k+1)-u)*N(u,i+1,k-1,t)/(t(i+k+1)-t(i+1));

}

VecPosition CBpline::Qu(double u, int n, VecPosition * CP, double * weight)
{

	VecPosition Q;
	double Qx=0,Qy=0;
	double Q_den=0;
	int k=Def_k;

	for(int j=0;j<n+2;j++)
	{	
		
		Qx+=CP[j].getX()*N(u,j,k)*weight[j];
		Qy+=CP[j].getY()*N(u,j,k)*weight[j];	//Qu=sigema B(i)*Nu(i)*weight(i)
		Q_den+=N(u,j,k)*weight[j];		//Q_den=sigema Nu(i)*weight(i)	
	}

	Q.setX(Qx/Q_den);
	Q.setY(Qy/Q_den);

	return Q;
}

void CBpline::GetT(int n)
{
	int k=Def_k;
		T=new float[n+k+1];
		
	for(int j=0;j<n+k+1;j++)
	{
		T[j]=(float)(j+1);
	}
}