#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <time.h>
#include <commctrl.h>
#include <commdlg.h>
#include <math.h>
#include <wininet.h>
#include <winbase.h>
#include "resource.h"
#include "ibeat.h"
#include "synctime.h"
#include "moon.h"
#include "AutoBuildCount.h"

#define WM_TRAY WM_APP+999
#define ID_TRAY 0

#define TIMEMODE_NLT 0
#define TIMEMODE_12H 1
#define TIMEMODE_24H 2
#define TIMEMODE_GMT 3

char szWinName[] = "iBeat2";
HINSTANCE hAppInst;

LRESULT CALLBACK WinProc(HWND,UINT,WPARAM,LPARAM);

BOOL CALLBACK SNTPSyncProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);
//BOOL CALLBACK SetUserProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK SetAlarmProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ViewAlarmProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK AboutProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK AboutAuthorProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK AboutBeatsProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK AboutInternetProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK AboutRegisterProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK AboutMoonProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam);

void ShowClosedWindow(HWND hwnd,HDC dc);
void SetClosedBackground(HDC dc);
void ShowTime(HWND hwnd,HDC hvDC,int TimeDisplayMode, bool Centybeats,bool Y2KCounter);
void CenterWindow(HWND Parent, HWND Child);
HWND CreateBigTooltip(HWND hwndOwner);
HWND CreateSmallTooltip(HWND hwndOwner);

bool SetMenuText(HMENU hMenu, UINT mID, BOOL byCommand, HINSTANCE hInst, UINT sID);
bool SetDlgText(HWND hwnd, HINSTANCE hInst, UINT sID);

static COLORREF  BGColor;
static COLORREF  FNTColor;
static HFONT hFontWB=NULL;
static LOGFONT logfnt;
static bool PlayAlarm=false;
static bool Small=false;

static char BaseDir[512];

static int TDM;
static char GlobalSettingUserName[256];
static int GlobalSettingUserFoobar;
static char GlobalSettingUserEMail[256];
static char GlobalSettingUserLocation[256];
static char GlobalSettingUserPassword[8];
static int GlobalSettingUAID;
static char GlobalSNTPHostaddress[256];

static IALARMLIST *AlarmList;

/*
BOOL CALLBACK EnumResLangProc(
HMODULE hModule, // resource-module handle 
LPCTSTR lpszType, // address of resource type 
LPCTSTR lpszName, // address of resource name 
LANGID wIDLanguage, // resource language identifier
LPARAM lParam) 
{
char szItemText[4096];

// Display language ID components in the list box text. The 
// format of the text will be: "PriLang SubLang - FileName".
	HRSRC hrsrc;
	HGLOBAL hglb;

	hrsrc = FindResourceEx(hModule, RT_MENU,"Menu", wIDLanguage );
	hglb = LoadResource(hModule, hrsrc);

	LoadString(hAppInst,IDS_FIRST_TITEL,szItemText,4096);

	return TRUE;
}
*/

