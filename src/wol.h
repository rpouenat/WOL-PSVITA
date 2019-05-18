#define PACKET_BUF         17*6

#define MAC_ADDR_MAX       6
#define MAC_ADDR_STR_MAX   64

#define CONVERT_BASE       16

#define ADDR_LEN           16

void read_config(char *file_name);


void wol_pc(char *ip_addr, char *mac_addr_str) ;


int send_WOL( char *ip_addr, char *mac_addr);


void convert( const char *mac, char *mac_addr );
