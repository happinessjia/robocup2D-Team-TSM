/*
Copyright (c) 2000-2003, Jelle Kok, University of Amsterdam
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the University of Amsterdam nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! \file PlayerTeams.cpp
<pre>
<b>File:</b>          PlayerTest.cpp
<b>Project:</b>       Robocup Soccer Simulation Team: UvA Trilearn
<b>Authors:</b>       Jelle Kok
<b>Created:</b>       10/12/2000
<b>Last Revision:</b> $ID$
<b>Contents:</b>      This file contains the class definitions for the
                      Player that are used to test the teams' high level
                      strategy.
<hr size=2>
<h2><b>Changes</b></h2>
<b>Date</b>             <b>Author</b>          <b>Comment</b>
10/12/2000        Jelle Kok       Initial version created
</pre>
*/

#include "Player.h"
#include "Global.h"
#include "CommunicationSystem.h"
#include "InterceptUtil.h"

/*!This method is the first complete simple team and defines the actions taken
   by all the players on the field (excluding the goalie). It is based on the
   high-level actions taken by the simple team FC Portugal that it released in
   2000. The players do the following:
   - if ball is kickable
       kick ball to goal (random corner of goal)
   - else if i am fastest player to ball 
       intercept the ball
   - else
       move to strategic position based on your home position and pos ball */

//
VecPosition posTeammateBeforeOneCyc[7];
VecPosition posOpponentBeforeOneCyc[7];
VecPosition posTeammateAfterOneCycle[7];
VecPosition posOpponentAfterOneCycle[7];
VecPosition velTeammateAfterOneCycle[7];
VecPosition velOpponentAfterOneCycle[7];
//

SoccerCommand Player::deMeer5(  )
{
	
	SoccerCommand soc(CMD_ILLEGAL);
	VecPosition   posAgent = WM->getAgentGlobalPosition();
	VecPosition   posBall  = WM->getBallPos();
//init	
	//load position_cp
	if (WM->isBeforeKickOff())
		position_cp = position_cp_start;
	else if (WM->isBallInOurPossesion())
		{
		if (g_ballPos.getX() > 0.0)
			position_cp = position_cp_A;
		else
			position_cp = position_cp_M;
		}
	else
		{
		if (g_ballPos.getX() > 0.0)
			position_cp = position_cp_M;
		else
			position_cp = position_cp_D;
		}
		
	//end load position_cp
	//

		//opponent
	int iIndex;
	for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_OPPONENTS, 0 );
		o != OBJECT_ILLEGAL;
		o = WM->iterateObjectNext ( iIndex, OBJECT_SET_OPPONENTS, 0 ) )
		{
		VecPosition posObject = WM->getGlobalPosition(o);
		VecPosition velObject = WM->getGlobalVelocity(o);
		int num = SoccerTypes::getIndex(o);
		posOpponentAfterOneCycle[num] = posObject + posObject 
			- posOpponentBeforeOneCyc[num];
		velOpponentAfterOneCycle[num] = velObject;
		posOpponentBeforeOneCyc[num] = posObject;
		}
	WM->iterateObjectDone( iIndex );
		//end opponent
		//teammate
	for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_TEAMMATES, 0 );
		o != OBJECT_ILLEGAL;
		o = WM->iterateObjectNext ( iIndex, OBJECT_SET_TEAMMATES, 0 ) )
		{
		VecPosition posObject = WM->getGlobalPosition(o);
		VecPosition velObject = WM->getGlobalVelocity(o);
		int num = SoccerTypes::getIndex(o);
		if (canKick(o))
			{
			posTeammateAfterOneCycle[num] = posObject + velObject;
			velTeammateAfterOneCycle[num] = velObject*SS->getPlayerDecay();
			}
		else
			{
			posTeammateAfterOneCycle[num] = posObject + posObject 
				- posTeammateBeforeOneCyc[num];
			velTeammateAfterOneCycle[num] = velObject;
			}
		posTeammateBeforeOneCyc[num] = posObject;
		}
	WM->iterateObjectDone( iIndex );
		//end teammate
	//
//end init	
	 if( WM->isBeforeKickOff( ) ==true)
  {
  int timestop=0;
  if(WM->isTimeStopped()==true)
   timestop=g_cyc;
   int timenow=g_cyc;
  //VecPosition strpos=WM->getStrategicPosition();
    if( WM->isKickOffUs( ) && WM->getAgentObjectType()==OBJECT_TEAMMATE_5) 
    	{

        return Function_rule();
	}
    else if( (formations->getFormation() != FT_INITIAL || g_agentPos.getDistanceTo( g_strpos ) > 0.5)
		&& (timenow-timestop<=2))  
    {
         formations->setFormation( FT_INITIAL );    
	  soc=teleportToPos( g_strpos );
         ACT->putCommandInQueue( soc);
	  return soc;
    }
    else                                          
        return Dead_Ball_Move();
  }

	else
	{

	/////////////////////////////////////////////
	
	if(WM->isDeadBallUs()==true)
         	{
      	    	 ObjectT goalie=WM->getOppGoalieType();
		  VecPosition pos=WM->getGlobalPosition(goalie);
    			if(pos.getDistanceTo(g_ballPos)<2)
    				{
				soc=moveToPos(g_strpos, 7, 1, false, 1);
				ACT->putCommandInQueue( soc);
	  			return soc;
				}
			else
  	  	return Function_rule();
		}
	  else if(WM->isDeadBallThem()==true)
	  	{
	      ObjectT oppgoalie=WM->getOppGoalieType();
		  VecPosition pos=WM->getGlobalPosition(oppgoalie);
    			if(pos.getDistanceTo(g_ballPos)<2)
    				{
				soc=moveToPos(g_strpos, 7, 1, false, 1);
				ACT->putCommandInQueue( soc);
	  			return soc;
				}
			else
  			return Dead_Ball_Move();
		}
	 else if(Defend_Mode()==true)
	  	{
		return Defend();
	  	}
  	  else
   		
	///////////////////////////////////////////
		{
		if(I_VS_Goalie())
			return somewhat_smart_attacker();
		else
     		return generateAction();
  	  	}
	}
	return soc;
}
SoccerCommand Player::deMeer5_goalie(  )
{
SoccerCommand soc;
	static const VecPosition lookatposbot(-47,PITCH_WIDTH/2);
	static const VecPosition lookatpostop(-47,-PITCH_WIDTH/2);
	 VecPosition basepos(-47,0);
	static const VecPosition A(-52.5,-9);
	static const VecPosition C(-47,-9);
	static const VecPosition B(-52.5,9);
	static const VecPosition D(-47,9);
	static const VecPosition goal(-PITCH_LENGTH/2,0);
	Rect rect=Rect(VecPosition(-PITCH_LENGTH/2,-PENALTY_AREA_WIDTH/2),
				VecPosition(-PITCH_LENGTH/2+PENALTY_AREA_LENGTH,PENALTY_AREA_WIDTH/2));
	Line ball_goal=Line::makeLineFromTwoPoints(g_ballPos, goal);
	static Line CD=Line::makeLineFromTwoPoints(C, D);
	static Line AC=Line::makeLineFromTwoPoints(A, C);
	static Line BD=Line::makeLineFromTwoPoints(B, D);
	VecPosition movetopos=CD.getIntersection(ball_goal);
	if(movetopos.getY()<C.getY())
		movetopos=AC.getIntersection(ball_goal);
	if(movetopos.getY()>D.getY())
		movetopos=BD.getIntersection(ball_goal);
	if(movetopos.getY()<C.getY()&&g_ballPos.getY()<-24)//防止我方守门员跑的太靠底，
		movetopos=C;
	if(movetopos.getY()>D.getY()&&g_ballPos.getY()>24)//beast禁区外大脚传中，来不及跑
		movetopos=D;
if( WM->isBeforeKickOff( ) ==true)
	{
		
		if( formations->getFormation() != FT_INITIAL||basepos.getDistanceTo(g_agentPos)>0.5)  
			
		{
			formations->setFormation( FT_INITIAL );       
			soc=teleportToPos(basepos) ;
			ACT->putCommandInQueue( soc);
			return soc;
		}
		else                                            
		{
                     soc = turnBodyToPoint(g_ballPos, 1 );
			ACT->putCommandInQueue( soc);
			return soc;
			
		}
		
	}
	
/////////////////////////////////////////////////////////the second part.
	else if( WM->getPlayMode() == PM_PLAY_ON || WM->isDeadBallThem()==true)               
	{     
		//VecPosition movetopos=goalie_Movepos();
		double d=g_agentPos.getDistanceTo(movetopos);
		if( WM->isBallCatchable() )
		{
                     soc = catchBall();
			ACT->putCommandInQueue( soc );
			return soc;
		}
		else if(WM->isBallKickable())
			{
			soc=kickTo(VecPosition(0,0), 2);
			ACT->putCommandInQueue( soc );
			return soc;
			}
		else if( intercept_goalie()==true)
		{
		      soc=intercept(true);
			 ACT->putCommandInQueue( soc );
			return soc;
			//return Action_Intercept();
		}
	
		/*else if(interceptScoring_goalie()==true)
		{
			Line balltra=Line::makeLineFromPositionAndAngle(g_ballPos, WM->getBallDirection());
			VecPosition posIntersectgoalieline = goalieline.getIntersection( balltra);
			if(g_ballPos.getX()<=-PITCH_LENGTH/2+SS->getCatchableAreaL())
			{
			if(g_ballPos.getY()<-SS->getGoalWidth()/2.0)
				{
				Line end=Line::makeLineFromTwoPoints(posleftgoal, posLeftgoalie);
				posIntersectgoalieline=end.getIntersection(balltra);	
				}
			else if(g_ballPos.getY()>SS->getGoalWidth()/2.0)
				{
				Line end=Line::makeLineFromTwoPoints(posrightgoal, posRightgoalie);
				posIntersectgoalieline=end.getIntersection(balltra);	
				}
			else
				{
				Line end=Line::makeLineFromTwoPoints(posrightgoal, posleftgoal);
				posIntersectgoalieline=end.getIntersection(balltra);	
				}
			}
			if(posIntersectgoalieline.getY()>SS->getGoalWidth()/2.0)
				posIntersectgoalieline=posRightgoalie;
			if(posIntersectgoalieline.getY()<-SS->getGoalWidth()/2.0)
				posIntersectgoalieline=posLeftgoalie;
			//if(fabs(g_agentPos.getX()-posIntersectgoalieline.getX())>2)
				//{
				soc=moveToPos(posIntersectgoalieline, 3, 0, 0,1);			
				ACT->putCommandInQueue( soc );
				return soc;
				//}
			//else
				//{
			//	soc=moveToPosAlongLine(posIntersectgoalieline, 90, 0.5, 1, 5, 2);
				//soc=dashToPoint(posIntersectgoalieline, 1);
			//	ACT->putCommandInQueue( soc );
			//	return soc;
			//	}
			
		}*/
		else if(rect.isInside(g_ballPos))
		{
			
                 		//soc=dashToPoint(movetopos, 1);
                 		if(movetopos.getX()<-47.5)
					{
					soc=moveToPos(movetopos, 7, 2.5, false, 1);
		      			ACT->putCommandInQueue( soc );
		     			 return soc;
					}
				else
					{
                   			soc=moveToPosAlongLine(movetopos, 90, 0.5, -1, 2, 2);
		      			ACT->putCommandInQueue( soc );
		     		 	return soc;
					 }
		}

		

		else
		{
			
			if(rect.isInside(g_ballPos)==false)
				{
				soc=moveToPos(movetopos, 7, 2.5, false, 1);
		      		ACT->putCommandInQueue( soc );
		     		 return soc;
				}
			else if(g_ballPos.getY()>=0)
				{
                     	soc = turnBodyToPoint( lookatposbot, 1 );
				ACT->putCommandInQueue( soc);
				return soc;
				}
			else 
				{
                     	soc = turnBodyToPoint( lookatpostop, 1 );
				ACT->putCommandInQueue( soc);
				return soc;
				}
		}
		
	}

	 

///////////////////////////////////////////////////////////the third part. 
	else if(WM->isDeadBallUs()==true)
		return Function_rule();
	else
	{
	soc = turnBodyToPoint( g_ballPos,1);
	ACT->putCommandInQueue( soc );
	return soc;
	}	
}

	
	