int WINAPI WinMain(HINSTANCE hThisInst,HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
  HWND hwnd;
  MSG msg;
  WNDCLASSEX wcl;
  INITCOMMONCONTROLSEX icex;

        hAppInst  = hThisInst;
        hPrevInst = hPrevInst;
        lpszArgs  = lpszArgs;

		//only NT
//		SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),SORT_DEFAULT));	
/*
	HRSRC hrsrc;
	HGLOBAL hglb;
		
		hrsrc = FindResourceEx(hThisInst, RT_MENU,"Menu", MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US ) );
		hglb = LoadResource(hThisInst, hrsrc);
*/


//		EnumResourceLanguages(NULL,RT_MENU,"MENU",EnumResLangProc,0l);




		strncpy(BaseDir,lpszArgs,512);

        wcl.hInstance           = hThisInst;
        wcl.lpszClassName       = szWinName;
        wcl.lpfnWndProc         = WinProc;
        wcl.style               = CS_DBLCLKS;
        wcl.cbSize              = sizeof(WNDCLASSEX);
        wcl.hIcon               = (HICON) LoadImage(hThisInst,MAKEINTRESOURCE(IDI_SMALL),IMAGE_ICON,32,32,LR_DEFAULTCOLOR);
        wcl.hIconSm             = (HICON) LoadImage(hThisInst,MAKEINTRESOURCE(IDI_SMALL),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
        wcl.hCursor             = NULL;//LoadCursor(NULL,IDC_ARROW);
        wcl.lpszMenuName        = NULL;//"Menu";
        wcl.cbClsExtra          = 0;
        wcl.cbWndExtra          = 0;
        wcl.hbrBackground       = (HBRUSH)GetStockObject(LTGRAY_BRUSH); //HOLLOW_BRUSH  LTGRAY_BRUSH

        if (!RegisterClassEx(&wcl))
          return 0;

		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_DATE_CLASSES;    
		InitCommonControlsEx(&icex);

        hwnd = FindWindow(szWinName,NULL);
        if (hwnd)
        {
            if (IsIconic(hwnd))
            {
                ShowWindow(hwnd,SW_RESTORE);
            }
            SetForegroundWindow(hwnd);

            return FALSE;
        }
		hwnd = CreateWindowEx( WS_EX_TOOLWINDOW,  
							   szWinName,
							   "iBeat2",	
							   0,//WS_VISIBLE,    
							   200,100,             
							   300,300,
							   HWND_DESKTOP,   
	                           NULL,      
							   hThisInst, 
							   NULL);

       //ShowWindow(hwnd,nWinMode);
        UpdateWindow(hwnd);


        while(GetMessage(&msg,NULL,0,0))
        {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

        return msg.wParam;
}


LRESULT CALLBACK WinProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	FILE *fset;
	static long xmax=0,ymax=0;
	static HDC hvDC=NULL,memDC;
	static HBITMAP hvBMP;
	static int timer=0;
	HDC hDC;
	PAINTSTRUCT paintstruct;
    LPMINMAXINFO lpmmi;
	static HMENU hMenu,hTimeMenu,hSoundMenu,hSizeMenu,hSettingMenu;
	static HWND twnd=NULL;
	POINT maxmin;
	POINT pnt;
	static int oldbeat=0;
	static POINT pos;
	static POINT oldpos;
	static BOOL Local=TRUE;
	static BOOL GMT=TRUE;
	static BOOL OPEN=FALSE;
	static BOOL MOVED=FALSE;
	RECT rect;
    NOTIFYICONDATA pnid;
	static int TimeDisplayMode;
	static int AlarmDisplayMode;
	static bool Centybeats=true;
	static bool Y2KCounter=false;
	static bool Visible=true;
	static bool TopMost=false;
	static bool HeartBeat=true;
	static bool Alarm=false;
	static COLORREF CustomColorRef[16];
	SYSTEMTIME local;
	BEATTIME   beat;
	static HCURSOR cursor_open,cursor_closed;
	static bool mouse;
	static UseTimer=true;
	char buffer1[4096], buffer2[256];



	long fsize;
	switch(message)
    {

	case WM_CREATE:

			
			xmax = 300;//GetSystemMetrics(SM_CXSCREEN);
			ymax = 300;//GetSystemMetrics(SM_CYSCREEN);

			//Setup Virtual Screen
			hDC = GetDC(hwnd);
			hvDC = CreateCompatibleDC(hDC);
			hvBMP =	CreateCompatibleBitmap(hDC,xmax,ymax);
			SelectObject(hvDC,hvBMP);
			ReleaseDC(hwnd,hDC);	

			hMenu = LoadMenu(hAppInst, "Menu");
			hMenu = GetSubMenu(hMenu,0);
			hSettingMenu = GetSubMenu(hMenu,4);
			hTimeMenu = GetSubMenu(hSettingMenu,0);
			hSizeMenu = GetSubMenu(hSettingMenu,4);
			hSoundMenu = GetSubMenu(hSettingMenu,7);

			timer =  SetTimer(hwnd,1,250,NULL);   //200

			sprintf(buffer1,"%s\\ibeat.ini",BaseDir);
			fset=fopen(buffer1,"rb");
			if(fset) 
			{
				fseek(fset,0,SEEK_END);
				fsize=ftell(fset);
			}
			else fsize=0;

			if(fset && (fsize==1132))
			{
				fseek(fset,0,SEEK_SET);
				fread(&pnt,sizeof(POINT),1,fset);
				fread(&TimeDisplayMode,sizeof(int),1,fset);
				fread(&AlarmDisplayMode,sizeof(int),1,fset);
				fread(&Centybeats,sizeof(bool),1,fset);
				fread(&Y2KCounter,sizeof(bool),1,fset);
				fread(&SwatchMode,sizeof(bool),1,fset);
				fread(&Visible,sizeof(bool),1,fset);
				fread(&TopMost,sizeof(bool),1,fset);
				fread(&HeartBeat,sizeof(bool),1,fset);
				fread(&Alarm,sizeof(bool),1,fset);
				fread(&Small,sizeof(bool),1,fset);
				fread(&BGColor,sizeof(COLORREF),1,fset);
				fread(&FNTColor,sizeof(COLORREF),1,fset);
				fread(&logfnt,sizeof(LOGFONT),1,fset);
				fread(GlobalSettingUserName,sizeof(char),256,fset);
				fread(GlobalSettingUserEMail,sizeof(char),256,fset);
				fread(GlobalSettingUserLocation,sizeof(char),256,fset);
				fread(GlobalSettingUserPassword,sizeof(char),8,fset);
				fread(&GlobalSettingUserFoobar,sizeof(int),1,fset);
				fread(&GlobalSettingUAID,sizeof(int),1,fset);
				fread(GlobalSNTPHostaddress,sizeof(char),256,fset);

				hFontWB= CreateFontIndirect(&logfnt);
				fclose(fset);
/*
				if(pnt.x<0 || pnt.y<0)
				{
					fset=fopen("ibeat.ini","w");
					fclose(fset);
				}				
*/
				//Visible Bug
//				Visible=true;
//				if(!Visible) 
//					SendMessage(hwnd,WM_COMMAND,IDM_HIDE,0);
			}
			else
			{
				pnt.x=100;
				pnt.y=100;
				TimeDisplayMode=TIMEMODE_24H;
				AlarmDisplayMode=TIMEMODE_24H;
				Centybeats=true;
				Y2KCounter=false;
				SwatchMode=false;
				HeartBeat=true;
				Alarm=true;
				Small=false;
				Visible=true;
				TopMost=false;
				strcpy(GlobalSettingUserName,"John Doe");
				strcpy(GlobalSettingUserEMail,"John.Doe@somewere.org");
				strcpy(GlobalSettingUserLocation,"Somewere");
				strcpy(GlobalSettingUserPassword,"secret");
				GlobalSettingUserFoobar=0;
				GlobalSettingUAID=1;
				strcpy(GlobalSNTPHostaddress,"unios.rz.Uni-Osnabrueck.DE");
			
				BGColor= RGB(0xb6,0xd3,0xab);

				FNTColor= RGB(0x00,0x00,0x00);
				memset(&logfnt,0,sizeof(LOGFONT));

				logfnt.lfHeight=24;
				logfnt.lfWidth=0;
				logfnt.lfEscapement=0;
				logfnt.lfOrientation=0;
				logfnt.lfWeight=400;
				logfnt.lfItalic=false;
				logfnt.lfUnderline=false;
				logfnt.lfStrikeOut=false;
				logfnt.lfCharSet=DEFAULT_CHARSET;
				logfnt.lfOutPrecision=OUT_DEFAULT_PRECIS;
				logfnt.lfClipPrecision=CLIP_DEFAULT_PRECIS;
				logfnt.lfQuality=PROOF_QUALITY;
				logfnt.lfPitchAndFamily=DEFAULT_PITCH|FF_DONTCARE;
				strcpy(logfnt.lfFaceName,"ARIAL");
				hFontWB= CreateFontIndirect(&logfnt);

				LoadString(hAppInst,IDS_FIRST_TEXT,buffer1,4096);
				LoadString(hAppInst,IDS_FIRST_TITEL,buffer2,256);
				MessageBox(hwnd,buffer1,buffer2,MB_OK);
				SendMessage(hwnd,WM_COMMAND,(WPARAM)IDM_ABOUT,0);

			}

			sprintf(buffer1,"%s\\alarm.dat",BaseDir);

			LoadAlarmList(buffer1,&AlarmList);
			
			pnid.cbSize=sizeof(NOTIFYICONDATA);
			pnid.hWnd=hwnd;
			pnid.hIcon=(HICON) LoadImage(hAppInst,MAKEINTRESOURCE(IDI_SMALL),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
			pnid.uID=ID_TRAY;
			pnid.uFlags=NIF_ICON|NIF_MESSAGE;
			pnid.uCallbackMessage=WM_TRAY;
			Shell_NotifyIcon(NIM_ADD , &pnid);

			TDM=TimeDisplayMode;
			switch(TimeDisplayMode)
			{
			case TIMEMODE_12H:				
				CheckMenuItem(hTimeMenu,IDM_12H,MF_BYCOMMAND|MF_CHECKED);
				break;
			case TIMEMODE_24H:				
				CheckMenuItem(hTimeMenu,IDM_24H,MF_BYCOMMAND|MF_CHECKED);
				break;
			case TIMEMODE_NLT:				
				CheckMenuItem(hTimeMenu,IDM_NLT,MF_BYCOMMAND|MF_CHECKED);
				break;
//			case TIMEMODE_Y2K:				
//				CheckMenuItem(hTimeMenu,IDM_Y2K,MF_BYCOMMAND|MF_CHECKED);
				break;
			}

			if(SwatchMode)
			{
				CheckMenuItem(hTimeMenu,IDM_UDT,MF_BYCOMMAND|MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hTimeMenu,IDM_UDT,MF_BYCOMMAND|MF_UNCHECKED);
			}

			if(Y2KCounter)
			{
				CheckMenuItem(hTimeMenu,IDM_Y2K,MF_BYCOMMAND|MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hTimeMenu,IDM_Y2K,MF_BYCOMMAND|MF_UNCHECKED);
			}

			if(Centybeats)
			{
				CheckMenuItem(hTimeMenu,IDM_CENTY,MF_BYCOMMAND|MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hTimeMenu,IDM_CENTY,MF_BYCOMMAND|MF_UNCHECKED);
			}

			if(HeartBeat) 
			{
				CheckMenuItem(hSoundMenu,IDM_HEARTBEAT,MF_BYCOMMAND|MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hSoundMenu,IDM_HEARTBEAT,MF_BYCOMMAND|MF_UNCHECKED);
			}

			if(Alarm) 
			{
				CheckMenuItem(hSoundMenu,IDM_ALARM,MF_BYCOMMAND|MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hSoundMenu,IDM_ALARM,MF_BYCOMMAND|MF_UNCHECKED);
			}

			if(Small) 
			{
				twnd=CreateSmallTooltip(hwnd);
				CheckMenuItem(hSizeMenu,IDM_NORMAL,MF_BYCOMMAND|MF_UNCHECKED);
				CheckMenuItem(hSizeMenu,IDM_SMALL,MF_BYCOMMAND|MF_CHECKED);
			}
			else
			{
				twnd=CreateBigTooltip(hwnd);
				CheckMenuItem(hSizeMenu,IDM_NORMAL,MF_BYCOMMAND|MF_CHECKED);
				CheckMenuItem(hSizeMenu,IDM_SMALL,MF_BYCOMMAND|MF_UNCHECKED);
			}

			ShowClosedWindow(hwnd,hvDC);			

			if(TopMost) 
			{
				SetWindowPos(hwnd,HWND_TOPMOST,pnt.x,pnt.y,0,0, SWP_NOSIZE );
				CheckMenuItem(hSettingMenu,IDM_TOP,MF_BYCOMMAND|MF_CHECKED);
			}
			else
			{
				SetWindowPos(hwnd,HWND_NOTOPMOST,pnt.x,pnt.y,0,0, SWP_NOSIZE );
				CheckMenuItem(hSettingMenu,IDM_TOP,MF_BYCOMMAND|MF_UNCHECKED);
			}
	
			mouse=false;
			cursor_open=LoadCursor(hAppInst,MAKEINTRESOURCE(IDC_HAND_OPEN));
			cursor_closed=LoadCursor(hAppInst,MAKEINTRESOURCE(IDC_HAND_CLOSED));
	
			UseTimer=true;

			if(!Visible) 
				SendMessage(hwnd,WM_COMMAND,IDM_HIDE,0);
			else
				SendMessage(hwnd,WM_COMMAND,IDM_SHOW,0);

			break;


		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code) 
			{
				case TTN_SHOW:
					ShowWindow(twnd,SW_SHOW);
					break;
				break;
			}
			break;


        case WM_GETMINMAXINFO :
			maxmin.x = 300;
			maxmin.y = 300;

			lpmmi= (LPMINMAXINFO) lParam;
			lpmmi->ptMaxSize = maxmin;
			lpmmi->ptMinTrackSize = maxmin;
			lpmmi->ptMaxTrackSize = maxmin;
			break;

        case WM_TIMER:
			char *textbuffer;

			if(UseTimer)
			{
				SetClosedBackground(hvDC);
				ShowTime(hwnd,hvDC,TimeDisplayMode,Centybeats,Y2KCounter);
				
				GetLocalTime(&local);
				LocalToBeat(&beat,&local);

				if(ScanForAlarm(&local,AlarmList,&textbuffer))
				{
					PlayAlarm=true;
					PlaySound(MAKEINTRESOURCE(IDW_ALARM),hAppInst,SND_ASYNC|SND_RESOURCE);
					
					LoadString(hAppInst,IDS_ALARM,buffer2,256);
					MessageBox(hwnd,textbuffer,buffer2,MB_OK|MB_ICONWARNING);
				}
				if(oldbeat!=beat.iBeat)
				{
					if(PlayAlarm)
					{
						SendMessage(hwnd,WM_COMMAND,IDM_SHOW,0);
						PlaySound(MAKEINTRESOURCE(IDW_ALARM),hAppInst,SND_ASYNC|SND_RESOURCE);
					}
					else
					if(HeartBeat)		
						PlaySound(MAKEINTRESOURCE(IDW_HEARTBEAT),hAppInst,SND_ASYNC|SND_RESOURCE);

					oldbeat=beat.iBeat;
				}
			}
			break;

		case WM_DESTROY:
			SocketTool SInit;
			SInit.Exit();

		 	pnid.cbSize=sizeof(NOTIFYICONDATA);
			pnid.hWnd=hwnd;
			pnid.uID=ID_TRAY;
			Shell_NotifyIcon(NIM_DELETE, &pnid);

		 	DeleteDC(hvDC);
			if(timer) KillTimer(hwnd,timer);
			PostQuitMessage(0);
			break;

		case WM_TRAY:
            switch(lParam)
			{
				case WM_LBUTTONDOWN:
					if(!Visible)
						SendMessage(hwnd,WM_COMMAND,IDM_SHOW,0);
					else
					{
	 				 SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE| SWP_NOSIZE );
					 SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE| SWP_NOSIZE );
					 ShowWindow(hwnd,SW_SHOW);
					 BringWindowToTop(hwnd);
					}
					break;

				case WM_RBUTTONDOWN:
					GetCursorPos(&pnt);
					if(hMenu)
					{
//						if(!Visible)
//							SendMessage(hwnd,WM_COMMAND,IDM_SHOW,0);
						TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_VCENTERALIGN,pnt.x,pnt.y,0,hwnd,NULL);
					}
					break;
				case WM_LBUTTONDBLCLK:
//					if(!Visible)
//						SendMessage(hwnd,WM_COMMAND,IDM_SHOW,0);
					SendMessage(hwnd,WM_COMMAND,IDM_ALARM_SET,0);
				break;
			}
			 break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
             {

			 case IDM_SNTP_SYNC:
				DialogBox(hAppInst,MAKEINTRESOURCE(IDD_SNTP_SYNC),hwnd,SNTPSyncProc);
				break;
			 case IDM_ALARM_SET:
				UseTimer=false;
				DialogBox(hAppInst,MAKEINTRESOURCE(IDD_SET_ALARM),hwnd,SetAlarmProc);
				sprintf(buffer1,"%s\\alarm.dat",BaseDir);
				SaveAlarmList(buffer1,AlarmList);
				UseTimer=true;
				break;
			 case IDM_ALARM_VIEW:
				UseTimer=false;
				DialogBox(hAppInst,MAKEINTRESOURCE(IDD_VIEW_ALARM),hwnd,ViewAlarmProc);
				sprintf(buffer1,"%s\\alarm.dat",BaseDir);
				SaveAlarmList(buffer1,AlarmList);
				UseTimer=true;
				break;
/*			 
			case IDM_USER_SET:
				DialogBox(hAppInst,MAKEINTRESOURCE(IDD_SET_USER),hwnd,SetUserProc);
				break;
*/
			 case IDM_12H:
				CheckMenuItem(hTimeMenu,IDM_GMT,MF_BYCOMMAND|MF_UNCHECKED);
				CheckMenuItem(hTimeMenu,IDM_12H,MF_BYCOMMAND|MF_CHECKED);
				CheckMenuItem(hTimeMenu,IDM_24H,MF_BYCOMMAND|MF_UNCHECKED);
				CheckMenuItem(hTimeMenu,IDM_NLT,MF_BYCOMMAND|MF_UNCHECKED);
				TimeDisplayMode = TIMEMODE_12H;
				TDM=TIMEMODE_12H;
				break;
			 case IDM_24H:
				CheckMenuItem(hTimeMenu,IDM_GMT,MF_BYCOMMAND|MF_UNCHECKED);
				CheckMenuItem(hTimeMenu,IDM_12H,MF_BYCOMMAND|MF_UNCHECKED);
				CheckMenuItem(hTimeMenu,IDM_24H,MF_BYCOMMAND|MF_CHECKED);
				CheckMenuItem(hTimeMenu,IDM_NLT,MF_BYCOMMAND|MF_UNCHECKED);
				TimeDisplayMode = TIMEMODE_24H;
				TDM=TIMEMODE_24H;
				break;
			 case IDM_NLT:
				CheckMenuItem(hTimeMenu,IDM_GMT,MF_BYCOMMAND|MF_UNCHECKED);
				CheckMenuItem(hTimeMenu,IDM_12H,MF_BYCOMMAND|MF_UNCHECKED);
				CheckMenuItem(hTimeMenu,IDM_24H,MF_BYCOMMAND|MF_UNCHECKED);
				CheckMenuItem(hTimeMenu,IDM_NLT,MF_BYCOMMAND|MF_CHECKED);
				TimeDisplayMode = TIMEMODE_NLT;
				TDM=TIMEMODE_NLT;
				break;
			 case IDM_Y2K:
				if(Y2KCounter)
				{
					CheckMenuItem(hTimeMenu,IDM_Y2K,MF_BYCOMMAND|MF_UNCHECKED);
					Y2KCounter=false;
				}
				else
				{
					CheckMenuItem(hTimeMenu,IDM_Y2K,MF_BYCOMMAND|MF_CHECKED);
					Y2KCounter=true;
				}
				break;
			 case IDM_CENTY:
				if(Centybeats)
				{
					CheckMenuItem(hTimeMenu,IDM_CENTY,MF_BYCOMMAND|MF_UNCHECKED);
					Centybeats=false;
				}
				else
				{
					CheckMenuItem(hTimeMenu,IDM_CENTY,MF_BYCOMMAND|MF_CHECKED);
					Centybeats=true;
				}
				break;
			 case IDM_UDT:
				if(SwatchMode)
				{
					CheckMenuItem(hTimeMenu,IDM_UDT,MF_BYCOMMAND|MF_UNCHECKED);
					SwatchMode=false;
				}
				else
				{
					CheckMenuItem(hTimeMenu,IDM_UDT,MF_BYCOMMAND|MF_CHECKED);
					SwatchMode=true;
				}

				break;
			 case IDM_HEARTBEAT:
				 if(HeartBeat) 
				 {
					 CheckMenuItem(hSoundMenu,IDM_HEARTBEAT,MF_BYCOMMAND|MF_UNCHECKED);
					 HeartBeat=false;
				 }
				 else
				 {
					 CheckMenuItem(hSoundMenu,IDM_HEARTBEAT,MF_BYCOMMAND|MF_CHECKED);
					 HeartBeat=true;
				 }
				 break;
			 case IDM_ALARM:
				 if(Alarm) 
				 {
					 CheckMenuItem(hSoundMenu,IDM_ALARM,MF_BYCOMMAND|MF_UNCHECKED);
					 Alarm=false;
				 }
				 else
				 {
					 CheckMenuItem(hSoundMenu,IDM_ALARM,MF_BYCOMMAND|MF_CHECKED);
					 Alarm=true;
				 }
				 break;
			 case IDM_SMALL:
				 Small=true;
				 CheckMenuItem(hSizeMenu,IDM_SMALL,MF_BYCOMMAND|MF_CHECKED);
				 CheckMenuItem(hSizeMenu,IDM_NORMAL,MF_BYCOMMAND|MF_UNCHECKED);
				 ShowClosedWindow(hwnd,hvDC);			
				 DestroyWindow(twnd);
				 twnd=CreateSmallTooltip(hwnd);
				 break;
			 case IDM_NORMAL:
				 Small=false;
				 CheckMenuItem(hSizeMenu,IDM_SMALL,MF_BYCOMMAND|MF_UNCHECKED);
				 CheckMenuItem(hSizeMenu,IDM_NORMAL,MF_BYCOMMAND|MF_CHECKED);
	 			 ShowClosedWindow(hwnd,hvDC);			
				 DestroyWindow(twnd);
 				 twnd=CreateBigTooltip(hwnd);
				 break;
			 case IDM_TOP:
				 if(TopMost)
				 {
					 SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE| SWP_NOSIZE );
					 CheckMenuItem(hSettingMenu,IDM_TOP,MF_BYCOMMAND|MF_UNCHECKED);
					 TopMost=false;
				 }
				 else
				 {
	 				 SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE| SWP_NOSIZE );
					 CheckMenuItem(hSettingMenu,IDM_TOP,MF_BYCOMMAND|MF_CHECKED);
					 TopMost=true;
				 }
				 break;
				 
			 case IDM_HIDE:
				 GetWindowRect(hwnd,&rect);
				 oldpos.x=rect.left;
				 oldpos.y=rect.top;
		 		 ShowWindow(hwnd,SW_HIDE);
				 if(GetMenuItemID(hMenu,0)==IDM_HIDE)
				 {
					 ChangeMenu(hMenu,IDM_HIDE,"&Show",IDM_SHOW,MF_BYCOMMAND);
		  			 SetMenuText(hMenu, IDM_SHOW, FALSE, hAppInst,IDS_SHOW);
					 DeleteMenu(hMenu,IDM_HIDE,MF_BYCOMMAND);
				 }
				 Visible=false;
				 break;
			 case IDM_SHOW:
		 		 ShowWindow(hwnd,NULL);
				 ShowWindow(hwnd,SW_SHOW);
				 ShowWindow(hwnd,SW_RESTORE);
				 if(GetMenuItemID(hMenu,0)==IDM_SHOW)
				 {
					 ChangeMenu(hMenu,IDM_SHOW,"&Hide",IDM_HIDE,MF_BYCOMMAND);
		  			 SetMenuText(hMenu, IDM_HIDE, FALSE, hAppInst,IDS_HIDE);
					 DeleteMenu(hMenu,IDM_SHOW,MF_BYCOMMAND);
				 }
 				 BringWindowToTop(hwnd);
				 Visible=true;
				 break;

			 case IDM_ABOUT:
				DialogBox(hAppInst,MAKEINTRESOURCE(IDD_ABOUT),hwnd,AboutProc);
				break;
			 case IDM_EXIT:
				sprintf(buffer1,"%s\\ibeat.ini",BaseDir);
				fset=fopen(buffer1,"wb");
				if(fset)
				{
					if(Visible)
					{
						GetWindowRect(hwnd,&rect);
						pnt.x=rect.left;
						pnt.y=rect.top;
					}
					else
					{
						pnt.x=oldpos.x;
						pnt.y=oldpos.y;
					}
					fwrite(&pnt,sizeof(POINT),1,fset);
					fwrite(&TimeDisplayMode,sizeof(int),1,fset);
					fwrite(&AlarmDisplayMode,sizeof(int),1,fset);
					fwrite(&Centybeats,sizeof(bool),1,fset);
					fwrite(&Y2KCounter,sizeof(bool),1,fset);
					fwrite(&SwatchMode,sizeof(bool),1,fset);
					fwrite(&Visible,sizeof(bool),1,fset);
					fwrite(&TopMost,sizeof(bool),1,fset);
					fwrite(&HeartBeat,sizeof(bool),1,fset);
					fwrite(&Alarm,sizeof(bool),1,fset);
					fwrite(&Small,sizeof(bool),1,fset);
					fwrite(&BGColor,sizeof(COLORREF),1,fset);
					fwrite(&FNTColor,sizeof(COLORREF),1,fset);
					fwrite(&logfnt,sizeof(LOGFONT),1,fset);
					fwrite(GlobalSettingUserName,sizeof(char),256,fset);
					fwrite(GlobalSettingUserEMail,sizeof(char),256,fset);
					fwrite(GlobalSettingUserLocation,sizeof(char),256,fset);
					fwrite(GlobalSettingUserPassword,sizeof(char),8,fset);
					fwrite(&GlobalSettingUserFoobar,sizeof(int),1,fset);
					fwrite(&GlobalSettingUAID,sizeof(int),1,fset);
					fwrite(GlobalSNTPHostaddress,sizeof(char),256,fset);
					fclose(fset);
				}
				sprintf(buffer1,"%s\\alarm.dat",BaseDir);
				SaveAlarmList(buffer1,AlarmList);

				SendMessage(hwnd,WM_DESTROY,0,0);
				break;
			 case IDM_COLOR:
				CHOOSECOLOR *clref;
				clref = new CHOOSECOLOR;
				 clref->lStructSize=sizeof(CHOOSECOLOR);
				 clref->hwndOwner=hwnd;
				 clref->hInstance= NULL;
				 clref->lCustData=NULL;
				 clref->lpCustColors=CustomColorRef; //__VERY__ important to have this array of 16 colorrefs
				 clref->lpfnHook=NULL;
				 clref->lpTemplateName=NULL;
				 clref->rgbResult=BGColor;
				 clref->Flags=CC_RGBINIT|CC_SOLIDCOLOR;
				 if(ChooseColor(clref))
					 BGColor=clref->rgbResult;
				 delete clref;
				break;

			 case IDM_FONT:
				CHOOSEFONT *fntref;
				fntref = new CHOOSEFONT;
				memset(fntref,0,sizeof(CHOOSEFONT));
				fntref->lStructSize=sizeof(CHOOSEFONT);
				fntref->hwndOwner=hwnd;
				fntref->hDC=hvDC;
				fntref->lpLogFont=&logfnt;
				fntref->iPointSize=220;
				fntref->lCustData=NULL;
				fntref->lpfnHook=NULL;
				fntref->Flags=CF_LIMITSIZE|CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS|CF_EFFECTS;
				fntref->rgbColors=FNTColor;
				fntref->lpTemplateName=NULL;
				fntref->hInstance=NULL;
				fntref->nFontType=SCREEN_FONTTYPE;
				fntref->nSizeMin=10;
				fntref->nSizeMax=16;

				if(ChooseFont(fntref))
				{
					FNTColor=fntref->rgbColors;
					//set new Font
					DeleteObject(hFontWB);
					hFontWB=CreateFontIndirect(&logfnt);
				}
				break;
				delete fntref;
			}
			break;

		case WM_PAINT:
			hDC = BeginPaint(hwnd,&paintstruct);
			BitBlt(hDC,0,0,xmax,ymax,hvDC,0,0,SRCCOPY);
			EndPaint(hwnd,&paintstruct);
			break;

		case WM_NCMOUSEMOVE:
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			break;
		case WM_MOUSEMOVE:
			if (twnd != NULL) 
			{
				MSG msg;  
                msg.lParam = lParam; 
                msg.wParam = wParam; 
                msg.message = message;                 
				msg.hwnd = hwnd; 
                SendMessage(twnd, TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &msg);             
			} 			
	
			if (wParam&MK_LBUTTON)
			{
				SetCursor(cursor_closed);
				MOVED=TRUE;
				pnt.x=LOWORD(lParam);
				pnt.y=HIWORD(lParam);

				ClientToScreen(hwnd,&pnt);
				pnt.x-=pos.x;
				pnt.y-=pos.y;
				SetWindowPos(hwnd,HWND_TOP,pnt.x,pnt.y,0,0, SWP_NOSIZE );
			}
			else
				SetCursor(cursor_open);
			break;

		case WM_LBUTTONDBLCLK:
			DialogBox(hAppInst,MAKEINTRESOURCE(IDD_SET_ALARM),hwnd,SetAlarmProc);
			break;

		case WM_LBUTTONDOWN:
