#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

using namespace std;

const int ITERATIONS_COUNT = 10000;

void init_array(int* arr, int n) {
    for(int i = 0; i < n; ++i) {
        arr[i] = rand() % 1000 + 1;
    }
}

void sort(int* arr, int n) {
    for(int i = 0; i < n - 1; ++i) {
        for(int j = 0; j < n - i - 1; ++j) {
            if(arr[j] > arr[j+1]) {
                int buff = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = buff;
            }
        }
    }
}

void print_message(const string& message) {
    write(0, message.c_str(), message.size());
}

long get_duration(timespec start, timespec end) {
    return end.tv_nsec - start.tv_nsec;
}

//use size of array = 5 for most similar results
int main(int argc, char* argv[]) {
    if(argc != 2) {
        print_message("Invalid arguments\n");
        return 1;
    }
    int n = stoi(argv[1], nullptr, 10);
    int* arr = new int[n];
    init_array(arr, n);
    timespec start, end;
    long long sum_duration = 0;
    for(int i = 0; i < ITERATIONS_COUNT; ++i) {
        init_array(arr, n);
        clock_gettime(CLOCK_REALTIME, &start);
        sort(arr, n);
        clock_gettime(CLOCK_REALTIME, &end);
        sum_duration+= get_duration(start, end);
    }
    print_message("sorting time: " + to_string(sum_duration/(long double)ITERATIONS_COUNT)+" ns\n");
    sum_duration = 0;
    for(int i = 0; i < ITERATIONS_COUNT; ++i) {
        clock_gettime(CLOCK_REALTIME, &start);
        getpid();
        clock_gettime(CLOCK_REALTIME, &end);
        sum_duration+= get_duration(start, end);
    }
    print_message("getpid() time: " + to_string(sum_duration/(long double)ITERATIONS_COUNT)+" ns\n");
}