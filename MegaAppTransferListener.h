#ifndef MEGAAPPTRANSFERLISTENER_H
#define MEGAAPPTRANSFERLISTENER_H
#include "include/megaapi.h"
#include <condition_variable>
#include <mutex>

#include "Callbacks.h"

class MegaAppTransferListener : public mega::MegaTransferListener 
{
private:
    std::condition_variable m_cv;
    std::mutex m_mu;
    bool m_notified;
    int64_t m_completedLength;
    int64_t m_totalLength;
    int64_t m_speed;
    int m_state;
    int m_errCode; 
    std::string m_errString;
    bool m_isComplete;
    bool m_isCancelled;
    bool m_isFailed;
    std::string m_gid;

    mega::MegaTransfer *m_currentTransfer;
    mega::MegaApi *m_api; 
    Callbacks *m_callbacks;

private:
    void onTransferStart(mega::MegaApi *api, mega::MegaTransfer *transfer);
    void onTransferFinish(mega::MegaApi *api, mega::MegaTransfer *transfer, mega::MegaError* e);
    void onTransferTemporaryError(mega::MegaApi *api, mega::MegaTransfer *transfer, mega::MegaError e);
    void onTransferUpdate(mega::MegaApi *api, mega::MegaTransfer *transfer);
    void lockAndNotify();

public:
    MegaAppTransferListener(std::string gid, mega::MegaApi *api);
    void CancelTransfer();
    int64_t CompletedLength();
    int64_t TotalLength();
    int64_t Speed();
    void SetTotalLength(int64_t length);
    int GetState();
    int GetErrorCode();
    std::string GetErrorString();
    bool IsCompleted();
    bool IsCancelled();
    bool IsFailed();
    void SetCallbacks(Callbacks *cb);
    void Reset();
    void Wait();
};

#endif 