//			sprintf(buffer1,"x:%d, y:%d",LOWORD(lParam),HIWORD(lParam));
//			MessageBox(hwnd,buffer1,"xy pos",MB_OK);
//			break;
			SetCursor(cursor_closed);
			PlayAlarm=false;
			SetCapture(hwnd);
			pos.x=LOWORD(lParam)+3;
			pos.y=HIWORD(lParam)+22;
			MOVED=FALSE;
			break;

		case WM_LBUTTONUP:
			SetCursor(cursor_open);
			if(false)//!MOVED)
			{
				if(Small)
					SendMessage(hwnd,WM_COMMAND,IDM_NORMAL,0);
				else
					SendMessage(hwnd,WM_COMMAND,IDM_SMALL,0);
			}			
			//open window

			ReleaseCapture();
			break;

		case WM_RBUTTONDOWN:
			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);

			ClientToScreen(hwnd,&pnt);
	
			if(hMenu)
			{
				TrackPopupMenu(hMenu,0,pnt.x,pnt.y,0,hwnd,NULL);
			}
			break;

        default :
             return DefWindowProc(hwnd,message,wParam,lParam);
    }

    return 0;
}


void SetClosedBackground(HDC dc)
{
static HBRUSH hBrush=NULL,hBrushAlarm=NULL;
static HPEN hPen1,hPen2,hPen3,hPen4,hPen5;
static init=1;	
static COLORREF oldcolor=-1;

	if(init)
	{
		hBrush = (HBRUSH)CreateSolidBrush(BGColor);
		hBrushAlarm = (HBRUSH)CreateSolidBrush(RGB( 0xff,0x00,0x00));
		hPen1 = CreatePen(PS_SOLID,2,RGB( 0xa0,0xa0,0xa0)); //b0 upper outer
		hPen2 = CreatePen(PS_SOLID,2,RGB( 0x40,0x40,0x40)); //50 lower outer
		hPen3 = CreatePen(PS_SOLID,2,RGB( 0x90,0x90,0x90)); //90 middle
		hPen4 = CreatePen(PS_SOLID,1,RGB( 0xb5,0xb5,0xb5)); //b0 lower inner
		hPen5 = CreatePen(PS_SOLID,1,RGB( 0x50,0x50,0x50)); //50 upper inner
		init=0;
	}

	if(BGColor!=oldcolor)
	{
		DeleteObject(hBrush);
		hBrush = (HBRUSH)CreateSolidBrush(BGColor);
		oldcolor=BGColor;
	}

	//Background
	if(!PlayAlarm)
		SelectObject(dc,hBrush);
	else
		SelectObject(dc,hBrushAlarm);

	PatBlt(dc,5,35,205,104,PATCOPY);

	if(!Small) //Normal
	{
		PatBlt(dc,5,35,205,104,PATCOPY);
		//upper outer border
		SelectObject(dc,hPen1);
		Arc(dc,7,38,206,138,  206,60,7,112);

		//lower outer border
		SelectObject(dc,hPen2);
		Arc(dc,7,39,206,137,7,112,206,60);

		//middle border
		SelectObject(dc,hPen3);
		Arc(dc,9,40,205,136, 0,0,0,0);

		//lower inner border
		SelectObject(dc,hPen4);
		Arc(dc,10,40,203,134,   10,112,206,60);

		//upper inner border
		SelectObject(dc,hPen5);
		Arc(dc,10,40,203,135,	206,60,7,112);
	}
	else //Small
	{
		PatBlt(dc,5,35,107,80,PATCOPY);


		//upper outer border
		SelectObject(dc,hPen1);
		Arc(dc,7,38,107,78,	107,48,7,68);

		//lower outer border
		SelectObject(dc,hPen2);
		Arc(dc,7,38,107,78,	7,68,107,48);

		//middle border
		SelectObject(dc,hPen3);
		Arc(dc,9,40,105,76,	0,0,0,0);


		//lower inner border
		SelectObject(dc,hPen4);
		Arc(dc,10,41,104,75,	15,65,110,45);

		//upper inner border
		SelectObject(dc,hPen5);
		Arc(dc,10,41,104,75,	110,45,15,65);
	}



}

