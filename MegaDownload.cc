#include "MegaDownload.h"

MegaDownload::MegaDownload(std::string name, std::string gid, MegaAppTransferListener *listener)
{
    this->m_name = name; 
    this->m_gid = gid;
    this->m_listener = listener;
}

int MegaDownload::CancelDownload() 
{
    this->m_listener->CancelTransfer();
    return 0;
}

bool MegaDownload::IsCompleted()
{
    return this->m_listener->IsCompleted();
}

void MegaDownload::SetEventCallbacks(Callbacks *cb)
{
    this->m_listener->SetCallbacks(cb);
}

const char* MegaDownload::Gid()
{
    return this->m_gid.c_str();
}

const char* MegaDownload::Name()
{
    return this->m_name.c_str();
}

int64_t MegaDownload::CompletedLength()
{
    return this->m_listener->CompletedLength();
}

int64_t MegaDownload::TotalLength()
{
    return this->m_listener->TotalLength();
}

int64_t MegaDownload::Speed()
{
    return this->m_listener->Speed();
}

int MegaDownload::GetErrorCode()
{
    return this->m_listener->GetErrorCode();
}

const char* MegaDownload::GetErrorString()
{
    return this->m_listener->GetErrorString().c_str();
}

int MegaDownload::GetState() 
{
    return this->m_listener->GetState();
}