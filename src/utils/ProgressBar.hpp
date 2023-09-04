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
    static MultiThreadProgressBar *getInstance(int numThreads = 0) {
        if (instance == NULL) {
            if (numThreads == 0) {
                std::cout << "Progress Bar has to be initialized with at least "
                             "one thread"
                          << std::endl;
                exit(1);
            }

            instance = new MultiThreadProgressBar(numThreads);
        }
        return instance;
    }
    void print(int threadId, int threadProgress,
               const std::string &threadWord) {
        // acquire lock
        if (mutex.try_lock()) {
            progress[threadId] = threadProgress;
            word[threadId] = threadWord;
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
    static void destroy() {
        if (instance != NULL) {
            delete instance;
            instance = NULL;
        }
    }
    ~MultiThreadProgressBar() {
        delete[] progress;
        delete[] word;
    }
};

#endif