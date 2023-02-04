#include "CommunicationSystem.h"
#include "Connection.h"
#include "Global.h"

CommunicationSystem coms;

int msg_format[MSG_TYPE_MAX][MAX_DATA] = 
{
{	2,	360, 27	},//MSG_PASS_ADV
};

void log_message( char *str, ... )
{
	#if 0
	char buf[200];
	va_list ap;
	va_start( ap, str );
	vsprintf( buf, str, ap );
	va_end(ap);
	cout<<buf<<endl;
	#endif
}

/*
此函数把可通讯字符转换成0－73的整数
字符的排列是
0－9A-Za-Z+-* /()<>?._空格

*/
static char  char2int(char c)
{
	unsigned char i=255;
	if(c>='0'&&c<='9')
		i=c-'0';
	if(c>='A'&&c<='Z')
		i=c-'A'+10;
	if(c>='a'&&c<='z')
		i=c-'a'+10+26;
	if(c=='+')
		i=62;
	if(c=='-')
		i=63;
	if(c=='*')
		i=64;
	if(c=='/')
		i=65;
	if(c=='(')
		i=66;
	if(c==')')
		i=67;
	if(c=='<')
		i=68;
	if(c=='>')
		i=69;
	if(c=='?')
		i=70;
	if(c=='.')
		i=71;
	if(c=='_')
		i=72;
	if(c==' ')
		i=73;
	return i;
}

/*
此函数把0－73的整数换成可通讯字符转
字符的排列是
0－9A-Za-Z+-* /()<>?._空格
*/
static char int2char(char i)
{
	if(i==62)
		return '+';
	if(i==63)
		return '-';
	if(i==64)
		return '*';
	if(i==65)
		return '/';
	if(i==66)
		return '(';
	if(i==67)
		return ')';
	if(i==68)
		return '<';
	if(i==69)
		return '>';
	if(i==70)
		return '?';
	if(i==71)
		return '.';
	if(i==72)
		return '_';
	if(i==73)
		return ' ';

	if(i<=9)
		return i+'0';
	i-=10;
	if(i<26)
		return i+'A';
	i-=26;
	if(i<26)
		return i+'a';
	return (char)0xFF;
}

static void decode_long(unsigned long long &value, char* str10)
{
	unsigned long long x,b;
	int i;
	x=0;
	b=1;
	for(i=0;i<10;i++){
		x+=char2int(str10[i])*b;
		b=b*74;
	}
	value=x;
}

static void encode_long(char* str10,unsigned long long value)
{
	int i;
	unsigned long long x=value;
	for(i=0;i<10;i++){
		str10[i]=int2char(char(x%74));
		x=x/74;
	}
}

void CommunicationSystem::Reset()
{
	msg_num_stored = 0;
	
	if(lastTimeHear == WM->getCurrentTime())
		return;
	
	num_msg_recieved  = 0;
}
bool CommunicationSystem::AddMsg(Message msg)
{
	int i,j;
	if( msg_num_stored >= MSG_STORE_MAX ) 
		return false;
	for( i = 1;i <= msg_format[msg.msg_type][0];i ++ ){
		if( msg.data[i] < 0 )
			msg.data[i] = 0;
		if( msg.data[i] >= msg_format[msg.msg_type][i] - 1)
			msg.data[i] = msg_format[msg.msg_type][i] - 1;
	}
	for( i = 0;i < msg_num_stored;i ++ ){
		if( msg.priority > msg_to_send[i].priority ){
			for( j = msg_num_stored;j > i; j -- ){
				msg_to_send[j] = msg_to_send[j - 1];
			}
			msg_to_send[i] = msg;
			msg_num_stored ++;
			return true;
		}
	}
	msg_to_send[msg_num_stored ++] = msg;
	return true;
}