void ShowClosedWindow(HWND hwnd,HDC dc)
{
RECT rect;
HRGN hrgn;


	if(!Small)
	{
		rect.top=57;//61
		rect.left=10;//11
		rect.bottom=157;//161
		rect.right=210;//211

		hrgn= CreateEllipticRgnIndirect(&rect);
		SetWindowRgn(hwnd,hrgn,TRUE);
	}
	else
	{
		rect.top=57;//61
		rect.left=10;//11
		rect.bottom=97;//101
		rect.right=110;//111

		hrgn= CreateEllipticRgnIndirect(&rect);
		SetWindowRgn(hwnd,hrgn,TRUE);
	}


	SetClosedBackground(dc);
	InvalidateRect(hwnd,NULL,0);
}

void Y2KTimer(char * string)
{
	SYSTEMTIME local;
	BEATTIME beat;
	int days, hours,minutes,seconds;
	char out[256];
	char buffer[256];

	GetLocalTime(&local);

	hours=23-local.wHour;
	minutes=59-local.wMinute;
	seconds=59-local.wSecond;
	if(hours>=24) hours-=24;
	if(minutes>=60) minutes-=60;
	if(seconds>=60) seconds-=60;

	LocalToBeat(&beat,&local);
	days=beat.iDay;
	local.wYear=1999;
	local.wMonth=12;
	local.wDay=31;
	local.wHour=12;
	local.wMinute=0;
	local.wSecond=0;
	LocalToBeat(&beat,&local);

	days=beat.iDay-days;

	LoadString(hAppInst,IDS_DAYS,buffer,256);
	if(local.wYear<2000)
	{
		sprintf(out,"%d %s, %2d:%02d.%02d",days,buffer,hours,minutes,seconds);
		strcpy(string,out);		
	}
	else
		strcpy(string,"");

}


void ShowTime(HWND hwnd,HDC hvDC,int TimeDisplayMode, bool Centybeats,bool Y2KCounter)
{
	char text[256];
	static HFONT hFontLT;
	RECT rect;
    NOTIFYICONDATA pnid;
	static int init=1;
	static COLORREF oldBG=-1,oldFNT=-1;
	SYSTEMTIME local;
	BEATTIME beat;
	IALARM *ialarm;
	char buffer[256];

	GetLocalTime(&local);
	LocalToBeat(&beat,&local);
	

	if (init)
	{
		hFontLT = (HFONT)GetStockObject( ANSI_VAR_FONT ); 								
		init=0;	
	}

	if(oldFNT!=FNTColor)
	{
		SetTextColor(hvDC,FNTColor);
		oldFNT=FNTColor;
	}
	
	if(oldBG!=BGColor)
	{
		SetBkColor(hvDC,BGColor);
		oldBG=BGColor;
	}
	if(PlayAlarm)
	{
		SetBkColor(hvDC,RGB( 0xff,0x00,0x00));
		oldBG=RGB( 0xff,0x00,0x00);
	}


	//small ibeat time
	if(Small)
	{
		SelectObject(hvDC,hFontWB);
		sprintf(text,"@%03d",beat.iBeat);

		rect.top=45;
		rect.bottom=100;
		rect.left=8;
		rect.right=106;

		DrawText(hvDC,text,strlen(text),&rect,DT_CENTER);
	}
	else
	{
		//big ibeat time
		SelectObject(hvDC,hFontWB);
		sprintf(text,"iBeat: %03d@%03d",beat.iDay,beat.iBeat);

		if(TimeDisplayMode)
		{
			rect.top=72; //72
			rect.bottom=102; //102
		}
		else
		{
			rect.top=76;
			rect.bottom=106;
		}
		
		if(Centybeats)
		{
			rect.left=15; //20
			rect.right=190; //195
		}
		else
		{
			rect.left=20; //20
			rect.right=195; //195
		}


		DrawText(hvDC,text,strlen(text),&rect,DT_CENTER);
		
		SelectObject(hvDC,hFontLT);

		if(Centybeats)
		{
			rect.left=180; //20
			rect.right=195; //195

			if(TimeDisplayMode)
			{
				rect.top=80;
				rect.bottom=95;
			}
			else
			{
				rect.top=84;
				rect.bottom=95;
			}
			sprintf(text,".%02d",beat.iFraction/10);
			DrawText(hvDC,text,strlen(text),&rect,DT_CENTER);
		}

		//local time
		SelectObject(hvDC,hFontLT);
		if(TimeDisplayMode)
		{
			rect.top=105;
			rect.left=20;
			rect.bottom=125;
			rect.right=195;
			LoadString(hAppInst,IDS_LOCAL,buffer,256);
			switch(TimeDisplayMode)
			{
				case TIMEMODE_12H:
					sprintf(text,"%s %d/%d - %2d:%02d.%02d %s",buffer,local.wMonth,local.wDay,(local.wHour>12)?local.wHour-12:local.wHour,local.wMinute,local.wSecond,(local.wHour>12)?"pm":"am");
					break;
				case TIMEMODE_24H:
					sprintf(text,"%s %d.%d. - %02d:%02d.%02d",buffer,local.wDay,local.wMonth,local.wHour,local.wMinute,local.wSecond);
					break;
				default:
					sprintf(text,"");
					break;
			}

			DrawText(hvDC,text,strlen(text),&rect,DT_CENTER);

		}		

		//alarm time
		rect.top=53;
		rect.left=20;
		rect.bottom=73;
		rect.right=195;

		if(Y2KCounter)
		{

			Y2KTimer(text);
			DrawText(hvDC,text,strlen(text),&rect,DT_CENTER);

		}
		else
		{		
			GetLocalTime(&local);
			ialarm = ScanNextAlarm(AlarmList,&local);
			if(ialarm)
			{
				LoadString(hAppInst,IDS_ALARM,buffer,256);
				switch(TimeDisplayMode)
				{
					case TIMEMODE_12H:
						sprintf(text,"%s %d/%d - %02d:%02d",buffer,ialarm->AlarmTime.wMonth,ialarm->AlarmTime.wDay,ialarm->AlarmTime.wHour,ialarm->AlarmTime.wMinute);
						break;
					case TIMEMODE_24H:
						sprintf(text,"%s %d.%d. - %02d:%02d",buffer,ialarm->AlarmTime.wDay,ialarm->AlarmTime.wMonth,ialarm->AlarmTime.wHour,ialarm->AlarmTime.wMinute);
						break;
					default:
						sprintf(text,"");
						break;
				}
				DrawText(hvDC,text,strlen(text),&rect,DT_CENTER);
			}
		}
	}
	//tool tip in systray

	sprintf(text,"iBeat: %d@%03d",beat.iDay,beat.iBeat);
//	SetWindowText(hwnd,text);
	strcpy(pnid.szTip,text);

	pnid.cbSize=sizeof(NOTIFYICONDATA);
	pnid.uID=ID_TRAY;
	pnid.hWnd=hwnd;
	pnid.uFlags=NIF_TIP;
	Shell_NotifyIcon(NIM_MODIFY , &pnid);

	InvalidateRect(hwnd,NULL,0);

}

bool StringIsBeat(char * text)
{
	
	if(strchr(text,'@'))
		return true;
	else
		return false;


}

BEATTIME GetBeatFromString(char *text)
{
int beat;
int iday;
char *cnt;
int h,m;
char am[5];
BEATTIME Beat;

	Beat.iBeat=0;
	Beat.iDay=0;
	Beat.iFraction=0;

	// Beat only
	if(text[0]=='@')
	{
		iday=0;
		if(sscanf(text+1,"%d",&beat))
		{
			Beat.iBeat=beat;
			Beat.iDay=-1;
			Beat.iFraction=0;
			return Beat;
		}
	}
	
	// iday@beat or normal time
	if(text[0]>='0' &&text[0] <='9')
	{

		// iday@beat
		cnt=strchr(text,'@');
		if(cnt)
		{
			sscanf(text,"%d %1s %d",&iday,am,&beat);
			Beat.iBeat=beat;
			Beat.iDay=iday;
			Beat.iFraction=0;
			return Beat;
		}

		// normal time
		cnt=strchr(text,':');
		if(cnt)
		{
			sscanf(text,"%d %1s %d %2s",&h,am,&m,am);

			if(am[0]=='p')
			{
				h+=12;
			}
			//			convert local to gmt time
			SYSTEMTIME local;
			GetLocalTime(&local);
			local.wHour=h;
			local.wMinute=m;
			local.wSecond=0;
			LocalToBeat(&Beat,&local);
			Beat.iDay=-1;
			return Beat;
		}
	}

	Beat.iBeat=-1;
	Beat.iDay=-1;
	Beat.iFraction=0;
	return Beat;
}

int round(double value)
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

void TurnAddOffset(POINT *plist,int angle)
{
int i;
double y,x,a,b;
double sa;
double ca;
double pi;
double rad;
int xofs,yofs;


	pi=3.141592;
	
	xofs=280;
	yofs=80;

	rad = pi/180.0;
	rad = rad *angle;

		

	for (i=0;i<4;i++)
	{
		sa=sin(rad);
		ca=cos(rad);
		x=(float)plist[i].x;
		y=(float)plist[i].y;
		a= (ca * x) - (sa * y);
		b= (sa * x) + (ca * y);
		plist[i].x = round(a);
		plist[i].y = round(b);
		plist[i].x +=xofs;
		plist[i].y +=yofs;
	
	}
}


