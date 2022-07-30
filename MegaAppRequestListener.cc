#include "include/megaapi.h"
#include <cstdint>
#include <iostream>

#include "MegaAppRequestListener.h"

MegaAppRequestListener::MegaAppRequestListener(mega::MegaApi *api)
{
    this->m_notified = false;
    this->m_api = api; 
}

void MegaAppRequestListener::lockAndNotify() 
{
    {
        std::unique_lock<std::mutex> lock(this->m_mu);
        m_notified = true;
    }
    m_cv.notify_all();
}

void MegaAppRequestListener::onRequestStart(mega::MegaApi* api, mega::MegaRequest *request)
{
    std::cout << "OnRequestStart"<<std::endl;
}

void MegaAppRequestListener::onRequestUpdate(mega::MegaApi* api,mega::MegaRequest *request) 
{
    std::cout << "OnRequestUpdate"<<std::endl;
}

void MegaAppRequestListener::onRequestFinish(mega::MegaApi* api, mega::MegaRequest *request, mega::MegaError* e)
{
    this->m_err = e->copy();
    int requestType = request->getType();
    if(e->getErrorCode() != mega::MegaError::API_OK)
    {
        this->lockAndNotify();
    } else {
        switch(requestType)
        {
            case mega::MegaRequest::TYPE_LOGIN:
                api->fetchNodes(this);
                break;
            case mega::MegaRequest::TYPE_FETCH_NODES:
                this->m_publicNode = api->getRootNode()->copy();
                break;
            case mega::MegaRequest::TYPE_GET_PUBLIC_NODE:
                this->m_publicNode = request->getPublicMegaNode()->copy();
                break;
        }
    }

    if(requestType != mega::MegaRequest::TYPE_LOGIN && requestType != mega::MegaRequest::TYPE_DELETE)
    {
        this->lockAndNotify();
    }
}

void MegaAppRequestListener::onRequestTemporaryError(mega::MegaApi* api, mega::MegaRequest *request, mega::MegaError* error)
{
    std::cout << "OnRequestTempError: "<< error->toString() <<std::endl;
}

mega::MegaNode* MegaAppRequestListener::GetPublicNode()
{
    return this->m_publicNode;
}

void MegaAppRequestListener::SetPublicNode(mega::MegaNode *node)
{
    this->m_publicNode = node;
}

mega::MegaError* MegaAppRequestListener::GetError()
{
    return this->m_err;
}

void MegaAppRequestListener::Reset()
{
    this->m_notified = false;
}

void MegaAppRequestListener::Wait()
{
    std::unique_lock<std::mutex> lock(this->m_mu);
	this->m_cv.wait(lock, [this]{return this->m_notified;});
}