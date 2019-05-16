#define PACKET_BUF         17*6

#define MAC_ADDR_MAX       6
#define MAC_ADDR_STR_MAX   64

#define CONVERT_BASE       16

#define ADDR_LEN           16
#define REMOTE_ADDR        "255.255.255.255"

#define MAC_ADDR_SET	   "XX:XX:XX:XX:XX:XX"

#define REMOTE_IP		   "192.168.1.X"

#define REMOTE_ADDR_MAX	   0xFFFFFFFF

#define REMOTE_PORT        9

#define ARGS_BUF_MAX       128

/**
* @brief Structure for mac address
*/
typedef struct
{
  unsigned char mac_addr[MAC_ADDR_MAX];
  char mac_addr_str[MAC_ADDR_STR_MAX];
} mac_addr_t;

/**
* @brief Structure for wol packet ( header )
*/
typedef struct
{
  char remote_addr[ADDR_LEN];
  mac_addr_t *mac_addr;
} wol_header_t;



void wol_pc();

int send_WOL( const wol_header_t *wol_header );

int packMacAddr( const char *mac, mac_addr_t *packedMac );
