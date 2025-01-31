Pass the .log file's path as an argument (path to the log file in root dir)

Compile Info: 
g++ -o extractLogs src/solution.cpp -std=c++17 -pthread
./extractLogs test_logs.log 2024-01-12 4

reads tests_logs.log file 
filters the logs as 'YYYY-MM-DD'
outputs extracted logs to output/ dir

4 is the no. of cores you wish to use to compute the task
more the cores -> more is the no. of chunks that can be processed in parallel -> more the memory usage, but faster results
