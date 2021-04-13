#ifndef FUNCHEADER_H
#define FUNCHEADER_H
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

struct DownloadInfo
{
    const char *name;
    int64_t completedLength;
    int64_t totalLength;
    int64_t speed;
    int state;
    const char *gid;
    int errorCode;
    const char* errorString;
};

struct AddDownloadResp
{
    const char *gid;
    const char *errorString;
    int errorCode;
};

struct LoginResp 
{
    int errorCode;
    const char* errorString;

};
int initmega(const char* API_KEY);
struct LoginResp* login(const char* email, const char* password);
struct AddDownloadResp* addDownload(const char* link,const char* dir);
int cancelDownload(const char* gid);
struct DownloadInfo* getDownloadByGid(const char* gid); 

#ifdef __cplusplus
}
#endif
#endif