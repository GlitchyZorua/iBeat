#ifndef _IBEATHEADER_
#define _IBEATHEADER_
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <time.h>

struct _BEATTIME
{
	int iDay;
	int iBeat;
	int iFraction;
};
typedef struct _BEATTIME BEATTIME;

extern bool SwatchMode;

struct _IALARM
{
	SYSTEMTIME AlarmTime;
	int CurrentUserFooBar;
	int CurrentUniqueAlarmID;
	int OriginUserFooBar;
	int OriginUniqueAlarmID;

	bool SendFlag;
	bool ConfirmFlag;
	bool Expired;

	bool ReserverBool2;
	bool ReserverBool3;
	bool ReserverBool4;
	int ReservedInt1;
	int ReservedInt2;
	int ReservedInt3;
	int ReservedInt4;

	char Note[2048];
};
typedef struct _IALARM IALARM;

struct _IALARMLIST
{
	IALARM *Alarm;
	struct _IALARMLIST *Next;
	struct _IALARMLIST *Prev;
};

typedef struct _IALARMLIST IALARMLIST;

void LocalToBeat(BEATTIME *beat,SYSTEMTIME *local);

void BeatToLocal(SYSTEMTIME *local,BEATTIME *beat);

bool RemoveAlarmList(IALARMLIST *List);

bool LoadAlarmList(char *filename, IALARMLIST **List);

bool SaveAlarmList(char *filename, IALARMLIST *List);

bool AddAlarm(IALARMLIST *List, IALARM *alarm);

bool DelAlarm(IALARMLIST **List, int OriginUserFooBar, int OriginUAID);

bool ModAlarm(IALARMLIST *List, IALARM *oldalarm, IALARM *newalarm);

bool ScanForAlarm(SYSTEMTIME *now,IALARMLIST *List,char **text);

void SortAlarmList(IALARMLIST **List);

IALARM *ScanNextAlarm(IALARMLIST *List,SYSTEMTIME *now);

#endif