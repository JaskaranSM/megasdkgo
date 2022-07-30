package megasdkgo

type DownloadInfo struct {
	Name            string
	ErrorCode       int
	ErrorString     string
	Gid             string
	Speed           int64
	CompletedLength int64
	TotalLength     int64
	State           int
}

func NewDownloadInfo() *DownloadInfo {
	return &DownloadInfo{}
}
