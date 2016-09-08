/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jrawsocket.h"
#include "jioexception.h"
#include "jsocketexception.h"
#include "jsocketlib.h"

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

void set_col(int fg_color, int bg_color)
{
	fg_color += 30; bg_color += 40;
	std::cout << "\033[" << fg_color << ";" << bg_color << "m" << std::flush;
}

void nocol(void)
{
	std::cout << RESET << std::endl;
}

char * Indirizzo(unsigned int I)
{
	struct in_addr A;

	A.s_addr=I;
	return inet_ntoa(A);
}
			
void LoggaUDP(struct udphdr *uh, unsigned long S, unsigned long D )
{
	set_col(CYN,BLK);
	std::cout << "UDP";
	nocol();
	set_col(RED,BLK);
	std::cout << "packet from " << Indirizzo(S) << " on (" << ntohs(uh->dest) << ")";
	nocol();
	std::cout << std::endl;
}

void LoggaICMP(struct icmphdr * ih, unsigned long S , unsigned long D )
{
	set_col(BLU,BLK);
	std::cout << "ICMP";
	nocol();
	set_col(GRN,BLK);

	switch (ih->type) {
		case ICMP_ECHOREPLY:
			std::cout << " echo reply  ";
			break;
		case ICMP_DEST_UNREACH:
			std::cout << " dest unreach";
			break;
		case ICMP_ECHO:
			std::cout << " echo request";
			break;
		case ICMP_SOURCE_QUENCH:
			std::cout << " SourceQuench";
			break;
		case ICMP_REDIRECT:
			std::cout << " Redirect    ";
			break;
		case ICMP_TIME_EXCEEDED:
			std::cout << " TimeExcedeed";
			break;
		case ICMP_PARAMETERPROB:
			std::cout << " ParamProblem";
			break;
		case ICMP_TIMESTAMP:
			std::cout << " TimeStampREQ";
			break;
		case ICMP_TIMESTAMPREPLY:
			std::cout << " TimeStampREP";
			break;
		case ICMP_INFO_REQUEST:
			std::cout << " Info Request";
			break;
		case ICMP_INFO_REPLY:
			std::cout << " Info Reply  ";
			break;
		case ICMP_ADDRESS:
			std::cout << " Addr MasqReq";
			break;
		case ICMP_ADDRESSREPLY:
			std::cout << " Addr MasqRep";
			break;
	}

	std::cout << " from " << Indirizzo(S);
	nocol();
	std::cout << std::endl;
}

void LoggaTCP(void)
{
	set_col(RED, BLK);
	std::cout << "TCP\n";
	nocol();
	std::cout << std::endl;
}

void LoggaConnessioneTCP( struct tcphdr  * th, unsigned long S, unsigned long D)
{
	struct servent *P = NULL; //porta --> servizio
	
	set_col(RED,BLK);
	std::cout << "TCP  ";
	nocol();
	set_col(YEL,BLK);
	std::cout << "connection from " << Indirizzo(S) << " on (" << ntohs(th->dest) << ")";
	if ((P!=NULL)) {
		std::cout << " <" << P->s_name << ">";
	}
	nocol();
	std::cout << std::endl;
}
void sniffer(std::string device) 
{
	char receive[4098];
	struct tcphdr  *tcp_header;
	struct iphdr *ip_header;
	struct icmphdr *icmp_header;
	struct udphdr *udp_header;
	// struct ethhdr *ether_header;
	// struct ip *IP;
	long ContaTCP,
		 ContaUDP,
		 QuantiUDP,
		 ContaICMP,
		 QuantiICMP;
	int r,
		CheckSYN=0;
	
	ContaTCP = ContaUDP = QuantiUDP = ContaICMP = QuantiICMP = 0;

	try {
		RawSocket sniff(device);

		do {
			if ((r = sniff.Receive(receive, sizeof(receive))) <= 0) {
				return;
			}
			
			// ether_header = (struct ethhdr *)receive;
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
				// IP = (struct ip *)(receive + sizeof(ether_header));

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
	} catch (jio::IOException &e) {
		std::cout << e.what() << std::endl;
	}
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << "usage:: " << argv[0] << " <device>" << std::endl;

		return 0;
	}

	InitializeSocketLibrary();

	sniffer(argv[1]);

	ReleaseSocketLibrary();
}

