#include "jrawsocket.h"
#include "jdatagramsocket.h"
#include "jsocketexception.h"
#include "jsocketlib.h"
#include "jsocketinputstream.h"

#include <iostream>

#include <linux/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#define RESET      "\033[0m"
#define BOLD       "\033[1m"
#define SAVE       "\033[s"
#define RESTORE    "\033[u"
#define CLR2TOP    "\033[1J"

#define RED 1
#define GRN 2
#define YEL 3
#define BLU 4
#define PUR 5
#define CYN 6
#define WHT 7
#define BLK 0

using namespace jsocket;
using namespace jio;

void set_col(char fg_color,char bg_color)
{
	fg_color += 30; bg_color += 40;
    printf ("\033[%d;%dm",fg_color,bg_color);
}

void nocol(void)
{
	printf("%s",RESET);
}

char * Indirizzo(unsigned int I)
{
	struct in_addr A;

	A.s_addr=I;
	return inet_ntoa(A);
}
			
void LoggaUDP(struct udphdr *uh, unsigned long S, unsigned long D )
{
	struct servent *P; //porta --> servizio
	P=NULL;
	set_col(CYN,BLK);
	printf("UDP  ");
	nocol();
	set_col(RED,BLK);
	printf("packet       ");

	printf("from %15s on (%5d)", Indirizzo(S), ntohs(uh->dest));
	nocol();
	printf("\n");
	fflush(stdout);
}

void LoggaICMP(struct icmphdr * ih, unsigned long S , unsigned long D )
{
	set_col(BLU,BLK);
	printf("ICMP");
	nocol();
	set_col(GRN,BLK);
	switch (ih->type)
	{
		case ICMP_ECHOREPLY:
			printf(" echo reply  ");
			break;
		case ICMP_DEST_UNREACH:
			printf(" dest unreach");
			break;
		case ICMP_ECHO:
			printf(" echo request");
			break;
		case ICMP_SOURCE_QUENCH:
			printf(" SourceQuench");
			break;
		case ICMP_REDIRECT:
			printf(" Redirect    ");
			break;
		case ICMP_TIME_EXCEEDED:
			printf(" TimeExcedeed");
			break;
		case ICMP_PARAMETERPROB:
			printf(" ParamProblem");
			break;
		case ICMP_TIMESTAMP:
			printf(" TimeStampREQ");
			break;
		case ICMP_TIMESTAMPREPLY:
			printf(" TimeStampREP");
			break;
		case ICMP_INFO_REQUEST:
			printf(" Info Request");
			break;
		case ICMP_INFO_REPLY:
			printf(" Info Reply  ");
			break;
		case ICMP_ADDRESS:
			printf(" Addr MasqReq");
			break;
		case ICMP_ADDRESSREPLY:
			printf(" Addr MasqRep");
			break;
	}

	printf(" from %15s", Indirizzo(S));
	nocol();
	printf("\n");			
	fflush(stdout);
}

void LoggaTCP(void)
{
	set_col(RED, BLK);
	printf("TCP\n");
	nocol();
	fflush(stdout);
}

void LoggaConnessioneTCP( struct tcphdr  * th, unsigned long S, unsigned long D)
{
	struct servent *P; //porta --> servizio
	P=NULL;
	set_col(RED,BLK);
	printf("TCP  ");
	nocol();
	set_col(YEL,BLK);
	printf("connection   ");
	    printf("from %15s on (%5d)", Indirizzo(S), ntohs(th->dest));
	if ((P!=NULL))
	{
		printf(" <%s>", P->s_name);
	}
	nocol();
	printf("\n");
	
	fflush(stdout);
}
void sniffer() 
{
	char receive[4098];
	struct tcphdr  *tcp_header;
	struct iphdr *ip_header;
	struct icmphdr *icmp_header;
	struct udphdr *udp_header;
	struct ethhdr *ether_header;
	struct ip *IP;
	long ContaTCP,
		 ContaUDP,
		 QuantiUDP,
		 ContaICMP,
		 QuantiICMP;
	int r,
		CheckSYN=0;
	
	ContaTCP = ContaUDP = QuantiUDP = ContaICMP = QuantiICMP = 0;

	try {
		RawSocket sniff("lo");

		do {
			if ((r = sniff.Receive(receive, sizeof(receive))) <= 0) {
				return;
			}
			
			ether_header = (struct ethhdr *)receive;
			ip_header =  (struct iphdr *)(receive + sizeof(struct ethhdr));

			switch (ip_header->protocol) {
				case IPPROTO_UDP:
					ContaUDP++;
					if (QuantiUDP<=ContaUDP) {
						udp_header = (struct udphdr *)(receive + sizeof(struct ethhdr)+ sizeof(struct iphdr));
						LoggaUDP(udp_header,ip_header->saddr, ip_header->daddr);
						ContaUDP=0;
					}
					break;

				case IPPROTO_ICMP:
					icmp_header = (struct icmphdr *)(receive + sizeof(struct ethhdr)+ sizeof(struct iphdr));
					ContaICMP++;
					if (QuantiICMP<=ContaICMP) {
						LoggaICMP(icmp_header,ip_header->saddr, ip_header->daddr);
						ContaICMP=0;
				}
				break;

				case IPPROTO_TCP:
				tcp_header = (struct tcphdr *)(receive + sizeof(struct ethhdr)+ sizeof(struct iphdr));
				IP = (struct ip *)(receive + sizeof(ether_header));

				if (CheckSYN==0) {
					if ((tcp_header->syn==0x01) && (tcp_header->ack!=0x01)) {
						LoggaConnessioneTCP(tcp_header,ip_header->saddr, ip_header->daddr);
					}
				} else {
					ContaTCP++;
					if (ContaTCP==CheckSYN) {
						LoggaTCP();
						ContaTCP=0;
					}
				}
				break;
			}
		} while (true);
		sniff.Close();
	} catch (SocketException &e) {
		std::cout << e.what() << std::endl;
	}
}

int main()
{
	InitWindowsSocket();

	sniffer();

	ReleaseWindowsSocket();
}

