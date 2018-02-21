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
		//cerr<< "Creating Thread "<< i <<endl;
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
	//cerr<<scanIPArray[scanIP]<<endl;
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
		timeoutTime.tv_sec=5;			//TCP Sending time 10 sec.
		timeoutTime.tv_usec=0;
		testaddr.sin_port=htons(port);
		if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
			cerr<< "Fail to Establish A Socket, Error:"<<strerror(errno)<<endl;
			exit(EXIT_FAILURE);
		}
		setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&nonBlock,sizeof(int));
		setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(struct timeval *)&timeoutTime,sizeof(struct timeval));
		//cerr<<"Try to Connect"<<endl;
		if((connect(sockfd,(struct sockaddr *)&testaddr,(socklen_t)sizeof(testaddr)))<0){
			if(errno==110){
				cerr<<" "<<scanIPArray[scanIP]<<" Port "<<port<<" Package Dropped"<<endl;		//Consider ETIMEOUT as Port closed
				close(sockfd);
				continue;
			}
			else if(errno==111){
				cerr<<" "<<scanIPArray[scanIP]<<" Port "<<port<<" is closed"<<endl;
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
						cerr<<" "<<scanIPArray[scanIP]<<" SO_ERROR "<<errorReturn<<endl;
						//cerr<< "Port "<<port<<" is open"<<portMap[port]<<endl;
						close(sockfd);
						continue;
					}
				}
				else if(selectReturn==0){
					cerr<<" "<<scanIPArray[scanIP]<<" Port "<<port<<" Package Dropped"<<endl;
					close(sockfd);
					continue;
				}
				else{
					cerr<<" "<<scanIPArray[scanIP]<<" Select Error: "<<errno<<" "<<strerror(errno)<<endl;
					close(sockfd);
					continue;
				}
			}
			else if(errno==113){
				cerr<<" "<<scanIPArray[scanIP]<<" Port "<<port<<" Package Dropped"<<endl;
				close(sockfd);
				continue;
			}
			else{
				cerr<<" "<<scanIPArray[scanIP]<<" Fail to Establish Connection, Error:"<<errno<<" "<<strerror(errno)<<endl;
				close(sockfd);
				continue;
			}
		}
		cerr<<" "<<scanIPArray[scanIP]<<" Port "<<port<<" is open "<<portMap[port]<<endl;
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
	portMap[81]="Torpark";
	portMap[88]="Kerberos";
	portMap[101]="HOSTNAME";
	portMap[102]="ISO-TSAP";
	portMap[107]="Remote Telnet Service";
	portMap[109]="POP";
	portMap[110]="POP3";
	portMap[111]="SUNRPC";
	portMap[113]="ident";
	portMap[115]="SFTP";
	portMap[117]="UUCP-PATH";
	portMap[118]="SQL Service";
	portMap[119]="NNTP";
	portMap[135]="EPMAP";
	portMap[137]="NetBIOS Name Service";
	portMap[143]="IMAP4";
	portMap[152]="BFTP";
	portMap[153]="SGMP";
	portMap[156]="SQL Service";
	portMap[158]="DMSP";
	portMap[161]="SNMP";
	portMap[170]="Print-srv";
	portMap[179]="BGP";
	portMap[194]="IRC";
	portMap[209]="The Quick Mail Transfer";
	portMap[213]="IPX";
	portMap[218]="MPP";
	portMap[220]="IMAP";
	portMap[259]="ESRO";
	portMap[318]="TSP";
	portMap[323]="IMMP";
	portMap[427]="SLP";
	portMap[443]="HTTPS";
	portMap[444]="SNPP";
	portMap[513]="Login";
	portMap[604]="TUNNEL";
	portMap[631]="IPP";
}
	
	