bool Player::intercept_goalie()
{
if(g_ballPos.getX()>-30)
	return false;
Rect rect=Rect(VecPosition(-PITCH_LENGTH/2,-PENALTY_AREA_WIDTH/2),
	VecPosition(-PITCH_LENGTH/2+PENALTY_AREA_LENGTH,PENALTY_AREA_WIDTH/2));
//VecPosition goal(-PITCH_LENGTH/2,0);
//Rect rectdanger=Rect(VecPosition(-47,-5),VecPosition(-42,5));
//ObjectT  obj;
VecPosition posIntercept;
int me;
WM->predictCommandToInterceptBall(OBJECT_TEAMMATE_1, SoccerCommand(CMD_ILLEGAL), &me, & posIntercept , 
	NULL, NULL, NULL);
ObjectT fastestopp;
WM->predictFastestOppInterCyc_2(g_ballPos, g_ballVel, & fastestopp);
int fastopp;
VecPosition opp;
WM->predictCommandToInterceptBall(fastestopp, SoccerCommand(CMD_ILLEGAL), &fastopp, 
	&opp, NULL, NULL,  NULL);
//VecPosition pos= predict_interceptballpos(& obj);
if((rect.isInside(posIntercept)==true) &&(me<=fastopp-2))
	return true;
else if(g_ballPos.getDistanceTo(g_agentPos)<4)
	return true;
else
	return false;
			
}

/*SoccerCommand Player::generateAction()
{
 SoccerCommand soc;
 ActionT mode=determineActionMode();
 VecPosition posagent=WM->getAgentGlobalPosition();
 VecPosition posstra=WM->getStrategicPosition();
 ObjectT markopp;
  VecPosition  needestmarkpos=WM->getGlobalPosition(WM->getMarkneedest_opp());
 VecPosition  needermarkpos=WM->getGlobalPosition(WM->getMarkneeder_opp());
 if(WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, needestmarkpos)==WM->getAgentObjectType())
 	markopp=WM->getMarkneedest_opp();
 else
 	markopp=WM->getMarkneeder_opp();
 if(mode==ACT_KICK_BALL)
 	soc=determineActionWithBall();
 else if(mode==ACT_INTERCEPT)
 	soc=intercept(false);
 
 else if(mode==ACT_MARK)
 	soc=mark(markopp, 3.0,MARK_BALL);//这个函数的参数待定
 else if(mode==ACT_GOTO_STRATEGIC_POSITION)
 	soc=moveToPos(posstra, 5, 0.0, false,  1); //这个函数的参数待定
 else if(mode==ACT_WATCH_BALL)
 	soc=turnBodyToPoint(OBJECT_BALL);
 else
 	soc = SoccerCommand(CMD_ILLEGAL);
 return soc;
}*/


////////////////////////////////////////////////////////////

