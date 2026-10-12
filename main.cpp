#include <iostream>
#include <Windows.h>
#include <fstream>
#include <ctime>
#include <cstdlib>  
#include <climits> 
#include<semaphore>

using namespace std;

int tasks, workers, timeLimit;
int* durations;
int** ability;
int bestDiff = INT_MAX;
int maxAnswer = INT_MIN;
int minAnswer = INT_MAX;
int ThreadId = -1;
int id = 0;
int* bestAssignments;
// Suggested alternative (not applied): declare int* bestsum = nullptr; here, then allocate after reading workers.
int* bestsum= new int[workers];
int numcore;


HANDLE ParentWakeup, Writer, Reader, Queue, Finish, child1;
int read_count = 0;
int finishedThreadsCount = 0;

DWORD WINAPI Child(LPVOID Param) {


	WaitForSingleObject(Finish, INFINITE);
	int tid = id++;
	cout << "tid = " <<tid<< endl;
	ReleaseSemaphore(Finish, 1, NULL);


	srand(static_cast<unsigned int>(time(0)) + tid);
	int diff;
	int worker;
	int localBest = INT_MAX;

	time_t t = time(NULL);
	while (time(NULL) - t < timeLimit) {
		int* sum = new int[workers];
		int* assign_worker = new int[tasks];

		for (int i = 0; i < workers; i++) {
			sum[i] = 0;
		}
		int max_value = INT_MIN;
		int min_value = INT_MAX;
		for (int i = 0; i < tasks; i++) {
			do {
				worker = rand() % workers;

			} while (ability[i][worker] == 0);

			sum[worker] += durations[i];
			assign_worker[i] = worker;

		}
		int max = INT_MIN;
		int min = INT_MAX;
		for (int i = 0; i < workers; i++) {
			if (sum[i] > max) max = sum[i];
			if (sum[i] < min) min = sum[i];
		}
		diff = max - min;
		max_value = max;
		min_value = min;

		if (diff >= localBest) continue;


		WaitForSingleObject(Queue, INFINITE);
		WaitForSingleObject(Reader, INFINITE);
		if (read_count == 0) {
			WaitForSingleObject(Writer, INFINITE);
		}
		read_count++;
		ReleaseSemaphore(Queue, 1, NULL);
		ReleaseSemaphore(Reader, 1, NULL);

		localBest = bestDiff;

		WaitForSingleObject(Reader, INFINITE);
		read_count--;
		if (read_count == 0) {
			ReleaseSemaphore(Writer, 1, NULL);
		}
		ReleaseSemaphore(Reader, 1, NULL);


		if (diff < localBest) {
			WaitForSingleObject(Queue, INFINITE);
			WaitForSingleObject(Writer, INFINITE);
			ReleaseSemaphore(Queue, 1, NULL);

			if (diff < bestDiff) {
				bestDiff = diff;
				maxAnswer = max_value;
				minAnswer = min_value;
				ThreadId = tid;


				for (int i = 0; i < tasks; i++) {
					bestAssignments[i] = assign_worker[i];
				}
				// Suggested alternative (not applied): for (int i = 0; i < workers; i++) bestsum[i] = sum[i];
				for (int i = 0; i < tasks; i++) {
					bestsum[i] = sum[i];
				}

				ReleaseSemaphore(ParentWakeup, 1, NULL);
				WaitForSingleObject(child1, INFINITE);

			}
			ReleaseSemaphore(Writer, 1, NULL);
		}
	}


	WaitForSingleObject(Finish, INFINITE);
	finishedThreadsCount++;
	if (finishedThreadsCount == numcore) {
		ReleaseSemaphore(ParentWakeup, 1, NULL);
	}
	ReleaseSemaphore(Finish, 1, NULL);
	return 0;
}
int main() {

	cout << "Enter time limit: ";
	cin >> timeLimit;

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	numcore = sysinfo.dwNumberOfProcessors;

	ifstream file("input.txt");
	if (!file) {
		cout << "Can not open file!" << endl;
		return 1;
	}
	file >> tasks >> workers;

	durations = new int[tasks];
	for (int i = 0; i < tasks; i++) {
		file >> durations[i];
	}

	ability = new int* [tasks];
	for (int i = 0; i < tasks; i++) {
		ability[i] = new int[workers];
		for (int j = 0; j < workers; j++) {
			file >> ability[i][j];
		}
	}
	file.close();


	ParentWakeup = CreateSemaphore(NULL, 0, numcore, NULL);
	Reader = CreateSemaphore(NULL, 1, numcore, NULL);
	Writer = CreateSemaphore(NULL, 1, numcore, NULL);
	Queue = CreateSemaphore(NULL, 1, numcore, NULL);
	Finish = CreateSemaphore(NULL, 1, numcore, NULL);
	child1 = CreateSemaphore(NULL, 0, numcore, NULL);


	HANDLE* thread = new HANDLE[numcore];
	bestAssignments = new int[tasks];
	// Suggested alternative (not applied): bestsum = new int[workers]; after input is read.

	for (int i = 0; i < numcore; i++) {
		thread[i] = CreateThread(NULL, 0, Child, NULL, 0, NULL);
	}

	while (true) {
		WaitForSingleObject(ParentWakeup, INFINITE);
		if (finishedThreadsCount == numcore)
		{break;
		}
		cout << "Thread ID: " << ThreadId << endl;

		for (int i = 0; i < tasks; i++) {
			cout << "Work " << i + 1 << " To Worker " << bestAssignments[i] + 1 << endl;
		}
		cout << endl;
		for (int i = 0; i < workers; i++) {
			cout << "Worker " << i + 1 << " = " << bestsum[i] << endl;
		}
		cout << endl;
		cout << "\tmax = " << maxAnswer << endl;
		cout << "\tmin = " << minAnswer << endl;
		cout << "------------------" << endl;
		cout << "max - min = " << bestDiff << endl;
		cout << "_______________________________" << endl;
		ReleaseSemaphore(child1, 1, NULL);

	}


	cout << "Thread ID: " << ThreadId << endl;
	for (int i = 0; i < tasks; i++) {
		cout << "Work " << i + 1 << " To Worker " << bestAssignments[i] + 1 << endl;
	}
	cout << endl;
	for (int i = 0; i < workers; i++) {
		cout << "Worker " << i + 1 << " : " << bestsum[i] << endl;
	}
	cout << endl;

	cout << "\tmax = " << maxAnswer << endl;
	cout << "\tmin = " << minAnswer << endl;
	cout << "-------------------------" << endl;

	delete[] durations;
	for (int i = 0; i < tasks; i++) {
		delete[] ability[i];
	}


	return 0;
}