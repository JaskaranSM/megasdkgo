#include "include/megaapi.h"
#include <cstdint>
#include <string.h>
#include <stdio.h>

class MegaAppRequestListener : public mega::MegaRequestListener 
{

private:
    std::condition_variable cv;
    std::mutex m;
    bool notified;
public:
    mega::MegaNode* public_node;
    mega::MegaNode* root_node;
    mega::MegaError *err;
    bool isKeyFound;
    MegaAppRequestListener()
    {
        notified = false;
    }
    void onRequestStart(mega::MegaApi* api, mega::MegaRequest *request)
    {

    }

    void onRequestUpdate(mega::MegaApi* api,mega::MegaRequest *request)
    {

    }

    void onRequestFinish(mega::MegaApi* api, mega::MegaRequest *request, mega::MegaError* e)
    {
        this->err = e->copy();
        int equal;
        int requestType = request->getType();
        this->isKeyFound = true;
        if(e->getErrorCode() == mega::MegaError::API_OK)
        {
            switch(requestType)
            {
                case mega::MegaRequest::TYPE_LOGIN:
                    api->fetchNodes(this);
                    break;
                case mega::MegaRequest::TYPE_FETCH_NODES:
                    this->public_node = api->getRootNode()->copy();
                    equal = strcmp(this->public_node->getBase64Key(), "");
                    if(equal == 0 && this->public_node->isForeign()) 
                    {
                        this->isKeyFound = false;   
                    }
                    break;
                case mega::MegaRequest::TYPE_GET_PUBLIC_NODE:
                    this->public_node = request->getPublicMegaNode()->copy();
                    equal = strcmp(this->public_node->getBase64Key(), "");
                    if(equal == 0 && this->public_node->isForeign()) 
                    {
                        this->isKeyFound = false;   
                    }
                    break;
            }
        } else {
            {
                std::unique_lock<std::mutex> lock(m);
		    	notified = true;
            }
            cv.notify_all();
        }
        if(requestType != mega::MegaRequest::TYPE_LOGIN /*&& requestType != mega::MegaRequest::TYPE_FETCH_NODES*/ && requestType != mega::MegaRequest::TYPE_DELETE)
        {   {
                std::unique_lock<std::mutex> lock(m);
		    	notified = true;
            }
            cv.notify_all();
		}
    }

    void onRequestTemporaryError(mega::MegaApi* api, mega::MegaRequest *request, mega::MegaError* error)
    {
        std::cout << error->toString() <<std::endl;
    }

    void reset()
	{
		notified = false;
	}

    void wait() 
	{
		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock, [this]{return notified;});
	}

};

class MegaAppTransferListener : public mega::MegaTransferListener 
{
private:
    std::condition_variable cv;
    std::mutex m;
    bool notified;
public:
    int64_t transferredbytes = 0;
    int64_t totalbytes = 0;
    int64_t speed = 0;
    int state = 0;
    std::string gid;
    int errorCode = 0;
    const char* errorString;
    bool is_complete = false;
    bool is_canceled = false;
    bool is_active = false;
    mega::MegaTransfer* this_transfer;
    int (*downloadEventCallback)(const char*,int);
public:
    void doEventCallback(int event)
    {
        if(this->downloadEventCallback != NULL)
        {
            this->downloadEventCallback(this->gid.c_str(),event);
        }
    }
    void onTransferStart(mega::MegaApi *api, mega::MegaTransfer *transfer)
    {
        this->state = transfer->getState();
        this->is_active = true;
        this->this_transfer = transfer->copy();
        doEventCallback(this->state);
    }

    void onTransferFinish(mega::MegaApi *api, mega::MegaTransfer *transfer, mega::MegaError* e)
    {
        if(e->getErrorCode() != mega::MegaError::API_OK)
        {
            this->errorCode = e->getErrorCode();
            this->errorString = e->getErrorString();
        } 
        this->state = transfer->getState();
        {
			std::unique_lock<std::mutex> lock(m);
			notified = true;
		}
        if(transfer->isFolderTransfer() && transfer->isFinished())
        {
            this->is_complete = true;
            doEventCallback(this->state);
            cv.notify_all();
        } else if(transfer->isFinished()) {
            this->is_complete = true;
            doEventCallback(this->state);
            cv.notify_all();
        }
    }

    void onTransferTemporaryError(mega::MegaApi *api, mega::MegaTransfer *transfer, mega::MegaError e)
    {

    }

    void onTransferUpdate(mega::MegaApi *api, mega::MegaTransfer *transfer)
    {   
        if(this->is_canceled && this->is_active)
        {
            this->is_active = false;
            api->cancelTransfer(this->this_transfer);
            this->state = transfer->getState();
        }
        speed = transfer->getSpeed();
        transferredbytes = transfer->getTransferredBytes();
        if(this->state != mega::MegaTransfer::STATE_CANCELLED) {
            this->state = transfer->getState();
        }
    }

    void reset()
	{
		notified = false;
	}

    void wait() 
	{
		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock, [this]{return notified;});
	}

    void CancelTransfer()
    {
        this->is_canceled = true;
        this->state = mega::MegaTransfer::STATE_CANCELLED;
    }

};

class MegaDownload
{
public:
    std::string name;
    std::string gid;
    MegaAppTransferListener* listener;
    MegaDownload(const char* n, MegaAppTransferListener* l, std::string g)
    {
        name = std::string(n);
        listener = l;
        gid = g;
    }
    ~MegaDownload()
    {
        
    }
    int CancelDownload()
    {
        this->listener->CancelTransfer();
        return 0;
    }
    bool IsCompleted()
    {
        return this->listener->is_complete;
    }
    void SetEventCallback(int (*callback)(const char*,int))
    {
        this->listener->downloadEventCallback = callback;
    }
    const char* GetGid()
    {
        return this->gid.c_str();
    } 
    const char* GetName()
    {
        return this->name.c_str();
    }
    int64_t GetTransferredBytes()
    {
        return this->listener->transferredbytes;
    }

    int64_t GetTotalBytes()
    {
        return this->listener->totalbytes;
    }

    int64_t GetSpeed()
    {
        return this->listener->speed;
    }

};
