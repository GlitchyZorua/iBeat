#include "ibeat.h"
#include <windows.h>
#include <math.h>

// Global UDT variable
bool SwatchMode;

//Helper Function to check is a given year is a leap year
static bool LeapYear(int year)
{
	if(!(year%4)) //durch 4 teilbar (ja)
	{
		if(!(year%100)) //durch 100 teilbar (nein)
		{
			if(!(year%400))  //durch 400 teilbar (ja)
			{
				if(!(year%1000)) //durch 1000 teilbar (nein)
				{
					return false;
				}
				else
					return true;
			}
			else
				return false;
		}
		else
			return true;
	}
	else
		return false;
}

//Helper function to round a double to int [0.5->1] and [0.49 -> 0]
static int round(double value)
{
	int x,y;

	x=(int)value;
	value-=x;
	value*=10;
	y=(int)value;

	if(y>=5)
		return x+1;
	return x;
}

bool SummerTime(void)
{
	struct tm *stm;
	time_t now;
	
	_tzset();
	time(&now);

	if(now<0)
		return false;
	
	stm = localtime(&now);
	
	if(stm->tm_isdst)
		return true;
	else return false;
}

void LocalToBeat(BEATTIME *beat,SYSTEMTIME *local)
{
	int day,month;
	int iday,ibeat;
	int m,h,s,ms,tz;
	double fraction;

	day=local->wDay;
	month=local->wMonth;
	
	day--;
	switch(month)
	{
	case 1:
		iday=0;
		break;
	case 2:
		iday=31;
		break;
	case 3:
		if(LeapYear(local->wYear)	)
			iday=31+29;
		else
			iday=31+28;
		break;
	case 4:
		if(LeapYear(local->wYear)	)
			iday=31+29+31;
		else
			iday=31+28+31;
		break;
	case 5:
		if(LeapYear(local->wYear)	)
			iday=31+29+31+30;
		else
			iday=31+28+31+30;
		break;
	case 6:
		if(LeapYear(local->wYear)	)
			iday=31+29+31+30+31;
		else
			iday=31+28+31+30+31;
		break;
	case 7:
		if(LeapYear(local->wYear)	)
			iday=31+29+31+30+31+30;
		else
			iday=31+28+31+30+31+30;
		break;
	case 8:
		if(LeapYear(local->wYear)	)
			iday=31+29+31+30+31+30+31;
		else
			iday=31+28+31+30+31+30+31;
		break;
	case 9:
		if(LeapYear(local->wYear)	)
			iday=31+29+31+30+31+30+31+31;
		else
			iday=31+28+31+30+31+30+31+31;
		break;
	case 10:
		if(LeapYear(local->wYear)	)
			iday=31+29+31+30+31+30+31+31+30;
		else
			iday=31+28+31+30+31+30+31+31+30;
		break;
	case 11:
		if(LeapYear(local->wYear)	)
			iday=31+29+31+30+31+30+31+31+30+31;
		else
			iday=31+28+31+30+31+30+31+31+30+31;
		break;
	case 12:
		if(LeapYear(local->wYear)	)
			iday=31+29+31+30+31+30+31+31+30+31+30;
		else
			iday=31+28+31+30+31+30+31+31+30+31+30;
		break;
	}
	iday+=day;

	_tzset();
	tz =_timezone/60; 

	_tzname[0];
	_tzname[1];

	m=local->wMinute;
	h=local->wHour;
	s=local->wSecond;
	ms=local->wMilliseconds;

	m=m+tz;

	if(_daylight)
	{
		if(SummerTime())
		{
			m-=60;
		}
	}

	//now gmt
	//UDT references UTC
	if(SwatchMode)  
	{
		//Swatch references UTC+1
		h++;
		//now gmt+1
	}

	while(m>=60)
	{
		h++;
		m-=60;
	}
	while(m<0)
	{
		h--;
		m+=60;
	}

	while(h>=24)
	{
		iday++;
		h-=24;
	}
	
	while(h<0)
	{
		iday--;
		h+=24;
	}

	fraction  = h * 3600 ;
	fraction += m * 60;
	fraction += s;
	fraction *= 1000;
	fraction += ms;
	fraction /= 86400;
	ibeat =(int)fraction;
	while (ibeat>=1000)
	{ 
		ibeat -=1000;
	}

	while (ibeat<0)
	{ 
		ibeat +=1000;
	}
	fraction -= ibeat;
	fraction *= 1000;

	while (fraction>=1000)
	{ 
		fraction -=1000;
	}
	while (fraction<0)
	{ 
		fraction +=1000;
	}

	//save beats
	beat->iDay=iday+1;  //ISO 8601 conform 
	beat->iBeat=ibeat;
	beat->iFraction=(int)fraction;
}