void DrawTime(HDC dc, SYSTEMTIME *time)
{
int h;
int m;
int s;
POINT hour[4];
POINT minute[4];
POINT second[4];
int h_angle;
int m_angle;
int s_angle;
static HPEN outPen,fillPen,secPen;
static HBRUSH outBrush;
static HBRUSH fillBrush;
static HBRUSH secBrush;

static int init=1;

	if(init)
	{
		secPen=		CreatePen(  PS_SOLID,0,RGB(0xa0,0xa0,0xa0));
		fillPen=	CreatePen(  PS_SOLID,0,RGB(0xff,0xff,0xff));
		outPen=		CreatePen(  PS_SOLID,0,RGB(0xb0,0xb0,0xb0));

		outBrush=	CreateSolidBrush(RGB(0xd0,0xd0,0xd0));
		fillBrush=	CreateSolidBrush(RGB(0xff,0xff,0xff));
		secBrush=	CreateSolidBrush(RGB(0xa0,0xa0,0xa0));
		init=0;
	}

	h=time->wHour;
	while (h>=12) h-=12;
	m=time->wMinute;
	s=time->wSecond;

		//hour=30°
		//minute=6°
	h_angle = h*30;
	h_angle = h_angle + (m/2);

	m_angle = m*6;

	s_angle = s*6;

	minute[0].x=-2;
	minute[0].y=+2;

	minute[1].x=-2;
	minute[1].y=-48;

	minute[2].x=+2;
	minute[2].y=-48;

	minute[3].x=+2;
	minute[3].y=+2;

	TurnAddOffset(minute,m_angle);
	SelectObject(dc,outPen);
	SelectObject(dc,outBrush);
	Polygon(dc,minute,4);

	minute[0].x=-2;
	minute[0].y=-15;

	minute[1].x=-2;
	minute[1].y=-46;

	minute[2].x=+2;
	minute[2].y=-46;

	minute[3].x=+2;
	minute[3].y=-15;

	TurnAddOffset(minute,m_angle);
	SelectObject(dc,fillBrush);
	Polygon(dc,minute,4);

	hour[0].x=-2;
	hour[0].y=+2;

	hour[1].x=-2;
	hour[1].y=-32;

	hour[2].x=+2;
	hour[2].y=-32;

	hour[3].x=+2;
	hour[3].y=+2;

	TurnAddOffset(hour,h_angle);
	SelectObject(dc,outBrush);
	Polygon(dc,hour,4);

	hour[0].x=-2;
	hour[0].y=-15;

	hour[1].x=-2;
	hour[1].y=-30;

	hour[2].x=+2;
	hour[2].y=-30;

	hour[3].x=+2;
	hour[3].y=-15;

	TurnAddOffset(hour,h_angle);
	SelectObject(dc,fillBrush);
	Polygon(dc,hour,4);
	
	second[0].x=-0;
	second[0].y=+0;

	second[1].x=-0;
	second[1].y=-48;

	second[2].x=+0;
	second[2].y=-48;

	second[3].x=+0;
	second[3].y=+0;

	TurnAddOffset(second,s_angle);
	SelectObject(dc,outBrush);
	Polygon(dc,second,4);

//	SelectObject(dc,outPen);
//	SelectObject(dc,outBrush);
//	Ellipse(dc,78,159,83,164);

}
void UnixTimeToFileTime(time_t t, LPFILETIME pft)   
{
     // Note that LONGLONG is a 64-bit value     
LONGLONG ll;

     ll = Int32x32To64(t, 10000000) + 116444736000000000;
	 pft->dwLowDateTime = (DWORD)ll;
     pft->dwHighDateTime = long(ll >> 32);   
}



BOOL CALLBACK SNTPSyncProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam)
{
	char text[256];
	static long sync_time;
	static int timer;
	const long Tmod=2206051200;
	SocketTool SInit;
	struct SNTP_Sync *ReturnBuffer;
	static long Offset=0;
	static int mOffset=0;
	char buffer1[4096],buffer2[256];

	lParam = lParam;

	switch(Msg)
    {
		case WM_INITDIALOG :
			CenterWindow(NULL,hwnd);
			SetDlgItemText(hwnd,IDC_SNTP,GlobalSNTPHostaddress);

		    SendMessage(GetDlgItem(hwnd,IDC_STATUS), PBM_SETRANGE, 0, MAKELPARAM(0,50)); 
			SendMessage(GetDlgItem(hwnd,IDC_STATUS), PBM_SETSTEP, (WPARAM) 1, 0); 
//			EnableWindow(GetDlgItem(hwnd,IDC_SET),false);
			timer=0;
			
			SInit.Init();
			EnableWindow(GetDlgItem(hwnd,IDC_SET),false);
			if(!InitSyncTime(hwnd,3,IDC_READ))
			{
				LoadString(hAppInst,IDS_SNTP_OPEN_TEXT,buffer1,4096);
				LoadString(hAppInst,IDS_SNTP_OPEN_TITEL,buffer2,256);
				MessageBox(hwnd,buffer1,buffer2,MB_OK);
			}
			break;
        case WM_TIMER :
			timer++;
			if(timer>50)
			{
				EnableWindow(GetDlgItem(hwnd,IDC_SYNC),true);
				SendMessage(GetDlgItem(hwnd,IDC_STATUS), PBM_SETPOS , 0, 0); 			
				KillTimer(hwnd,2);
				timer=0;

				LoadString(hAppInst,IDS_SNTP_OPEN_TEXT,buffer1,4096);
				LoadString(hAppInst,IDS_SNTP_OPEN_TITEL,buffer2,256);
				MessageBox(hwnd,buffer1,buffer2,MB_OK);
			}
			else
			{
				SendMessage(GetDlgItem(hwnd,IDC_STATUS), PBM_STEPIT , 0, 0); 			
			}
			break;
        case WM_COMMAND :
			switch (LOWORD(wParam))
			{
			case IDC_SET:
				long ts;
				FILETIME ft;     
				SYSTEMTIME systime;
				
				time(&ts);
				ts += Offset;
				UnixTimeToFileTime(ts, &ft);
				FileTimeToSystemTime(&ft,&systime);
				SetSystemTime(&systime);
				EnableWindow(GetDlgItem(hwnd,IDC_SET),false);
				break;

			case IDC_SYNC:
				EnableWindow(GetDlgItem(hwnd,IDC_SYNC),false);
				SendMessage(GetDlgItem(hwnd,IDC_SNTP),WM_GETTEXT,256,(LPARAM)text);			
				if(GetSyncTime(text))
				{
					timer=0;
					SetTimer(hwnd,2,100,NULL);
				}
				else
				{
					EnableWindow(GetDlgItem(hwnd,IDC_SYNC),true);
				}
				break;
			case IDC_READ:
				SendMessage(GetDlgItem(hwnd,IDC_STATUS), PBM_SETPOS , 0, 0); 			
				timer=0;
				KillTimer(hwnd,2);
				
				ReturnBuffer=(struct SNTP_Sync *)lParam;
				switch(ReturnBuffer->LI)
				{		
				case 0:
					SetDlgItemText(hwnd,IDC_LI,"OK");
					Offset=ReturnBuffer->Offset;
					mOffset=ReturnBuffer->mOffset;
					EnableWindow(GetDlgItem(hwnd,IDC_SET),true);
					break;
				case 1:
					SetDlgItemText(hwnd,IDC_LI,"+ Leap Second");
					
					LoadString(hAppInst,IDS_LI1_TEXT,buffer1,4096);
					LoadString(hAppInst,IDS_LI1_TITEL,buffer2,256);
					MessageBox(hwnd,buffer1,buffer2,MB_OK);
					break;
				case 2:
					SetDlgItemText(hwnd,IDC_LI,"- Leap Second");
					LoadString(hAppInst,IDS_LI2_TEXT,buffer1,4096);
					LoadString(hAppInst,IDS_LI2_TITEL,buffer2,256);
					MessageBox(hwnd,buffer1,buffer2,MB_OK);
					break;
				case 3:
					SetDlgItemText(hwnd,IDC_LI,"NOT synchronized");
					break;
				}

				SetDlgItemInt(hwnd,IDC_STRATUM,ReturnBuffer->Stratum,false);
				SetDlgItemInt(hwnd,IDC_MODE,ReturnBuffer->Mode,false);
				SetDlgItemInt(hwnd,IDC_VERSION,ReturnBuffer->Version,false);
				SetDlgItemInt(hwnd,IDC_DELAY2,ReturnBuffer->Delay,false);
				SetDlgItemInt(hwnd,IDC_OFFSET,ReturnBuffer->Offset,true);
				
				delete ReturnBuffer;

				EnableWindow(GetDlgItem(hwnd,IDC_SYNC),true);
				break;

				case IDC_CLOSE:
				GetDlgItemText(hwnd,IDC_SNTP,GlobalSNTPHostaddress,256);
				EndDialog(hwnd,0);
				return 1;
				break;
           }
		   break;
	} 
    return 0;



}

BOOL CALLBACK SetAlarmProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam)
{
	lParam = lParam;
	char reason[2048];
	IALARM alarm;
	static HWND dwnd;
	HDC hDC;
	static HDC hvDC;
	static HBITMAP hvBMP;
	static int VISIBLE;
	SYSTEMTIME time;
	SYSTEMTIME date;
	BEATTIME beat;
	char beatstr[80];

	switch(Msg)
    {
		case WM_INITDIALOG :
			CenterWindow(NULL,hwnd);
			VISIBLE=false;
			break;

		case WM_NOTIFY:
				DateTime_GetSystemtime(GetDlgItem(hwnd,IDC_TIME),&time);

				hDC = GetDC(hwnd);
				SelectObject(hDC,(HBRUSH)GetStockObject(LTGRAY_BRUSH));
				PatBlt(hDC,230,30,100,100,PATCOPY);
				DrawTime(hDC,&time);
				ReleaseDC(hwnd,hDC);

				MonthCal_GetCurSel(GetDlgItem(hwnd,IDC_DATE),&date);

				time.wDay=date.wDay;
				time.wMonth=date.wMonth;
				time.wYear=date.wYear;
				LocalToBeat(&beat,&time);
	
				sprintf(beatstr,"%d@%03d",beat.iDay,beat.iBeat);
				SetWindowText(GetDlgItem(hwnd,IDC_EDIT_BEATS),beatstr);

			break;
        case WM_COMMAND :
			switch (LOWORD(wParam))
			{
			case IDC_EDIT_REASON:
				SendMessage(hwnd,DM_SETDEFID,IDC_ALARM,0);
				break;
			case IDC_EDIT_BEATS:
				SendMessage(hwnd,DM_SETDEFID,IDC_TRANSLATE,0);
				break;
			case IDC_TRANSLATE:
				GetWindowText(GetDlgItem(hwnd,IDC_EDIT_BEATS),beatstr,80);
				if(StringIsBeat(beatstr))
				{
					beat=GetBeatFromString(beatstr);
					if(beat.iBeat>0)
					{
						BeatToLocal(&time,&beat);
						hDC = GetDC(hwnd);
						SelectObject(hDC,(HBRUSH)GetStockObject(LTGRAY_BRUSH));
						PatBlt(hDC,230,30,100,100,PATCOPY);
						DrawTime(hDC,&time);
						ReleaseDC(hwnd,hDC);
						DateTime_SetSystemtime(GetDlgItem(hwnd,IDC_TIME),GDT_VALID ,&time);
						if(beat.iDay>=0)
							MonthCal_SetCurSel(GetDlgItem(hwnd,IDC_DATE),&time);
					}
				}
				break;

			case IDC_NOW:
				GetLocalTime(&time);
				SetLocalTime(&time);
				MonthCal_SetCurSel(GetDlgItem(hwnd,IDC_DATE),&time);
				DateTime_SetSystemtime(GetDlgItem(hwnd,IDC_TIME),GDT_VALID ,&time);
				hDC = GetDC(hwnd);
				SelectObject(hDC,(HBRUSH)GetStockObject(LTGRAY_BRUSH));
				PatBlt(hDC,230,30,100,100,PATCOPY);
				DrawTime(hDC,&time);
				ReleaseDC(hwnd,hDC);
				LocalToBeat(&beat,&time);
				sprintf(beatstr,"%d@%03d",beat.iDay,beat.iBeat);
				SetWindowText(GetDlgItem(hwnd,IDC_EDIT_BEATS),beatstr);
				break;

			case IDC_ALARM:
				GetDlgItemText(hwnd,IDC_EDIT_REASON,reason,2048);
				MonthCal_GetCurSel(GetDlgItem(hwnd,IDC_DATE),&alarm.AlarmTime);
				DateTime_GetSystemtime(GetDlgItem(hwnd,IDC_TIME),&time);

				alarm.AlarmTime.wHour=time.wHour;
				alarm.AlarmTime.wMinute=time.wMinute;
				alarm.AlarmTime.wSecond=time.wSecond;

				strcpy(alarm.Note,reason);

				alarm.Expired=false;
				alarm.CurrentUserFooBar=GlobalSettingUserFoobar;
				alarm.CurrentUniqueAlarmID=++GlobalSettingUAID;
				alarm.ConfirmFlag=false;
				alarm.OriginUniqueAlarmID=GlobalSettingUAID;
				alarm.OriginUserFooBar=GlobalSettingUserFoobar;
				alarm.SendFlag=false;
					
				AddAlarm(AlarmList,&alarm);
				break;
			case IDC_CLOSE:

					EndDialog(hwnd,0);
					return 1;
					break;
           }
		   break;
        case WM_PAINT:
			if(!VISIBLE)
			{
				hDC = GetDC(hwnd);
				SelectObject(hDC,(HBRUSH)GetStockObject(LTGRAY_BRUSH));
				PatBlt(hDC,230,30,100,100,PATCOPY);
				GetLocalTime(&time);
				DrawTime(hDC,&time);
				ReleaseDC(hwnd,hDC);
				VISIBLE=true;
			}
			break;
	} 
    return 0;
}

