#include "config.h"
#include <netinet/in.h>
#include <netdb.h>
#include <psp2/display.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>




// convertir les caractères UTF8
unsigned char readUTF(unsigned char c, int fd){
	int uni = 0;char u;
	if(c>=0b11000000){uni = uni?:c & 0b011111;read(fd,&u,sizeof(u));uni = (uni << 6) | (u & 0x3f);}
	if(c>=0b11100000){uni = uni?:c & 0b001111;read(fd,&u,sizeof(u));uni = (uni << 6) | (u & 0x3f);}
	if(c>=0b11110000){uni = uni?:c & 0b000111;read(fd,&u,sizeof(u));uni = (uni << 6) | (u & 0x3f);}
	switch(uni) {
	case  176:return 0xBC;
	case 8213:return 0x17;
	case 8216:return 0x60;
	case 8217:return 0x27;
	case 8218:return ',';
	case 8230:return 0x2E;
	case 8592:return '<';
	case 8593:return 0xCE;
	case 8594:return '>';
	case 8595:return 0xCD;
	case 8598:return '\\';
	case 8599:return '/';
	case 8601:return '\\';
	case 8600:return '\\';
	case 8602:return '/';
	case 9472:return 0x17;
	case 9474:return 0x16;
	case 9484:return 0x18;
	case 9488:return 0x19;
	case 9492:return 0x1A;
	case 9496:return 0x1B;
	case 9500:return 0x14;
	case 9508:return 0x13;
	case 9516:return 0x12;
	case 9524:return 0x11;
	case 9532:return 0x15;
	case 9600:return 0xC0;
	case 9601:return 0xC0;
	case 9602:return 0xDC;
	case 9603:return 0xDC;
	case 9604:return 0xC2;
	case 9605:return 0xC2;
	case 9606:return 0xDB;
	case 9607:return 0xDB;
	case 9608:return 0xDB;
	}
	return '?';
}




// Initialise la connexion pour le réseau
void netInit() {
	// psvDebugScreenPrintf("Loading module SCE_SYSMODULE_NET\n");
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	
	// psvDebugScreenPrintf("Running sceNetInit\n");
	SceNetInitParam netInitParam; /* Net init param structure */
	int size = 1*1024*1024;
	netInitParam.memory = malloc(size);
	netInitParam.size = size;
	netInitParam.flags = 0;
	sceNetInit(&netInitParam);

	//psvDebugScreenPrintf("Running sceNetCtlInit\n");
	sceNetCtlInit();
}




// Permet d'afficher les informations sur le WIFI
void printWifiInformation() {

	/* On initialise l'élément SceNetCtlInfo pour y stocker les informations */
	SceNetCtlInfo info; 
	// création d'un élément de type SceNetEtherAddr
	SceNetEtherAddr mac; 
	// ip
	char vita_ip[16];
	// masque
	char netmask[16];
	// route par défaut
	char default_route[16];
	// SSID
	char ssid[SCE_NETCTL_INFO_SSID_LEN_MAX + 1];
	// stock l'adresse mac
	static char macAddress[0x12]; 
	psvDebugScreenPrintf("\nConfiguration reseau : \n\n\n");

	netInit();

	sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_ETHER_ADDR, &info);
	sceNetGetMacAddress(&mac, 0);
	psvDebugScreenPrintf(" - Adresse MAC : %02X:%02X:%02X:%02X:%02X:%02X\n\n", mac.data[0], mac.data[1], mac.data[2], mac.data[3], mac.data[4], mac.data[5]);
	/* Get IP address */
	sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &info);
	// On stock l'IP de la PS vita dans vita_ip
	strcpy(vita_ip, info.ip_address);
	psvDebugScreenPrintf(" - IP console : %s\n\n", vita_ip);
	/* GET MASK */
	sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_NETMASK, &info);
	psvDebugScreenPrintf(" - Masque : / %s\n\n", info.netmask);
	/* Route par défaut */
	sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_DEFAULT_ROUTE, &info);
	psvDebugScreenPrintf(" - Route par default : %s\n\n", info.default_route);
	/* SSID */
	sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_SSID, &info);
	psvDebugScreenPrintf(" - SSID : %s\n\n", info.ssid);
}



void getIpPublic(char* url[]){

	int fd = socket(PF_INET, SOCK_STREAM, 0);

	// contient l'ip public 
	char vita_ip_public[16];

	/* On initialise une connexion */
	connect(fd, (const struct sockaddr *)&((struct sockaddr_in){.sin_family = AF_INET, .sin_port = htons(80), .sin_addr.s_addr = *(long*)(gethostbyname(url[0])->h_addr)}), sizeof(struct sockaddr_in));

	/* On définit les headers */
	char*header[] = {"GET ",url[1]," HTTP/1.1\r\n", "User-Agent: curl/PSVita\r\n", "Host: ",url[0],"\r\n", "\r\n", 0};

	//send all request header to the server
	for(int i = 0; header[i]; i++)
		write(fd, header[i], strlen(header[i]));


	// On fait la requête GET
	//8117FEDC

	unsigned pos = 0;
	int start_ip = 0;
	unsigned char c, line[4096];
	while(read(fd,&c,sizeof(c)) > 0 && pos < sizeof(line)) {
		if (c>>6==3) // if fetched char is a UTF8 
			c = readUTF(c,fd); // convert it back to ASCII

			if (c == '\n') { // end of line reached


				if(start_ip){

					for( int i = 0; i < pos; i = i + 1 ){
				      vita_ip_public[i] = line[i];
				   }
				   // caractère de fin de ligne
				   vita_ip_public[pos] = '\0';
				   // on positionne le compteur à la fin
				   pos = sizeof(line);
				   close(fd);
					//psvDebugScreenPrintf("%.*s\n", pos, line); // printf the received line into the screen
				}

				// si la ligne fait 1 alors la ligne suivante est l'ip
				if(pos == 1)
					start_ip = 1;

				pos = 0; // reset the buffer pointer back to 0
			} else {

				line[pos] = c;
				pos++;
			}
	}
	// close(fd);

	psvDebugScreenPrintf(" - IP Public : %s\n",vita_ip_public);

	// ip.src==192.168.1.28 && ip.dst==192.168.1.28



}








// On lit le fichier de conf
void open_file(char *file_name){
	char ch;	
	FILE *fp;

	fp = fopen(file_name, "r"); // read mode
	if (fp == NULL) {
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	psvDebugScreenPrintf("The contents of %s file are:\n", file_name);
	while((ch = fgetc(fp)) != EOF){
		psvDebugScreenPrintf("%c", ch);
		// On affiche la première ligne
		if (ch == '\n') {
			// On attend avant d'afficher les caractères
			sceKernelDelayThread(100000);
            break;
        }
	}

	fclose(fp);
}