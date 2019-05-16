#include "wol.h"

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



void wol_pc() {

  psvDebugScreenPrintf( "Enter into WOL. \n\n");

  char mac_addr[MAC_ADDR_STR_MAX];

  // Convertit l'adresse mac
  packMacAddr( MAC_ADDR_SET, mac_addr );

  psvDebugScreenPrintf( "Try to sent WOL magic packet to %s ...!\n\n", MAC_ADDR_SET );

  send_WOL( mac_addr);

}




int packMacAddr( const char *mac, char *mac_addr )
{
  char *tmpMac    = (char *) malloc( strlen( mac ) * sizeof( char ));
  char *delimiter = (char *) ":";
  char *tok;
  int i;

  strncpy( tmpMac, mac, strlen( mac ));
  tok = strtok( tmpMac, delimiter );

  for( i = 0; i < MAC_ADDR_MAX; i++ )
  {
    if( tok == NULL )
    {
      return -1;
    }

    mac_addr[i] = (unsigned char) strtol( tok, NULL, CONVERT_BASE );
    tok = strtok( NULL, delimiter );
  }

  // strncpy( packedMac->mac_addr_str, mac, MAC_ADDR_STR_MAX );
  return 0;
}




int send_WOL( char *mac_addr)
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
  psvDebugScreenPrintf("Converting IP address.\n");
  sceNetInetPton(SCE_NET_AF_INET, REMOTE_IP, (void*)&dst_addr);

  /* Create raw socket type with UDP net protocol */
  sfd = sceNetSocket("ping_test", SCE_NET_AF_INET, SCE_NET_SOCK_RAW, SCE_NET_IPPROTO_UDP);
  if (sfd < 0)
    psvDebugScreenPrintf("SOCKET ERROR");
  else
    psvDebugScreenPrintf("Raw socket created.\n\n");


  /* On autorise le sokcet a envoyer les datagrammes à l'adreese de diffusion */
  retval = sceNetSetsockopt(sfd, SCE_NET_SOL_SOCKET, SCE_NET_SO_BROADCAST, (const void*)&on, sizeof(on)); 
  if (retval == -1)
    psvDebugScreenPrintf("NOT ALLOW TO BROADCAST");
  else
    psvDebugScreenPrintf("Allow socket to broadcast.\n\n");



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

  psvDebugScreenPrintf("Packet send : ");
  for (int i = 0; i < sizeof( packet ); i++) {
    psvDebugScreenPrintf("%x", packet[i]);
  }
  psvDebugScreenPrintf("\n\n");

  

  // On envoie le packet
  // sent_data = 0;
  sent_data = sceNetSendto(sfd, packet, sizeof( packet ), 0, (SceNetSockaddr*)&serv_addr, sizeof(SceNetSockaddr));
  if (sent_data < 1){
      psvDebugScreenPrintf( "Cannot send data: %s ...!\n", strerror( errno ));
  }else{
      psvDebugScreenPrintf( "Successful sent WOL magic packet to %s ...!\n", MAC_ADDR_SET );
  }


  
  return 0;
}
