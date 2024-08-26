#ifndef _SOCKET_
#define _SOCKET_ 

#include <windows.h>
#include <winsock.h>

#define SA_READ 1
#define SA_WRITE 2
#define SA_ERROR 3
#define SA_BLOCK 0
#define SA_NOBLOCK 1


//possible init types are:
//SOCK_DGRAM
//SOCK_STREAM

class SocketTool{
public:
        int Init(void);
        int Exit(void);
};

class Socket {
private:
        SOCKADDR_IN sin;
        SOCKADDR_IN sin_to;

        SOCKET s;
        fd_set FAR* set;
        int OK;
        int Binding;
        char* hostname;
        
public:
		unsigned long My_Address;

        Socket(int Type);
        ~Socket(void);

		SOCKADDR_IN remote;
        int IsOK() {return OK;};
        int Change(void);

		int Check(int type,int sec,int mil);
        char* GetName();
        long GetAddress();
        short GetPort();
        void Block(long value);
		int SetOption(int level, int optname, const char FAR * optval,int optlen  );

        int Listen(void);
        Socket* Accept(void);

        int Bind(unsigned long address, unsigned short port);
        int Bind(char *host,unsigned short port);
        int Connect(unsigned long address, unsigned short port);
        int Connect(char *host,unsigned short port);

        int Send(char * buffer,int size);
		int SendTo(char * buffer,int size,unsigned long host, unsigned short port);
		int SendTo(char * buffer,int size,char *host, unsigned short port);

        int Recv(char *buffer,int size);
		int RecvFrom(char *buffer,int size);

		SOCKET GetSocket(void) { return s; }
};


#endif