void BeatToLocal(SYSTEMTIME *local,BEATTIME *beat)
{

int lm;
int tz;
SYSTEMTIME sysdate;
BEATTIME lbeat;
double buffer;
	
	GetSystemTime(&sysdate);

	if(LeapYear(sysdate.wYear))
		lm=1;
	else lm=0;

	local->wYear=sysdate.wYear;
	
	if(beat->iBeat==-1)
	{
		GetLocalTime(&sysdate);
		LocalToBeat(&lbeat,&sysdate);
		beat->iDay=lbeat.iDay;
	}
	
	if (beat->iDay<=31)
	{
		local->wMonth=1;
		local->wDay=beat->iDay-1;
	}
	else
		if (beat->iDay<=31+28+lm)
		{
			local->wMonth=2;
			local->wDay=beat->iDay-(31+1);
		}
	else
		if (beat->iDay<=31+28+lm+31)
		{
			local->wMonth=3;
			local->wDay=beat->iDay-(31+28+lm+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30)
		{
			local->wMonth=4;
			local->wDay=beat->iDay-(31+28+lm+31+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30+31)
		{
			local->wMonth=5;
			local->wDay=beat->iDay-(31+28+lm+31+30+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30+31+30)
		{
			local->wMonth=6;
			local->wDay=beat->iDay-(31+28+lm+31+30+31+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30+31+30+31)
		{
			local->wMonth=7;
			local->wDay=beat->iDay-(31+28+lm+31+30+31+30+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30+31+30+31+31)
		{
			local->wMonth=8;
			local->wDay=beat->iDay-(31+28+lm+31+30+31+30+31+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30+31+30+31+31+30)
		{
			local->wMonth=9;
			local->wDay=beat->iDay-(31+28+lm+31+30+31+30+31+31+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30+31+30+31+31+30+31)
		{
			local->wMonth=10;
			local->wDay=beat->iDay-(31+28+lm+31+30+31+30+31+31+30+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30+31+30+31+31+30+31+30)
		{
			local->wMonth=11;
			local->wDay=beat->iDay-(31+28+lm+31+30+31+30+31+31+30+31+1);
		}
	else
		if (beat->iDay<=31+28+lm+31+30+31+30+31+31+30+31+30+31)
		{
			local->wMonth=12;
			local->wDay=beat->iDay-(31+28+lm+31+30+31+30+31+31+30+31+30+1);
		}

		if(beat->iDay<=0)
		{
			local->wMonth=1;
			local->wDay=1;
		}
		local->wDay++;

		_tzset();
		tz  =_timezone/60; 
		tz *=-1;		

		if(_daylight)
		{
			if(SummerTime())
			{
				tz+=60;
			}
		}


		buffer=beat->iBeat*1000+beat->iFraction;
		buffer/=41666;
		local->wHour=floor(buffer);

	    buffer=beat->iBeat*1000+beat->iFraction;
		buffer/=694;
		local->wMinute=floor(buffer);
		local->wMinute%=60;
		
	    buffer=beat->iBeat*1000+beat->iFraction;
		buffer/=12;
		local->wSecond=floor(buffer);
		local->wSecond%=60;
		local->wSecond%=60;

		//time at gmt+1
		local->wHour--;
		
		//time at gmt
		if (local->wHour<0) local->wHour+=24;

		local->wHour+=tz/60;
		local->wMinute+=tz%60;
		local->wHour+=local->wMinute/60;
		local->wMinute=local->wMinute%60;

		local->wMilliseconds=0;
		local->wDayOfWeek=0;
		//LocalTime	is now
		//Hour, Minute, Second
		//Day, Month, Year(system year, not saved in beats)
}

bool RemoveAlarmList(IALARMLIST *List)
{
IALARMLIST *Link;

	while(List)
	{
		Link=List;
		List=List->Next;
		delete Link;
	}
	return true;
}

bool LoadAlarmList(char *filename, IALARMLIST **List)
{
FILE *fset;
IALARMLIST *Link;
IALARM buffer;

	fset=fopen(filename,"rb");
	if(fset)
	{
		Link = new IALARMLIST;
		*List = Link;
		Link->Prev=NULL;
		Link->Alarm=NULL;
		Link->Next = NULL;

		while(fread(&buffer,sizeof(IALARM),1,fset))
		{
			Link->Alarm =new IALARM;
			memcpy(Link->Alarm,&buffer,sizeof(IALARM));
			Link->Next = new IALARMLIST;
			Link->Next->Next=NULL;
			Link->Next->Prev=Link;
			Link->Next->Alarm=NULL;
			Link=Link->Next;
		}
		fclose(fset);
		return true;
	}
	else
	{
		Link = new IALARMLIST;
		Link->Prev=NULL;
		Link->Next=NULL;
		Link->Alarm=NULL;
		*List = Link;
		return false;
	}
}
void SortAlarmList(IALARMLIST **List)
{
	IALARMLIST *SortList;
	IALARM *nextalarm;
	SYSTEMTIME now;

	SortList= new IALARMLIST;
	SortList->Alarm=NULL;
	SortList->Next=NULL;
	SortList->Prev=NULL;

	GetLocalTime(&now);

	while (nextalarm=ScanNextAlarm(*List,&now))
	{
		AddAlarm(SortList,nextalarm);
		DelAlarm(List,nextalarm->OriginUserFooBar,nextalarm->OriginUniqueAlarmID);
		memcpy(&now,&nextalarm->AlarmTime,sizeof(SYSTEMTIME));
	}
	
	*List=SortList;

}


IALARM *ScanNextAlarm(IALARMLIST *List,SYSTEMTIME *now)
{
	IALARMLIST *Link;
	IALARM *alarm;
	BEATTIME nowbeat,alarmbeat;

	int daydiff;
	int bestday=400;
	int beatdiff;
	int bestbeat=2000;

	LocalToBeat(&nowbeat,now);

	Link=List;
	
	bestday=400;
	bestbeat=2000;

	alarm = NULL;
	while(Link)
	{
		if(Link->Alarm)
		{
			if(!Link->Alarm->Expired)
			{
				LocalToBeat(&alarmbeat,&Link->Alarm->AlarmTime);
				daydiff=alarmbeat.iDay-nowbeat.iDay;
				beatdiff=alarmbeat.iBeat-nowbeat.iBeat;
				if(daydiff>=0)
				{
					if(bestday>=daydiff)
					{
						bestday=daydiff;
						
						if(bestday==0)
							beatdiff=alarmbeat.iBeat-nowbeat.iBeat;
						else
							beatdiff=alarmbeat.iBeat;

						if((bestbeat>=beatdiff) && (beatdiff>=0))
						{	
							bestbeat=beatdiff;
							alarm = Link->Alarm;
						}
					}
				}
			}
		}
		Link=Link->Next;
	}

	if(alarm)
		return alarm;
	
	//warp arround
	Link=List;
	bestday=400;
	bestbeat=2000;
	alarm = NULL;
	while(Link)
	{
		if(Link->Alarm)
		{
			if(!Link->Alarm->Expired)
			{
				LocalToBeat(&alarmbeat,&Link->Alarm->AlarmTime);
				daydiff=alarmbeat.iDay-nowbeat.iDay;
				beatdiff=alarmbeat.iBeat-nowbeat.iBeat;
			
				if(bestday>=daydiff)
				{
					bestday=daydiff;

					beatdiff=alarmbeat.iBeat;
					if(bestbeat>=beatdiff)
					{	
						bestbeat=beatdiff;
						alarm = Link->Alarm;
					}
				}
			}
		}
		Link=Link->Next;
	}

	return alarm;
}
bool ScanForAlarm(SYSTEMTIME *now,IALARMLIST *List,char **text)
{
	IALARMLIST *Link;

	Link=List;

	while(Link)
	{
		if(Link->Alarm)
		{
			if(!Link->Alarm->Expired)
			if(now->wDay == Link->Alarm->AlarmTime.wDay)
			if(now->wMonth == Link->Alarm->AlarmTime.wMonth)
			if(now->wYear == Link->Alarm->AlarmTime.wYear)
			if(now->wHour == Link->Alarm->AlarmTime.wHour)
			if(now->wMinute == Link->Alarm->AlarmTime.wMinute)
			{
				*text=Link->Alarm->Note;
				Link->Alarm->Expired=true;
				return true;
			}
		}
		Link=Link->Next;
	}
	return false;
}

bool SaveAlarmList(char *filename, IALARMLIST *List)
{
	FILE *fset;
	IALARMLIST *Save;
	
	fset=fopen(filename,"wb");
	if(fset)
	{

		Save=List;
		while(Save)
		{
			if(Save->Alarm) 
				fwrite(Save->Alarm,sizeof(IALARM),1,fset);			
			Save=Save->Next;

		}
		fclose(fset);
		return true;
	}
	return false;
}

bool AddAlarm(IALARMLIST *List, IALARM *alarm)
{
IALARMLIST *Link;

	Link=List;
	while(Link)
	{
		if(Link->Alarm==NULL)
		{
			Link->Alarm=new IALARM;
			memcpy(Link->Alarm,alarm,sizeof(IALARM)); 
			Link->Next = new IALARMLIST;
			Link->Next->Alarm=NULL;
			Link->Next->Next=NULL;
			Link->Next->Prev=Link;
			return true;
		}
		
		if(Link->Next)
			Link=Link->Next;
		else
			return false;
	}
	return false;
}

bool DelAlarm(IALARMLIST **List, int OriginUserFooBar, int OriginUAID)
{
IALARMLIST *Link;
IALARMLIST *DelLink;

	Link=*List;
	while(Link)
	{
		if(Link->Alarm)
		if(Link->Alarm->OriginUserFooBar==OriginUserFooBar && Link->Alarm->OriginUniqueAlarmID==OriginUAID)
		{
			DelLink=Link;
			//next and prev item -> middle item
			if(Link->Next && Link->Prev)
			{
				Link->Next->Prev=Link->Prev;
				Link->Prev->Next=Link->Next;
			}
			else //next, but no prev -> first item
			if(Link->Next && !Link->Prev)
			{
				Link->Next->Prev=NULL;
				*List=Link->Next;
			}
			else //no next, but prev -> last item
			if(!Link->Next && Link->Prev)
			{
				Link->Prev=NULL;
			}
			else //no next, and no prev -> empty list
			if(!Link->Next && !Link->Prev)
			{
				*List=NULL;
			}

			delete DelLink;
			return true;
		}
		Link=Link->Next;
	}

	return false;
}


bool ModAlarm(IALARMLIST *List, IALARM *oldalarm, IALARM *newalarm)
{
IALARMLIST *Link;

	Link=List;
	while(Link)
	{
		if(Link->Alarm->OriginUserFooBar==oldalarm->OriginUserFooBar && Link->Alarm->OriginUniqueAlarmID==oldalarm->OriginUniqueAlarmID)
		{
			memcpy(Link->Alarm,newalarm,sizeof(IALARM));
			return true;
		}
		Link=Link->Next;
	}
	return false;
}
