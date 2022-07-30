git clone https://github.com/meganz/sdk
cd sdk 
sh autogen.sh
./configure --disable-examples --disable-shared --enable-static --without-freeimage
make 
sudo make install
mkdir libs
mv sdk/src/.libs/* libs/
mkdir include
mv sdk/include/* include/ 
go tool cgo MegaSDKGo.go