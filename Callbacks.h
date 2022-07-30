#pragma once
typedef void(*EventCallback) (const char* gid);

typedef struct {
    EventCallback OnDownloadStartCallback;
    EventCallback OnDownloadStopCallback; 
    EventCallback OnFileDownloadCompleteCallback; 
    EventCallback OnDownloadCompleteCallback;  
} Callbacks;