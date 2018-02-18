#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <map>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <assert.h>

using namespace std;
void createMap(void);
void *portScanner(void *threadArgu);
map<int,string> portMap;
char **scanIPArray;
int scanRange[3];

int main(int argc, char *argv[]){
	if(argc<4){
		cerr<<"Arguments Error"<<endl;
		exit(EXIT_FAILURE);
	}
	scanIPArray=argv;
	scanRange[0]=atoi(argv[1]);
	scanRange[1]=atoi(argv[2]);
	pthread_t threads[argc-3];
	long i=3;
	for(i;i<argc;i++){
		cerr<< "Creating Thread "<< i <<endl;
		if((pthread_create(&threads[i-3],NULL,portScanner,(void *)i))!=0){
			cerr<< "Create Thread Error:" << strerror(errno)<<endl;
			exit(EXIT_FAILURE);
		}
	}
	pthread_exit(NULL);
}
void *portScanner(void *threadArgu){
	int sockfd=0;
	int nonBlock=1;
	struct timeval timeoutTime;
	timeoutTime.tv_sec=10;
	timeoutTime.tv_usec=0;
	long scanIP=(long)threadArgu;
	in_port_t port=0;	
	createMap();
	struct in_addr netaddr;
	cerr<<scanIPArray[scanIP]<<endl;

	struct hostent *destin=gethostbyname(scanIPArray[scanIP]);
	if(destin==NULL){
		cerr<< "Fail to Get Destination Addr, Error:"<<strerror(h_errno)<<endl;
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in testaddr;
	testaddr.sin_family=AF_INET;
	memcpy(&testaddr.sin_addr,destin->h_addr,destin->h_length);
	port=scanRange[0];
	for(port;port<=scanRange[1];port++){
		timeoutTime.tv_sec=10;
		timeoutTime.tv_usec=0;
		testaddr.sin_port=htons(port);
		if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
			cerr<< "Fail to Establish A Socket, Error:"<<strerror(errno)<<endl;
			exit(EXIT_FAILURE);
		}
		setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&nonBlock,sizeof(int));
		setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(struct timeval *)&timeoutTime,sizeof(struct timeval));
		cerr<<"Try to Connect"<<endl;
		if((connect(sockfd,(struct sockaddr *)&testaddr,(socklen_t)sizeof(testaddr)))<0){
			if(errno==110){
				cerr<< "Port "<<port<<" No Response"<<endl;
				close(sockfd);
				continue;
			}
			else if(errno==115){
				fd_set err,write;
				FD_ZERO(&err);
				FD_ZERO(&write);
				FD_SET(sockfd,&err);
				FD_SET(sockfd,&write);
				int selectReturn=0;
				int errorReturn=0;
				int errorSize=sizeof(int);
				if((selectReturn=select(0,NULL,&write,&err,&timeoutTime))>0){
					if(FD_ISSET(sockfd,&write)){
						if((getsockopt(sockfd,SOL_SOCKET,SO_ERROR,(void *)&errorReturn,(socklen_t *)&errorSize))<0){
							cerr<<"Get Socket Option Error "<<errno<<strerror(errno)<<endl;
							exit(EXIT_FAILURE);
						}
						cerr<<"SO_ERROR "<<errorReturn<<endl;
						cerr<< "Port "<<port<<" is open"<<portMap[port]<<endl;
						close(sockfd);
						continue;
					}
				}
				else if(selectReturn==0){
					cerr<< "Port "<<port<<" No Response"<<endl;
					close(sockfd);
					continue;
				}
				else{
					cerr<<"Select Error: "<<errno<<strerror(errno)<<endl;
					close(sockfd);
					continue;
				}
			}
			else if(errno==113){
				cerr<<"Port "<<port<<" Package Dropped"<<endl;
				close(sockfd);
				continue;
			}
			else{
				cerr<< "Fail to Establish Connection, Error:"<<errno<<strerror(errno)<<endl;
				close(sockfd);
				continue;
			}
		}
		cerr<< "Port "<<port<<" is open "<<portMap[port]<<endl;
		close(sockfd);
	}
}

void createMap(){
	portMap[1]="TCPMUX";
	portMap[5]="RJE";
	portMap[7]="ECHO";
	portMap[9]="DISCARD";
	portMap[11]="SYSTAT";
	portMap[13]="DAYTIME";
	portMap[17]="QOTD";
	portMap[18]="Message Send";
	portMap[19]="CHARGEN";
	portMap[20]="FTP, Data";
	portMap[21]="FTP, Control";
	portMap[22]="SSH";
	portMap[23]="Telnet";
	portMap[25]="SMTP";
	portMap[26]="RSFTP";
	portMap[37]="TIME";
	portMap[38]="Route Access";
	portMap[39]="Resource Location";
	portMap[41]="Graphics";
	portMap[42]="Host Name Server";
	portMap[43]="WHOIS";
	portMap[49]="TACACS Login Host";
	portMap[53]="DNS";
	portMap[57]="MTP";
	portMap[70]="Gopher";
	portMap[79]="Finger";
	portMap[80]="HTTP";
}
	
	
