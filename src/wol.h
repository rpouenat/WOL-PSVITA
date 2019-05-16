#define PACKET_BUF         17*6

#define MAC_ADDR_MAX       6
#define MAC_ADDR_STR_MAX   64

#define CONVERT_BASE       16

#define ADDR_LEN           16
#define REMOTE_ADDR        "255.255.255.255"

#define MAC_ADDR_SET	   "XX:XX:XX:XX:XX:XX"

#define REMOTE_IP		   "192.168.1.X"


#define ARGS_BUF_MAX       128




void wol_pc();


int send_WOL( char *mac_addr);


int packMacAddr( const char *mac, char *mac_addr );
