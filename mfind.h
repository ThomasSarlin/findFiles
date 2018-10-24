#include <pthread.h>
#include <dirent.h>
typedef struct mfindData{
	int nrthrd;
	char type;
	char *filename;
}data;
data* initializeData(char*);
void freeData(data*);
void checkOptions(int,char**,data*);
void* traverseFolders(void*);
void addFoldersToList(int,char**,data*);
void startThreads(pthread_t[],data*);
void waitForThreads(pthread_t[],data*);
void waitCheck(data*);
void openDirectory(data*);
void broadcastEnd(void);
void awaitBroadcast(void);
void checkDirEntry(char*,struct dirent*,data*);
int checkFileName(char*,struct dirent*,data*);
int checkType(struct dirent*,char);
