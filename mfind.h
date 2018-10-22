#include <pthread.h>
#include <dirent.h>
void checkOptions(int,char**);
void* traverseFolders(void*);
void addFoldersToList(int,char**);
void startThreads(pthread_t[]);
void waitForThreads(pthread_t[]);
void checkDirEntry(char*,struct dirent*);
int checkFileName(struct dirent*);
