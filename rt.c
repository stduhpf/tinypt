#include "pt.c"

int main(){
	//printf("%f %f\n",drand(),drand());
	#ifdef OUTPUT
	printf("P3\n");
	printf("%d %d\n%d\n",WIDTH,HEIGHT,CDEPTH);
	#endif
	int i,j;
	for(j=0;j<HEIGHT;j++){
		for(i=0;i<WIDTH;i++){
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
			printf("%.0f %.0f %.0f\n",col->x,col->y,col->z);
			#endif
			free(col);
		}
	}
	return 0;
}
