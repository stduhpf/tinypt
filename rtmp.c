#include "pt.c"


#include <unistd.h>

#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

#include "semaphore.h"

#include <time.h>

/******************************************************************************/
//#define DEBUG
#define ETA
#define UP 15

#define N 10
#define N2 500

int sigR = 0;
int Id;

void handler(int signo){
	#ifdef DEBUG
	printf("signal requested %d\n",Id);
	#endif
	sigR = 1;
}

#define P 3 //on cree 2^P porcessus

#define mutex nproc
/******************************************************************************/
int main(){
	int tim = time(NULL);
	signal(SIGUSR1,handler);

	FILE * inputFile = fopen( "out2.ppm", "w" );

	#ifdef OUTPUT
	fprintf( inputFile,"P3\n");
	fprintf( inputFile,"%d %d\n%d\n",WIDTH,HEIGHT,CDEPTH);
	fflush( inputFile);
	#endif

	const int nproc = 0x01<<P; //2^P: nombre total de procesus
    key_t cle =  ftok("/tmp", '1');
    int semid = semget(cle,nproc+1,IPC_CREAT|0666);
    if(semid<0){
        printf("Erreur creation Semaphore: %s \n", strerror( errno ));
        return -1;
    }
    
    union semun arg;
	{
	int i;
	for(i=0;i<nproc+1;i++){
		#ifdef DEBUG
		printf("init sem %d ...\n",i);
		#endif
		if(seminit(semid,&arg,i==0||i==mutex,i)<0.)
			return -1;
	}}


	int pid[P];
	int id = 0;
	{
	int i;
	for(i=0;i<P;i++){
		pid[i]=fork();
    	id = id|(!pid[i])<<i;
		}
	}
	Id=id;

    int Did = (id+1)%nproc,
		Bid = id;
	#ifdef DEBUG
	printf("%d started..., %d %d\n",id,Did,Bid);
	#endif
	int i,j;
	for(j=0;j<HEIGHT;j++){
		#ifdef ETA
		if(!id && !(j%UP)){
			int dt = time(NULL)-tim;
			int eta = (uint)((float)(dt*HEIGHT)/(float)j);
			int rem = eta-dt;
			printf("Completion: %.2f%% (line %d /%d) 	elapsed: %dh %02dm %02ds 	ETA: %dh %02dm %02ds 	remaining: %dh %02dm %02ds\n",100.*(float)j/(float)HEIGHT,j,HEIGHT,dt/3600,(dt/60)%60,dt%60,eta/3600,(eta/60)%60,eta%6,rem/3600,(rem/60)%60,rem%60);
		}
		#endif
		for(i=id;i<WIDTH;i+=nproc){
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
				semP(semid,mutex);
				fprintf( inputFile,"%.0f %.0f %.0f\n",col->x,col->y,col->z);
				fflush( inputFile);
				semV(semid,mutex);
				semV(semid,Did);
			#endif
			free(col);
		}
	}
	semP(semid,Bid);
	semV(semid,Did);
	#ifdef DEBUG
	printf("%d ending...\n",id);
	#endif
	{
	int i;
	for(i=P-1;i>=0;i--){
		if(pid[i]){
			kill(pid[i],SIGUSR1);
			waitpid(pid[i],NULL,0);
		}else{
			
			#ifdef DEBUG
			if(!sigR)
				printf("%d waiting...\n",id);
			#endif
			while(!sigR){
			}
			#ifdef DEBUG
			printf("%d killed...\n",id);
			#endif
			break;
		}
	}}

	if(!id){
		fclose(inputFile);
		if(semctl(semid,0,IPC_RMID)<0)
			printf("Erreur destruction du semaphore : %x\n", strerror( errno ));
	}
	return 0;
}
