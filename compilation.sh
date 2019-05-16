mkdir build
cd build
cmake ..
make
mv *.vpk ../
cd ..
rm -rf build
curl -T *.vpk ftp://192.168.1.X:1337/ux0:/