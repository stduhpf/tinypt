#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main(int argc, char * argv[]){
	if(argc < 2 || argc > 3)
		return -1;
	char nomin[20],nomout[20] = "out.png";
	strcpy(nomin,argv[1]);
	if(argc==3)
		strcpy(nomout,argv[2]);
	printf("input  file : %s\n",nomin );
	printf("output file : %s\n",nomout);
	FILE* input = fopen(nomin,"r");
	if(input == NULL){
		printf("Erreur ouverture fichier %s\n",nomin);
		return -1;
	}
	FILE* output= fopen(nomout,"w");
	if(output == NULL){
		printf("Erreur ouverture fichier %s\n",nomout);
		return -1;
	}
#define LINE 100
#define read_f fgets(buff,LINE,input)
	char buff[LINE];
	read_f;
	printf("%s",buff);
	char bufff[LINE] = {'P','3'};
	if(strncmp(buff,bufff,2)){
		printf("Bad format!\n");
		return -1;
	}
	//get dimensions
	read_f;
	bufff[0]=0;
	strncat(bufff,buff,4);
	int width = atoi(bufff);
	bufff[0]=0;
	strncat(bufff,buff+4,4);
	int height = atoi(bufff);
	printf("w:%d  h:%d\n",width,height);

	//get color format
	read_f;
	int cdepth = atoi(buff);

	//metadata is read
	//now we're pointing to the actual data
}