BOOL CALLBACK ViewAlarmProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam)
{
	lParam = lParam;
	LVCOLUMN lvc;
	LVITEM	 lvi;
	IALARMLIST *List;
	static IALARM *AlarmArray;
	BEATTIME beat;
	char text[256];
	char Note[2048];
	int i,sel,o;
	static int MaxItems=0;
	char buffer[4096];
	
	switch(Msg)
    {
        case WM_INITDIALOG :
			
			ListView_SetExtendedListViewStyleEx(GetDlgItem(hwnd,IDC_LIST),
						LVS_EX_FULLROWSELECT ,
						LVS_EX_FULLROWSELECT );

			CenterWindow(NULL,hwnd);
			

			lvc.mask=LVCF_TEXT |LVCF_WIDTH ;
			lvc.cx =100;
			lvc.pszText =buffer;
			LoadString(hAppInst,IDS_DATETIME,buffer,4096);
			lvc.cchTextMax =strlen(buffer);
			ListView_InsertColumn(GetDlgItem(hwnd,IDC_LIST),0,&lvc);
			lvc.cx =60;
			LoadString(hAppInst,IDS_IBEAT,buffer,4096);
			lvc.pszText =buffer;
			lvc.cchTextMax =strlen(buffer);
			ListView_InsertColumn(GetDlgItem(hwnd,IDC_LIST),1,&lvc);
			lvc.cx =440;
			LoadString(hAppInst,IDS_NOTE,buffer,4096);
			lvc.pszText =buffer;
			lvc.cchTextMax =strlen(buffer);
			ListView_InsertColumn(GetDlgItem(hwnd,IDC_LIST),2,&lvc);

			ListView_DeleteAllItems(    GetDlgItem(hwnd,IDC_LIST)		);		

			SortAlarmList(&AlarmList);

			List=AlarmList;
			i=0;
			while(List)
			{
				if(List->Alarm)
				{
					i++;
				}
				List=List->Next;
			}
			if(AlarmArray)
				delete [] AlarmArray;
			AlarmArray= new IALARM[i];
			MaxItems=i;
			List=AlarmList;
			i=0;
			while(List)
			{
				if(List->Alarm)
				{
		
					if(TDM==TIMEMODE_12H)
					{
						sprintf(text,"%2d/%2d %4d - %02d:%02d\0",List->Alarm->AlarmTime.wMonth,
															   List->Alarm->AlarmTime.wDay,
															   List->Alarm->AlarmTime.wYear,
															   List->Alarm->AlarmTime.wHour,
															   List->Alarm->AlarmTime.wMinute);
					}
					else
					{
						sprintf(text,"%2d.%2d.%4d - %02d:%02d\0",List->Alarm->AlarmTime.wDay,
															   List->Alarm->AlarmTime.wMonth,
															   List->Alarm->AlarmTime.wYear,
															   List->Alarm->AlarmTime.wHour,
															   List->Alarm->AlarmTime.wMinute);
					}
					lvi.mask =  LVIF_TEXT;
					lvi.iItem = i;
					lvi.iSubItem = 0;
					lvi.pszText = text;
					lvi.cchTextMax = strlen(text);
					ListView_InsertItem(GetDlgItem(hwnd,IDC_LIST),&lvi);
					LocalToBeat(&beat,&List->Alarm->AlarmTime);
					sprintf(text,"%d@%03d\0",beat.iDay,beat.iBeat);
					ListView_SetItemText(GetDlgItem(hwnd,IDC_LIST),i,1,text);
				
					o=0;
					while(List->Alarm->Note[o]!=0)
					{
						//Note[o]=List->Alarm->Note[o];
						if(List->Alarm->Note[o]==13)
							Note[o]='/';
						else
						if(List->Alarm->Note[o]==10)
							Note[o]='/';
						else
						Note[o]=List->Alarm->Note[o];
						o++;

					}
					Note[o]=0;
					ListView_SetItemText(GetDlgItem(hwnd,IDC_LIST),i,2,Note);
					memcpy(&AlarmArray[i],List->Alarm,sizeof(IALARM));
					i++;
				}
				List=List->Next;
			}
			break;

        case WM_COMMAND :
			switch (LOWORD(wParam))
			{
				case IDC_DELETE:
					sel=ListView_GetSelectionMark(GetDlgItem(hwnd,IDC_LIST));
					if(sel>=0)
					{
						if(DelAlarm(&AlarmList,AlarmArray[sel].OriginUserFooBar,AlarmArray[sel].OriginUniqueAlarmID))
							ListView_DeleteItem(GetDlgItem(hwnd,IDC_LIST),sel);

						for(i=sel;i<MaxItems;i++)
						{	
							if(i+1<MaxItems)
								memcpy(&AlarmArray[i],&AlarmArray[i+1],sizeof(IALARM));
							else
								memset(&AlarmArray[i],0,sizeof(IALARM));
						}
					}
					break;
				case IDC_CLOSE:
					if(AlarmArray)
						delete [] AlarmArray;
					AlarmArray=NULL;
					EndDialog(hwnd,0);
					return 1;
					break;
           }
		   break;
	} 
    return 0;
}


#define C_PAGES 5

typedef struct tag_dlghdr 
{
	HWND hwndTab;
	HWND hwndDisplay;
	RECT rcDisplay;
	DLGTEMPLATE *apRes[C_PAGES];
	BOOL (CALLBACK *Proc[C_PAGES])(HWND hwnd, UINT Msg,WPARAM wParam,LPARAM lParam);

} DLGHDR;

// DoLockDlgRes - loads and locks a dialog template resource. 
// Returns the address of the locked resource. 
// lpszResName - name of the resource  

DLGTEMPLATE * WINAPI DoLockDlgRes(LPCSTR lpszResName) 	
{ 
    HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG); 
    HGLOBAL hglb = LoadResource(hAppInst, hrsrc); 

    return (DLGTEMPLATE *) LockResource(hglb); 	
}  


// OnSelChanged - processes the TCN_SELCHANGE notification. 
// hwndDlg - handle to the parent dialog box.  

VOID WINAPI OnSelChanged(HWND hwndDlg) 	
{ 
    DLGHDR *pHdr = (DLGHDR *) GetWindowLong( hwndDlg, GWL_USERDATA); 
    int iSel = TabCtrl_GetCurSel(pHdr->hwndTab);  
    
	// Destroy the current child dialog box, if any. 
    
	if (pHdr->hwndDisplay != NULL)         
		DestroyWindow(pHdr->hwndDisplay);  
    
	// Create the new child dialog box. 
    pHdr->hwndDisplay = CreateDialogIndirect(hAppInst, 
											 pHdr->apRes[iSel], 
											 hwndDlg, 
											 pHdr->Proc[iSel]);//ChildDialogProc); 	
} 

// OnChildDialogInit - Positions the child dialog box to fall 
//     within the display area of the tab control.  

VOID WINAPI OnChildDialogInit(HWND hwndDlg) 	
{ 
    HWND hwndParent = GetParent(hwndDlg); 
    DLGHDR *pHdr = (DLGHDR *) GetWindowLong(hwndParent, GWL_USERDATA); 
    


	SetWindowPos(hwndDlg, 
				 HWND_TOP, 
				 pHdr->rcDisplay.left, 
				 pHdr->rcDisplay.top,         
				 0, 0, 
				 SWP_NOSIZE); 
} 

VOID WINAPI OnTabbedDialogInit(HWND hwndDlg)
{
    DLGHDR *pHdr = (DLGHDR *) LocalAlloc(LPTR, sizeof(DLGHDR)); 
    DWORD dwDlgBase = GetDialogBaseUnits(); 
    int cxMargin = LOWORD(dwDlgBase) / 4; 
    int cyMargin = HIWORD(dwDlgBase) / 8;     
	TCITEM tie;     
	RECT rcTab; 
    DWORD style;
	HWND hwndButton;     
	RECT rcButton;     
	int i;  
    char label[256];

	// Save a pointer to the DLGHDR structure. 
    SetWindowLong(hwndDlg, GWL_USERDATA, (LONG) pHdr);  

    
	style=GetWindowLong(hwndDlg,GWL_STYLE);
	SetWindowLong(hwndDlg, GWL_STYLE, style|WS_CHILD);  
    
    pHdr->hwndTab = GetDlgItem(hwndDlg,IDC_TAB);
	
	if (pHdr->hwndTab == NULL) 
	{
        // handle error    
	} 
    
	// Add a tab for each of the three child dialog boxes. 
    tie.mask = TCIF_TEXT | TCIF_IMAGE;     
	tie.iImage = -1; 

	LoadString(hAppInst,IDS_AUTHOR,label,256);
    tie.pszText = label;     
	TabCtrl_InsertItem(pHdr->hwndTab, 0, &tie); 
	
	LoadString(hAppInst,IDS_BEATS,label,256);
    tie.pszText = label;     
	TabCtrl_InsertItem(pHdr->hwndTab, 1, &tie); 
	
	LoadString(hAppInst,IDS_INTERNET,label,256);
    tie.pszText = label;     
	TabCtrl_InsertItem(pHdr->hwndTab, 2, &tie);  
	
	LoadString(hAppInst,IDS_REGISTER,label,256);
    tie.pszText = label;     
	TabCtrl_InsertItem(pHdr->hwndTab, 3, &tie);  

	LoadString(hAppInst,IDS_MOON,label,256);
    tie.pszText = label;     
	TabCtrl_InsertItem(pHdr->hwndTab, 4, &tie);  

    // Lock the resources for the three child dialog boxes. 
    pHdr->apRes[0] = DoLockDlgRes(MAKEINTRESOURCE(IDD_T_AUTHOR)); 
    pHdr->apRes[1] = DoLockDlgRes(MAKEINTRESOURCE(IDD_T_BEATS)); 
    pHdr->apRes[2] = DoLockDlgRes(MAKEINTRESOURCE(IDD_T_INTERNET));  
    pHdr->apRes[3] = DoLockDlgRes(MAKEINTRESOURCE(IDD_T_REGISTER));  
    pHdr->apRes[4] = DoLockDlgRes(MAKEINTRESOURCE(IDD_T_MOON));  

    // Set Callback Functions
    pHdr->Proc[0] = AboutAuthorProc;
    pHdr->Proc[1] = AboutBeatsProc;
    pHdr->Proc[2] = AboutInternetProc;
    pHdr->Proc[3] = AboutRegisterProc;
    pHdr->Proc[4] = AboutMoonProc;

    // Determine the bounding rectangle for all child dialog boxes. 

    SetRectEmpty(&rcTab);     
	
	for (i = 0; i < C_PAGES; i++) 
	{ 
        if (pHdr->apRes[i]->cx > rcTab.right) 
            rcTab.right = pHdr->apRes[i]->cx; 
        if (pHdr->apRes[i]->cy > rcTab.bottom) 
            rcTab.bottom = pHdr->apRes[i]->cy;     
	}
	
    rcTab.right = (rcTab.right * LOWORD(dwDlgBase)) / 4; 
    rcTab.bottom = (rcTab.bottom * HIWORD(dwDlgBase)) / 8;  
    
	rcTab.right -= rcTab.right/4;
	rcTab.bottom -= rcTab.bottom/8;

	// Calculate how large to make the tab control, so 
    // the display area can accommodate all the child dialog boxes. 
    TabCtrl_AdjustRect(pHdr->hwndTab, TRUE, &rcTab); 
    OffsetRect(&rcTab, cxMargin - rcTab.left, cyMargin - rcTab.top);      

	// Calculate the display rectangle. 
    CopyRect(&pHdr->rcDisplay, &rcTab); 
    TabCtrl_AdjustRect(pHdr->hwndTab, FALSE, &pHdr->rcDisplay);  
    
	// Set the size and position of the tab control, buttons, 
    // and dialog box. 
    SetWindowPos(pHdr->hwndTab, 
				 NULL, 
				 rcTab.left, 
				 rcTab.top,
				 rcTab.right - rcTab.left, 
				 rcTab.bottom - rcTab.top, 
				 SWP_NOZORDER);      
	
	// Determine the size of the button. 
    hwndButton = GetDlgItem(hwndDlg, IDM_CLOSE); 
    GetWindowRect(hwndButton, &rcButton);     
	// Move the first button below the tab control. 
	SetWindowPos(hwndButton, 
				 NULL, 
				 rcTab.right - (rcButton.right - rcButton.left), 
				 rcTab.bottom + cyMargin, 
				 0, 0, 
				 SWP_NOSIZE | SWP_NOZORDER);  

	// Determine the size of the button. 
    GetWindowRect(hwndButton, &rcButton);     
	rcButton.right -= rcButton.left; 
    rcButton.bottom -= rcButton.top;  
    
   
    // Size the dialog box.     
	SetWindowPos(hwndDlg, 
				 NULL, 0, 0, 
				 rcTab.right + cyMargin +  2 * GetSystemMetrics(SM_CXDLGFRAME), 
				 rcTab.bottom + rcButton.bottom + GetSystemMetrics(SM_CYCAPTION) + 2 * cyMargin + 2 * GetSystemMetrics(SM_CYDLGFRAME), 
				 SWP_NOMOVE | SWP_NOZORDER);  

    // Simulate selection of the first item.     

	OnSelChanged(hwndDlg); 	
}  

