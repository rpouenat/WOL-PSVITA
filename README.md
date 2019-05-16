# WOL-PSVITA

Compile application with :

```
./compilation.sh
```


MAC Adress target:

Set your MAC Adress in `src/wol.h`

```
#define MAC_ADDR_SET	   "XX:XX:XX:XX:XX:XX"
```

IP Adresse target: 

Set your IP Adress in `src/wol.h`

```
#define REMOTE_IP		   "192.168.1.X"
```

PS VITA IP : 

```
curl -T *.vpk ftp://192.168.1.X:1337/ux0:/
```
