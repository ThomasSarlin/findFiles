#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include "mfind.h"
#include "linkedList.h"
list *linkedList;
int waitcount=0,running=1;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER,print=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait;

int main(int argc,char **argv){
	if(argc<3){
		fprintf(stderr,"invalid number of arguments\n");
		exit(1);	
	}
	data *mData=initializeData(argv[argc-1]);
	checkOptions(argc,argv,mData);
	linkedList = list_create();

	pthread_t tid[mData->nrthrd];
	
	addFoldersToList(argc,argv,mData);
	startThreads(tid,mData);
	traverseFolders(mData);
	waitForThreads(tid,mData);
	list_clear(linkedList,NULL);
	
	freeData(mData);
	return 0;
}
data *initializeData(char* filename){
	data *mData=malloc(sizeof(struct mfindData));
 	mData->nrthrd=1;
	mData->type='z';
	mData->filename=malloc((strlen(filename)+1)*sizeof(char));
	strcpy(mData->filename,filename);
	return mData;
}
void freeData(data* mData){
	free(mData->filename);
	free(mData);
}
void checkOptions(int argc, char** argv,data *mData){
	char c;
    	while ((c = getopt (argc, argv, "p:t:")) != -1){
	    if(((argc-1)-optind)<1){
		    fprintf(stderr,"invalid number of arguments\n");
		    freeData(mData);
		    exit(1);
	    }
	    switch (c){
		    case 'p':
			    mData->nrthrd= atoi(optarg);
				    if(mData->nrthrd<=0){
					    fprintf(stderr, "invalid type following -p flag");
					    exit(1);
				    }
			    break;
		    case 't':
			    if(optarg[0]=='f'||optarg[0]=='d'||optarg[0]=='l')
				    mData->type = optarg[0];
			    else{
				    fprintf(stderr,"invalid type following -t flag");
				    exit(1);
			    }
			    break;
		    default:
			    exit(1);
	    }
	}
}
void addFoldersToList(int argc,char **argv,data *mData){
	char * dir;
	for(int i=optind;i<argc-1;i++){
		dir=malloc((strlen(argv[i])+1)*sizeof(char));
		strcpy(dir,argv[i]);
		if(!strcmp(basename(dir),mData->filename) && (mData->type=='d' || mData->type=='z')){
			printf("%s\n",dir);
		}
		list_insert(linkedList,list_first(linkedList),dir);
	}
}
void startThreads(pthread_t tid[],data *mData){
	for(int i=0;i<mData->nrthrd-1;i++)
    		if(pthread_create(&tid[i],NULL,traverseFolders,mData)){
			perror("pthread");
			exit(1);
		}
}
void* traverseFolders(void* arg){
	int count=0;

	while(running){
		if(list_isEmpty(linkedList)){
			waitCheck(arg);
		} else{
			openDirectory(arg);
			count++;
		}
	}
	pthread_mutex_lock(&print);
	printf("Thread: %li Reads: %i\n",pthread_self(),count);
	pthread_mutex_unlock(&print);
	return NULL;
}
void openDirectory(data* mData){
	DIR *dir;
	struct dirent *ent;
	char *dirName;

	dirName=list_pop(linkedList);
	if(dirName!=NULL && (dir=opendir(dirName))!=NULL){

		checkFileName(dirName,NULL,mData);
		while((ent=readdir(dir))!=NULL){
			checkDirEntry(dirName,ent,mData);
		}
		closedir(dir);
	}else perror(dirName);
	free(dirName);
}
void waitCheck(data *mData){
	pthread_mutex_lock(&lock);
	waitcount++;
	if(waitcount==mData->nrthrd){
		broadcastEnd();
	}else{
		awaitBroadcast();
	}
}
void waitForThreads(pthread_t tid[],data *mData){
	for(int i=0;i<mData->nrthrd-1;i++){
		if(pthread_join(tid[i],NULL)){
			perror((char*)tid[i]);
		}
	    }
}
void broadcastEnd(void){
	running=0;
	pthread_cond_broadcast(&wait);
	pthread_mutex_unlock(&lock);
}
void awaitBroadcast(void){
	pthread_cond_wait(&wait,&lock);
	waitcount--;
	pthread_mutex_unlock(&lock);
}
void checkDirEntry(char * dirName,struct dirent *ent,data *mData){
	char *fullPath;
	if(checkType(ent,'d') && strcmp(ent->d_name,".") && strcmp(ent->d_name,"..")){	
		pthread_mutex_lock(&lock);
		if(!(fullPath=malloc((strlen(dirName)+strlen(ent->d_name)+2)*sizeof(char))))
			perror("not enough memory");
		if(sprintf(fullPath,"%s/%s",dirName,ent->d_name)<0)
			perror("sprintf error");
		list_insert(linkedList,list_first(linkedList),fullPath);
		pthread_cond_broadcast(&wait);
		pthread_mutex_unlock(&lock);
	}
	checkFileName(dirName,ent,mData);
}
int checkFileName(char* dirName,struct dirent *ent,data *mData){
	if(ent!=NULL && checkType(ent,mData->type)){	
		if(!strcmp(ent->d_name,".")||!strcmp(ent->d_name,".."))return 0;
		if(!strcmp(ent->d_name,mData->filename)){
			char* path=malloc(((strlen(dirName)+strlen(ent->d_name))+2)*sizeof(char));
			if(!sprintf(path,"%s/%s",dirName,ent->d_name))perror("sprintf error");
			pthread_mutex_lock(&print);	
			printf("%s\n",path);
			pthread_mutex_unlock(&print);
			free(path);
		}
	}

	return 0;
}
int checkType(struct dirent *ent,char fileType){
	int result=1;
	switch(fileType){
		case 'l':
			result= ent->d_type==DT_LNK;
			break;
		case 'd':
			result= ent->d_type==DT_DIR; 
			break;
		case 'f':
			result=ent->d_type==DT_REG;
		default:
			break;
	}
	return result;
}
