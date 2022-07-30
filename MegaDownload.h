#include <string>
#include "MegaAppTransferListener.h"


class MegaDownload
{
private:
    std::string m_name; 
    std::string m_gid;
    MegaAppTransferListener *m_listener;
public:
    MegaDownload(std::string name, std::string gid, MegaAppTransferListener *listener);
    int CancelDownload();
    bool IsCompleted();
    void SetEventCallbacks(Callbacks *cb);
    const char* Gid();
    const char* Name();
    int64_t CompletedLength();
    int64_t TotalLength();
    int64_t Speed();
    int GetErrorCode();
    const char* GetErrorString();
    int GetState();
};