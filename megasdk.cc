#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include "_cgo_export.h"
#include <chrono>
#include <map>
#include <unistd.h>

#include <condition_variable>
#include "megaheader.h"
#include "funcheader.h"

#include <condition_variable>
#include "include/megaapi.h"

mega::MegaApi* api;
std::vector<MegaDownload*> AllDls;

bool isMegaFolder(const char* link)
{
    std::string mlink = link;
    return mlink.find("folder") != std::string::npos || mlink.find("#F!") != std::string::npos;
}

const char* gen_random(const int len) 
{
    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    srand( (unsigned) time(NULL) * getpid());
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s.c_str();
}

int genericCallback(const char* gid, int event)
{
    std::cout << gid << " " << event << std::endl;
    return 0;
}

class MegaDownloader
{
public:
    MegaDownloader()
    {

    }
    int (*downloadEventCallback)(const char*,int);

public:
    void SetEventCallback(int (*callback)(const char*,int))
    {
        this->downloadEventCallback = callback;
    }

    bool IsLoggedIn()
    {
        return api->isLoggedIn();
    }
    struct LoginResp* Login(const char* email, const char* password)
    {
        struct LoginResp* resp = new LoginResp();
        resp->errorCode = 0;
        MegaAppRequestListener* req_listener = new MegaAppRequestListener();
        api->login(email,password,req_listener);
        req_listener->wait();
        req_listener->reset();
        if(req_listener->err != NULL)
        {
            resp->errorCode = req_listener->err->getErrorCode();
            resp->errorString = req_listener->err->getErrorString();
        } 
        return resp;
    }
    MegaDownload* GetDownloadByGid(const char* gid)
    {
        for (auto const& dl : AllDls)
        {
            int equal = strcmp(dl->GetGid(),gid); 
            if(equal == 0)
            {
                return dl;
            }
        }
        return nullptr;
    }
    void moveDownloadToActive(std::string gid, MegaDownload* d)
    {
        AllDls.push_back(d);
    }
    int CancelDownloadByGid(const char* gid) 
    {
        MegaDownload* dl = GetDownloadByGid(gid);
        if(dl == nullptr)
        {
            return 1;
        }
        return dl->CancelDownload();
    }
    AddDownloadResp* AddDownload(const char* link, const char* dir)
    {
        struct AddDownloadResp* resp = new AddDownloadResp();
        std::string gid = gen_random(12);
        resp->gid = gid.c_str();
        MegaAppRequestListener* req_listener = new MegaAppRequestListener();
        MegaAppTransferListener* transfer_listener = new MegaAppTransferListener();
        if(isMegaFolder(link))
        {
            api->loginToFolder(link,req_listener);
            req_listener->wait();
            req_listener->reset();
        } else {
            api->getPublicNode(link,req_listener);
            req_listener->wait();
            req_listener->reset();
        }
        if( req_listener->err != NULL && req_listener->err->getErrorCode() != mega::MegaError::API_OK)
        {
            resp->errorCode = req_listener->err->getErrorCode();
            resp->errorString = req_listener->err->getErrorString();
            return resp;
        }
        const char* fname = req_listener->public_node->getName();
        std::string fpath = dir;
        fpath += "/";
        fpath += fname;
        transfer_listener->gid = gid;
        transfer_listener->totalbytes = api->getSize(req_listener->public_node);
        transfer_listener->state = mega::MegaTransfer::STATE_QUEUED;
        MegaDownload* dl = new MegaDownload(req_listener->public_node->getName(),transfer_listener,gid);
        api->startDownload(req_listener->public_node,fpath.c_str(),transfer_listener);
        moveDownloadToActive(gid,dl);
        return resp;
    }
    
};

MegaDownloader *downloader;

int initmega(const char* API_KEY){
    std::cout << "Initiliazing Library with API_KEY: "  << API_KEY << std::endl;
    api = new mega::MegaApi(API_KEY, (const char*)NULL, "Mega CLI Client");
    MegaDownloader* downloader = new MegaDownloader();
    downloader->SetEventCallback(genericCallback);
    return 0;
}

struct LoginResp* login(const char* email, const char* password)
{
    return downloader->Login(email,password);
}

struct AddDownloadResp* addDownload(const char* link,const char* dir) 
{
    return downloader->AddDownload(link,dir);
}

int cancelDownload(const char* gid)
{
    return downloader->CancelDownloadByGid(gid);
}

struct DownloadInfo* getDownloadByGid(const char* gid)
{
    MegaDownload* dl = downloader->GetDownloadByGid(gid);
    if(dl  == nullptr) 
    {
        std::cout << "Returnin nullptr to go: " <<std::endl;
        return nullptr;
    }
    struct DownloadInfo *di = new DownloadInfo();
    di->name = dl->GetName();
    di->completedLength = dl->GetTransferredBytes();
    di->gid = dl->GetGid();
    di->speed = dl->GetSpeed();
    di->errorCode = dl->listener->errorCode;
    di->state = dl->listener->state;
    di->errorString = dl->listener->errorString;
    di->totalLength = dl->GetTotalBytes();
    return di;
}