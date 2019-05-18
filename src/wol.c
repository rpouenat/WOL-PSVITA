#include "wol.h"


#include <psp2/ctrl.h> // import pour les boutons
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

#define NET_PARAM_MEM_SIZE (1*1024*1024)


#include <ctype.h>
#include <errno.h>



// On lit le fichier de conf
void read_config(char *file_name){
  int ch;  
  char ip_addr_str[16];
  char mac_addr_file[64];
  SceCtrlData ctrl;

  // permet de savoir si l'on stock dans l'ip ou dans l'adresse mac
  int ip = 1;

  FILE *fp;

  fp = fopen(file_name, "r"); // read mode
  if (fp == NULL) {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  // Compteur pour stocker les char dans l'array IP ou MAC
  int cpt_info = 0;

  // On parcourt le fichier
  do{
      ch = fgetc(fp);
      // On stock le contenue des lignes dans ip
      if(ip){
        // printf("%c", ch);
        ip_addr_str[cpt_info] = ch;
      }else{
        // printf("%c", ch);
        mac_addr_file[cpt_info] = ch;
      }

      // On incrémente le compteur
      cpt_info++;

      // Fin de ligne ou fin de fichier on clos le tableau
      if ((ch == '\n') || (ch == EOF) ) {
          if(ip){
            // i-1 car dernier caractère \n
            ip_addr_str[cpt_info-1] = '\0';
            // On passe ip a 0 pour stocker l'adresse MAC
            ip = 0;
            // On réinitialise le compteur
            cpt_info = 0;
          }else{
            // i-1 car dernier caractère \n
            mac_addr_file[cpt_info-1] = '\0';
            // On appelle la fonction de WOL
            psvDebugScreenPrintf("Machine to wake : \n\n");
            psvDebugScreenPrintf(" - IP : %s\n",ip_addr_str); 
            psvDebugScreenPrintf(" - MAC : %s\n\n",mac_addr_file);

            // On wake le PC
            wol_pc(ip_addr_str, mac_addr_file);

            // On passe l'ip a 1 pour stocker dans l'IP
            ip = 1;
            // On réinitialise le compteur
            cpt_info = 0;

            memset(ip_addr_str, 0, 16);
            memset(mac_addr_file, 0, 64);
          }

     }
  
  } while(ch != EOF);

  // On ferme le fichier
  fclose(fp);
}










void wol_pc(char *ip_addr, char *mac_addr_str) {

  // psvDebugScreenPrintf( "Enter into WOL. \n\n");

  char mac_addr[MAC_ADDR_STR_MAX];

  // Convertit l'adresse mac
  convert( mac_addr_str, mac_addr );

  // psvDebugScreenPrintf( "Try to sent WOL magic packet to %s ...!\n\n", mac_addr_str );

  send_WOL( ip_addr, mac_addr);

}


//  AA:BB:CC:DD:EE:FF -> aabbccddeeff
void convert( const char *mac, char *mac_addr )
{
  char *tmpMac    = (char *) malloc( strlen( mac ) * sizeof( char ));
  char *delimiter = (char *) ":";
  char *tok;
  int i;

  strncpy( tmpMac, mac, strlen( mac ));
  tok = strtok( tmpMac, delimiter );

  for( i = 0; i < MAC_ADDR_MAX; i++ )
  {

    mac_addr[i] = (unsigned char) strtol( tok, NULL, CONVERT_BASE );
    tok = strtok( NULL, delimiter );
  }
}




int send_WOL( char *ip_addr, char *mac_addr)
{


  int32_t retval; /* return value */
  int32_t sfd; /* Socket file descriptor */
  int32_t on; /* used in Setsockopt function */
  SceNetInAddr dst_addr; /* adresse de destination, contient l'IP */
  SceNetSockaddrIn serv_addr; /* server address to send data to */
  int32_t sent_data; /* return value for sendto function */
  SceNetInitParam net_init_param; /* Net init param structure */
  unsigned char packet[PACKET_BUF]; /*


  sceSysmoduleLoadModule(SCE_SYSMODULE_NET); /* load NET module */

  /* Initialisation du module NET */
  net_init_param.memory = malloc(NET_PARAM_MEM_SIZE);
  memset(net_init_param.memory, 0, NET_PARAM_MEM_SIZE);
  net_init_param.size = NET_PARAM_MEM_SIZE;
  net_init_param.flags = 0;
  sceNetInit(&net_init_param);

  
  /* Change IP string to IP uint */
  // psvDebugScreenPrintf("Converting IP address.\n");
  sceNetInetPton(SCE_NET_AF_INET, ip_addr, (void*)&dst_addr);

  /* Create raw socket type with UDP net protocol */
  sfd = sceNetSocket("ping_test", SCE_NET_AF_INET, SCE_NET_SOCK_RAW, SCE_NET_IPPROTO_UDP);
  // if (sfd < 0)
  //   psvDebugScreenPrintf("SOCKET ERROR");
  // else
  //   psvDebugScreenPrintf("Raw socket created.\n\n");


  /* On autorise le sokcet a envoyer les datagrammes à l'adreese de diffusion */
  retval = sceNetSetsockopt(sfd, SCE_NET_SOL_SOCKET, SCE_NET_SO_BROADCAST, (const void*)&on, sizeof(on)); 
  // if (retval == -1)
  //   psvDebugScreenPrintf("NOT ALLOW TO BROADCAST");
  // else
  //   psvDebugScreenPrintf("Allow socket to broadcast.\n\n");



  int i, j;

  // On set les parametre pour l'adresse 
  // Type de protocol
  serv_addr.sin_family = SCE_NET_AF_INET;

  serv_addr.sin_addr = dst_addr; /* set destination address */
  memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero)); /* fill sin_zero with zeroes */


  // Construction du packet WOL 
  for( i = 0; i < 6; i++ )
  {
    packet[i] = 0xFF;
  }

  for( i = 1; i <= 16; i++ )
  {
    for( j = 0; j < 6; j++ )
    {
      packet[i * 6 + j] = mac_addr[j];
    }
  }

  // psvDebugScreenPrintf("Packet send : ");
  // for (int i = 0; i < sizeof( packet ); i++) {
  //   psvDebugScreenPrintf("%x", packet[i]);
  // }
  // psvDebugScreenPrintf("\n\n");

  

  // On envoie le packet
  // sent_data = 0;
  sent_data = sceNetSendto(sfd, packet, sizeof( packet ), 0, (SceNetSockaddr*)&serv_addr, sizeof(SceNetSockaddr));
  if (sent_data < 1){
      psvDebugScreenPrintf( "Cannot send data: %s ...!\n", strerror( errno ));
  }else{
      psvDebugScreenPrintf( "Successful sent WOL magic packet !\n");
  }


  
  return 0;
}
