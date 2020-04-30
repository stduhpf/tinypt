#include "pt.c"


#include <unistd.h>

#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

#include "semaphore.h"

/******************************************************************************/

#define N 10
#define N2 500

#define mutex30 0
#define mutex01 1
#define mutex12 2
#define mutex23 3
#define mutexAcces 4

int sigR = 0;
int Id;

void handler(int signo){
	printf("signal requested %d\n",Id);
	sigR = 1;
}

/******************************************************************************/
int main(){
	//printf("%f %f\n",drand(),drand());
	signal(SIGUSR1,handler);

	FILE * inputFile = fopen( "out2.ppm", "w" );

	#ifdef OUTPUT
	fprintf( inputFile,"P3\n");
	fprintf( inputFile,"%d %d\n%d\n",WIDTH,HEIGHT,CDEPTH);
	fflush( inputFile);
	#endif

    key_t cle =  ftok("/tmp", '0');
    int semid = semget(cle,5,IPC_CREAT|0666);
    if(semid<0){
        printf("Erreur creation Semaphore \n");
        return -1;
    }
    
    union semun arg;
    if(seminit(semid,&arg,1,mutex30)<0.)
        return -1;
    if(seminit(semid,&arg,0,mutex01)<0.)
        return -1;
    if(seminit(semid,&arg,0,mutex12)<0.)
        return -1;
    if(seminit(semid,&arg,0,mutex23)<0.)
        return -1;
    if(seminit(semid,&arg,1,mutexAcces)<0.)
        return -1;

    int pid1 = fork();
    int pid2 = fork();
    int id = (!pid1)|(!pid2)<<1;
	Id=id;

    int Did,Bid;

    switch(id){
        case 0:
            Bid = mutex30;
            Did = mutex01;
        break;
        case 1:
            Bid = mutex01;
            Did = mutex12;
        break;
        case 2:
            Bid = mutex12;
            Did = mutex23;
        break;
        case 3:
            Bid = mutex23;
            Did = mutex30;
        break;
        default:
            return -1;
    }
	printf("%d started...\n",id);

	int i,j;
	for(j=0;j<HEIGHT;j++){
		for(i=id;i<WIDTH;i+=4){
			vec *col = vec3(0,0,0);
			int k;
			for(k=0;k<SPP;k++){
				double x = (double)(2*i-WIDTH)+drand(),
				       y = -(double)(2*j-HEIGHT)+drand();

				double z = .5*(double)WIDTH;
				vec *d = vec3(x,y,z);
				normalize(d);
				vec* o = vec3(0,0,-.5);

				vec c = trace(o,d,DEPTH);
				add(col,&c);
			}
			sca(col,1./(double)SPP);
			saturate(col);
			gam(col);
			sca(col,(double)CDEPTH);
			#ifdef OUTPUT
            	semP(semid,Bid);
				semP(semid,mutexAcces);
				fprintf( inputFile,"%.0f %.0f %.0f\n",col->x,col->y,col->z);
				fflush( inputFile);
				semV(semid,mutexAcces);
				semV(semid,Did);
			#endif
			free(col);
		}
	}
	printf("%d ending...\n",id);
	if(pid2){
		printf("-2 : %d\n",id);
		kill(pid2,SIGUSR1);
		printf("+2 : %d\n",id);
		waitpid(pid2,NULL,0);
		printf("*2 : %d\n",id);
		if(pid1){
			printf("-1 : %d\n",id);
			kill(pid1,SIGUSR1);
			printf("+1 : %d\n",id);
			waitpid(pid1,NULL,0);
			printf("*1 : %d\n",id);
		}else{
			if(!sigR){
				printf("%d waiting...\n",id);
				pause();
			}
			printf("%d killed...\n",id);
		}
	}else{
		if(!sigR){
			printf("%d waiting...\n",id);
			pause();
		}
		printf("%d killed...\n",id);
	}
	
	if(!id){
		fclose(inputFile);
	}
	return 0;
}