char * CommunicationSystem::GenerateMsgString()
{
	int i,j;
	int num_code;
	unsigned long long value = 0,tmp_value,tmp_max = 1;
	unsigned long long encodemax = 1;
    double tmp_max_double;
	for(i = 0;i < 10;i ++ )
		encodemax *= 74;
	for( i = 0;i < msg_num_stored;i ++ ){
		tmp_max = 1;
        tmp_max_double = 1;
		num_code = msg_format[msg_to_send[i].msg_type][0];
		for( j = 0;j < num_code;j ++ ){
            tmp_max_double *= msg_format[msg_to_send[i].msg_type][j+1];
			tmp_max *= msg_format[msg_to_send[i].msg_type][j+1];
		}
        tmp_max_double *= MSG_TYPE_MAX;
		tmp_max *= MSG_TYPE_MAX;
		if(double(tmp_max_double*value)> encodemax )
			break;
		tmp_value = 0;
		for( j = num_code;j > 1;j -- ){
			tmp_value += msg_to_send[i].data[j];
			tmp_value *= msg_format[msg_to_send[i].msg_type][j-1];
		}
		tmp_value += msg_to_send[i].data[1];
		tmp_value *= MSG_TYPE_MAX;
		tmp_value += msg_to_send[i].msg_type;
		value *= tmp_max;
		value += tmp_value;
	}
	encode_long(str_message, value);
	return str_message;
}
int CommunicationSystem::ExtractMsg(char * msg)
{
	unsigned long long value;
	int num_code,i;
	MSG_TYPE msg_type;
	num_msg_recieved = 0;
	decode_long(value, msg);
	while(value){
		msg_type = (MSG_TYPE)(value%MSG_TYPE_MAX);
		value /= MSG_TYPE_MAX;
		
		num_code = msg_format[msg_type][0];
		if(num_msg_recieved >= MSG_RCV_MAX -1 )
			break;
		msg_recieved[num_msg_recieved].msg_type = msg_type;
		msg_recieved[num_msg_recieved].data[0] = msg_format[msg_type][0];
		for( i = 0;i < num_code;i ++ ){
			msg_recieved[num_msg_recieved].data[i+1] =(int)( value % msg_format[msg_type][i+1]);
			value /= msg_format[msg_type][i+1];
		}
		num_msg_recieved ++;
	}
	msg_sender = ObjectT(OBJECT_TEAMMATE_1+WM->getMessageSender()-1);
	lastTimeHear = WM->getCurrentTime();
	updateHearMessages();
	num_msg_recieved = 0;
	return num_msg_recieved;
	
}

void CommunicationSystem::Init(WorldModel *wm)
{
	WM=wm;

	str_message[10] = 0;
	msg_num_stored = 0;
	num_msg_recieved  = 0;
}

void CommunicationSystem::SayPassAdv(VecPosition vel)
{
	Message msg;
	double data1,data2;
	msg.priority = 1;
	
	msg.msg_type = MSG_PASS_ADV;
	msg.data[0] = MSG_PASS_ADV;
	
	data1 = ((int)(vel.getDirection()+360))%360;
	data2 = vel.getMagnitude()*10;
	msg.data[1] =(int) data1;
	msg.data[2] =(int) data2;
	
	#if debug_coms
	cout<<"Say ball(VecPosition posBall,VecPosition velBall)  "<<posBall<<"  vel   "<<velBall<<endl;
	#endif
	AddMsg(msg);
}


void CommunicationSystem::updateHearMessages()
{
    //TODO: Add your source code here
	int i;
	double data1,data2;
	double ang,mag;
	VecPosition vel,pos;
	for(i=0;i<num_msg_recieved;i++){
		switch(msg_recieved[i].msg_type){
		case MSG_PASS_ADV:
			data1 = msg_recieved[i].data[1];
			data2 = msg_recieved[i].data[2];
			ang =VecPosition::normalizeAngle(data1);
			mag = (double)data2/10.0;
			vel = VecPosition(mag,ang,POLAR);

			velPassAdv = vel;
			hearPassAdvTime = WM->getCurrentTime();
			break;
		default:
			;
		}
	}
}
