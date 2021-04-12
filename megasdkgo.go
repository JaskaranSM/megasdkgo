package megasdkgo

/*
 #cgo CXXFLAGS: -std=c++11 -fpermissive
 #cgo LDFLAGS: -lmega
 #include <stdlib.h>
 #include "funcheader.h"
*/
import "C"
import (
	"fmt"
)

const (
	StateDownloading = 0
	StateFailed      = 1
	StateCompleted   = 2
	StateCanceled    = 3
)

func NewMegaClient(API_KEY string) *MegaClient {
	C.init(C.CString(API_KEY))
	return &MegaClient{
		API_KEY: API_KEY,
	}
}

type MegaClient struct {
	API_KEY string
}

func (m *MegaClient) Login(email string, password string) error {
	ret := C.login(C.CString(email), C.CString(password))
	if ret.errorCode != 0 {
		return fmt.Errorf("failed to login into mega account: %s", C.GoString(ret.errorString))
	}
	return nil
}

func (m *MegaClient) AddDownload(link string, dir string) (string, error) {
	ret := C.addDownload(C.CString(link), C.CString(dir))
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
	di.State = int(status.state)
	return di
}

func (m *MegaClient) CancelDownload(gid string) error {
	C.cancelDownload(C.CString(gid))
	return nil
}
