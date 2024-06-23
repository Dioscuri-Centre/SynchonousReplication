// two species
// rates 
// d_0 = p2 - p1*n_1/K
// d_1 = p4*n_2/K - p3*(1-n_1/K)
// in addition to this, there is reproduction with rate bb

/*{f1'[t] == 2 p1 f1[t] f2[t] - μ f1[t], 
 f2'[t] == r f2[t] (1 - f2[t]/k) - 2 (p1 + p2 + b) f1[t] f2[t], 
 f1[0] == 0.25, f2[0] == 0.25}
*/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;

//const float p1=30, p2=7, p3=1, p4=10, bb=1.0, kk=10000, tmax=300 ;
//const float p1=30, p2=7, p3=7, p4=10, kk=100000;
const float p[4]={39.7284, 20.8642, 2., 4.}, kk = 100000, dev=0.05 ;	// set S0.5
//float p[4]={56.45, 29.23, 0.8, 2.8}, kk = 100000, dev=0.04 ; // set S0.2
const int noff=1 ; 
float bb=1.0 ; // birth rate (inverse of the doubling time)
float tmax=50000  ;

int mode=0 ; // 0 =exponential, 1=quasi-synchr
int outtype=0 ; // 0 = nA,nB,  1 = amplitude of oscillations

struct Cell {
  float t ;
  int type ;
};

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
  const double dev=0.02 ; //0.1 ;
	if (mode==0) return -log(1-_drand48()) ;
	else return (1-dev+2*dev*_drand48())*0.693 ;  
  //if (_drand48()<0.3) return 1-dev+2*dev*_drand48() ;  else return -log(1-_drand48()) ;
}


// arguments:  filename		mode	outtype	  samples	  bb
int main(int argn, char **argc)
{
	char *name ;
	int nsam ;
	if (argn==6) {
		name=argc[1] ;
	  if (argc[2][0]=='e') mode=0 ; 
	  if (argc[2][0]=='s') mode=1 ; 
	  cout <<"mode="<<mode<<endl;
		outtype=atoi(argc[3]) ; cout<<"outtype="<<outtype<<endl;
		nsam=atoi(argc[4]) ; cout<<"nsam="<<nsam<<endl;
		bb=atof(argc[5]) ; cout <<"bb="<<bb<<endl;
	} else {
	  cout<<"incorrect number of parameters"<<endl ;
	  exit(0);
	}
			
  _srand48(10) ;
  int i,j,ntot;
  vector <Cell> cells ;
  Cell c ; 

  ofstream data(name) ;

	for (int nnn=0;nnn<nsam;nnn++) {

		long long int nA=0.2*kk, nB=0.2*kk;
		for (i=0;i<nA;i++) {
			c.t=_drand48() ; //rantime() ; 
			c.type=0 ;
	  	cells.push_back(c) ;
	  }
		for (i=0;i<nB;i++) {
			c.t=_drand48() ; //rantime() ; 
			c.type=1 ;
	  	cells.push_back(c) ;
	  }
	  //cout <<cells.size()<<endl ;
	  
		
	
	  double tt=0, ttt=0, dt=1./(1<<9) ; ;
	  for (i=0;;i++) {
	    for (j=0;j<cells.size();j++) {
	      cells[j].t-=dt ; if (cells[j].t<0) {
	        float ddt=cells[j].t ;
	        float birth=bb*2./(1+noff) ;
					if (cells[j].type==0) {	// type A reproduces
	        	//birth=p1*1.*nB/kk  //*(1-1.*nA/kk) ;        
	        	//birth=p1*(1-1.*nA/kk) ; 
	        	nA+=noff ;
	        } else {		// type B reproduces
						//birth=p3*(1-1.*nB/kk) ;
						nB+=noff;
	        }
	        //if (birth<1e-3) birth=1e-3 ;
	        cells[j].t=rantime()/birth+ddt ;
	        for (int k=0;k<noff;k++) { 
						c.t=rantime()/birth+ddt ; c.type=cells[j].type ;
	        	cells.push_back(c) ; 
	        }
	      } 
	    }
	    
	    for (j=0;j<cells.size();j++) {
	    	float death ;
	    	if (cells[j].type==0) {
					death = p[1] -  p[0]*1.*nB/kk; 
					//death=1.*nA/kk ; 
					if (nA<2) { death=0;  }
	    	} else {
					death = p[3]*1.*nA/kk  - p[2]*(1-1.*nB/kk);
					//death=1.*nB/kk;
					if (nB<2) { death=0;  }
	    	}
	    	if (dt*death>1) cout<<"death_prob="<<dt*death<<endl;
	      if (_drand48()<dt*death) {
	        if (cells[j].type==0) nA--; else nB--; 
	        if (j<cells.size()-1) {
	          cells[j]=cells[cells.size()-1] ;
	          j-- ; 
	        }
	        cells.pop_back() ;  
	      }
	    }
	    
	    ntot=cells.size() ;  
	    if (tt>tmax) break ;
	    if (ntot!=nA+nB) cout <<nA+nB-ntot<<endl ; //cout<<"error "<<ntot<<" nA="<<nA<<" nB="<<nB<<endl ;
	    
	//    for (j=0;j<cells.size();j++) {
	    
	//    }
	    	    
	    tt+=dt ; ttt+=dt ;
	    if (ttt>100) {
		  	ttt=0 ;
			  cout <<tt<<"\t"<<nA<<"\t"<<nB<<endl ;  
			}
			  
	    if (outtype==0) {
	    	if (i%16==0) data<<tt<<"\t"<<nA<<"\t"<<nB<<endl ;  
			} else if (outtype==1 && tt>0.2*tmax && i%128==0) {
			  data<<tt<<"\t"<<nA<<"\t"<<nB<<endl ; 
			}
	  }  
	}
  
  data.close() ;
//  system("pause") ;
}
