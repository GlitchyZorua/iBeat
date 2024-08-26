#include "socket.h"
#include "winsock.h"

Socket::Socket(int Type)
{
//possible types are:
//SOCK_DGRAM
//SOCK_STREAM
  
		OK=0;
        s = socket(AF_INET,Type,0);      
        if (s==INVALID_SOCKET)  
            return;

		memset(&sin,0,sizeof(SOCKADDR_IN));

        OK=1;
		return;
}

Socket::~Socket(void)
{
    closesocket(s);
}

int Socket::Check(int type,int sec,int mil)
{
struct timeval wait;
fd_set check;

		FD_ZERO(&check);
		FD_SET(s, &check);

        wait.tv_sec=sec;
        wait.tv_usec=mil;
        
		switch (type)
        {
        case SA_READ :
            return select (1,&check,NULL,NULL,&wait);
        case SA_WRITE :
            return select (1,NULL,&check,NULL,&wait);
        case SA_ERROR :
            return select (1,NULL,NULL,&check,&wait);
        }

    return 0;
}

long Socket::GetAddress()
{
    return sin.sin_addr.s_addr;
}

short Socket::GetPort()
{
    return sin.sin_port;
}

int Socket::SetOption(int level, int optname, const char FAR * optval,int optlen )
{

	return setsockopt (s,level,optname,optval,optlen);
}


int Socket::Bind(unsigned long address, unsigned short port)
{
int err;

        Binding=0;
       
        sin.sin_family = AF_INET;

        sin.sin_port = htons(port);
		sin.sin_addr.s_addr =htonl(address);

        err = bind(s, (LPSOCKADDR)&sin, sizeof (sin));
        if (err != 0)
            return FALSE;

        Binding=1;
        return TRUE;
}

int Socket::Bind(char FAR* host,unsigned short port)
{
struct hostent FAR* hostbuffer;
int err;
char FAR* sadr;
unsigned long adr;


		if(host[0]>='0' && host[0]<='9')
		{
			adr = inet_addr (host);
		} else {
			hostbuffer = gethostbyname(host);
			if (hostbuffer==NULL)
			   return FALSE;

			sadr = hostbuffer->h_addr_list[0];
			adr = *(unsigned long *) sadr; 
		}


        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr =adr;

        err = bind(s, (LPSOCKADDR)&sin, sizeof (sin));
        if (err != 0)
            return FALSE;

        Binding=1;
        return TRUE;
}


int Socket::Connect(unsigned long address, unsigned short port)
{
int err;

        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr =htonl(address);

        err = connect(s, (LPSOCKADDR)&sin, sizeof (sin));
        if (err != 0)
            return FALSE;

        return TRUE;
}
int Socket::Connect(char FAR* host,unsigned short port)
{
struct hostent FAR* hostbuffer;
int err;
char FAR* sadr;
unsigned long adr;

		if(host[0]>='0' && host[0]<='9')
		{
			adr = inet_addr (host);
		} else {
			hostbuffer = gethostbyname(host);
			if (hostbuffer==NULL)
			   return FALSE;

			sadr = hostbuffer->h_addr_list[0];
			adr = *(unsigned long *) sadr; 
		}

        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr =adr;

        err = connect(s, (LPSOCKADDR)&sin, sizeof (sin));
        if (err != 0)
            return FALSE;

        return TRUE;
}

int Socket::Listen(void)
{
int err;

        err = listen(s,5);
        if (err == SOCKET_ERROR)
        {
            return FALSE;
        }
        return TRUE;
}


Socket* Socket::Accept(void)
{
Socket* ret;
int size;

        size= sizeof(sin);
        ret = new Socket(SOCK_STREAM);
	
		ret->s = accept(s, (LPSOCKADDR)&ret->sin, &size);
        if (ret->s==INVALID_SOCKET)
            return NULL;

        return ret;
}

void Socket::Block(long value)
{
long val;
long FAR* ptr; 

    val=value;
    ptr=&val;
    
    ioctlsocket(s,FIONBIO,(unsigned long FAR*) ptr);

}

int Socket::Send(char * buffer,int size)
{
int err;
        err = send (s,buffer,size,0);
        if (err == SOCKET_ERROR)
        {
            return FALSE;
        }
        return err;
}

int Socket::SendTo(char * buffer,int size,unsigned long host, unsigned short port)
{
int err;
int len;

		len=sizeof(SOCKADDR_IN);
		
		memset(&sin_to,0,sizeof(SOCKADDR_IN));

		sin_to.sin_family=AF_INET;
		sin_to.sin_port=htons(port);
		sin_to.sin_addr.s_addr= htonl(host);

        err = sendto(s,buffer,size,0,(SOCKADDR *)&sin_to,len);
        if (err == SOCKET_ERROR)
        {
            return FALSE;
        }
        return err;
}

int Socket::SendTo(char * buffer,int size,char * host, unsigned short port)
{
int err;
int len;
char FAR* sadr;
unsigned long adr;
struct hostent FAR* hostbuffer;

		if(host[0]>='0' && host[0]<='9')
		{
			adr = inet_addr (host);
		} else {
			hostbuffer = gethostbyname(host);
			if (hostbuffer==NULL)
			{
				return FALSE;	
			}

			sadr = hostbuffer->h_addr_list[0];
			adr = *(unsigned long *) sadr; 
		}

		len=sizeof(SOCKADDR_IN);
		
		memset(&sin_to,0,sizeof(SOCKADDR_IN));

		sin_to.sin_family=AF_INET;
		sin_to.sin_port=htons(port);
		sin_to.sin_addr.s_addr= adr;

        err = sendto(s,buffer,size,0,(SOCKADDR *)&sin_to,len);
        if (err == SOCKET_ERROR)
        {
            return FALSE;
        }
        return err;
}

int Socket::RecvFrom(char *buffer,int size)
{
int err;
int len;
		len=sizeof(struct sockaddr);		
//		status=recvfrom(sg->GetSocket(),buffer,80,0,(struct sockaddr *)&sg->remote,&len);	
		err= recvfrom(s,buffer,size,0,(struct sockaddr *)&remote,&len);	
//		err= recvfrom(s,buffer,size,0,(SOCKADDR *)&remote,&len);
        if (err == SOCKET_ERROR)
        {
            return FALSE;
        }
        return err;

}
int Socket::Recv(char *buffer,int size)
{
int err;

        err = recv (s,buffer,size,0);
        if (err == SOCKET_ERROR)
        {
            return FALSE;
        }
        return err;
}

int SocketTool::Init(void)
{
WORD wVersionRequested; 
WSADATA wsaData; 
int err; 

  wVersionRequested = MAKEWORD(1, 1); 
  err = WSAStartup(wVersionRequested, &wsaData); 

  if (err != 0)
  {
      return FALSE;
  }

return TRUE;
}

int SocketTool::Exit(void)
{

   WSACleanup();

return TRUE;
}
