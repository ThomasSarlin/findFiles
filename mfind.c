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
char type='f';
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER,print=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait;

int main(int argc,char **argv){
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
        		break;
        	case 't':
            		type = optarg[0];
           		break;
        	default:
            		break;
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
	DIR *dir;
	struct dirent *ent;
	char *dirName;
	int count=0;

	while(running){
		if(list_isEmpty(linkedList)){
			pthread_mutex_lock(&lock);
			waitcount++;
			if(waitcount==nrthrd){
				running=0;
				pthread_cond_broadcast(&wait);
				pthread_mutex_unlock(&lock);
			}else{
				pthread_cond_wait(&wait,&lock);
				waitcount--;
				pthread_mutex_unlock(&lock);
			}
		} else{
			dirName=list_pop(linkedList);
			if((dir=opendir(dirName))!=NULL)
				while((ent=readdir(dir))!=NULL){
					checkDirEntry(dirName,ent);
					checkFileName(ent);
				}	
			else{
				perror(dirName);
			}
			free(dirName);
			free(dir);
			count++;
		}
	}
	pthread_mutex_lock(&print);
	printf(" I SEARCHED %i FOLDERS \n",count);
	pthread_mutex_unlock(&print);
	return NULL;
}
void waitForThreads(pthread_t tid[]){
	for(int i=0;i<nrthrd-1;i++){
		if(pthread_join(tid[i],NULL)){
			perror((char*)tid[i]);
		}
	}
}
void checkDirEntry(char * dirName,struct dirent *ent){
	char *fullPath;
	if(ent->d_type==DT_DIR && strcmp(ent->d_name,".") && strcmp(ent->d_name,"..")){	
		pthread_mutex_lock(&lock);
		if(!(fullPath=malloc((strlen(dirName)+strlen(ent->d_name)+2)*sizeof(char))))
			perror("not enough memory");
		if(sprintf(fullPath,"%s/%s",dirName,ent->d_name)<0)perror("sprintf error");
		list_insert(linkedList,list_first(linkedList),fullPath);
		if(waitcount>0)pthread_cond_broadcast(&wait);
		pthread_mutex_unlock(&lock);
	}
}
int checkFileName(struct dirent *ent){
	
	pthread_mutex_lock(&print);	
	if(strstr(ent->d_name,filename)!=NULL){
		printf("\nYAY");
	}
	pthread_mutex_unlock(&print);
	return 0;
}
