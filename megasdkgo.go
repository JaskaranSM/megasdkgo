package megasdkgo

/*
 #cgo CXXFLAGS: -std=c++11 -fpermissive
 #cgo LDFLAGS: -L./.libs
 #cgo LDFLAGS: -lmega -lsqlite3 -lpthread -lcryptopp -lcurl -lssl -lcrypto -lcares -lsodium -lrt -lz -lnghttp2 -lbrotlidec -lbrotlienc -lbrotlicommon
 #include <stdlib.h>
 #include "funcheader.h"
*/
import "C"
import (
	"fmt"
	"unsafe"
)

const (
	StateNone       = 0
	StateQueued     = 1
	StateActive     = 2
	StatePaused     = 3
	StateRetrying   = 4
	StateCompleting = 5
	StateCompleted  = 6
	StateCancelled  = 7
	StateFailed     = 8
)

func NewMegaClient(API_KEY string) *MegaClient {
	C.initmega(C.CString(API_KEY))
	return &MegaClient{
		API_KEY: API_KEY,
	}
}

type MegaClient struct {
	API_KEY string
}

func (m *MegaClient) Login(email string, password string) error {
	ret := C.login(C.CString(email), C.CString(password))
	defer func() {
		C.free(unsafe.Pointer(ret))
	}()
	if ret.errorCode != 0 {
		return fmt.Errorf("failed to login into mega account: %s", C.GoString(ret.errorString))
	}
	return nil
}

func (m *MegaClient) AddDownload(link string, dir string) (string, error) {
	ret := C.addDownload(C.CString(link), C.CString(dir))
	defer func() {
		C.free(unsafe.Pointer(ret))
	}()
	gidgo := C.GoString(ret.gid)
	if ret.errorCode != 0 {
		return gidgo, fmt.Errorf("failed to add mega download: %s", C.GoString(ret.errorString))
	}
	return gidgo, nil
}

func (m *MegaClient) GetDownloadInfo(gid string) *DownloadInfo {
	di := NewDownloadInfo()
	status := C.getDownloadByGid(C.CString(gid))
	if status == nil {
		return di
	}
	di.Name = C.GoString(status.name)
	di.ErrorCode = int(status.errorCode)
	di.Speed = int64(status.speed)
	di.CompletedLength = int64(status.completedLength)
	di.TotalLength = int64(status.totalLength)
	di.Gid = gid
	di.ErrorString = C.GoString(status.errorString)
	di.State = int(status.state)
	C.free(unsafe.Pointer(status))
	return di
}

func (m *MegaClient) CancelDownload(gid string) error {
	C.cancelDownload(C.CString(gid))
	return nil
}