BOOL CALLBACK AboutProc(HWND hwnd, UINT Msg,WPARAM wParam,LPARAM lParam)
{
	lParam = lParam;
	NMHDR *lpnmhdr;

	switch(Msg)
	{
	case WM_INITDIALOG :
		OnTabbedDialogInit(hwnd);
		CenterWindow(NULL,hwnd);
		break;

    case WM_NOTIFY :
		lpnmhdr = (LPNMHDR) lParam; 

		if(lpnmhdr->hwndFrom==GetDlgItem(hwnd,IDC_TAB))
		{
			switch(lpnmhdr->code)
			{
			case TCN_SELCHANGING:
				return 1;
				break;
			case TCN_SELCHANGE:
				OnSelChanged(hwnd) 	;
				return 1;
			}
		}
		break;

    case WM_CLOSE :
		EndDialog(hwnd,0);
		return 1;
		break;

    case WM_COMMAND :
		switch (LOWORD(wParam))
		{
		
		case IDM_CLOSE:
			EndDialog(hwnd,0);
			return 1;
			break;
 		}
		break;

	};
 
	return 0;
}


BOOL CALLBACK AboutAuthorProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam)
{
	lParam = lParam;

	switch(Msg)
	{
	case WM_INITDIALOG :
		OnChildDialogInit(hwnd);
		SetDlgItemText(hwnd,IDC_BUILD,BUILDCOUNT_STR);
		ShowWindow(hwnd,SW_SHOW);

		break;
	}
	return 0;
}

BOOL CALLBACK AboutBeatsProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	lParam = lParam;

	switch(Msg)
	{
	case WM_INITDIALOG :
		OnChildDialogInit(hwnd);
		ShowWindow(hwnd,SW_SHOW);
		break;
	}
	return 0;
}

BOOL CALLBACK AboutInternetProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	lParam = lParam;

	switch(Msg)
	{
    case WM_COMMAND :
		switch (LOWORD(wParam))
		{
		case IDM_HOME:
			ShellExecute(hwnd, "open", "http://www.artefakt.com/projects/ibeat/index.html", NULL, NULL, SW_SHOWNORMAL);
			return 1;
			break;
		case IDM_HELP:
			ShellExecute(hwnd, "open", "http://www.artefakt.com/projects/ibeat/help.html", NULL, NULL, SW_SHOWNORMAL);
			return 1;
			break;
		case IDM_MAIL:
			ShellExecute(hwnd, "open", "mailto:ibeat@artefakt.com?subject=I like iBeat", NULL, NULL, SW_SHOWNORMAL);
			return 1;
			break;
		case IDM_SWATCH:
			ShellExecute(hwnd, "open", "http://www.swatch.com/", NULL, NULL, SW_SHOWNORMAL);
			return 1;
			break;
		case IDM_UNIVERSAL:
			ShellExecute(hwnd, "open", "http://www.universal-time.org/", NULL, NULL, SW_SHOWNORMAL);
			return 1;
			break;
		}
		break;
	case WM_INITDIALOG :
		OnChildDialogInit(hwnd);
		ShowWindow(hwnd,SW_SHOW);
		break;
	}
	return 0;
}

BOOL CALLBACK AboutRegisterProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	lParam = lParam;

	switch(Msg)
	{
    case WM_COMMAND :
		switch (LOWORD(wParam))
		{
			case IDM_MAIL:
			ShellExecute(hwnd, "open", "mailto:ibeat@artefakt.com?subject=I use iBeat", NULL, NULL, SW_SHOWNORMAL);
			return 1;
			break;
		}
		break;

	case WM_INITDIALOG :
		OnChildDialogInit(hwnd);
		ShowWindow(hwnd,SW_SHOW);
		break;
	}
	return 0;
}


void DrawMoon(HWND hwnd,double MoonAge)
{

	HDC hDC;
	static HDC hDCWork,hDCMoon;
	static HBITMAP hBMPWork;
	static HBITMAP hBMPMoon;
	static int xmax,ymax;
	RECT rect;
	int bend;
	static int INIT=1;

	if(INIT)
	{
		xmax=133;
		ymax=132;

		hDC = GetDC(hwnd);
		hBMPWork =	CreateCompatibleBitmap(hDC,xmax,ymax);
		hDCWork = CreateCompatibleDC(hDC);
		SelectObject(hDCWork,hBMPWork);

		hBMPMoon=LoadBitmap(hAppInst,MAKEINTRESOURCE(IDB_MOON));
		hDCMoon = CreateCompatibleDC(hDC);
		SelectObject(hDCMoon,hBMPMoon);
		
		ReleaseDC(hwnd,hDC);	
		INIT=0;
	}

	rect.top=0;
	rect.left=0;
	rect.right=xmax;
	rect.bottom=ymax;

	//Background
	FillRect(hDCWork,&rect,CreateSolidBrush(RGB(0,0,0)));
	
	//Full Moon
	SelectObject(hDCWork,CreatePen(PS_SOLID,1,RGB(255,255,255)));
	SelectObject(hDCWork,CreateSolidBrush(RGB(255,255,255)));
	Ellipse(hDCWork,0,0,xmax,ymax);

	//14765.30 beats is half the moon lifetime (next full moon)
	if(MoonAge<=7382.65)
	{
		//1st Quarter
		rect.top=0;
		rect.left=0;
		rect.bottom=ymax;
		rect.right=xmax/2;
		FillRect(hDCWork,&rect,CreateSolidBrush(RGB(0,0,0)));

		bend=xmax/2;
		bend*=(int)(MoonAge/7382.65);

		SelectObject(hDCWork,CreatePen(PS_SOLID,1,RGB(0,0,0)));
		SelectObject(hDCWork,CreateSolidBrush(RGB(0,0,0)));
		Ellipse(hDCWork,0+bend,0,xmax-bend,ymax);
	}
	else
	{
		if(MoonAge<=(7382.65*2))
		{
			//2nd Quarter
			rect.top=0;
			rect.left=0;
			rect.bottom=ymax;
			rect.right=xmax/2;
			FillRect(hDCWork,&rect,CreateSolidBrush(RGB(0,0,0)));

			bend=xmax/2;
			bend=bend-(bend*((MoonAge-7382.65)/7382.65));
			
			SelectObject(hDCWork,CreatePen(PS_SOLID,1,RGB(255,255,255)));
			SelectObject(hDCWork,CreateSolidBrush(RGB(255,255,255)));
			Ellipse(hDCWork,0+bend,0,xmax-bend,ymax);
		}
		else
		{
			if(MoonAge<=(7382.65*3))
			{
				//3rd Quarter
				rect.top=0;
				rect.left=xmax/2;
				rect.bottom=ymax;
				rect.right=xmax;
				FillRect(hDCWork,&rect,CreateSolidBrush(RGB(0,0,0)));
				
				bend=xmax/2;
				bend=bend-(bend*((MoonAge-7382.65)/7382.65));
				
				SelectObject(hDCWork,CreatePen(PS_SOLID,1,RGB(255,255,255)));
				SelectObject(hDCWork,CreateSolidBrush(RGB(255,255,255)));
				Ellipse(hDCWork,0+bend,0,xmax-bend,ymax);
				
				bend=xmax/2;
				bend*=((MoonAge-(7382.65*2))/7382.65);

			}
			else
			{
				//4th Quarter
				rect.top=0;
				rect.left=xmax/2;
				rect.bottom=ymax;
				rect.right=xmax;
				FillRect(hDCWork,&rect,CreateSolidBrush(RGB(0,0,0)));

				bend=xmax/2;
				bend=bend-(bend*((MoonAge-(7382.65*3))/7382.65));

				SelectObject(hDCWork,CreatePen(PS_SOLID,1,RGB(255,255,255)));
				SelectObject(hDCWork,CreateSolidBrush(RGB(255,255,255)));
				Ellipse(hDCWork,0+bend,0,xmax-bend,ymax);
			}
		}
	}

  	BitBlt(hDCWork,0,0,xmax,ymax,hDCMoon,0,0,SRCAND);
	SendDlgItemMessage(hwnd,IDC_MOON,STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)hBMPWork);
}


BOOL CALLBACK AboutMoonProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	time_t now;
	static int timer=0;
	double MoonAge;
	double NewMoon;
	double FullMoon;
	double YearLenght;
	char text[1024];
	static char Days[80];
	static char Beats[80];
	static char And[80];
	struct tm *stm;

	lParam = lParam;
	switch(Msg)
	{
	case WM_INITDIALOG :
		OnChildDialogInit(hwnd);
		ShowWindow(hwnd,SW_SHOW);
		timer =  SetTimer(hwnd,2,1000,NULL);  

		LoadString(hAppInst,IDS_BEATS,Beats,80);
		LoadString(hAppInst,IDS_DAYS,Days,80);
		LoadString(hAppInst,IDS_AND,And,80);
		
		SendMessage(hwnd,WM_TIMER,0,0);
		break;

	case WM_DESTROY:
		if(timer) KillTimer(hwnd,timer);
		break;

	case WM_TIMER:
		_tzset();

		time(&now);
		MoonAge=MoonAgeInSeconds(now);
		
		//Moonage in Beats
		MoonAge = MoonAge/((double)86.4);
		
		sprintf(text,"%3d %s %s %03.0f %s",(int)MoonAge/1000,Days,And,MoonAge-(((int)MoonAge/1000)*1000),Beats);
		SetWindowText(GetDlgItem(hwnd,IDC_AGE),text);
		
		//fullmoon
		//14765.30 beats is half the moon lifetime (next full moon)
		if((14765.30-MoonAge)>=0)
		{
			FullMoon = 14765.30-MoonAge;
		}
		else
		{
			FullMoon = (29530.60+14765.30)-MoonAge;
		}
		sprintf(text,"%3d %s %s %03.0f %s",(int)FullMoon/1000,Days,And,FullMoon-(((int)FullMoon/1000)*1000),Beats);
		SetWindowText(GetDlgItem(hwnd,IDC_FULL),text);

		//newmoon
		//29530.60 beats is the moon lifetime (Next new moon)
		if((29530.60-MoonAge)>=0)
		{
			NewMoon = 29530.60-MoonAge;
		}
		else
		{
			NewMoon = (29530.60+29530.60)-MoonAge;
		}
		sprintf(text,"%3d %s %s %03.0f %s",(int)NewMoon/1000,Days,And,NewMoon-(((int)NewMoon/1000)*1000),Beats);
		SetWindowText(GetDlgItem(hwnd,IDC_NEW),text);

		stm = gmtime(&now);
		stm->tm_year;
		
		YearLenght = 365.24219879 - 61.4 * 0.00000001 * stm->tm_year;
		
		sprintf(text,"%3d %s %s %03.2f %s",(int)YearLenght,Days,And,(YearLenght-(int)YearLenght)*1000,Beats);
		SetWindowText(GetDlgItem(hwnd,IDC_YEAR),text);

	//	DrawMoon(hwnd,MoonAge);
	}
	return 0;
}

