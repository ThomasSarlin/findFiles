#include <pthread.h>
#include <dirent.h>
void checkOptions(int,char**);
void* traverseFolders(void*);
void addFoldersToList(int,char**);
void startThreads(pthread_t[]);
void waitForThreads(pthread_t[]);
void waitCheck(void);
void openDirectory(void);
void broadcastEnd(void);
void awaitBroadcast(void);
void checkDirEntry(char*,struct dirent*);
int checkFileName(char*,struct dirent*);
int checkType(struct dirent*,char);