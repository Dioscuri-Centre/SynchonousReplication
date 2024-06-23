// one species with logistic death term

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;

const float dd=1,kk=100000;
const int noff=1 ; 
float bb=1.0 ; // birth rate (inverse of the doubling time)
float tmax=10000  ;
double dev=0.1 ; //0.1 ;


int mode=0 ; // 0 =exponential, 1=quasi-synchr
int outtype=0 ; // 0 = n,  1 = amplitude of oscillations

long long int xdr48=0x000100010001LL, mndr48=0x0005deece66dLL, doddr48=0xbLL ;
double _drand48(void)  // works only on compilers with long long int!
{
  xdr48=mndr48*xdr48+doddr48 ; xdr48&=0xffffffffffffLL ;
  return (xdr48/281474976710656.0) ;
}

void _srand48(unsigned int x0)
{
	xdr48=(x0<<16)+0x330e ; 
}

double rantime()
{
	if (mode==0) return -log(1-_drand48()) ;
	else return (1-dev+2*dev*_drand48())*0.693 ;  
  //if (drand48()<0.3) return 1-dev+2*dev*drand48() ;  else return -log(1-drand48()) ;
}


// arguments:  filename		mode	outtype	  samples	  bb
int main(int argn, char **argc)
{
	char *name ;
	int nsam ;
	int seed=1 ;
	if (argn==8) {
		name=argc[1] ;
	  if (argc[2][0]=='e') mode=0 ; 
	  if (argc[2][0]=='s') mode=1 ; 
	  cout <<"mode="<<mode<<endl;
		outtype=atoi(argc[3]) ; cout<<"outtype="<<outtype<<endl;
		nsam=atoi(argc[4]) ; cout<<"nsam="<<nsam<<endl;
		bb=atof(argc[5]) ; cout <<"bb="<<bb<<endl;
		dev=atof(argc[6]) ; cout<<"dev="<<dev<<endl ;
		seed=atoi(argc[7]) ; cout <<"seed="<<seed<<endl;
	} else {
	  cout<<"incorrect number of parameters"<<endl ;
	  exit(0);
	}
			
  _srand48(seed) ;
  int i,j,ntot;
  vector <float> cells ;

  ofstream data(name) ;

	for (int nnn=0;nnn<nsam;nnn++) {

		
		ntot=1 ; //0.2*kk ;
		cells.clear() ;
		for (i=0;i<ntot;i++) {
	  		cells.push_back(_drand48()) ;
	  	}
	  
		
		const int precision=1 ; 
	
	  double tt=0, ttt=0, dt=1./((1<<9)*precision) ;
	  for (i=0;;i++) {
	    for (j=0;j<cells.size();j++) {
	      cells[j]-=dt ; if (cells[j]<0) {
	        float ddt=cells[j] ;
	        float birth=bb*2./(1+noff) ;
					ntot+=noff ;
	        cells[j]=rantime()/birth+ddt ;
	        for (int k=0;k<noff;k++) { 
	        	cells.push_back(rantime()/birth+ddt) ; 
	        }
	      } 
	    }
	    
	    for (j=0;j<cells.size();j++) {
	    	float death=dd*ntot/kk ; 
	    	if (dt*death>1) cout<<"death_prob="<<dt*death<<endl;
	      if (_drand48()<dt*death) {
	        ntot-- ; 
	        if (j<cells.size()-1) {
	          cells[j]=cells[cells.size()-1] ;
	          j-- ; 
	        }
	        cells.pop_back() ;  
	      }
	    }
	    
	    ntot=cells.size() ;  
	    if (tt>tmax) break ;
	    
	    tt+=dt ; ttt+=dt ;
	    if (ttt>10) {
		  	ttt=0 ;
			  cout <<tt<<"\t"<<ntot<<endl ;  
			}
			  
	    if (outtype==0) {
	    	if (i%(32*precision)==0) data<<tt<<"\t"<<ntot<<endl ;  
			} else if (outtype==1 && tt>0.2*tmax && i%(128*precision)==0) {
			  data<<tt<<"\t"<<ntot<<endl ; 
			}
	  }  
	}
  
  data.close() ;
//  system("pause") ;
}

