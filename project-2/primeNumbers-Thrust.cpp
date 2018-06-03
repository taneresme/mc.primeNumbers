#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/execution_policy.h>
#include <iostream>
#include <fstream>
#include <time.h>
 
 /* (from first project) HOW TO USE AND WHY:
 * So, what is going on here? First, note that we're trying to find prime numbers up to N.
 * This means that our "output" will be not N but around ln(N) numbers. We will find those
 * by comparing each number to previously found primes. "But wait a second! Then this
 * has to be a _sequential_ process, doesn't it?" Well, yes, but we know that any composite
 * number K has a factor up to sqrt(K), and since _every_ number in consideration is bound
 * by N, we know that checking primes up to sqrt(N) will suffice. So idea is: sequentially
 * construct primes up to sqrt(N), then check greater numbers in parallel with those.
 * 
 * This comes with some restrictions. First, these methods lose a lot when attempted to
 * generalize. For example, we cannot start from scratch and determine primes between 2
 * numbers. Second, we should not compare against an _unordered_ list. Third, our final
 * list possibly is hard to manage, that is, of course, if we desire an ordered list. One
 * possible solution is that we can get sublists from each parallel subroutine and combine
 * them as desired, but this necessitates synchronization. Or we can get lazy and order
 * the list after the fact. Also, different parallelization methods can yield unwieldly
 * sublists.
 * 
 * Therefore our expectation from the user is exactly as the project describes: provide
 * the program with a number and be done with it. No generalizations, no further use, a very
 * simple and honest transaction.
 */
 
/* Functor to find prime numbers */
struct prime_functor : public thrust::binary_function<int,int,int>
{
 	int *primes;
 	
 	prime_functor(int *_primes) : primes(_primes) {}

	__host__ __device__
	int operator()(const int& indis, const int& prime) const {
		int len = prime;
		for(int k = 0; k <= len; k++) {
			if (k == len) return indis;
			if (indis % (primes[k]) == 0) break;
			if ((indis / primes[k]) < primes[k] ) return indis;
		}
		return -1;
	}
};

/* To determine whether an element of array is prime or not */
struct greater_than_two
{
	__host__ __device__
	bool operator()(const int& item) const {
		return item >= 2;
	}
};

/* Runs the thrust iterations on host */
void hRun(int i, int N, int _sqrt, std::vector<int> primes, thrust::host_vector<int> h_primes){
	thrust::transform(thrust::host, 
					  thrust::make_counting_iterator(i),
					  thrust::make_counting_iterator(N),
					  thrust::make_constant_iterator(_sqrt),
					  h_primes.begin() + i,
					  prime_functor(thrust::raw_pointer_cast(&(primes[0])))
					  );
	
	thrust::sort(h_primes.begin(), h_primes.end());
	
};

/* Runs the thrust iterations on device */
void dRun(int i, int N, int _sqrt, std::vector<int> primes, thrust::host_vector<int> h_primes){
	thrust::device_vector<int> d_primes = h_primes;
	
	thrust::transform(thrust::device, 
					  thrust::make_counting_iterator(i),
					  thrust::make_counting_iterator(N),
					  thrust::make_constant_iterator(_sqrt),
					  d_primes.begin() + i,
					  prime_functor(thrust::raw_pointer_cast(&(primes[0])))
					  );
	
	thrust::sort(d_primes.begin(), d_primes.end());
	
};

/* finds the prime number till to N provided*/
double findPrimes(int N, int flag){
	try{
		
		int _sqrt = (int)sqrt(N);
		int i;
		
	    clock_t start = clock();
			
		//SERIAL SECTION
		std::vector<int> primes;
		primes.push_back(2);
		for(i = 3; i <= _sqrt; i+=2){
			for (int j = 0; j < primes.size(); j++){
				if( i % primes[j] == 0 ) break;
				if( i / primes[j] < primes[j] ){ 
					primes.push_back(i); 
					break; 
				}
			}
		/*	for(int j : primes){
				if( i % j == 0 ) break;
				if( i / j < j ){ 
					primes.push_back(i); 
					break; 
				}
			}*/
		}
		_sqrt = primes.size(); 
			
		//PARALLEL SECTION
		thrust::host_vector<int> h_primes(N);
		thrust::copy(primes.begin(), primes.end(), h_primes.begin());
		if (flag == 1) hRun(i, N, _sqrt, primes, h_primes);
		else dRun(i, N, _sqrt, primes, h_primes);
		cudaThreadSynchronize(); // block until kernel is finished
		
	    double duration = ((double) (clock() - start)) / CLOCKS_PER_SEC;
		
		//thrust::copy_if(vPrimes.begin(), vPrimes.end(), std::ostream_iterator<int>(std::cout, "\n"), greater_than_two());
		
		return duration;
	}
	catch (int e){
		std::cout << "exception e: " << e << "\n";
		return -1;
	}
};

/* Entry point for application */
int main(int argc, char *argv[])
{	
	std::cout << "Press any button to continue...";
	std::cin.get();
	
	/* Inputs to find primes */
	int m[] = {100000, 1000000, 5000000, 10000000, 50000000};
	int const m_count = sizeof(m) / sizeof(int);
	
	//OUTPUT FILE CREATION
	std::ofstream ofs;
	ofs.open("results.csv");
	ofs << "M,Host-Duration,Device-Duration\n";
	
	/* Measure the durations of host and device for each M */
	for (int i = 0; i < m_count; i++){
		double h_duration = findPrimes(m[i], 1); // host
		double d_duration = findPrimes(m[i], 0); // device
		
		ofs << m[i] << "," << h_duration << "," << d_duration << "\n";
		
		std::cout << "h_duration[" << i << "]: " << h_duration << "\n";
	}
			
	return 0;
}
