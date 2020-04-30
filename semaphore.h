#include <errno.h>
#include <string.h>

union semun{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int seminit(int semid, union semun *arg, int val, int semnum){
    (*arg).val = val;

	if(semctl(semid,semnum,SETVAL,*arg)<0){
		printf("Erreur initialisation semaphore : %s\n",strerror( errno ));
		return -1;
	}
    return 0;
}

void semV(int semid,int semnum){
    struct sembuf sop;
    sop.sem_num = semnum;
    sop.sem_op  = 1;
    sop.sem_flg = SEM_UNDO;
    while(semop(semid,&sop,1)<0){
        if(errno != EINTR){
            printf("erreur V %d: %s \n",semnum,strerror( errno ));
            break;
        }
    }
}
void semP(int semid,int semnum){
    struct sembuf sop;
    sop.sem_num = semnum;
    sop.sem_op  = -1;
    sop.sem_flg = SEM_UNDO;
    while(semop(semid,&sop,1)<0){
        if(errno != EINTR){
            printf("erreur P %d: %s \n",semnum,strerror( errno ));
            break;
        }
    }
}