#include "include/megaapi.h"
#include <condition_variable>
#include <mutex>
#include <cstdint>

class MegaAppRequestListener : public mega::MegaRequestListener 
{
private:
    std::condition_variable m_cv;
    std::mutex m_mu;
    bool m_notified;
    mega::MegaNode *m_publicNode;
    mega::MegaError *m_err;
    mega::MegaApi *m_api;

private:
    void onRequestStart(mega::MegaApi* api, mega::MegaRequest *request);
    void onRequestUpdate(mega::MegaApi* api,mega::MegaRequest *request);
    void onRequestFinish(mega::MegaApi* api, mega::MegaRequest *request, mega::MegaError* e);
    void onRequestTemporaryError(mega::MegaApi* api, mega::MegaRequest *request, mega::MegaError* error);
    void lockAndNotify();

public:
    MegaAppRequestListener(mega::MegaApi *m_api);
    mega::MegaNode* GetPublicNode();
    void SetPublicNode(mega::MegaNode *node);
    mega::MegaError* GetError();
    void Reset();
    void Wait();
};