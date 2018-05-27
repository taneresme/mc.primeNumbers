#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/execution_policy.h>
#include <iostream>
#include <omp.h>
#include <fstream>
 
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

struct greater_than_two
{
	__host__ __device__
	bool operator()(const int& item) const {
		return item >= 2;
	}
};

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

double findPrimes(int N, int flag){
	int _sqrt = (int)sqrt(N);
	int i;
	
	double start = omp_get_wtime();
		
	//SERIAL SECTION
	std::vector<int> primes;
	primes.push_back(2);
	for(i = 3; i <= _sqrt; i+=2){
		for(int j : primes){
			if( i % j == 0 ) break;
			if( i / j < j ){ 
				primes.push_back(i); 
				break; 
			}
		}
	}
	_sqrt = primes.size(); 
		
	//PARALLEL SECTION
	thrust::host_vector<int> h_primes(N);
	thrust::copy(primes.begin(), primes.end(), h_primes.begin());
	if (flag == 1) hRun(i, N, _sqrt, primes, h_primes);
	else dRun(i, N, _sqrt, primes, h_primes);
	
	double duration = omp_get_wtime() - start;
	
	//thrust::copy_if(vPrimes.begin(), vPrimes.end(), std::ostream_iterator<int>(std::cout, "\n"), greater_than_two());
	
	return duration;
};

int main(int argc, char *argv[])
{	
	std::cout << "Press any button to continue...";
	std::cin.get();
	
	int m[] = {2000, 10000, 100000, 1000000, 100000000};
	int const m_count = 5;
	
	//OUTPUT FILE CREATION
	std::ofstream ofs;
	ofs.open("results.csv");
	ofs << "M,Host-Duration,Device-Duration\n";
	
	for (int i = 0; i < m_count; i++){
		double h_duration = findPrimes(m[i], 1);
		double d_duration = findPrimes(m[i], 0);
		ofs << m[i] << "," << h_duration << "," << d_duration << "\n";
		
		std::cout << "h_duration[" << i << "]: " << h_duration << "\n";
	}
			
	return 0;
}
