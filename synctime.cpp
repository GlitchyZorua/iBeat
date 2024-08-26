#include <windows.h>
#include <process.h>    
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include "socket.h"
#include "synctime.h"
#include "resource.h"

#define SNTP_PORT 123

//difference in seconds between 1900 and 1970 (70 Years + 17 Leap Days)
static const long TOFS=2208988800;
static UINT ReturnMessage;

static HWND rThread=NULL;
static Socket *s=NULL;

bool SendSNTPRequest(char * hostaddress)
{
struct SNTP_Packet buffer;
long ts;
int i;
	if(!s->IsOK())
		return false;

	memset(&buffer,0,sizeof(struct SNTP_Packet));
	_tzset();
	
	
	buffer.x=htonl(0x23000000);
//	buffer.x=htonl(0x1b000000);
	time(&ts);
	ts+=TOFS;
	buffer.tra_timestamp1=htonl(ts);

	for(i=0;i<5;i++)
	{
		if(!s->SendTo((char *)&buffer,sizeof(struct SNTP_Packet),hostaddress,SNTP_PORT))
		{
			return false;
		}
		Sleep(10);
		time(&ts);
		ts+=TOFS;
		buffer.tra_timestamp1=htonl(ts);
	}
	return true;
}

DWORD WINAPI RecvSNTPRequest(void * passedThread)
{
int status;
int * passedWnd;
HWND Parent;
struct SNTP_Packet PacketBuffer;
struct timeb timeptr;
struct SNTP_Sync *ReturnBuffer;
LONGLONG T1,T2,T3,T4,Tdel,Toff;
unsigned long Helper;
 
	passedWnd=(int *)passedThread;;
	
//	Parent=(HWND)*passedWnd;
	
	while(true) //loop forever
	{
		status=s->Check(SA_READ,5,0);
		if(status!=SOCKET_ERROR && status!=0)
		{
			ftime(&timeptr);

			memset(&PacketBuffer,0,sizeof(struct SNTP_Packet));
			status=s->RecvFrom((char *)&PacketBuffer,sizeof(struct SNTP_Packet));
			if(status)
			{
				ReturnBuffer= new struct SNTP_Sync;

				T1=ntohl(PacketBuffer.org_timestamp1);
				T2=ntohl(PacketBuffer.rec_timestamp1);
				T3=ntohl(PacketBuffer.tra_timestamp1);
				T4=timeptr.time;
				T4+=TOFS;

				T1<<=32;
				T2<<=32;
				T3<<=32;
				T4<<=32;

				Tdel=ntohl(PacketBuffer.org_timestamp2);
				Tdel&=0xffffffff;
				T1|=Tdel;

				Tdel=ntohl(PacketBuffer.rec_timestamp2);
				Tdel&=0xffffffff;
				T2|=Tdel;

				Tdel=ntohl(PacketBuffer.tra_timestamp2);
				Tdel&=0xffffffff;
				T3|=Tdel;

				Tdel=timeptr.millitm;
				Tdel&=0xffff;
				Tdel<<=16;
				T4|=Tdel;

				Tdel= (T4 - T1) - (T2 - T3);
				Toff= ((T2 - T1 ) + ( T3 - T4))/2;

				ReturnBuffer->mOffset = (unsigned int)((Toff&0xffff0000)>>16);
				ReturnBuffer->mDelay  = (unsigned int)((Tdel&0xffff0000)>>16);

				Tdel>>=32;
				Toff>>=32;

				ReturnBuffer->Delay=(long)Tdel;
				ReturnBuffer->Offset=(long)Toff;
				
				ReturnBuffer->RefTime=ntohl(PacketBuffer.ref_timestamp1);
				ReturnBuffer->mRefTime=(unsigned int)(ntohl(PacketBuffer.ref_timestamp2)>>16);

				Helper=ntohl(PacketBuffer.x);
				Helper&=0xc0000000;
				Helper>>=30;
				ReturnBuffer->LI=Helper;

				Helper=ntohl(PacketBuffer.x);
				Helper&=0x00ff0000;
				Helper>>=16;
				ReturnBuffer->Stratum=Helper;

				Helper=ntohl(PacketBuffer.x);
				Helper&=0x38000000;
				Helper>>=27;
				ReturnBuffer->Version=Helper;
				
				Helper=ntohl(PacketBuffer.x);
				Helper&=0x07000000;
				Helper>>=24;
				ReturnBuffer->Mode=Helper;

				Parent=(HWND)*passedWnd;
				SendMessage(Parent,WM_COMMAND,(WPARAM)ReturnMessage,(LPARAM)ReturnBuffer);
			}
		}
	}

	ExitThread(1);
	return 0;
}

bool GetSyncTime (char * hostaddress)
{
	if(SendSNTPRequest(hostaddress))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool InitSyncTime (HWND Parent,int Version, UINT Message)
{
DWORD TID;
BOOL OptValue;
static HANDLE ReturnThread=NULL;

	rThread=Parent;
	ReturnMessage=Message;
	Version=Version;
	if(!s)
	{
		s=new Socket(SOCK_DGRAM);
		if(s->IsOK())
		{
			OptValue=TRUE;

			s->SetOption(SOL_SOCKET,SO_DONTROUTE,(const char *)&OptValue,sizeof(BOOL));
			s->SetOption(IPPROTO_TCP,TCP_NODELAY,(const char *)&OptValue,sizeof(BOOL));
			
			if(s->Bind(INADDR_ANY,SNTP_PORT))
			{
				if(!ReturnThread)
				{
					ReturnThread = CreateThread(NULL,0,RecvSNTPRequest,&rThread,0,&TID);
				}
				return true;
			}
		}
		return false;
	}
	return true;
}

/*
SNTP::Init(HWND hwnd, int Version, int Message)
SNTP::Sync(char * hostname);
*/