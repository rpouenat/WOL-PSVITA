#define PACKET_BUF         17*6

#define MAC_ADDR_MAX       6
#define MAC_ADDR_STR_MAX   64

#define CONVERT_BASE       16

#define ADDR_LEN           16
#define REMOTE_ADDR        "255.255.255.255"

#define MAC_ADDR_SET	   "14:DD:A9:27:95:B9"

#define REMOTE_IP		   "192.168.1.46"


#define ARGS_BUF_MAX       128




void wol_pc();


int send_WOL( char *mac_addr);


void convert( const char *mac, char *mac_addr );