/*
BOOL CALLBACK SetUserProc(HWND hwnd,  UINT Msg,WPARAM wParam,LPARAM lParam)
{

	lParam = lParam;

	switch(Msg)
    {

        case WM_INITDIALOG :
			CenterWindow(NULL,hwnd);
			SetDlgItemText(hwnd,IDC_NAME,GlobalSettingUserName);
			SetDlgItemText(hwnd,IDC_EMAIL,GlobalSettingUserEMail);
			SetDlgItemText(hwnd,IDC_LOCATION,GlobalSettingUserLocation);
			SetDlgItemText(hwnd,IDC_PASSWORD,GlobalSettingUserPassword);
			SetDlgItemInt(hwnd,IDC_FOOBAR,GlobalSettingUserFoobar,FALSE);
			break;

        case WM_COMMAND :
			switch (LOWORD(wParam))
			{
				case IDC_SAVE:
					GetDlgItemText(hwnd,IDC_NAME,GlobalSettingUserName,256);
					GetDlgItemText(hwnd,IDC_EMAIL,GlobalSettingUserEMail,256);
					GetDlgItemText(hwnd,IDC_LOCATION,GlobalSettingUserLocation,256);
					GetDlgItemText(hwnd,IDC_PASSWORD,GlobalSettingUserPassword,8);
					GlobalSettingUserFoobar=GetDlgItemInt(hwnd,IDC_FOOBAR,NULL,FALSE);

					EndDialog(hwnd,0);
					return 1;
					break;
				case IDC_CANCEL:
					EndDialog(hwnd,0);
					return 1;
					break;
           }
		   break;
	} 
    return 0;
}
*/
void CenterWindow(HWND Parent, HWND Child)
{
RECT rChild;
RECT rParent;
RECT rWork;
int wChild,hChild;
int wParent,hParent;
int bResult;
int NewX,NewY;

	GetWindowRect(Child,&rChild);
	GetWindowRect(Parent,&rParent);

	if(Parent==NULL)
	{
		bResult= SystemParametersInfo(SPI_GETWORKAREA,sizeof(RECT),&rParent,0);
		if(!bResult)
		{
			rParent.left = rParent.top = 0;
			rParent.right = GetSystemMetrics(SM_CXSCREEN);
			rParent.bottom = GetSystemMetrics(SM_CYSCREEN);
		}
	}

	wChild=rChild.right-rChild.left;
	hChild=rChild.bottom-rChild.top;

	wParent=rParent.right-rParent.left;
	hParent=rParent.bottom-rParent.top;

	bResult= SystemParametersInfo(SPI_GETWORKAREA,sizeof(RECT),&rWork,0);
	if(!bResult)
	{
		rWork.left = rWork.top = 0;
		rWork.right = GetSystemMetrics(SM_CXSCREEN);
		rWork.bottom = GetSystemMetrics(SM_CYSCREEN);
	}

	NewX=rParent.left + ((wParent-wChild)/2);
	NewY=rParent.top  + ((hParent-hChild)/2);

	if(NewX<rWork.left)
	{
		NewX = rWork.left;
	}
	else 
	if((NewX+wChild) > rWork.right)
	{
			NewX = rWork.right - wChild;
	}

	if(NewY<rWork.top)
	{
		NewY = rWork.top;
	}
	else 
	if((NewY+hChild) > rWork.bottom)
	{
			NewY = rWork.bottom - hChild;
	}

	SetWindowPos(Child,NULL,NewX,NewY,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

bool SetDlgText(HWND hwnd, HINSTANCE hInst, UINT sID)
{
  char buffer[4096];

	if(LoadString(hInst,sID,buffer,4096))
	{
		if( SetWindowText(  hwnd, buffer))
			return true;
	}
	
	return false;
}

bool SetMenuText(HMENU hMenu, UINT mID, BOOL byCommand, HINSTANCE hInst, UINT sID)
{
  MENUITEMINFO lmi;
  char buffer[4096];

	lmi.cbSize=sizeof(lmi);
	lmi.cch=4096;
	lmi.dwTypeData=buffer;
	lmi.fMask=MIIM_TYPE;
	GetMenuItemInfo(hMenu,mID,byCommand,&lmi);
	if(LoadString(hInst,sID,buffer,4096))
	{
		SetMenuItemInfo(hMenu,mID,byCommand,&lmi);
		return true;
	}
	else
		return false;
}



HWND CreateBigTooltip(HWND hwndOwner) 	
{
	
    HWND hwndTT;    // handle of tooltip 
	TOOLINFO ti;    // tool information 
    int id = 0;     // offset to string identifiers 
    char tip[256];	// tip text     

    // Ensure that the common control DLL is loaded, and create 
    // a tooltip control.     
	
	InitCommonControls();  
    
	hwndTT = CreateWindow(TOOLTIPS_CLASS, 
						  (LPSTR) NULL, TTS_ALWAYSTIP|WS_POPUP|WS_VISIBLE,
						  CW_USEDEFAULT, CW_USEDEFAULT, 
						  CW_USEDEFAULT, CW_USEDEFAULT, 
						  NULL, 
						  (HMENU) NULL, 
						  hAppInst, 
						  NULL);      
	
	if (hwndTT == (HWND) NULL) 
        return (HWND) NULL;  
     
	ti.cbSize = sizeof(TOOLINFO);             
	ti.uFlags = 0; 
    ti.hwnd = hwndOwner;             
	ti.hinst = hAppInst; 
    ti.uId = (UINT) 0;

	LoadString(hAppInst,IDS_2000_TT,tip,256);
	ti.lpszText = (LPSTR) tip;  
	
	ti.rect.top=50;
	ti.rect.left=60;
	ti.rect.bottom=65;
	ti.rect.right=155;

    if (!SendMessage(hwndTT, TTM_ADDTOOL, 0,(LPARAM) (LPTOOLINFO) &ti))                
		return NULL; 

	ti.cbSize = sizeof(TOOLINFO);             
	ti.uFlags = 0; 
    ti.hwnd = hwndOwner;             
	ti.hinst = hAppInst; 
    ti.uId = (UINT) 1;
	LoadString(hAppInst,IDS_LOCAL_TT,tip,256);
	ti.lpszText = (LPSTR) tip;  

	ti.rect.top=103;
	ti.rect.left=50;
	ti.rect.bottom=117;
	ti.rect.right=165;

    if (!SendMessage(hwndTT, TTM_ADDTOOL, 0,(LPARAM) (LPTOOLINFO) &ti))                
		return NULL; 

	ti.cbSize = sizeof(TOOLINFO);             
	ti.uFlags = 0; 
    ti.hwnd = hwndOwner;             
	ti.hinst = hAppInst; 
    ti.uId = (UINT) 2;
	LoadString(hAppInst,IDS_IDAY_TT,tip,256);
	ti.lpszText = (LPSTR) tip;  

	ti.rect.top=75;
	ti.rect.left=85;
	ti.rect.bottom=100;
	ti.rect.right=120;

    if (!SendMessage(hwndTT, TTM_ADDTOOL, 0,(LPARAM) (LPTOOLINFO) &ti))                
		return NULL; 

	ti.cbSize = sizeof(TOOLINFO);             
	ti.uFlags = 0; 
    ti.hwnd = hwndOwner;             
	ti.hinst = hAppInst; 
    ti.uId = (UINT) 3;
	LoadString(hAppInst,IDS_BEAT_TT,tip,256);
	ti.lpszText = (LPSTR) tip;  

	ti.rect.top=75;
	ti.rect.left=140;
	ti.rect.bottom=100;
	ti.rect.right=177;

    if (!SendMessage(hwndTT, TTM_ADDTOOL, 0,(LPARAM) (LPTOOLINFO) &ti))                
		return NULL; 

	ti.cbSize = sizeof(TOOLINFO);             
	ti.uFlags = 0; 
    ti.hwnd = hwndOwner;             
	ti.hinst = hAppInst; 
    ti.uId = (UINT) 4;
	LoadString(hAppInst,IDS_CBEAT_TT,tip,256);
	ti.lpszText = (LPSTR) tip;  

	ti.rect.top=79;
	ti.rect.left=180;
	ti.rect.bottom=100;
	ti.rect.right=196;

    if (!SendMessage(hwndTT, TTM_ADDTOOL, 0,(LPARAM) (LPTOOLINFO) &ti))                
		return NULL; 

	ti.cbSize = sizeof(TOOLINFO);             
	ti.uFlags = 0; 
    ti.hwnd = hwndOwner;             
	ti.hinst = hAppInst; 
    ti.uId = (UINT) 4;
	LoadString(hAppInst,IDS_APP_TT,tip,256);
	ti.lpszText = (LPSTR) tip;  

	ti.rect.top=75;
	ti.rect.left=22;
	ti.rect.bottom=100;
	ti.rect.right=76;

    if (!SendMessage(hwndTT, TTM_ADDTOOL, 0,(LPARAM) (LPTOOLINFO) &ti))                
		return NULL; 

    SendMessage(hwndTT, TTM_ACTIVATE,(WPARAM) true,(LPARAM) 0) ;

	return hwndTT; 	
} 
HWND CreateSmallTooltip(HWND hwndOwner) 	
{
	
    HWND hwndTT;    // handle of tooltip 
	TOOLINFO ti;    // tool information 
    int id = 0;     // offset to string identifiers 
    char tip[256];	// tip text     

    // Ensure that the common control DLL is loaded, and create 
    // a tooltip control.     
	
	InitCommonControls();  
    
	hwndTT = CreateWindow(TOOLTIPS_CLASS, 
						  (LPSTR) NULL, TTS_ALWAYSTIP|WS_POPUP|WS_VISIBLE,
						  CW_USEDEFAULT, CW_USEDEFAULT, 
						  CW_USEDEFAULT, CW_USEDEFAULT, 
						  NULL, 
						  (HMENU) NULL, 
						  hAppInst, 
						  NULL);      
	
	if (hwndTT == (HWND) NULL) 
        return (HWND) NULL;  
     
	ti.cbSize = sizeof(TOOLINFO);             
	ti.uFlags = 0; 
    ti.hwnd = hwndOwner;             
	ti.hinst = hAppInst; 
    ti.uId = (UINT) 0;

	LoadString(hAppInst,IDS_BEAT_TT,tip,256);
	ti.lpszText = (LPSTR) tip;  
	
	ti.rect.top=47;
	ti.rect.left=45;
	ti.rect.bottom=64;
	ti.rect.right=84;

    if (!SendMessage(hwndTT, TTM_ADDTOOL, 0,(LPARAM) (LPTOOLINFO) &ti))                
		return NULL; 


    SendMessage(hwndTT, TTM_ACTIVATE,(WPARAM) true,(LPARAM) 0) ;

	return hwndTT; 	
} 
