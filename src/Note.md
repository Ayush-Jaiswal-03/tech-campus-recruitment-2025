Make sure that the log file path is passed as an argument in logFilePath (ensure you call this program with the path to the log file in the root directory).

Compile Info: 
g++ -o extractLogs src/solution.cpp -std=c++17 -pthread
./extractLogs your_log_file.log 2024-01-12 4

This will read your_log_file.log, filter logs for 2025-01-31, and output the extracted logs to the output/ directory.
