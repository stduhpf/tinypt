#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 1920
#define HEIGHT 1080
#define CDEPTH 255

double min(double a, double b){
	return a<b?a:b;
}
double max(double a, double b){
	return a>b?a:b;
}

double clamp(double x, double d, double f){
	return min(max(x,d),f);
}


/***************************************************************************/
typedef struct {
	double x;
	double y;
	double z;
}vec;

vec* vec3(double x,double y,double z){
	vec* v = malloc(sizeof(vec));
	v->x = x;
	v->y = y;
	v->z = z;
	return v;
}

vec* vdup(vec* a){
	vec* v = malloc(sizeof(vec));
	v->x=a->x;
	v->y=a->y;
	v->z=a->z;
	return v;
}

double dot(vec *v1, vec *v2){
	return v1->x*v2->x+v1->y*v2->y+v1->z*v2->z;
}

double length(vec *v){
	return sqrt(dot(v,v));
}

void sca(vec* v, double a){
	v->x*=a;
	v->y*=a;
	v->z*=a;
}

void mul(vec* v1, vec* v2){
	v1->x*=v2->x;
	v1->y*=v2->y;
	v1->z*=v2->z;
}

void cross(vec* v1, vec* v2){
	double x,y,z;
	x=v1->y*v2->z-v1->z*v2->y;
	y=v1->z*v2->x-v1->x*v2->z;
	z=v1->x*v2->y-v1->y*v2->x;
	v1->x=x,
	v1->y=y,
	v1->z=z;
}

void off(vec* v, double b){
	v->x+=b;
	v->y+=b;
	v->z+=b;
}

void add(vec* v1, vec* v2){
	v1->x+=v2->x;
	v1->y+=v2->y;
	v1->z+=v2->z;
}

void sub(vec* v1, vec* v2){
	v1->x-=v2->x;
	v1->y-=v2->y;
	v1->z-=v2->z;
}

void mad(vec* v1, double k, vec v2){
	v1->x = (k * v1->x) + v2.x;
	v1->y = (k * v1->y) + v2.y;
	v1->z = (k * v1->z) + v2.z;
}

void mad2(vec* v2, vec *v1, double k){
	v2->x = (k * v1->x) + v2->x;
	v2->y = (k * v1->y) + v2->y;
	v2->z = (k * v1->z) + v2->z;
}

void normalize(vec* v){
	double l = length(v);
	sca(v,1./l);
}
/*
vec* getdir(vec* v){
	vec* v2 = vdup(v);
	normalize(v2);
	return v2;
}
*/
void saturate(vec *v){
	v->x = clamp(v->x,0.,1.);
	v->y = clamp(v->y,0.,1.);
	v->z = clamp(v->z,0.,1.);
}

/************************************************************************/
typedef struct {
	vec cen;
	vec col;
	double rad;
	double ir;
	double rough;
	double kt;
	double emission;
}sphere;

//                  cen				col			rad		ir	rough	kt	emission
sphere sph[] = {	0.,2.,2.,	 	1.,1.,1.,	.9,		2.,	.1,		0.,	10.,
					1.,0.,1.9,	 	1.,1.,1.,	.9,		2.,	.1,		0.,	0.,
					0.,-1002.,4.,	.4,.5,.6, 	1000.,	2.,	.1,		0.,	0.,
					0.,1002.,4.,	.4,.5,.6, 	1000.,	2.,	.1,		0.,	0.,
					-1002.,0.,0.,	.8,.1,.1,	1000.,	2.,	.1,		0.,	0.,
					1002.,0.,0.,	.1,.8,.1,	1000.,	2.,	.1,		0.,	0.,
					0.,0.,1006.,	1.,1.,1.,	1000.,	2.,	.1,		0.,	0.};

const int nSphere = sizeof(sph)/sizeof(sphere);
/************************************************************************/
typedef struct{
	double  d;
	sphere *s;
	vec n;
} hitresult;

double sign(double a){
	return (a<0.)?-1.:
			(a>0.?1.:0.);
}

hitresult intersect(vec *O, vec *D){
	sphere *close = sph;
	vec n ;
	double dmin = 1e30;
	sphere *s = sph + nSphere;
	while(s-->sph){
		vec *P = vdup(O);
		sub(P,&(s->cen));
		double demi_b  = dot(D,P);
		double moins_c = (s->rad)*(s->rad)-dot(P,P);
		double quart_delta = demi_b*demi_b + moins_c;

		double d =(moins_c>0. || demi_b<0. && quart_delta>0.)?-demi_b+sign(moins_c)*sqrt(quart_delta):-1.;
		vec * point = vdup(D);
		mad(point,d,*P);
		if(d>0 && d<dmin ){
			close = s;
			n = *point;
			sca(&n,1./s->rad);
			dmin = d;
		}
		free(P);
		free(point);
	}
	hitresult hit;
	hit.d = dmin;
	hit.s = close;
	hit.n = n;
	return hit;
}

double drand(){
	return (double)rand()/(double)RAND_MAX;
}

vec* randomdir(vec* n){
	/*
	vec* uu = vec3(1,0,0);
	mad2(uu,n,n->x);
	normalize(uu);
	vec* vv = vdup(uu);
	cross(vv,n);
	float r= drand();
	float t = drand()*6.28318530717958647692528676655900576839433879875021164194988918461563281257241799725606965068423413596429617302656;
	vec* d =vdup(n);
	sca(d,sqrt(1.-r));
	r=sqrt(r);
	mad2(d,uu,r*cos(t));
	mad2(d,vv,r*sin(t));
	free(uu);
	free(vv);
	return d;
	*/
	return vec3(drand()*2.-1.,drand()*2.-1.,drand()*2.-1.);
}

#define DEPTH 8

vec trace(vec* O, vec* D, int depth){
	if(depth == 0){
		vec* a = vec3(0,0,0);
		vec b =*a;
		free(a);

		free(O);
		free(D);

		return b;
	}
	hitresult hit = intersect(O,D);
	if(hit.d>=1e30){
		vec* a = vec3(.1,.12,.2);
		vec b =*a;
		free(a);

		free(O);
		free(D);

		return b;
	}
	vec albedo = (hit.s->col);

	vec* P = vdup(D);
	mad(P,hit.d,*O);
	mad2(P,&hit.n,1e-15);

	vec *L = randomdir(&hit.n);
	

	double ndotl = dot(&hit.n,L);

	sca(L,sign(ndotl));

	double intensity = max(ndotl,0.);

	free(O);
	free(D);

	vec light = trace(P,L,depth-1);
	sca(&light,intensity);
	off(&light, hit.s->emission);
	mul(&albedo,&light);
	return albedo;
}

void gam(vec *c){
	c->x = sqrt(c->x);
	c->y = sqrt(c->y);
	c->z = sqrt(c->z);
}

#define SPP 256
#define OUTPUT 