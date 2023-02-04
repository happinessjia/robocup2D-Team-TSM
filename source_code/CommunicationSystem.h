
#ifndef _COMMUNICATIONSYSTEM_H
#define _COMMUNICATIONSYSTEM_H

#include "ActHandler.h"
#include "WorldModel.h"
//#include "InterceptUtil.h"

#define MAX_DATA 30
#define MSG_STORE_MAX 50
#define MSG_TYPE_MAX 16
#define MSG_RCV_MAX 20
enum MSG_TYPE
{
	MSG_PASS_ADV,
};


struct _Message
{
	MSG_TYPE msg_type;
	int priority;
	int data[MAX_DATA];
};
typedef struct _Message Message;
class CommunicationSystem
{
private:
	WorldModel *WM;

	char str_message[11];
	Message msg_to_send[MSG_STORE_MAX];

	Time lastTimeHear;
	ObjectT msg_sender;
	Message msg_recieved[MSG_RCV_MAX];
	int num_msg_recieved;
	int time_recieved;

	bool AddMsg(Message msg);
	void updateHearMessages();
public:
	int msg_num_stored;	

	void Init(WorldModel *wm);
	void Reset();	
	char * GenerateMsgString();
	int ExtractMsg(char * msg);//返回有多少条消息
	
	void SayPassAdv(VecPosition vel);
	
	VecPosition velPassAdv;
	Time hearPassAdvTime;
};

extern CommunicationSystem coms;

#endif

