#ifndef _SYNCTIME_
#define _SYNCTIME_

#include <windows.h>
#include "socket.h"

struct SNTP_Packet
{
	unsigned long x;
	unsigned long root_delay;
	unsigned long root_dispersion;
	unsigned long ref_ident;

	unsigned long ref_timestamp1;
	unsigned long ref_timestamp2;
	
	unsigned long org_timestamp1;
	unsigned long org_timestamp2;
	
	unsigned long rec_timestamp1;
	unsigned long rec_timestamp2;
	
	unsigned long tra_timestamp1;
	unsigned long tra_timestamp2;
	
	unsigned long key_ident;

	unsigned long msg_digest1;
	unsigned long msg_digest2;
	unsigned long msg_digest3;
	unsigned long msg_digest4;

};

struct SNTP_Sync
{
	int				LI;
	int				Stratum;
	int				Mode;
	int				Version;

	unsigned long	Identifier;
	
	long			Offset;
	unsigned int	mOffset;

	unsigned long	Delay;
	unsigned int	mDelay;

	unsigned long	RefTime;
	unsigned int	mRefTime;
};

unsigned long GetINetTime (char * hostaddress);

bool InitSyncTime (HWND Parent, int Version, UINT Message);
bool GetSyncTime (char * hostaddress);

#endif