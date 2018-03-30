#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <cmath>
#include <omp.h>
#define N 25
int prime[N] ; // stores the first 50 primes
int primes[N] ; // stores the first 50 primes

/*
void loop(int *primes, int len){
	for(int k=1 ; k < len ; k++) {
		quo = n / primes[k] ;
		rem = n % primes[k] ;
		if (rem == 0) {
			n = n + 2 ;
			k=1 ;
		}
		else if (quo <= primes[k]) 
			break;
	}
}
*/

int main() {
	int j ;
	int k ;
	int n ;
	int quo,rem ;
P1: prime[0] = 2 ;
	n = 3 ;
	j = 0 ;
P2: j =j+1;
	prime[j] = n ;
P3: if (j == (N-1)) 
		goto P9 ;
P4: n=n+2;
P5: k=1;
P6: quo = n / prime[k] ;
	rem = n % prime[k] ;
	if (rem == 0) 
		goto P4 ;
P7: if (quo <= prime[k]) 
		goto P2 ; 
P8: k=k+1;
	goto P6 ;
P9: printf("prime: \n") ;
	for(j=0 ; j < N ; j++) 
		printf("%d\n",prime[j]) ;
		
	
	
	primes[0] = 2 ;
	n = 3 ;
	for(j=1 ; j < N ; j++) {
		primes[j] = n ;
		n = n + 2 ;
		for(k=1 ; k < N ; k++) {
			quo = n / primes[k] ;
			rem = n % primes[k] ;
			if (rem == 0) {
				n = n + 2 ;
				k = 0 ; 
			}
			else if (quo <= primes[k])
				break;
		}
	}
	
	printf("primes: \n") ;
	for(j=0 ; j < N ; j++) 
		printf("%d\n",primes[j]) ;
		
}
