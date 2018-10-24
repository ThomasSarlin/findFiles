#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include "mfind.h"
#include "linkedList.h"
list *linkedList;
int nrthrd=1,waitcount=0,running=1;
char *filename;
char type='z';
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER,print=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait;

int main(int argc,char **argv){
	if((argc-optind)<2){
		fprintf(stderr,"invalid number of arguments");
		exit(1);
	}
	linkedList = list_create();
	filename=malloc((strlen(argv[argc-1])+1)*sizeof(char));
	strcpy(filename,argv[argc-1]);
	checkOptions(argc,argv);
	pthread_t tid[nrthrd];
	addFoldersToList(argc,argv);
	startThreads(tid);
	traverseFolders(0);
	waitForThreads(tid);
	list_clear(linkedList,NULL);
	free(filename);
	return 0;
}

void checkOptions(int argc, char** argv){
	char c;
    	while ((c = getopt (argc, argv, "p:t:")) != -1)
        switch (c){
        	case 'p':
        		nrthrd = atoi(optarg);
				if(nrthrd<=0){
					fprintf(stderr, "invalid type following -p flag");
					exit(1);
				}
        		break;
        	case 't':
					if(optarg[0]=='f'||optarg[0]=='d'||optarg[0]=='l')
            			type = optarg[0];
					else{
						fprintf(stderr,"invalid type following -t flag");
						exit(1);
					}
           		break;
        	default:
				exit(1);
        }
}
void addFoldersToList(int argc,char **argv){
	char * dir;
	for(int i=optind;i<argc-1;i++){
		dir=malloc((strlen(argv[i])+1)*sizeof(char));
		strcpy(dir,argv[i]);
		list_insert(linkedList,list_first(linkedList),dir);
	}
}
void startThreads(pthread_t tid[]){
	for(int i=0;i<nrthrd-1;i++)
    		if(pthread_create(&tid[i],NULL,traverseFolders,(&i+1))){
			perror("pthread");
			exit(1);
		}
}
void* traverseFolders(void* arg){
	int count=0;

	while(running){
		if(list_isEmpty(linkedList)){
			waitCheck();
		} else{
			openDirectory();
			count++;
		}
	}
	pthread_mutex_lock(&print);
	printf("Thread: %li Reads: %i\n",pthread_self(),count);
	pthread_mutex_unlock(&print);
	return NULL;
}
void openDirectory(void){
	DIR *dir;
	struct dirent *ent;
	char *dirName;

	dirName=list_pop(linkedList);
	if(dirName!=NULL && (dir=opendir(dirName))!=NULL){

		checkFileName(dirName,NULL);
		while((ent=readdir(dir))!=NULL){
			checkDirEntry(dirName,ent);
		}
		closedir(dir);
	}else perror(dirName);
	free(dirName);
}
void waitCheck(void){
	pthread_mutex_lock(&lock);
	waitcount++;
	if(waitcount==nrthrd){
		broadcastEnd();
	}else{
		awaitBroadcast();
	}
}
void waitForThreads(pthread_t tid[]){
	for(int i=0;i<nrthrd-1;i++){
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
void checkDirEntry(char * dirName,struct dirent *ent){
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
	checkFileName(dirName,ent);
}
int checkFileName(char* dirName,struct dirent *ent){
	if(ent!=NULL && checkType(ent,type)){	
		if(!strcmp(ent->d_name,".")||!strcmp(ent->d_name,".."))return 0;
		char* path=malloc(((strlen(dirName)+strlen(ent->d_name))+2)*sizeof(char));
		if(!sprintf(path,"%s/%s",dirName,ent->d_name))perror("sprintf error");
		if(!strcmp(ent->d_name,filename)){
			pthread_mutex_lock(&print);	
			printf("%s\n",path);
			pthread_mutex_unlock(&print);
		}
		free(path);
	}
	else{
		if(!strcmp(dirName,filename) && type=='d'){
			pthread_mutex_lock(&print);	
			printf("%s\n",dirName);
			pthread_mutex_unlock(&print);
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
