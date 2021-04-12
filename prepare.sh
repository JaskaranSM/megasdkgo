git clone https://github.com/meganz/sdk
cd sdk 
sh autogen.sh
./configure -disable-examples
make 
sudo make install
cd ..
mkdir include 
cp -r sdk/include/* include
go tool cgo megasdkgo.go