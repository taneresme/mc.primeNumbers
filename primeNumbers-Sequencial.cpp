#include <stdio.h>
#define N 25
int prime[N] ; // stores the first 50 primes

int main() {
	int j ;
	int k ;
	int n ;
	int quo, rem ;
	prime[0] = 2 ;
	n = 3 ;
	for(j=1 ; j < N ; j++) {
		prime[j] = n ;
		n = n + 2 ;
		for(k=1 ; k < N ; k++) {
			quo = n / prime[k] ;
			rem = n % prime[k] ;
			if (rem == 0) {
				n = n + 2 ;
				k = 0 ; 
			}
			else if (quo <= prime[k])
				break;
		}
	}
	
	printf("primes: \n") ;
	for(j=0 ; j < N ; j++) 
		printf("%d\n",prime[j]) ;
		
}
