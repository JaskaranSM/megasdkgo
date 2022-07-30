#include "MegaDownloader.h"
#include "MegaAppRequestListener.h"
#include "MegaAppTransferListener.h"
#include "Utils.h"
#include "GoHeader.h"

#include <string.h>

MegaDownloader::MegaDownloader(const char* apiKey, mega::MegaApi *api)
{
    this->m_apiKey = apiKey;
    this->m_api = api; 
}

bool MegaDownloader::IsLoggedIn()
{
    return this->m_api->isLoggedIn();
}

struct LoginResp* MegaDownloader::Login(const char* email, const char* password)
{
    struct LoginResp* resp = new LoginResp();
    resp->errorCode = 0;

    MegaAppRequestListener* reqListener = new MegaAppRequestListener(this->m_api);
    this->m_api->login(email,password, reqListener);
    reqListener->Wait();
    reqListener->Reset();

    if (reqListener->GetError() != NULL)
    {
        mega::MegaError* err = reqListener->GetError();
        resp->errorCode = err->getErrorCode();
        resp->errorString = err->getErrorString();
    }

    return resp; 
}

MegaDownload* MegaDownloader::GetDownloadByGid(const char* gid)
{
    for (auto const& dl : this->m_downloads)
    {
        int equal = strcmp(dl->Gid(), gid); 
        if(equal == 0)
        {
            return dl;
        }
    }
    return nullptr;
}

int MegaDownloader::CancelDownloadByGid(const char* gid)
{
    MegaDownload* dl = GetDownloadByGid(gid);
    if(dl == nullptr)
    {
        return 1;
    }
    return dl->CancelDownload();
}

AddDownloadResp* MegaDownloader::AddDownload(const char* link, const char* dir)
{
    struct AddDownloadResp* resp = new AddDownloadResp();
    std::string gid = GenerateRandomID(16);
    resp->gid = gid.c_str();
    MegaAppRequestListener* reqListener = new MegaAppRequestListener(this->m_api);
    MegaAppTransferListener* transferListener = new MegaAppTransferListener(gid, this->m_api);
    transferListener->SetCallbacks(this->m_callbacks);
    if(IsMegaFolder(link))
    {
        mega::MegaApi* folderApi = new mega::MegaApi(this->m_apiKey, (const char*)NULL, "Mega FolderApi temp Client");
        folderApi->loginToFolder(link, reqListener);
        reqListener->Wait();
        reqListener->Reset();

        if(reqListener->GetError() != NULL && reqListener->GetError()->getErrorCode() != mega::MegaError::API_OK)
        {
            resp->errorCode = reqListener->GetError()->getErrorCode();
            resp->errorString = reqListener->GetError()->getErrorString();
            return resp;
        }

        reqListener->SetPublicNode(folderApi->authorizeNode(reqListener->GetPublicNode()));
    } else {
        this->m_api->getPublicNode(link, reqListener);
        reqListener->Wait();
        reqListener->Reset();
    }

    if(reqListener->GetError() != NULL && reqListener->GetError()->getErrorCode() != mega::MegaError::API_OK)
    {
        resp->errorCode = reqListener->GetError()->getErrorCode();
        resp->errorString = reqListener->GetError()->getErrorString();
        return resp;
    }

    const char* fname = reqListener->GetPublicNode()->getName();
    std::string fpath = dir;
    fpath += "/";
    fpath += fname;
    transferListener->SetTotalLength(this->m_api->getSize(reqListener->GetPublicNode()));
    MegaDownload* dl = new MegaDownload(reqListener->GetPublicNode()->getName(), gid, transferListener);
    this->m_api->startDownload(reqListener->GetPublicNode(), fpath.c_str(), "", NULL, false, NULL, transferListener);
    return resp;
}

void MegaDownloader::registerDownload(MegaDownload *dl)
{
    this->m_downloads.push_back(dl);
}

void MegaDownloader::SetCallbacks(Callbacks *cb) 
{
    this->m_callbacks = cb;
}