#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h> // import pour quitter l'appli
#include <stdio.h>
#include <stdlib.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/display.h>
#include <psp2/sysmodule.h>

#include "wol.h"
#include "config.h"
#include "debugScreen.h"

#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>



#define NET_INIT_SIZE 1*1024*1024


#define printf psvDebugScreenPrintf // permet de redéfinir la fonction printf





int main(int argc, char *argv[]) {

	SceCtrlData pad; 	//initialisation de la variable des touches de controle
	psvDebugScreenInit(); //initialisation de la variable de l'écran
	int ret; // variable de retour contenant 



	// création du répertoire
	//sceIoMkdir("ux0:/data/vpn", 0777);


	open_file("ux0:/data/vpn/config.ovpn");

	//sceNetShowIfconfig((void*) p,0);


	// /* Save the IP of PSVita to a global variable */
	// sceNetInetPton(SCE_NET_AF_INET, info.ip_address, &vita_ip);


	char* url[] = {"icanhazip.com","/"};

	printWifiInformation();

	getIpPublic(url);

	wol_pc();



	/* Select to exit */
	printf("\n\n\n\n\n\n                Press select to exit.\n");
	SceCtrlData ctrl;
	while(1){
		sceCtrlPeekBufferPositive(0, &ctrl, 1);
		if (ctrl.buttons & SCE_CTRL_SELECT)
			break;

		sceKernelDelayThread(100*1000);
	}






	
	//sceKernelDelayThread(3*1000000); // Wait for 3 seconds
	//sceKernelExitProcess(0);
	return 0;
}
