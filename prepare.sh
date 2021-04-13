git clone https://github.com/meganz/sdk
cd sdk 
sh autogen.sh
./configure --disable-examples --disable-shared --enable-static
make 
sudo make install
cd ..
mkdir include 
cp -r sdk/include/* include
go tool cgo megasdkgo.go