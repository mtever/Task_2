#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>
#include <iostream>

using namespace std::chrono;

/// ‘ункци€ ReducerMaxTest() определ€ет максимальный элемент массива,
/// переданного ей в качестве аргумента, и его позицию
/// mass_pointer - указатель исходный массив целых чисел
/// size - количество элементов в массиве
void ReducerMaxTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
	cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("Maximal element = %d has index = %d\n",
		maximum->get_reference(), maximum->get_index_reference());
}

void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("Minimal element = %d has index = %d\n",
		minimum->get_reference(), minimum->get_index_reference());
}

/// ‘ункци€ ParallelSort() сортирует массив в пор€дке возрастани€
/// begin - указатель на первый элемент исходного массива
/// end - указатель на последний элемент исходного массива
duration <double> ParallelSort(int *begin, int *end)
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	if (begin != end)
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle);
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);
	return duration;
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
		std::cout << "-------------Size of massive: " << sizes[jj] << std::endl;
		int *mass_begin, *mass_end;
		int *mass = new int[sizes[jj]];

		duration <double> duration;

		for (i = 0; i < sizes[jj]; ++i)
		{
			mass[i] = (rand() % 25000) + 1;
		}

		mass_begin = mass;
		mass_end = mass_begin + sizes[jj];

		//printf("Before sort:\n");
		ReducerMaxTest(mass, sizes[jj]);
		ReducerMinTest(mass, sizes[jj]);

		//printf("After sort:\n");

		duration = ParallelSort(mass_begin, mass_end);
		std::cout << "-----Time sort: " << duration.count() << " seconds" << std::endl;
		ReducerMaxTest(mass, sizes[jj]);
		ReducerMinTest(mass, sizes[jj]);

		delete[]mass;
	}

	return 0;
}