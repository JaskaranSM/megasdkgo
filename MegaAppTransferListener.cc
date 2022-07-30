#include "include/megaapi.h"
#include "MegaAppTransferListener.h"

#include <iostream>

MegaAppTransferListener::MegaAppTransferListener(std::string gid, mega::MegaApi *api)
{
    this->m_notified = false;
    this->m_gid = gid;
    this->m_api = api;
    this->m_state = mega::MegaTransfer::STATE_QUEUED;
}

void MegaAppTransferListener::lockAndNotify() 
{
    {
        std::unique_lock<std::mutex> lock(this->m_mu);
        m_notified = true;
    }
    m_cv.notify_all();
}

void MegaAppTransferListener::onTransferStart(mega::MegaApi *api, mega::MegaTransfer *transfer) 
{
    this->m_state = transfer->getState();
    this->m_currentTransfer = transfer->copy();

    this->m_callbacks->OnDownloadStartCallback(this->m_gid.c_str());
}

void MegaAppTransferListener::onTransferFinish(mega::MegaApi *api, mega::MegaTransfer *transfer, mega::MegaError* e)
{  
    if(e->getErrorCode() != mega::MegaError::API_OK)
    {
        this->m_errCode = e->getErrorCode();
        this->m_errString = e->getErrorString();
        return;
    } 

    this->m_state = transfer->getState();
    this->lockAndNotify();

    if(transfer->isFolderTransfer() && transfer->isFinished())
    {
        this->m_isComplete = true;
        this->m_cv.notify_all();

        this->m_callbacks->OnDownloadCompleteCallback(this->m_gid.c_str());
    } else if(transfer->isFinished()) {
        this->m_isComplete = true;
        this->m_cv.notify_all();

        this->m_callbacks->OnDownloadCompleteCallback(this->m_gid.c_str());
    }
}

void MegaAppTransferListener::onTransferTemporaryError(mega::MegaApi *api, mega::MegaTransfer *transfer, mega::MegaError e)
{
    std::cout << "OnTransferTempError: "<< e.toString() << std::endl;
}

void MegaAppTransferListener::onTransferUpdate(mega::MegaApi *api, mega::MegaTransfer *transfer)
{
    if(this->IsCancelled())
    {
        api->cancelTransfer(this->m_currentTransfer);
        this->m_state = mega::MegaTransfer::STATE_CANCELLED;
    }

    this->m_speed = transfer->getSpeed();
    this->m_completedLength = transfer->getTransferredBytes();
    
    this->m_state = transfer->getState();
}

void MegaAppTransferListener::CancelTransfer()
{
    this->m_isCancelled = true;
}

int64_t MegaAppTransferListener::CompletedLength()
{
    return this->m_completedLength;
}

int64_t MegaAppTransferListener::TotalLength()
{
    return this->m_totalLength;
}

int64_t MegaAppTransferListener::Speed()
{
    return this->m_speed;
}

void MegaAppTransferListener::SetTotalLength(int64_t length)
{
    this->m_totalLength = length;
}

int MegaAppTransferListener::GetState()
{
    return this->m_state;
}

int MegaAppTransferListener::GetErrorCode()
{
    return this->m_errCode;
}

std::string MegaAppTransferListener::GetErrorString()
{
    return this->m_errString;
}

bool MegaAppTransferListener::IsCompleted()
{
    return this->m_isComplete;
}

bool MegaAppTransferListener::IsCancelled()
{
    return this->m_isCancelled;
}

bool MegaAppTransferListener::IsFailed()
{
    return this->m_isFailed;
}

void MegaAppTransferListener::SetCallbacks(Callbacks *cb)
{
    this->m_callbacks = cb; 
}

void MegaAppTransferListener::Reset()
{
    this->m_notified = false;
}

void MegaAppTransferListener::Wait()
{
    std::unique_lock<std::mutex> lock(this->m_mu);
	this->m_cv.wait(lock, [this]{return this->m_notified;});
}
