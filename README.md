# Thread Synchronization with Semaphores in C++

A Windows C++ multithreading project that explores randomized task allocation and workload balancing, with semaphore-based synchronization of shared results.

## Features

- Creates worker threads using the Windows API (`CreateThread`)
- Uses Windows semaphores to coordinate access to shared data
- Applies a readers–writers-style synchronization approach
- Generates randomized assignments to eligible workers
- Calculates each worker's total workload and tracks improvements to the maximum–minimum workload difference
- Uses `ParentWakeup` to signal the main thread when a new result is available
- Accepts a user-defined search time limit

## Technologies

- C++
- Windows API
- Multithreading and semaphores
- Shared-memory synchronization within a process
- File input/output

## Project Structure

```text
OS-Thread-Synchronization-Semaphores/
├── main.cpp
├── input.txt
└── README.md
```

## Input Format

The program reads `input.txt` from its working directory:

1. Number of tasks and number of workers
2. Duration of each task
3. A task-by-worker eligibility matrix (`1` means eligible; `0` means not eligible)

Example:

```text
3 3
5 9 8
1 1 0
1 0 1
0 1 1
```

Every task must have at least one eligible worker.

## Running

**Requirements:** Windows and a C++ compiler supporting the Windows API (such as Visual Studio).

1. Compile `main.cpp` as a Windows console application.
2. Put `input.txt` in the executable's working directory.
3. Run the program and enter the time limit in seconds when prompted.

## How It Works

The program reads task durations and worker eligibility, then starts worker threads based on the number of logical processors reported by Windows. Each thread explores randomized valid assignments and evaluates the difference between the largest and smallest worker workloads. Semaphores coordinate reading and updating the best shared result, and the main thread prints reported improvements.

The search is randomized; it does not guarantee a globally optimal allocation.
