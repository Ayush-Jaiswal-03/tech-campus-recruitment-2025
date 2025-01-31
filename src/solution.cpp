#include <iostream>
#include <fstream>
#include <string>
#include <thread> // for parallel programming
#include <vector>
#include <mutex> // to apply locks (helps avoid inconsistency)
#include <filesystem>

namespace fs = std::filesystem;
std::mutex outputMutex; 

// this function processes a part of the log file
void processChunk(const std::string& logFilePath, const std::string& date, 
                  std::streampos start, std::streampos end, const std::string& outputFilePath) {
    std::ifstream logFile(logFilePath, std::ios::in);
    if (!logFile) {
        std::cerr << "Error: Unable to open log file.\n";
        return;
    }

    logFile.seekg(start); // moves the chunk to the start ( so that we can process it) explanation in extractLogs function
    std::string line;
    std::vector<std::string> buffer; // to store the result in a buffer (before writing it to the disk)

    // Process each line
    while (logFile.tellg() < end && std::getline(logFile, line)) {
        if (line.find(date) == 0) { 
            buffer.push_back(line);
        }
    }

    logFile.close();

    // writes the result to the file
    // applies a lock to the file to avoid inconsitency
    std::lock_guard<std::mutex> lock(outputMutex);
    std::ofstream outputFile(outputFilePath, std::ios::app);
    for (const std::string& logLine : buffer) {
        outputFile << logLine << '\n';
    }
    outputFile.close();
}

// here we want to process the chunks in parallel
// this function divides the file into chunks 
void extractLogsForDate(const std::string& logFilePath, const std::string& date, int numThreads) {
    std::ifstream logFile(logFilePath, std::ios::ate | std::ios::binary);
    if (!logFile) {
        std::cerr << "Error: Unable to open log file.\n";
        return;
    }

    std::streampos fileSize = logFile.tellg(); // to get file size
    logFile.close();

    std::string outputDir = "output";
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }

    std::string outputFilePath = outputDir + "/output_" + date + ".txt";

    // divides files into chunks 
    std::vector<std::thread> threads;
    std::streampos chunkSize = fileSize / numThreads;
    
    for (int i = 0; i < numThreads; i++) {
        std::streampos start = i * chunkSize;
        std::streampos end = (i == numThreads - 1) ? fileSize : (i + 1) * chunkSize;

        // adjusting the starting position 
        // moving the pointer to the start of the file because the chunk to be processed is 
        // placed in the beginning of the file
        if (i > 0) {
            std::ifstream logFile(logFilePath);
            logFile.seekg(start);
            std::string temp;
            std::getline(logFile, temp); 
            start = logFile.tellg();
        }

        threads.emplace_back(processChunk, logFilePath, date, start, end, outputFilePath);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Logs for " << date << " extracted to " << outputFilePath << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <log_file_path> <YYYY-MM-DD> <num_threads>\n";
        return 1;
    }

    std::string logFilePath = argv[1];  // Specifying the log file path in the root directory
    std::string date = argv[2];
    int numThreads = std::stoi(argv[3]);

    extractLogsForDate(logFilePath, date, numThreads);
    return 0;
}
