#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std::chrono;

void CompareForAndCilk_For(size_t sz)
{
	std::vector<int> my_vect;
	cilk::reducer<cilk::op_vector<int>>red_vec;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 0; i < sz; i++)
		my_vect.push_back(rand() % 20000 + 1);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	cilk_for(int i = 0; i < sz; i++)
		red_vec->push_back(rand() % 20000 + 1);
	high_resolution_clock::time_point t3 = high_resolution_clock::now();
	duration<double> duration_for = (t2 - t1);
	duration<double> duration_cilk = (t3 - t2);

	std::cout << std::endl;
	std::cout << "-------------Size of massive: " << sz << std::endl;
	std::cout << "Time for 'for': " << duration_for.count() << " seconds" << std::endl;
	std::cout << "Time for 'cilk_for': " << duration_cilk.count() << " seconds" << std::endl;
}

int main()
{
	srand((unsigned)time(0));

	// устанавливаем количество работающих потоков = 4
	__cilkrts_set_param("nworkers", "4");

	long i;
	const long mass_size = 10000;

	long sizes[] = { 10, 50, 100, 500, 1000, 10000, 100000, 500000, 1000000 };
	int number_size = sizeof(sizes) / sizeof(long);

	for (int jj = 0; jj < number_size; jj++)
	{
		CompareForAndCilk_For(sizes[jj]);
	}
	return 0;
}