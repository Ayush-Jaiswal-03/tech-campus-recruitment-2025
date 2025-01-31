#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <filesystem>

namespace fs = std::filesystem;
std::mutex outputMutex;  // Ensures thread-safe writing

// Function to process a chunk of the log file
void processChunk(const std::string& logFilePath, const std::string& date, 
                  std::streampos start, std::streampos end, const std::string& outputFilePath) {
    std::ifstream logFile(logFilePath, std::ios::in);
    if (!logFile) {
        std::cerr << "Error: Unable to open log file.\n";
        return;
    }

    logFile.seekg(start); // Move to the chunk's start position
    std::string line;
    std::vector<std::string> buffer; // Store results in a buffer before writing to disk

    // Process each line within the assigned range
    while (logFile.tellg() < end && std::getline(logFile, line)) {
        if (line.find(date) == 0) {  // Fast string match (logs start with date)
            buffer.push_back(line);
        }
    }

    logFile.close();

    // Write results to file with thread-safe access
    std::lock_guard<std::mutex> lock(outputMutex);
    std::ofstream outputFile(outputFilePath, std::ios::app);
    for (const std::string& logLine : buffer) {
        outputFile << logLine << '\n';
    }
    outputFile.close();
}

// Function to divide the file into chunks and process them in parallel
void extractLogsForDate(const std::string& logFilePath, const std::string& date, int numThreads) {
    std::ifstream logFile(logFilePath, std::ios::ate | std::ios::binary);
    if (!logFile) {
        std::cerr << "Error: Unable to open log file.\n";
        return;
    }

    std::streampos fileSize = logFile.tellg(); // Get file size
    logFile.close();

    // Create the output directory if it doesn't exist
    std::string outputDir = "output";
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }

    std::string outputFilePath = outputDir + "/output_" + date + ".txt";

    // Divide file into chunks
    std::vector<std::thread> threads;
    std::streampos chunkSize = fileSize / numThreads;
    
    for (int i = 0; i < numThreads; i++) {
        std::streampos start = i * chunkSize;
        std::streampos end = (i == numThreads - 1) ? fileSize : (i + 1) * chunkSize;

        // Adjust start position to the beginning of a line
        if (i > 0) {
            std::ifstream logFile(logFilePath);
            logFile.seekg(start);
            std::string temp;
            std::getline(logFile, temp); // Move to the next line start
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

    std::string logFilePath = argv[1];  // Specify the log file path in the root directory
    std::string date = argv[2];
    int numThreads = std::stoi(argv[3]);

    extractLogsForDate(logFilePath, date, numThreads);
    return 0;
}
