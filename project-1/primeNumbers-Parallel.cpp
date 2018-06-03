#include <stdio.h>
#include <iostream>
#include <time.h>
#include <omp.h>

double parallel(int N, int threadCount, omp_sched_t schedule, int chunk){
	int i ;
	int j ;
	int k ;
	int n ;
	int quo, rem ;
	double t1, t2 ; 
	int prime[N] ;
	
	prime[0] = 2 ;
	n = 3 ;
	
	omp_set_schedule(schedule, chunk);
	
	t1 = omp_get_wtime() ; 
#pragma omp parallel for private(j,k,quo,rem) shared(prime,n) num_threads(threadCount)
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
	t2 = omp_get_wtime() ; 
	
	return t2-t1;
}

int main() {
	int i, M;
	int chunkCount = 100;
	double t1, t2, t4, t8, t12, s2, s4, s8 ;
	omp_sched_t schedule = omp_sched_auto ;
	FILE *fptr ;
   	
	omp_set_schedule(schedule, 0) ;
	
	/* open a file */
	fptr = fopen("output.csv", "w") ;
	if(fptr == NULL)
	{
      printf("File cannot open!\n");   
      exit(1);             
	}

	
	fprintf(fptr, "M,Scheduling,Chunk,T1,T2,T4,T8,T12,S2,S4,S8\n") ;
	printf("M,Scheduling,Chunk,T1,T2,T4,T8,T12,S2,S4,S8\n") ;
	for(i=0 ; i <= chunkCount ; i = i + 10) {
		M = 1000 ;
		t1 = parallel(M, 1, omp_sched_dynamic, i) ;
		t2 = parallel(M, 2, omp_sched_dynamic, i) ;
		t4 = parallel(M, 4, omp_sched_dynamic, i) ;
		t8 = parallel(M, 8, omp_sched_dynamic, i) ;
		t12 = parallel(M, 12, omp_sched_dynamic, i) ;
		s2 = 2 / (2 * t1 + (1 - t1)) ;
		s4 = 4 / (4 * t1 + (1 - t1)) ;
		s8 = 8 / (8 * t1 + (1 - t1)) ;
	
		fprintf(fptr, "%d,dynamic,%d,%f,%f,%f,%f,%f,%f,%f,%f\n", M, i, t1, t2, t4, t8, t12, s2, s4, s8) ;
		printf("%d,dynamic,%d,%f,%f,%f,%f,%f,%f,%f,%f\n", M, i, t1, t2, t4, t8, t12, s2, s4, s8) ;
	}
	for(i=0 ; i <= chunkCount ; i = i + 10) {
		M = 10000 ;
		t1 = parallel(M, 1, omp_sched_guided, i) ;
		t2 = parallel(M, 2, omp_sched_guided, i) ;
		t4 = parallel(M, 4, omp_sched_guided, i) ;
		t8 = parallel(M, 8, omp_sched_guided, i) ;
		t12 = parallel(M, 12, omp_sched_guided, i) ;
		s2 = 2 / (2 * t1 + (1 - t1)) ;
		s4 = 4 / (4 * t1 + (1 - t1)) ;
		s8 = 8 / (8 * t1 + (1 - t1)) ;
		
		fprintf(fptr, "%d,guided,%d,%f,%f,%f,%f,%f,%f,%f,%f\n", M, i, t1, t2, t4, t8, t12, s2, s4, s8) ;
		printf("%d,guided,%d,%f,%f,%f,%f,%f,%f,%f,%f\n", M, i, t1, t2, t4, t8, t12, s2, s4, s8) ;
	}
	for(i=0 ; i <= chunkCount ; i = i + 10) {
		M = 100000 ;
		t1 = parallel(M, 1, omp_sched_static, i) ;
		t2 = parallel(M, 2, omp_sched_static, i) ;
		t4 = parallel(M, 4, omp_sched_static, i) ;
		t8 = parallel(M, 8, omp_sched_static, i) ;
		t12 = parallel(M, 12, omp_sched_static, i) ;
		s2 = 2 / (2 * t1 + (1 - t1)) ;
		s4 = 4 / (4 * t1 + (1 - t1)) ;
		s8 = 8 / (8 * t1 + (1 - t1)) ;
		
		fprintf(fptr, "%d,static,%d,%f,%f,%f,%f,%f,%f,%f,%f\n", M, i, t1, t2, t4, t8, t12, s2, s4, s8) ;
		printf("%d,static,%d,%f,%f,%f,%f,%f,%f,%f,%f\n", M, i, t1, t2, t4, t8, t12, s2, s4, s8) ;
	}
	
	fclose(fptr) ;
}
