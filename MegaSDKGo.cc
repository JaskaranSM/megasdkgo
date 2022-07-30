#include "MegaDownloader.h"
#include "include/megaapi.h"

#include <iostream>

MegaDownloader *downloader;

void onDownloadStartCallback(const char* gid) 
{
    std::cout << "OnDownloadStart: " << gid <<std::endl;
}

void onDownloadStopCallback(const char* gid) 
{
    std::cout << "OnDownloadStop: " << gid <<std::endl;
}

void onDownloadCompleteCallback(const char* gid) 
{
    std::cout << "OnDownloadComplete: " << gid <<std::endl;
}

int initMega(const char* apiKey)
{
    mega::MegaApi *api = new mega::MegaApi(apiKey, (const char*)NULL, "Mega CLI Client");
    downloader = new MegaDownloader(apiKey, api);
    Callbacks* cb = new Callbacks();
    cb->OnDownloadCompleteCallback = onDownloadCompleteCallback;
    cb->OnDownloadStartCallback = onDownloadStartCallback;
    cb->OnDownloadStopCallback = onDownloadStopCallback;
    downloader->SetCallbacks(cb);
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
    di->name = dl->Name();
    di->completedLength = dl->CompletedLength();
    di->gid = dl->Gid();
    di->speed = dl->Speed();
    di->errorCode = dl->GetErrorCode();
    di->state = dl->GetState();
    di->errorString = dl->GetErrorString();
    di->totalLength = dl->TotalLength();
    return di;
}