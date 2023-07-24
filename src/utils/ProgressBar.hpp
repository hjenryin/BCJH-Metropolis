#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP

#include <iostream>
#include <string>
#include <mutex>
static std::mutex mutex;
static const char *previousLine = "\033[1A";

class MultiThreadProgressBar {
    static MultiThreadProgressBar *instance;

    int numThreads;
    int *progress;
    std::string *word;
    bool printed = false;

    MultiThreadProgressBar(int numThreads) {
        this->numThreads = numThreads;
        progress = new int[numThreads]{0};
        word = new std::string[numThreads]{""};
    }

  public:
    static MultiThreadProgressBar *getInstance(int numThreads) {
        if (instance == NULL) {
            // std::cout << "Progress bar initialized with " << numThreads
            //   << " threads." << std::endl;
            instance = new MultiThreadProgressBar(numThreads);
        }
        return instance;
    }
    void print(int threadId, int threadProgress, std::string &threadWord) {
        // acquire lock
        progress[threadId] = threadProgress;
        word[threadId] = threadWord;
        if (mutex.try_lock()) {
            if (printed) {
                for (int i = 0; i < numThreads; i++) {
                    std::cout << previousLine;
                }
            }
            printed = true;
            for (int i = 0; i < numThreads; i++) {
                int p = progress[i];
                if (p == 100) {
                    std::cout << "\r线程" << i + 1 << "：完成 [";
                } else {
                    std::cout << "\r线程" << i + 1 << "：" << p << "%  [";
                }
                for (int j = 0; j < 10; j++) {
                    if (j < progress[i] / 10) {
                        std::cout << "#";
                    } else {
                        std::cout << " ";
                    }
                }
                std::cout << "] " << word[i] << "      " << std::endl;
            }
            mutex.unlock();
        }
    }
};

#endif