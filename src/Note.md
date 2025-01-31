Pass the .log file's path as an argument (path to the log file in root dir)

Compile Info: 
g++ -o extractLogs src/solution.cpp -std=c++17 -pthread
./extractLogs test_logs.log 2024-01-12

reads tests_logs.log file 
filters the logs as 'YYYY-MM-DD'
outputs extracted logs to output/ dir