SoccerCommand Player::generateAction()
{
/*////////////////////////////////////////////////////////////////
//这部分只有在我方其他对于可以踢到球的周期触发，目的是让踢球队员的预测更准确。
//而且在我方有人踢球时，我做这个动作也是合理的
//经检验，这样没有使预测更加准确。
static int   timeLastMove=-1;
double dDist;
ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_TEAMMATES_NO_GOALIE, OBJECT_BALL, &dDist, -1);
if(!(obj==WM->getAgentObjectType())&&dDist<WM->getMaximalKickDist( obj )&&(WM->getCurrentCycle()-timeLastMove)>=3)
{
timeLastMove=WM->getCurrentCycle();
SoccerCommand soc(CMD_ILLEGAL);
ACT->putCommandInQueue( soc );
return soc;
}
///////////////////////////////////////////////////////////////*/
SoccerCommand soc(CMD_ILLEGAL);
ActionT mode=determineActionMode();
 if(mode==ACT_KICK_BALL)
         return determineActionWithBall();
else if(mode==ACT_INTERCEPT)
 	  return Action_Intercept();
/*else if(mode==ACT_MARK)
{
ObjectT markopp(OBJECT_ILLEGAL);
  VecPosition  needestmarkpos=WM->getGlobalPosition(WM->getMarkneedest_opp());
 VecPosition  needermarkpos=WM->getGlobalPosition(WM->getMarkneeder_opp());
soc=mark(markopp, 3.0,MARK_BALL);
ACT->putCommandInQueue( soc );
 	return soc;
}*/
else if(mode==ACT_GOTO_STRATEGIC_POSITION)
{
if(isMove_Attack())
	return Move_Attack();
else
	 return Action_goto_strpos();
}
 	 
else
 	{ 
 	
 	soc=turnBodyToObject(OBJECT_BALL);
 	ACT->putCommandInQueue( soc );
 	return soc;
	}
}
SoccerCommand Player::Action_goto_strpos()
{
SoccerCommand soc;
//VecPosition strpos=WM->getStrategicPosition();

double dist;
double dist_to_ball=g_agentPos.getDistanceTo(g_ballPos);
		ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, g_strpos, &dist, PS->getPlayerConfThr());
		//VecPosition posmove=Position_Move_aroundpos(g_strpos, 0);
		//if(posmove!=NULL)
			//{
			//	soc=moveToPos(posmove, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			//	ACT->putCommandInQueue(soc);
			//	return soc;
			//}
	if(g_agentPos.getX()>36)
		{
		if(dist<(PS->getMarkDistance()+1.5)&&dist_to_ball>12)
			{
			soc=mark(obj, PS->getMarkDistance(), MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		else if(dist<(PS->getMarkDistance()+1.5)&&dist_to_ball<=12)
			
			{
			soc=mark(obj, PS->getMarkDistance(), MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		
		else
			{
			soc=moveToPos(g_strpos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
			}
		}
	else
		{
		soc=moveToPos(g_strpos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
		}
}
SoccerCommand Player::Move_Attack()
{
SoccerCommand soc;

	position_cp=position_cp_Attack;
	int  iCyclesToIntercept;
	ObjectT fastest=WM->getFastestInSetTo(OBJECT_SET_PLAYERS, OBJECT_BALL, & iCyclesToIntercept);
	VecPosition fastestpos=WM->getGlobalPosition(fastest);
	ObjectT iMyself=WM->getAgentObjectType();
	VecPosition obj6=WM->getGlobalPosition(OBJECT_TEAMMATE_6);
	VecPosition obj7=WM->getGlobalPosition(OBJECT_TEAMMATE_7);
	if(iMyself==OBJECT_TEAMMATE_7&&fastest==OBJECT_TEAMMATE_6)
	{
		double dist;
		VecPosition pos;
		if(fastestpos.getX()>42)//六号拿球时，防止7号位子和他重合，
		  pos=WM->getStrategicPosition(OBJECT_TEAMMATE_6);
		else
		  pos=WM->getStrategicPosition(OBJECT_TEAMMATE_7);
		ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, pos, &dist, PS->getPlayerConfThr());
		if(dist<(PS->getMarkDistance()+1.5))
			{
			soc=mark(obj, PS->getMarkDistance(), MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		else
			{
			soc=moveToPos(pos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
			}
	}
	else if(iMyself==OBJECT_TEAMMATE_2&&fastestpos.getY()>12)
		{
		double dist;
		VecPosition pos=WM->getStrategicPosition(OBJECT_TEAMMATE_7);
		ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, pos, &dist, PS->getPlayerConfThr());
		if(dist<(PS->getMarkDistance()+1.5))
			{
			soc=mark(obj, PS->getMarkDistance(), MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		else
			{
			soc=moveToPos(pos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
			}
		}
	else if(iMyself==OBJECT_TEAMMATE_3&&fastestpos.getY()<-12)
		{
		double dist;
		VecPosition pos=WM->getStrategicPosition(OBJECT_TEAMMATE_7);
		ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, pos, &dist, PS->getPlayerConfThr());
		if(dist<(PS->getMarkDistance()+1.5))
			{
			soc=mark(obj, PS->getMarkDistance(), MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		else
			{
			soc=moveToPos(pos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
			}
		}
	else if(iMyself==OBJECT_TEAMMATE_7&&g_agentPos.getX()<obj6.getX())
	{
		double dist;
		VecPosition pos=WM->getStrategicPosition(OBJECT_TEAMMATE_6);
		ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, pos, &dist, PS->getPlayerConfThr());
		if(dist<(PS->getMarkDistance()+1.5))
			{
			soc=mark(obj, PS->getMarkDistance(), MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		else
			{
			soc=moveToPos(pos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
			}
	}
	else if(iMyself==OBJECT_TEAMMATE_6&&g_agentPos.getX()>obj7.getX())
	{
		double dist;
		VecPosition pos=WM->getStrategicPosition(OBJECT_TEAMMATE_7);
		ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, pos, &dist, PS->getPlayerConfThr());
		if(dist<(PS->getMarkDistance()+1.5))
			{
			soc=mark(obj, PS->getMarkDistance(), MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		else
			{
			soc=moveToPos(pos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
			}
	}
	else
		return Action_goto_strpos();

}
bool Player::isMove_Attack()
{
int  iCyclesToIntercept;
ObjectT iMyself=WM->getAgentObjectType();
ObjectT fastest=WM->getFastestInSetTo(OBJECT_SET_PLAYERS, OBJECT_BALL, & iCyclesToIntercept);
bool a=(fastest==OBJECT_TEAMMATE_2||fastest==OBJECT_TEAMMATE_3||fastest==OBJECT_TEAMMATE_6
||fastest==OBJECT_TEAMMATE_7);
bool b=(g_ballPos.getX()>25);
bool c=(iMyself==OBJECT_TEAMMATE_2||iMyself==OBJECT_TEAMMATE_3||iMyself==OBJECT_TEAMMATE_6
||iMyself==OBJECT_TEAMMATE_7);
if(a&&b&&c)
	return true;
else
	return false;
														
}
ActionT  Player::determineActionMode()
{
 int iTmp;
 VecPosition agentPos = WM->getAgentGlobalPosition();
  if(WM->isBallKickable())
  	return ACT_KICK_BALL;
  else if(WM->getFastestInSetTo(OBJECT_SET_TEAMMATES, OBJECT_BALL, & iTmp)
  	==WM->getAgentObjectType())
  	return ACT_INTERCEPT;
  	
/*  else if(shouldIMark()==true)
  	return ACT_MARK;*/
  else if(agentPos.getDistanceTo(WM->getStrategicPosition())>1)
  	return ACT_GOTO_STRATEGIC_POSITION;
  else
  	return ACT_WATCH_BALL;
	
 }


//this function is not finished.
bool  Player::shouldIMark()


{

  VecPosition posagent=WM->getAgentGlobalPosition();

  VecPosition  needestmarkpos=WM->getGlobalPosition(WM->getMarkneedest_opp());
 VecPosition  needermarkpos=WM->getGlobalPosition(WM->getMarkneeder_opp());
  if (WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, needestmarkpos)==WM->getAgentObjectType()
  	&&needestmarkpos.getX()<-PITCH_LENGTH/4)
  	return 1;
  else if(WM->getClosestInSetTo(OBJECT_SET_TEAMMATES, needermarkpos)==WM->getAgentObjectType()
  	&&needermarkpos.getX()<-PITCH_LENGTH/4)
return 1;
  else 
  	return 0;
}


//this function is not good enough.
SoccerCommand Player::determineActionWithBall()
{
	SoccerCommand soc(CMD_ILLEGAL);
	VecPosition posAgent = WM->getAgentGlobalPosition();
//	first see if i can shoot
	soc = cmdShoot();
	if (soc.commandType != CMD_ILLEGAL)
		{
		ACT->putCommandInQueue(soc);
		return soc;
		}
//	if can not shoot ,do bestKickCmd
	soc = bestKickCmd(100.0);
	if (soc.commandType != CMD_ILLEGAL)
		{
		ACT->putCommandInQueue( soc );
		return soc;
		}

	soc = holdBall();
	ACT->putCommandInQueue( soc );
	return soc;

}
//cmdShoot()


SoccerCommand Player::cmdShoot()
{
	if (g_ballPos.getX() < 30.0)
		return SoccerCommand(CMD_ILLEGAL);
	SoccerCommand soc(CMD_ILLEGAL);
	VecPosition posBallA, velBallA;
	double pro;
	VecPosition posShoot;
	ObjectT iMyself = WM->getAgentObjectType();
	VecPosition posMe = WM->getAgentGlobalPosition();
//first check whether the first point is ok
	bool testOk = true;
	VecPosition posInterceptGoalie;
	posShoot = shootPosition_II(g_ballPos, &pro);
	if (pro < 0.9)
		return SoccerCommand(CMD_ILLEGAL);
	soc = kickTo(posShoot, SS->getBallSpeedMax());
	WM->predictBallInfoAfterCommand(soc, &posBallA, &velBallA);
	if ((posBallA - posMe).getMagnitude() < WM->getMaximalKickDist(iMyself)*0.9)
		return soc;
	WM->predictCommandToInterceptBall_2(OBJECT_OPPONENT_1, 
		SoccerCommand(CMD_ILLEGAL), 
		NULL, &posInterceptGoalie, NULL, NULL, NULL, posBallA, velBallA);
	if (WM->isInField(posInterceptGoalie, 0.1))
		testOk = false;	
	if (testOk == true)		//check if somebody else will get in the way
		{
		int iIndex;
		VecPosition posOpp;
		AngDeg angToBall,angBall = velBallA.getDirection();
		AngDeg angDelta, angDeltaMin = 180;
		AngDeg angBallA = velBallA.getDirection();
		ObjectT objDanger;
		VecPosition posInterceptSomebodyElse;
		for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_OPPONENTS, 0 );
			o != OBJECT_ILLEGAL;
			o = WM->iterateObjectNext ( iIndex, OBJECT_SET_OPPONENTS, 0 ) )
			{
			if (o == OBJECT_OPPONENT_1)
				continue;
			else
				{
				posOpp = WM->getGlobalPosition(o);
				angToBall = (posOpp - g_ballPos).getDirection();
				angDelta = fabs(VecPosition::normalizeAngle(angBallA - angToBall));
				if (angDelta < angDeltaMin)
					angDeltaMin = angDelta;
				objDanger = o;
				}
			}
		WM->iterateObjectDone( iIndex );
		if (angDeltaMin > 10.0)
			return soc;
		WM->predictCommandToInterceptBall_2(objDanger, 
			SoccerCommand(CMD_ILLEGAL), 
			NULL, &posInterceptSomebodyElse, NULL, NULL, NULL, posBallA, velBallA);
		if (!WM->isInField(posInterceptSomebodyElse, 0.1))
			return soc;
		}
	double y = posShoot.getY();
	if (y > 6.6 || y < -6.6)
		return SoccerCommand(CMD_ILLEGAL);
	else if (y > 0)
		y = (y + 7.0)/2.0;
	else
		y = (y - 7.0)/2.0;
	posShoot = VecPosition(PITCH_LENGTH/2.0, y);
	if (getgoalpro_II(posShoot, g_ballPos) < 0.9)
		return SoccerCommand(CMD_ILLEGAL);
	else
		{
		VecPosition posInterceptGoalie2;
		soc = kickTo(posShoot, SS->getBallSpeedMax());
		WM->predictBallInfoAfterCommand(soc, &posBallA, &velBallA);
		if ((posBallA - posMe).getMagnitude() < WM->getMaximalKickDist(iMyself)*0.9)
			return soc;
		WM->predictCommandToInterceptBall_2(OBJECT_OPPONENT_1, 
			SoccerCommand(CMD_ILLEGAL), 
			NULL, &posInterceptGoalie2, NULL, NULL, NULL, posBallA, velBallA);
		if (!WM->isInField(posInterceptGoalie2, 0.1))
			return soc;
		else
			return SoccerCommand(CMD_ILLEGAL);
		}
}



//////////////////////////////////////////////////////////
SoccerCommand Player::Action_Intercept()
{
SoccerCommand soc(CMD_ILLEGAL);
//ObjectT  obj;
/*
ObjectT iMyself=WM->getAgentObjectType();
if(obj==iMyself)

{*/


       soc=intercept(false);
	ACT->putCommandInQueue( soc );
       return soc;
/*}
else
{
VecPosition posIntercept =predict_interceptballpos(& obj);
 soc=moveToPos(posIntercept, PS->getPlayerWhenToTurnAngle(), 1, false, 0);
ACT->putCommandInQueue(soc);
return soc;

}*/
}

////////////////////////////////////////////

VecPosition Player::predict_interceptballpos(ObjectT *obj)
{

ObjectT  fastestopp;
int fastopp;
VecPosition  posInterceptOpp;
ObjectT fastestteammate;
int fastus;
VecPosition  posInterceptUs;
WM->predictFastestOppInterCyc_2(g_ballPos, g_ballVel, &fastestopp);
WM->predictFastestTeammateInterCyc_2(g_ballPos, g_ballVel, &fastestteammate);
WM->predictCommandToInterceptBall_2(fastestopp, SoccerCommand(CMD_ILLEGAL), &fastopp, &posInterceptOpp, 
															NULL, NULL, NULL, g_ballPos, g_ballVel);
WM->predictCommandToInterceptBall_2(fastestteammate, SoccerCommand(CMD_ILLEGAL), &fastus, &posInterceptUs, 
															NULL, NULL, NULL, g_ballPos, g_ballVel);
if(fastopp>=fastus)
{
*obj=fastestteammate;
return posInterceptUs;
}
	
else
{
*obj=fastestopp;
return posInterceptOpp;
}


}

double Player::valPosition_II(VecPosition pos)
{
	double val = 0.0;
		double pro;
	VecPosition posGoal(PITCH_LENGTH/2.0,0.0);
		double disToGoal = (posGoal - pos).getMagnitude();
		double x = pos.getX();
		double dx;
		if (x < 46)
			dx = x;
		else
			dx = 46 - (x - 46)*5;
	if (x >= 28.0 && x <= 48.0)
		{
		shootPosition_II(pos, &pro);
		if (pro > 0.6)
			val += pro*10000.0;	//maximum value is returned
		}
	val += 5000.0 + 50.0*dx - 50.0*disToGoal;
	return val;	
}


double Player::getgoalpro_II(VecPosition pos ,VecPosition ori)
{
	VecPosition  goalkeeper=WM->getGlobalPosition(OBJECT_OPPONENT_GOALIE);
	AngDeg agent_pos=(pos-ori).getDirection();
	AngDeg agent_goalkeeper=(goalkeeper-ori).getDirection();
	AngDeg a=fabs(agent_pos-agent_goalkeeper);
	double   b=ori.getDistanceTo(goalkeeper);
	double   u=(a-26.1)*0.043+(b-9.0)*0.09-0.2;
	double   probability=1.0/(1.0+exp(-9.5*u));
	if (b < 13.5 || a > 25.0)
		return probability;
	else
		return probability - (b - 13.5)*0.2;
}


VecPosition Player::shootPosition_II(VecPosition pos,double *maxPro)
{
	double y = -6.6;
	double pro;
	VecPosition point;
	VecPosition bestScoringPoint;
	*maxPro = 0.0;
	if (pos.getX() >= 28.0)
		{
		while (y <= 6.6)
			{
			point= VecPosition(PITCH_LENGTH/2.0,y);
			pro = getgoalpro_II(point, pos);
			if (pro >= 0.9 && *maxPro >= 0.9 && 
				fabs(point.getY()) < fabs(bestScoringPoint.getY()))
					{
					bestScoringPoint = point;
					if (pro > *maxPro)
						*maxPro = pro;
					}
			else if (pro > *maxPro)
				{
				*maxPro = pro;
				bestScoringPoint = point;
				}
				
			y += 0.2;
			}
			return bestScoringPoint;
		}
	else
		{
		*maxPro = 0.0;
		return VecPosition(PITCH_LENGTH/2.0,0.0);
		}
}
///////////////////////////////////////////////////




SoccerCommand Player::bestKickCmd(double a_cyc)
{
	SoccerCommand soc(CMD_ILLEGAL);
	SoccerCommand socTemp;
		VecPosition velocity;
		double speed;
		AngDeg ang;
	VecPosition posBallAfterCmd;
	VecPosition velBallAfterCmd;
		int cycOur;
		int cycOpp;
	VecPosition posIntercept;
		double val;
		double valMax = -30000.0;
	speed = 0.3;
	double max1=-3000,max2=-3000,max3=-3000;
			SoccerCommand 	best1(CMD_ILLEGAL);
			SoccerCommand    best2(CMD_ILLEGAL);
			SoccerCommand	best3(CMD_ILLEGAL);
	while (speed <= SS->getBallSpeedMax())
	{
		ang = -179.0;
		while (ang <= 180.0)
			{
			velocity = VecPosition(speed,ang,POLAR);
			socTemp = kickTo(velocity);
			WM->predictBallInfoAfterCommand(socTemp, &posBallAfterCmd, &velBallAfterCmd);
			cycOpp = predictFastestOppInterCyc_II(posBallAfterCmd, velBallAfterCmd);
			cycOur = predictFastestTeammateInterCyc_II(posBallAfterCmd, velBallAfterCmd);
			int cycDelta = cycOpp - cycOur;
			posIntercept = WM->predictPosAfterNrCycles(OBJECT_BALL, cycOur, 0, 
				&posBallAfterCmd, &velBallAfterCmd);
			double dMargin = min(max(2.0,cycOur*0.3),6.0);
			if (cycDelta < 1 || !WM->isInField(posIntercept, dMargin))
				val = -30000.0;
			else
				{
				VecPosition posGoal(PITCH_LENGTH/2.0,0.0);
				double distanceToGoal = (posIntercept - posGoal).getMagnitude();
				double x = posIntercept.getX();
				val = valPosition_II(posIntercept);
				if (cycDelta <= 3)
					val += a_cyc*(cycDelta-1);
				else
					val += a_cyc*2.0;
				}
			/*if (val > valMax)
				{
				valMax = val;
				soc = socTemp;
				}*/
			//////////////////////////////////////////////
			if(val>max1)
				{
				
				///////////////
				max3=max2;
				best3=best2;
				//
				max2=max1;
				best2=best1;
				//////////////
				max1=val;
				best1=socTemp;
				}
			else if(val>max2)
				{
				////////////
				max3=max2;
				best3=best2;
				/////////
				max2=val;
				best2=socTemp;
				}
			else if(val>max3)
				{
				max3=val;
				best3=socTemp;
				}
			else
				;
			//////////////////////////////////////////////
			if (ang > -90.0 && ang < 90.0)
				ang += 5.0;
			else
				ang += 7.0;
			
			}
		speed += 0.6;
		}
	///////////////////////////////////////
	if(best1.commandType!=CMD_ILLEGAL)
		{	
			VecPosition fastestposour;
			VecPosition fastestposopp;
			predictFastestInterceptballTeammate_aftersoc(&cycOur, &fastestposour,best1);
			ObjectT fastopp=predictFastestInterceptballOppont_aftersoc(&cycOpp, &fastestposopp,best1);
			int cycDelta = cycOpp - cycOur;	
			
			if (cycDelta <=2||(fastopp==WM->getOppGoalieType()&&cycDelta<=4))
				soc=SoccerCommand(CMD_ILLEGAL);
			else if(WM->isInField(fastestposour, 2)==false)
				soc=SoccerCommand(CMD_ILLEGAL);
			else
				soc=best1;
		}
	if(soc.commandType!=CMD_ILLEGAL)
		return soc;
	//////////////////////////////////////
	if(best2.commandType!=CMD_ILLEGAL)
		{	
			VecPosition fastestposour;
			VecPosition fastestposopp;
			predictFastestInterceptballTeammate_aftersoc(&cycOur, &fastestposour,best2);
			ObjectT fastopp=predictFastestInterceptballOppont_aftersoc(&cycOpp, &fastestposopp,best2);
			int cycDelta = cycOpp - cycOur;	
			
			if (cycDelta <=2 ||(fastopp==WM->getOppGoalieType()&&cycDelta<=4))
				soc=SoccerCommand(CMD_ILLEGAL);
			else if(WM->isInField(fastestposour, 2)==false)
				soc=SoccerCommand(CMD_ILLEGAL);
			else
				soc=best2;
		}
	if(soc.commandType!=CMD_ILLEGAL)
		return soc;
	//////////////////////////////////////
	if(best3.commandType!=CMD_ILLEGAL)
		{	
			VecPosition fastestposour;
			VecPosition fastestposopp;
			predictFastestInterceptballTeammate_aftersoc(&cycOur, &fastestposour,best3);
			ObjectT fastopp=predictFastestInterceptballOppont_aftersoc(&cycOpp, &fastestposopp,best3);
			int cycDelta = cycOpp - cycOur;	
			
			if (cycDelta <=0 ||(fastopp==WM->getOppGoalieType()&&cycDelta<=2))
				soc=SoccerCommand(CMD_ILLEGAL);
			else if(WM->isInField(fastestposour, 2)==false)
				soc=SoccerCommand(CMD_ILLEGAL);
			else
				soc=best3;
		}
	if(soc.commandType!=CMD_ILLEGAL)
		return soc;
	//////////////////////////////
	return soc;
}


int Player::predictInterCyc_II(ObjectT o, VecPosition posBall, VecPosition velBall)
{

	interUtil.ballPos = posBall;
	interUtil.angBody = WM->getGlobalBodyAngle(o);
	interUtil.maxSpeed = WM->getPlayerSpeedMax(o);
	interUtil.maxDashEffect = SS->getMaxPower() * WM->getDashPowerRate(o) *WM->getEffortMax(o);	
	int num = SoccerTypes::getIndex(o);	
	if (SoccerTypes::isTeammate(o))
		{
		if ((posBall - posTeammateAfterOneCycle[num]).getMagnitude() 
			< SS->getMaximalKickDist() - 0.1)
			return 0;
		interUtil.playerPos = posTeammateAfterOneCycle[num];
		interUtil.playerVel = velTeammateAfterOneCycle[num];
		if (o==WM->getOwnGoalieType())
			interUtil.kickableArea = SS->getCatchableAreaL();
		else
			interUtil.kickableArea = WM->getMaximalKickDist(o) - 0.2;
		}
	else
		{
		if ((posBall - posOpponentBeforeOneCyc[num]).getMagnitude() 
			< SS->getMaximalKickDist() + 0.1)
			return 0;
		interUtil.playerPos = posOpponentAfterOneCycle[num];
		interUtil.playerVel = velOpponentAfterOneCycle[num];
		if (o==WM->getOppGoalieType())
			interUtil.kickableArea = SS->getCatchableAreaL();
		else
			interUtil.kickableArea = WM->getMaximalKickDist(o) + 0.5;
		}
	return interUtil.getInterceptCyc(velBall);
}


int Player::predictFastestOppInterCyc_II(const VecPosition& posBall, const VecPosition& velBall)
{
	int minCyc = 100;
	
	int iIndex;
	for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_OPPONENTS, 0 );
		o != OBJECT_ILLEGAL;
		o = WM->iterateObjectNext ( iIndex, OBJECT_SET_OPPONENTS, 0 ) )
	{
		int cyc = predictInterCyc_II(o, posBall, velBall);
		if (cyc<minCyc)
			minCyc = cyc;
	}
	WM->iterateObjectDone( iIndex );

	return minCyc;
}

int Player::predictFastestTeammateInterCyc_II(const VecPosition& posBall, const VecPosition& velBall)
{
	int minCyc = 100;
	
	int iIndex;
	for( ObjectT o = WM->iterateObjectStart( iIndex, OBJECT_SET_TEAMMATES_NO_GOALIE, 0 );
		o != OBJECT_ILLEGAL;
		o = WM->iterateObjectNext ( iIndex, OBJECT_SET_TEAMMATES_NO_GOALIE, 0 ) )
	{
		int cyc = predictInterCyc_II(o, posBall, velBall);
		bool onSide = false;
		if (o == WM->getAgentObjectType())
			onSide = true;
		else if (WM->isOnside(o))
			onSide = true;
		if (cyc<minCyc && onSide)
			minCyc = cyc;
	}
	WM->iterateObjectDone( iIndex );

	return minCyc;

}

bool Player::canKick(ObjectT obj)
{
	VecPosition posPlayer = WM->getGlobalPosition(obj);
	VecPosition posBall =  WM->getBallPos();
	if ((posPlayer - posBall).getMagnitude() < SS->getMaximalKickDist())
		return true;
	else
		return false;
}
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////这个函数只能被持球队员调用，其他队员调用可能会出现问题。
//其中soc 为持球队员发送的命令。
//此函数其他队员的位置不采用下一周期模型，如果下一周期模型很
//准确，可以加进此函数。预测会更加准确。否则下一周期模型没有
 //什么意义。
 //我方持球队员不会主动将球传给守门员，所以不考虑我方守门员截球
 //周期。
ObjectT Player::predictFastestInterceptballTeammate_aftersoc(int *cycles,VecPosition *InterceptPos,SoccerCommand soc)
{
	
	
	int mincycles=100;
	ObjectT fastestTeammate;	
	   ////////////////////////////////////////////////////////////
	  VecPosition preballpos;
	  VecPosition preballvel;
	    WM->predictBallInfoAfterCommand( soc, &preballpos, &preballvel);
	   WM->predictFastestTeammateInterCyc_2(preballpos, preballvel,&fastestTeammate);
	   ObjectT o=fastestTeammate;
	   //////////////////////////////////////////////
	 
	ObjectT imyself=WM->getAgentObjectType();
	   if(o==imyself)
	  	{
	  	VecPosition agentposaftersoc;
		VecPosition agentvelaftersoc;
		AngDeg  angGlobalBody;
		AngDeg  angGlobalNeck;
		Stamina  sta;	
		 int iCyclesme;
		 VecPosition pre_interceptposme;
	  	WM->predictAgentStateAfterCommand(soc, &agentposaftersoc, &agentvelaftersoc, & angGlobalBody, &angGlobalNeck, & sta);
	  	WM->predictCommandToInterceptBall_2(o, SoccerCommand(CMD_ILLEGAL),&iCyclesme, &pre_interceptposme, &agentposaftersoc, 
	  	&agentvelaftersoc, &angGlobalBody, preballpos, preballvel);
		*cycles=iCyclesme;
		 *InterceptPos=pre_interceptposme;
	  	}
	  else
	  	{
	  	//在我可踢球那个周期，假设我方//除守门员//其他队员CMD_ILLEGAL，
	  	//为了配合这个预测，其他队员在该周期确实做这个动作。	  	
		VecPosition agentposaftersoc=NULL;
		VecPosition agentvelaftersoc=NULL;
		AngDeg  angGlobalBody=NULL;
		AngDeg  angGlobalNeck=NULL;
		Stamina  sta=NULL;	
		WM->predictObjectStateAfterCommand(o,SoccerCommand(CMD_ILLEGAL), &agentposaftersoc, &agentvelaftersoc, & angGlobalBody, &angGlobalNeck, &sta);
		/////////////////////////////////////////////
		int iCyclesother;
	  	 VecPosition pre_interceptposother;
	  	WM->predictCommandToInterceptBall_2(o, SoccerCommand(CMD_ILLEGAL),&iCyclesother, &pre_interceptposother, &agentposaftersoc, 
	  	&agentvelaftersoc,& angGlobalBody, preballpos, preballvel);
		*cycles=iCyclesother;	 
		*InterceptPos=pre_interceptposother;
	  	}
 return fastestTeammate;
}




/////这个函数只能被持球队员调用，其他队员调用可能会出现问题。
//其中soc 为持球队员发送的命令。
//此函数其他队员的位置不采用下一周期模型，如果下一周期模型很
//准确，可以加进此函数。预测会更加准确。否则下一周期模型没有
 //什么意义。
 

ObjectT Player::predictFastestInterceptballOppont_aftersoc(int *cycles,VecPosition *InterceptPos,SoccerCommand soc)
{
	
	ObjectT fastestOpp;
////////////////////////////////////////////////////////////
	   VecPosition preballpos;
	  VecPosition preballvel;
	    WM->predictBallInfoAfterCommand( soc, &preballpos, &preballvel);
	   WM->predictFastestOppInterCyc_2(preballpos, preballvel,&fastestOpp);	
	   ObjectT o=fastestOpp;
	  
	   /////////////////////////////////////////	
	    ///////////////////////////////////////////////////////////假设在当前周期，对方其他队员什么也没做
		 //这样预测会更准确。
	  VecPosition  pos=WM->getGlobalPosition( o);
		 VecPosition vel=WM->getGlobalVelocity( o);
		 AngDeg  angBody=WM->getGlobalBodyAngle( o);
		 AngDeg angNeck=WM->getGlobalNeckAngle( o);
		 Stamina  sta;
		 WM->predictObjectStateAfterCommand(o,SoccerCommand(CMD_ILLEGAL), &pos, & vel, &angBody, &angNeck, &sta);
		 ///////////////////////////////////////////////////////////////
		  int iCycles;
	  VecPosition pre_interceptpos;	 
	  	WM->predictCommandToInterceptBall_2(o, SoccerCommand(CMD_ILLEGAL),&iCycles, &pre_interceptpos, &pos, 
	  	& vel,&angBody, preballpos, preballvel);		     	
	 //  if(fastestOpp==WM->getOppGoalieType())
	  // *cycles=iCycles-2;
	  // else
	   	*cycles=iCycles;
	   *InterceptPos=pre_interceptpos;
	   return fastestOpp;
}





/////////////////////////////////////////////////////////////
SoccerCommand Player::Function_rule()
{
SoccerCommand soc(CMD_ILLEGAL);
position_cp = position_cp_M;
ObjectT iMyself=WM->getAgentObjectType();
static int stop=0;
if(WM->isGoalKickUs()==true&&g_cyc-stop>25)
 stop=g_cyc;

//VecPosition strpos=WM->getStrategicPosition();
//VecPosition   posAgent = WM->getAgentGlobalPosition();
//the goalkeeper part
if(WM->isBallInOwnPenaltyArea()==true)
{
	
	//the main part
	if(iMyself==OBJECT_TEAMMATE_1)
	{
	if( WM->isBallKickable()==true )
		{
		if( WM->getTimeSinceLastCatch() == 5  )
			{
				static const VecPosition posLeftTop( -PITCH_LENGTH/2.0 +
		                        0.7*PENALTY_AREA_LENGTH, -PENALTY_AREA_WIDTH/4.0 );
	                     static const VecPosition posRightTop( -PITCH_LENGTH/2.0 +
		                         0.7*PENALTY_AREA_LENGTH, +PENALTY_AREA_WIDTH/4.0 );
				if( WM->getNrInSetInCircle( OBJECT_SET_OPPONENTS, 
					Circle(posRightTop, 15.0 )) <
					WM->getNrInSetInCircle( OBJECT_SET_OPPONENTS, 
					Circle(posLeftTop,  15.0 )) )
					soc.makeCommand( CMD_MOVE,posRightTop.getX(),posRightTop.getY(),0.0);
				else
					soc.makeCommand( CMD_MOVE,posLeftTop.getX(), posLeftTop.getY(), 0.0);
				ACT->putCommandInQueue( soc );
				return soc;
			}
		 if( WM->getTimeSinceLastCatch() > 15||(WM->isGoalKickUs()==true&&g_cyc-stop>20)||WM->isFreeKickUs())
		 	{//pass the ball to the best player.we can add in the futrue.it depends the pass function.
			//ObjectT iMyself = WM->getAgentObjectType();
  			double dConfThr = PS->getPlayerConfThr();
  			int    iIndex;
			ObjectSetT set = OBJECT_SET_TEAMMATES_NO_GOALIE;
			double maxVal = -1000000.0;
			ObjectT bestObj = iMyself;
   			 for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
  				{
					VecPosition posTeammate(WM->getGlobalPosition(o));
					double val = 0.0;
					if (o != iMyself)
						{
						if(posTeammate.getX() <= WM->getOffsideX(false) - 0.5 
						|| g_agentPos.getX() >= WM->getOffsideX(false) + 0.5)
							{
							SoccerCommand soc1 = bestPathCmd(o);
							if (soc1.commandType != CMD_ILLEGAL)
							val = passPath(o,1) - 1000.0;
							}
						}
		
					else 
					val = -1000000.0;
					if (val > maxVal)
					{
				maxVal = val;
				bestObj = o;
				}
			
			}
			WM->iterateObjectDone( iIndex );
			VecPosition bestObjpos=WM->getGlobalPosition(bestObj);
			double angle=(bestObjpos-g_agentPos).getDirection()-g_agentBodyAng;
			double realangle=VecPosition::normalizeAngle(angle);
			if(fabs(realangle)>3)
				{
				soc=turnBodyToPoint(bestObjpos, 2);
				ACT->putCommandInQueue(soc);
				return soc;
				}
			
			if(bestPathCmd(bestObj).commandType!=CMD_ILLEGAL)
			{
			soc = bestPathCmd(bestObj);
			ACT->putCommandInQueue(soc);
			return soc;
			}
			else
			{
			ObjectT teammateClosest = WM->getClosestInSetTo(
					OBJECT_SET_TEAMMATES_NO_GOALIE, 
					iMyself, NULL, -1.0);
				ObjectT teammateSecondClosest = WM->getSecondClosestInSetTo(
					OBJECT_SET_TEAMMATES_NO_GOALIE, 
					iMyself, NULL, -1.0);
				if (bestPathCmd(teammateClosest).commandType!=CMD_ILLEGAL)
					{soc = bestPathCmd(teammateClosest);
					ACT->putCommandInQueue( soc );
					return soc;
					}
				if(bestPathCmd(teammateSecondClosest).commandType!=CMD_ILLEGAL)
					{soc = bestPathCmd(teammateClosest); 
					ACT->putCommandInQueue( soc );
					return soc;
					}
				else
					{
					soc=turnBodyToPoint((0,0),  1);
					ACT->putCommandInQueue( soc );
					return soc;
					}
			}
		 	}
		}
	else
		{
		soc=intercept(true);
		ACT->putCommandInQueue( soc );
 		return soc;
		}
	}
	else
	{
	return Dead_Ball_Move();
	}
}
//the common player part.
else
{	
	if(iMyself==OBJECT_TEAMMATE_1)
		{
		VecPosition lookatpos=VecPosition(-47,PITCH_WIDTH/2);
		VecPosition basepos=VecPosition(-47,0);
		 	if( basepos.getDistanceTo(g_agentPos)>0.5)  
			
			{
			      
			soc=moveToPos(basepos,3,0,0,1) ;
			ACT->putCommandInQueue( soc);
			return soc;
			}
			else                                            
			{
                     soc = turnBodyToPoint(lookatpos, 1 );
			ACT->putCommandInQueue( soc);
			return soc;
			
			}
		}
	if(WM->getClosestInSetTo(OBJECT_SET_TEAMMATES_NO_GOALIE, OBJECT_BALL)==iMyself)
	{
		if(WM->isBallKickable())
			{
			double  maxPro;
			VecPosition shootpos=shootPosition(iMyself, & maxPro);
			if(g_agentPos.getX()>34&&maxPro>0.9)
				{
				soc = kickTo(shootpos, SS->getBallSpeedMax());
				ACT->putCommandInQueue(soc);
				return soc;
				}
			//ObjectT iMyself = WM->getAgentObjectType();
  			double dConfThr = PS->getPlayerConfThr();
  			int    iIndex;
			ObjectSetT set = OBJECT_SET_TEAMMATES_NO_GOALIE;
			double maxVal = -1000000.0;
			ObjectT bestObj = iMyself;
    			for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       		o != OBJECT_ILLEGAL;
       		o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
  			{
			VecPosition posTeammate(WM->getGlobalPosition(o));
			double val = 0.0;
			if (o != iMyself)
				{
				if(posTeammate.getX() <= WM->getOffsideX(false) - 0.5 
					|| g_agentPos.getX() >= WM->getOffsideX(false) + 0.5)
					{
					SoccerCommand soc1 = bestPathCmd(o);
					if (soc1.commandType != CMD_ILLEGAL)
						val = passPath(o,1) - 1000.0;
					}
				}
		
			else 
				val = -1000000.0;
			if (val > maxVal)
				{
				maxVal = val;
				bestObj = o;
				}
			
			}
		WM->iterateObjectDone( iIndex );
			VecPosition bestObjpos=WM->getGlobalPosition(bestObj);
			double angle=(bestObjpos-g_agentPos).getDirection()-g_agentBodyAng;
			double realangle=VecPosition::normalizeAngle(angle);
			if(fabs(realangle)>3)
				{
				soc=turnBodyToPoint(bestObjpos, 2);
				ACT->putCommandInQueue(soc);
				return soc;
				}
			
		if(bestPathCmd(bestObj).commandType!=CMD_ILLEGAL)
			{
		soc = bestPathCmd(bestObj);
		ACT->putCommandInQueue(soc);
		return soc;
			}
		else
			{
			ObjectT teammateClosest = WM->getClosestInSetTo(
					OBJECT_SET_TEAMMATES_NO_GOALIE, 
					iMyself, NULL, -1.0);
				ObjectT teammateSecondClosest = WM->getSecondClosestInSetTo(
					OBJECT_SET_TEAMMATES_NO_GOALIE, 
					iMyself, NULL, -1.0);
				if (bestPathCmd(teammateClosest).commandType!=CMD_ILLEGAL)
					{soc = bestPathCmd(teammateClosest);
					ACT->putCommandInQueue( soc );
					return soc;
					}
				if(bestPathCmd(teammateSecondClosest).commandType!=CMD_ILLEGAL)
					{soc = bestPathCmd(teammateClosest); 
					ACT->putCommandInQueue( soc );
					return soc;
					}
				else
					{
					
					soc = turnBodyToPoint((0,0),  1);
					ACT->putCommandInQueue(soc);
					return soc;
					}
			
			}
				
			}
		else
			{
			soc=intercept(false);
			ACT->putCommandInQueue( soc );
 			return soc;
			}
	}
	else
	{
	return Dead_Ball_Move();
	}
}
soc=intercept(false);
ACT->putCommandInQueue( soc );
return soc;
}

//////////////////////////////////////////////////////////



//the players who need not  kick   will do it.
SoccerCommand Player::Dead_Ball_Move()
{
SoccerCommand soc(CMD_ILLEGAL);
if(WM->isDeadBallUs()==true)
	{

		//ObjectT myself=WM->getAgentObjectType();
		//VecPosition strpos=WM->getStrategicPosition(myself);
		double dist;
		ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, g_strpos, &dist, PS->getPlayerConfThr());
		double dist_to_ball=g_agentPos.getDistanceTo(g_ballPos);
		if(dist<(PS->getMarkDistance()+1.5)&&dist_to_ball>12)
			{
			soc=mark(obj, PS->getMarkDistance()/1.5, MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		else if(dist<(PS->getMarkDistance()+1.5)&&dist_to_ball<=12)
			{
			soc=mark(obj, PS->getMarkDistance()+1.0, MARK_BISECTOR);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		
		else
			{
			soc=moveToPos(g_strpos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
			}
		
	
	}
else
	{
	ObjectT  obj;
	if(isMark_deadballthem(&obj)==true)
		{
		return Mark_deadballthem();
		}
	else
		{
		//VecPosition strpos=WM->getStrategicPosition();
		double dist;
		ObjectT obj=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, g_strpos, &dist, PS->getPlayerConfThr());
		if(dist<(PS->getMarkDistance()+1.5))
			{
			soc=mark(obj, PS->getMarkDistance()/4, MARK_BALL);
	 		ACT->putCommandInQueue( soc );
			return soc;
			}
		
		else
			{
			soc=moveToPos(g_strpos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
			ACT->putCommandInQueue(soc);
			return soc;
			}
		}
	}
}
///////////////////////////////////////////////
SoccerCommand Player::bestPathCmd(ObjectT obj)
{
	//VecPosition posBall = WM->getBallPos();
	VecPosition velBall;
	VecPosition posTarget = WM->getGlobalPosition(obj);
	AngDeg angMiddle = (posTarget - g_ballPos).getDirection();
	AngDeg angLow = angMiddle - 15.0;
	AngDeg angHigh = angMiddle + 15.0;
	AngDeg ang;
	int deltaCycMax = -10;
	VecPosition velBest(0.0,0.0);
	SoccerCommand soc(CMD_ILLEGAL);
	ang = angMiddle;
	while(ang <= angHigh)
		{
		for (double speed = 2.7-0.1; speed >= 1.5; speed -= 0.3)
			{
			VecPosition velKick(speed, VecPosition::normalizeAngle(ang), POLAR);
			SoccerCommand socTemp = kickTo(velKick);
			WM->predictBallInfoAfterCommand(socTemp, &g_ballPos, &velBall);
			int opp = WM->predictFastestOppInterCyc(g_ballPos, velBall);
			int our = WM->predictInterCyc(obj, g_ballPos , velBall);

			if (our <= 15 && (opp - our) >= deltaCycMax)
				{
				deltaCycMax = opp - our;
				soc = socTemp;
				}
			}
		if (ang != angMiddle)
			ang += 0.8*fabs(angMiddle - ang);
		else
			ang += 0.5; 
		}
	ang = angMiddle;
	while(ang >= angLow)
		{
		for (double speed = 2.7-0.1; speed >= 1.5; speed -= 0.4)
			{
			VecPosition velKick(speed, VecPosition::normalizeAngle(ang), POLAR);
			SoccerCommand socTemp = kickTo(velKick);
			WM->predictBallInfoAfterCommand(socTemp, &g_ballPos, &velBall);
			int opp = WM->predictFastestOppInterCyc(g_ballPos, velBall);
			int our = WM->predictInterCyc(obj, g_ballPos , velBall);

			if (our <= 15 && (opp - our) >= deltaCycMax)
				{
				deltaCycMax = opp - our;
				soc = socTemp;
				}
			}
		if (ang != angMiddle)
			ang -= 0.8*fabs(angMiddle - ang);
		else
			ang -= 0.5; 
		}
	
	if (deltaCycMax >= 2)
		return soc;
	else
		return SoccerCommand(CMD_ILLEGAL);
}
/////////////////////////////////////////////////////

double Player::passPath(ObjectT obj,int num)
{
//if num == 0 ,this means that we will not allow Mr obj  to give a  pass
//so we will not consider his teammate
//if he is near the goal ,we judge the probability that he can shoot
//else we think his condition is good only if his position is very good
//for example ,no opponents near him,no opponents in front of him

	VecPosition posAgent = WM->getGlobalPosition(obj);
	if (num == 0)
		return valPlayer(obj);


//if num >=1 ,we will do an iteration ,see wo is the best teammate that Mr obj can pass to.
//let obj1,obj2...be all his teammate that he can pass the ball to(call the function canPass())
//we return max(condition(obj,0),condition(obj1,0)),condition(obj2,0)...}
//	ITERATE(
//	MAX{}
//	)
//	RETURN MAX{CONDITION(OBJ,0),CONDITION(OBJMAX,0)*0.8)
else
{
	double dConfThr = PS->getPlayerConfThr();
  	int    iIndex;
	ObjectSetT set = OBJECT_SET_TEAMMATES_NO_GOALIE;
	double maxVal = -10000.0;
	for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       o != OBJECT_ILLEGAL;
       o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
  		{
		double val;
		VecPosition posTarget = WM->getGlobalPosition(o);
		if (o != obj)
			{
			if (canDirectPass(o) 
				&& (posAgent - posTarget).getMagnitude() < PITCH_LENGTH/4.0)
				val = passPath(o , num - 1) - 1000.0;
			else val = 0.0;
			}
		else
			val = passPath(o, 0);
		if (val > maxVal)
			maxVal = val;
			
		}
		WM->iterateObjectDone( iIndex );
  		return maxVal;
}
}
////////////////////////////////////////

double Player::valPlayer(ObjectT obj)
{
	VecPosition posAgent = WM->getGlobalPosition(obj);
	VecPosition posGoal(PITCH_LENGTH/2.0,0);
	double disToGoal = (posGoal - posAgent).getMagnitude();
	double directionToGoal = (posGoal - posAgent).getDirection();
	ObjectSetT set = OBJECT_SET_OPPONENTS;
	ObjectT oppClosest = WM->getClosestInSetTo(set, obj);
	VecPosition posOppClosest = WM->getGlobalPosition(oppClosest);
	double disOppClosest = (posOppClosest - posAgent).getMagnitude();
	if (posAgent.getX() >= 28.0)
		{
		double pro = 0.0;	//maximun of scoring probability
		shootPosition(obj, &pro);
		if (pro >= 0.7)
			return pro*10000.0 + integrate(obj);	//maximum value is returned
		else
			return 5000 - 100*disToGoal+ valFree(obj, 0.0, 100.0) + integrate(obj);
		}

		else
			return 5000 - 100*disToGoal + valFree(obj, directionToGoal, 200) 
			+ 100*disOppClosest + integrate(obj);
}
////////////////////////////////////////////////
double Player::valFree(ObjectT obj,AngDeg ang,double valBase)
{
	VecPosition posPlayer =WM->getGlobalPosition(obj);
	Circle circle(obj,2.0);
	int num1 =WM->getNrInSetInCircle(OBJECT_SET_OPPONENTS, circle);
	int num2 =WM->getNrInSetInCone(OBJECT_SET_OPPONENTS, 0.3, posPlayer, 
		posPlayer + VecPosition(5.0,ang));
	return valBase*(10 - num1*num1 - num2*num2);
}
////////////////////////////////////////////////////////////

double Player::integrate(ObjectT obj)
{
	double dConfThr = PS->getPlayerConfThr();
	double val = 0.0;
	VecPosition posAgent = WM->getGlobalPosition(obj);
	VecPosition point;
	double step = (PITCH_LENGTH/2.0 - posAgent.getX())/10.0;
	point = posAgent;
	while(WM->isInField(point, 0.5))
		{
		val += 0.4*valPosition(point,0);
		point = point + VecPosition(step,0.0);
		}
	point = posAgent;
	while ((point - posAgent).getMagnitude() < 5.0 && WM->isInField(point, 0.5))
		{
		val += 20.0*valPosition(point, 1);
		point = point + VecPosition(0.5,0.0);
		}
	return val;
}
//////////////////////////////////////////////

double Player::valPosition(VecPosition pos,int type)
{
	double dConfThr = PS->getPlayerConfThr();
  	int    iIndex;
	ObjectSetT set = OBJECT_SET_OPPONENTS;
	double val = 0.0;
	if (type == 0)
		{
		for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       		o != OBJECT_ILLEGAL;
       		o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
  				{
				VecPosition posOpp = WM->getGlobalPosition(o);
				val += (posOpp - pos).getMagnitude();
				}
		WM->iterateObjectDone( iIndex );
		}
	else
		{
		for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       		o != OBJECT_ILLEGAL;
       		o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
  				{
				VecPosition posOpp = WM->getGlobalPosition(o);
				double distance = (posOpp - pos).getMagnitude();
				val -= 1.0/(1.0 + distance);
				}
		WM->iterateObjectDone( iIndex );
		}
	return val;
}
///////////////////////////////////////////////////
VecPosition Player::shootPosition(ObjectT obj,double *maxPro)
{
	double y = -6.6;
	double pro;
	VecPosition posAgent =WM->getGlobalPosition(obj);
	VecPosition point;
	VecPosition bestScoringPoint;
	*maxPro = 0.0;
	if (posAgent.getX() >= 28.0)
		{
		while (y <= 6.6)
			{
			point= VecPosition(PITCH_LENGTH/2.0,y);
			pro = getgoalpro(point, obj);
			if (pro >= 0.9 && *maxPro >= 0.9 && 
				fabs(point.getY()) < fabs(bestScoringPoint.getY()))
					{
					bestScoringPoint = point;
					if (pro > *maxPro)
						*maxPro = pro;
					}
			else if (pro > *maxPro)
				{
				*maxPro = pro;
				bestScoringPoint = point;
				}
				
			y += 0.2;
			}
			return bestScoringPoint;
		}
	else
		{
		*maxPro = 0.0;
		return VecPosition(PITCH_LENGTH/2.0,0.0);
		}
}

////////////////////////////////////////////


bool Player::isMark_deadballthem(ObjectT *obj)
{
	int iIndex;
	ObjectSetT set=OBJECT_SET_OPPONENTS;
	double dConfThr = PS->getPlayerConfThr();
	double close1=10000.0;
	double dist=10000.0;
	ObjectT mark_need1=WM->getFastestInSetTo(OBJECT_SET_OPPONENTS, OBJECT_BALL, NULL);
	
///////////////////////////////////////			
/*	ObjectT player1;
	ObjectSetT set1=OBJECT_SET_TEAMMATES_NO_GOALIE;
	double str_close1=10000.0;
	double dist1=10000.0;
	for( ObjectT o = WM->iterateObjectStart( iIndex, set1, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set1, dConfThr ) )
       			{
				VecPosition strpos=WM->getStrategicPosition();
				dist1=strpos.getDistanceTo(WM->getGlobalPosition(mark_need1));
				if(dist1<str_close1)
					{
					str_close1=dist1;
					player1=o;
					}
				}
			WM->iterateObjectDone( iIndex );
*/	
///////////////////////////////////////	
	double close2=10000.0;
	 dist=10000.0;
	ObjectT mark_need2;
		 for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
       			{
				VecPosition deadpos=g_ballPos;
				dist=deadpos.getDistanceTo(WM->getGlobalPosition(o));
				if(dist<close2&&o!=mark_need1&&o!=OBJECT_OPPONENT_1)
					{
					close2=dist;
					mark_need2=o;
					}
				}
			WM->iterateObjectDone( iIndex );
	ObjectT player2;
	ObjectSetT set1=OBJECT_SET_TEAMMATES_NO_GOALIE;
	double dist1=10000.0;
	double str_close2=10000.0;
	for( ObjectT o = WM->iterateObjectStart( iIndex, set1, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set1, dConfThr ) )
       			{
				VecPosition strpos=WM->getStrategicPosition(o);
				dist1=strpos.getDistanceTo(WM->getGlobalPosition(mark_need2));
				if(dist1<str_close2)
					{
					str_close2=dist1;
					player2=o;
					}
				}
			WM->iterateObjectDone( iIndex );

	double close3=10000.0;
	 dist=10000.0;
	ObjectT mark_need3;
		 for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
       			{
				VecPosition deadpos=g_ballPos;
				dist=deadpos.getDistanceTo(WM->getGlobalPosition(o));
				if(dist<close3&&o!=mark_need1&&o!=mark_need2&&o!=OBJECT_OPPONENT_1)
					{
					close3=dist;
					mark_need3=o;
					}
				}
			WM->iterateObjectDone( iIndex );
	ObjectT player3;

	double str_close3=10000.0;
	 dist1=10000.0;
	for( ObjectT o = WM->iterateObjectStart( iIndex, set1, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set1, dConfThr ) )
       			{
				VecPosition strpos=WM->getStrategicPosition(o);
				dist1=strpos.getDistanceTo(WM->getGlobalPosition(mark_need3));
				if(dist1<str_close3&&o!=player2)
					{
					str_close3=dist1;
					player3=o;
					}
				}
			WM->iterateObjectDone( iIndex );	


	double close4=10000.0;
	 dist=10000.0;
	ObjectT mark_need4;
		 for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
       			{
				VecPosition deadpos=g_ballPos;
				dist=deadpos.getDistanceTo(WM->getGlobalPosition(o));
				if(dist<close4&&o!=mark_need1&&o!=mark_need2&&o!=mark_need3&&o!=OBJECT_OPPONENT_1)
					{
					close4=dist;
					mark_need4=o;
					}
				}
			WM->iterateObjectDone( iIndex );
	ObjectT player4;

	double str_close4=10000.0;
	 dist1=10000.0;
	for( ObjectT o = WM->iterateObjectStart( iIndex, set1, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set1, dConfThr ) )
       			{
				VecPosition strpos=WM->getStrategicPosition(o);
				dist1=strpos.getDistanceTo(WM->getGlobalPosition(mark_need4));
				if(dist1<str_close4&&o!=player2&&o!=player3)
					{
					str_close4=dist1;
					player4=o;
					}
				}
			WM->iterateObjectDone( iIndex );	

ObjectT iMyself=WM->getAgentObjectType();
	 if(iMyself==player2)
		{
		*obj=mark_need2;
		return true;
		}
	else if(iMyself==player3)
		{
		*obj=mark_need3;
		return true;
		}
	else if(iMyself==player4)
		{
		*obj=mark_need4;
		return true;
		}
	else
		{
		*obj=OBJECT_ILLEGAL;
		return false;
		}
}
/////////////////////////////////////////////////


SoccerCommand Player::Mark_deadballthem()
{
SoccerCommand soc(CMD_ILLEGAL);
ObjectT mark_need;
if(isMark_deadballthem(&mark_need)==true)
	{
 	soc=mark(mark_need, PS->getMarkDistance()/4, MARK_BALL);
	 ACT->putCommandInQueue( soc );
	 return soc;
	}
else
return CMD_ILLEGAL;
}

/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
bool Player::canDirectPass(ObjectT obj)
{
	int num;
	//VecPosition posBall =WM->getBallPos();
	double dDist;
	WM->getClosestInSetTo(OBJECT_SET_OPPONENTS, 
			g_ballPos, &dDist, -1.0);
	if ((num =WM->getNrInSetInCone(OBJECT_SET_OPPONENTS, 0.2,
		g_ballPos, WM->getGlobalPosition(obj))) <= 1)
	{
		if (num == 0)
			return true;
		else if (dDist < (SS->getBallSpeedMax() - SS->getMaximalKickDist()) 
			&& dDist > SS->getMaximalKickDist())
			return true;
	}
	return false;
}

double Player::getgoalpro(VecPosition pos ,ObjectT obj)
{
VecPosition agentpos=WM->getGlobalPosition(obj);
VecPosition  goalkeeper=WM->getGlobalPosition(OBJECT_OPPONENT_GOALIE);
AngDeg agent_pos=(pos-agentpos).getDirection();
AngDeg agent_goalkeeper=(goalkeeper-agentpos).getDirection();
AngDeg a=fabs(agent_pos-agent_goalkeeper);
double   b=agentpos.getDistanceTo(goalkeeper);
double   u=(a-26.1)*0.043+(b-9.0)*0.09-0.2;
double   probability=1.0/(1.0+exp(-9.5*u));
return probability;
}
//////////////////////////////////////////////

bool Player::Defend_Mode()
{
Rect rect=Rect(VecPosition(-PITCH_LENGTH/2,-PITCH_WIDTH/2),VecPosition(0,PITCH_WIDTH/2));
int N_opp=WM->getNrInSetInRectangle(OBJECT_SET_OPPONENTS,  &rect);
int N_our=WM->getNrInSetInRectangle(OBJECT_SET_TEAMMATES_NO_GOALIE,  &rect);
int our=WM->predictFastestTeammateInterCyc(g_ballPos, g_ballVel);
int opp=WM->predictFastestOppInterCyc(g_ballPos, g_ballVel);
//WM->getFastestInSetTo(OBJECT_SET_TEAMMATES_NO_GOALIE,OBJECT_BALL, &our);
//ObjectT fastopp= WM->getFastestInSetTo(OBJECT_SET_OPPONENTS, OBJECT_BALL, &opp);
//VecPosition fastopppos=WM->getGlobalPosition(fastopp);
//ObjectT obj;
//VecPosition pre_ball=predict_interceptballpos(& obj);
if((N_opp>=3)&&(our>=opp))
	return true;

else if((N_our<=2)&&(our>=opp)&&(g_ballPos.getX()<15.0))
	return true;
else
	return false;
}
////////////////////////////////////////////////

SoccerCommand Player::Defend()
{
SoccerCommand soc(CMD_ILLEGAL);
double dConfThr = PS->getPlayerConfThr();
ObjectT fast1opp;
WM->predictFastestOppInterCyc_2(g_ballPos,g_ballVel, &fast1opp);
//ObjectT fast1opp=WM->getFastestInSetTo(OBJECT_SET_OPPONENTS, OBJECT_BALL, NULL);
VecPosition fast1opppos=WM->getGlobalPosition(fast1opp);
ObjectT iMyself=WM->getAgentObjectType();
/////////////
/*
if(WM->getFastestInSetTo(OBJECT_SET_PLAYERS, OBJECT_BALL, NULL)==iMyself)
{
soc=intercept(false);
ACT->putCommandInQueue( soc);
return soc;
}
*/
////////////
ObjectT player1=OBJECT_ILLEGAL;
ObjectT player2=OBJECT_ILLEGAL;
ObjectT player3=OBJECT_ILLEGAL;
ObjectT player4=OBJECT_ILLEGAL;
ObjectT player5=OBJECT_ILLEGAL;
//////////determine player1
double  dDist;
ObjectT clo_to_ball=WM->getClosestInSetTo(OBJECT_SET_TEAMMATES_NO_GOALIE, OBJECT_BALL, &dDist,  dConfThr);
if(dDist<PS->getMarkDistance()/2)
player1=clo_to_ball;

////////
/////////////determine player2
VecPosition markpos=getMarkingPosition(fast1opp, PS->getMarkDistance()/4, MARK_GOAL);
double d=1000.0;
double d_min=1000.0;
int iIndex;
ObjectSetT set=OBJECT_SET_TEAMMATES_NO_GOALIE;
for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
       			{
       			d=markpos.getDistanceTo(WM->getGlobalPosition(o));
				if(d<d_min&&o!=player1)
					{
					d_min=d;
					player2=o;
					}
				}
			WM->iterateObjectDone( iIndex );


/////////////////////////////determine mark orders.
ObjectSetT set1 = OBJECT_SET_OPPONENTS;
double mark_opps_val_largest=-10000.0;
double mark_opps_val=-10000.0;
ObjectT mark_need1=OBJECT_ILLEGAL;
for( ObjectT o = WM->iterateObjectStart( iIndex, set1, dConfThr );
       o != OBJECT_ILLEGAL;
       o = WM->iterateObjectNext ( iIndex, set1, dConfThr ) )

       	{
       	VecPosition pos_obj=WM->getGlobalPosition( o);
       	if(pos_obj.getX()<-PITCH_LENGTH/6&&o!=fast1opp)
			{
			
			mark_opps_val=Mark_Val(o);
			if(mark_opps_val>mark_opps_val_largest)
				{
				mark_opps_val_largest=mark_opps_val;
				mark_need1=o;
				}
       		}
       	}
	   WM->iterateObjectDone( iIndex );

 mark_opps_val_largest=-10000.0;
 mark_opps_val=-10000.0;
ObjectT mark_need2=OBJECT_ILLEGAL;
for( ObjectT o = WM->iterateObjectStart( iIndex, set1, dConfThr );
       o != OBJECT_ILLEGAL;
       o = WM->iterateObjectNext ( iIndex, set1, dConfThr ) )

       	{
       	VecPosition pos_obj=WM->getGlobalPosition( o);
       	if(pos_obj.getX()<-PITCH_LENGTH/6&&o!=fast1opp&&o!=mark_need1)
			{
			
			mark_opps_val=Mark_Val(o);
			if(mark_opps_val>mark_opps_val_largest)
				{
				mark_opps_val_largest=mark_opps_val;
				mark_need2=o;
				}
       		}
       	}
	   WM->iterateObjectDone( iIndex );
mark_opps_val_largest=-10000.0;
 mark_opps_val=-10000.0;
ObjectT mark_need3=OBJECT_ILLEGAL;
for( ObjectT o = WM->iterateObjectStart( iIndex, set1, dConfThr );
       o != OBJECT_ILLEGAL;
       o = WM->iterateObjectNext ( iIndex, set1, dConfThr ) )

       	{
       	VecPosition pos_obj=WM->getGlobalPosition( o);
       	if(pos_obj.getX()<-PITCH_LENGTH/6&&o!=fast1opp&&o!=mark_need1&&o!=mark_need2)
			{
			
			mark_opps_val=Mark_Val(o);
			if(mark_opps_val>mark_opps_val_largest)
				{
				mark_opps_val_largest=mark_opps_val;
				mark_need3=o;
				}
       		}
       	}
	   WM->iterateObjectDone( iIndex );
///////////////////////////////
//////determine player3 who marks markneed1
double dmark=1000.0;
double dmarkmin=1000.0;
if(mark_need1!=OBJECT_ILLEGAL)
{
VecPosition mark1pos=getMarkingPosition(mark_need1, PS->getMarkDistance()/3, MARK_BALL);
for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       o != OBJECT_ILLEGAL;
       o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )

       	{
       	dmark=mark1pos.getDistanceTo(WM->getGlobalPosition( o));
		if(dmark<dmarkmin&&o!=player1&&o!=player2)
			{
			dmarkmin=dmark;
			player3=o;
			}
       	}
	   WM->iterateObjectDone( iIndex );
}	   
//////////
 //////determine player4 who marks markneed2
 if(mark_need2!=OBJECT_ILLEGAL)
 {
 dmark=1000.0;
 dmarkmin=1000.0;
VecPosition mark2pos=getMarkingPosition(mark_need2, PS->getMarkDistance()/3, MARK_BALL);
for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       o != OBJECT_ILLEGAL;
       o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )

       	{
       	dmark=mark2pos.getDistanceTo(WM->getGlobalPosition( o));
		if(dmark<dmarkmin&&o!=player1&&o!=player2&&o!=player3)
			{
			dmarkmin=dmark;
			player4=o;
			}
       	}
	   WM->iterateObjectDone( iIndex ); 
 }
//////////
 //////determine player5 who marks markneed3
 if(mark_need3!=OBJECT_ILLEGAL)
 {
 dmark=1000.0;
 dmarkmin=1000.0;
VecPosition mark3pos=getMarkingPosition(mark_need3, PS->getMarkDistance()/3, MARK_BALL);
for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       o != OBJECT_ILLEGAL;
       o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )

       	{
       	dmark=mark3pos.getDistanceTo(WM->getGlobalPosition( o));
		if(dmark<dmarkmin&&o!=player1&&o!=player2&&o!=player3&&o!=player4)
			{
			dmarkmin=dmark;
			player5=o;
			}
       	}
	   WM->iterateObjectDone( iIndex ); 
 }
///////////////////////////////////	
if(WM->isBallKickable()==true)
	return determineActionWithBall();
else if(iMyself==player1||determineActionMode()==ACT_INTERCEPT)
return Action_Intercept();
else if(iMyself==player2)
{
soc=mark(fast1opp, PS->getMarkDistance()/4, MARK_GOAL);
ACT->putCommandInQueue( soc);
return soc;
}
else if(iMyself==player3)
{
soc=mark(mark_need1, PS->getMarkDistance()/3, MARK_BALL);
ACT->putCommandInQueue( soc);
return soc;
}
else if(iMyself==player4)
{
soc=mark(mark_need2, PS->getMarkDistance()/3, MARK_BALL);
ACT->putCommandInQueue( soc);
return soc;
}
else if(iMyself==player5)
{
soc=mark(mark_need3, PS->getMarkDistance()/3, MARK_BALL);
ACT->putCommandInQueue( soc);
return soc;
}

else
  return DefendMove();

}
///////////////////////////////////////////////////////////
SoccerCommand Player::DefendMove()
{
SoccerCommand soc;
Rect rect(VecPosition(-PITCH_LENGTH/2,-PITCH_WIDTH/2),VecPosition(0,PITCH_WIDTH/2));
//////////////////////determine dx
Line l=Line::makeLineFromTwoPoints(VecPosition (-PITCH_LENGTH/2,-PITCH_WIDTH/2), VecPosition(-PITCH_LENGTH/2,PITCH_WIDTH/2));
ObjectT frontopp=WM->getClosestInSetTo(OBJECT_SET_OPPONENTS,  l, VecPosition  (-PITCH_LENGTH/2,-PITCH_WIDTH/2), VecPosition (-PITCH_LENGTH/2,PITCH_WIDTH/2), NULL, NULL);
VecPosition frontopp_pos=WM->getGlobalPosition(frontopp);
double dx;
if(frontopp_pos.getX()>g_ballPos.getX())
	dx=g_ballPos.getX()-0.5;
else
	dx=frontopp_pos.getX()-0.5;
if(dx<-PITCH_LENGTH/2+PENALTY_AREA_LENGTH/2)
	dx=-PITCH_LENGTH/2+PENALTY_AREA_LENGTH/2;
///////////////////////determine dy
int iIndex;
ObjectSetT set=OBJECT_SET_OPPONENTS;
double dConfThr = PS->getPlayerConfThr();
ObjectT clo_to_topline;
VecPosition posObj;
VecPosition posOnLine;
double dDist=1000.0;
double d=1000.0;
Line l_top=Line::makeLineFromTwoPoints(VecPosition (-PITCH_LENGTH/2,-PITCH_WIDTH/2), VecPosition(PITCH_LENGTH/2,-PITCH_WIDTH/2));
///////determine the dy_top
 for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
       			{
       			 posObj    =WM->getGlobalPosition( o );
   				 posOnLine = l_top.getPointOnLineClosestTo( posObj );
    				 dDist     = posObj.getDistanceTo( posOnLine );
				 if(dDist<d&&rect.isInside( posObj)==true )
				 	{
					d=dDist;
					clo_to_topline=o;
				 	}
				}
			WM->iterateObjectDone( iIndex );
VecPosition pos_top=WM->getGlobalPosition(clo_to_topline);
double dy_top=pos_top.getY()+0.5;
if(dy_top<-PITCH_WIDTH/2+(PITCH_WIDTH-PENALTY_AREA_WIDTH)/4)
	dy_top=-PITCH_WIDTH/2+(PITCH_WIDTH-PENALTY_AREA_WIDTH)/4;
////////determine the dy_bot
dDist=1000.0;
d=1000.0;
ObjectT clo_to_botline;
Line l_bot=Line::makeLineFromTwoPoints(VecPosition (-PITCH_LENGTH/2,PITCH_WIDTH/2), VecPosition(PITCH_LENGTH/2,PITCH_WIDTH/2));
for( ObjectT o = WM->iterateObjectStart( iIndex, set, dConfThr );
       			o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set, dConfThr ) )
       			{
       			 posObj    =WM->getGlobalPosition( o );
   				 posOnLine = l_bot.getPointOnLineClosestTo( posObj );
    				 dDist     = posObj.getDistanceTo( posOnLine );
				 if(dDist<d&&rect.isInside( posObj)==true )
				 	{
					d=dDist;
					clo_to_botline=o;
				 	}
				}
			WM->iterateObjectDone( iIndex );
VecPosition pos_bot=WM->getGlobalPosition(clo_to_botline);
double dy_bot=pos_bot.getY()-0.5;
if(dy_bot>PITCH_WIDTH/2-(PITCH_WIDTH-PENALTY_AREA_WIDTH)/4)
dy_bot=PITCH_WIDTH/2-(PITCH_WIDTH-PENALTY_AREA_WIDTH)/4;
////////calculate the four pos which the defenders run to.
VecPosition pos1=VecPosition(dx,dy_top);
VecPosition pos2=VecPosition(dx,(dy_bot+dy_top*2)/3);
VecPosition pos3=VecPosition(dx,(2*dy_bot+dy_top)/3);
VecPosition pos4=VecPosition(dx,dy_bot);

ObjectT iMyself=WM->getAgentObjectType();
if(iMyself==OBJECT_TEAMMATE_4)
{
soc=moveToPos(pos2, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
ACT->putCommandInQueue(soc);
return soc;
}
if(iMyself==OBJECT_TEAMMATE_2)
{
soc=moveToPos(pos4, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
ACT->putCommandInQueue(soc);
return soc;
}
if(iMyself==OBJECT_TEAMMATE_5)
{
soc=moveToPos(pos3, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
ACT->putCommandInQueue(soc);
return soc;
}
if(iMyself==OBJECT_TEAMMATE_3)
{
soc=moveToPos(pos1, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
ACT->putCommandInQueue(soc);
return soc;
}
else
{
//VecPosition strpos=WM->getStrategicPosition(iMyself);
soc=moveToPos(g_strpos, PS->getPlayerWhenToTurnAngle(), 1, false, 1);
ACT->putCommandInQueue(soc);
return soc;
}
}
///////////////////////////////////////////////////

double Player::Mark_Val(ObjectT obj)
{
double a;
int c;
VecPosition p=(-41.5,0);//error p=(41.5,0);
VecPosition opppos=WM->getGlobalPosition(obj);
if(sign(opppos.getY())==sign(g_ballPos.getY()))
	c=0;//old c=1
else
	c=1;//old c=0
a=(1+2*c)/opppos.getDistanceTo(p);
return
	a;
}

bool Player::I_VS_Goalie()
{

int iIndex;
ObjectSetT set=OBJECT_SET_PLAYERS;
double max_x=-1000.0;
ObjectT obj;
 for( ObjectT o = WM->iterateObjectStart( iIndex, set, 0 );
       						o != OBJECT_ILLEGAL;
       			o = WM->iterateObjectNext ( iIndex, set, 0 ) )
       	{
		if(o!=WM->getOppGoalieType()&&o!=WM->getAgentObjectType())
			{
			VecPosition pos=WM->getGlobalPosition(o);
			if(pos.getX()>max_x)
				{
				max_x=pos.getX();
				obj=o;
				}
			}
		
		}
		WM->iterateObjectDone( iIndex );
	if(g_agentPos.getX()-max_x>3&&g_ballPos.getX()>30&&fabs(g_agentPos.getY())<15&&WM->isBallKickable())
		return true;
	else
		return false;
}
SoccerCommand Player::somewhat_smart_attacker()
{


	SoccerCommand soc;
	ObjectT iMyself = WM->getAgentObjectType();
	ObjectT goalie = WM->getOppGoalieType();
	VecPosition agentPos = WM->getAgentGlobalPosition();
	double xAgent = agentPos.getX();
	double yAgent = agentPos.getY();
	VecPosition posBall = WM->getBallPos();
	double xBall = posBall.getX();
	double yBall = posBall.getY();
	VecPosition posGoalie = WM->getGlobalPosition(goalie);
	double xGoalie = posGoalie.getX();
	static double direction = -1.0;
	
	VecPosition posBallA, velBallA;
	VecPosition posIntercept;
	soc = kickTo(VecPosition(PITCH_LENGTH/2.0, 6.6), SS->getBallSpeedMax());
	WM->predictBallInfoAfterCommand(soc, &posBallA, &velBallA);
	WM->predictCommandToInterceptBall_2(goalie, SoccerCommand(CMD_ILLEGAL), 
		NULL, &posIntercept, NULL, NULL, NULL, posBallA, velBallA);
	if (!WM->isInField(posIntercept, 0.1))
		{
		ACT->putCommandInQueue(soc);
		return soc;
		}
	soc = kickTo(VecPosition(PITCH_LENGTH/2.0, -6.6), SS->getBallSpeedMax());
	WM->predictBallInfoAfterCommand(soc, &posBallA, &velBallA);
	WM->predictCommandToInterceptBall_2(goalie, SoccerCommand(CMD_ILLEGAL), 
		NULL, &posIntercept, NULL, NULL, NULL, posBallA, velBallA);
	if (!WM->isInField(posIntercept, 0.1))
		{
		ACT->putCommandInQueue(soc);
		return soc;
		}	
	if (1)
		{
		VecPosition target(xBall + (xGoalie - 3.5 - xBall)*0.5, 3.0*direction);
		double angle = (target - posBall).getDirection();
		soc = dribble(angle, DRIBBLE_SLOW);
		if (yBall > 2.0)
			direction = -1.0;
		else if (yBall < -2.0)
			direction = 1.0;
		}
	ACT->putCommandInQueue(soc);
	return soc;
}

