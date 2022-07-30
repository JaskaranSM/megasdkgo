#include <vector>
#include "MegaDownload.h"
#include "include/megaapi.h"
#include "GoHeader.h"

class MegaDownloader 
{
private:
    std::vector<MegaDownload*> m_downloads; 
    mega::MegaApi *m_api;
    const char* m_apiKey;
    Callbacks *m_callbacks;

private:
    void registerDownload(MegaDownload *dl);

public:
    MegaDownloader(const char* apiKey, mega::MegaApi *api);
    bool IsLoggedIn();
    struct LoginResp* Login(const char* email, const char* password);
    MegaDownload* GetDownloadByGid(const char* gid);
    int CancelDownloadByGid(const char* gid);
    AddDownloadResp* AddDownload(const char* link, const char* dir);

    void SetCallbacks(Callbacks *cb);
};