#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <exception>

class NoChefException : public std::exception {
  private:
    int numChef;

  public:
    virtual const char *what() const throw() {
        if (numChef >= 0) {
            std::cout << "拥有的厨师太少，当前拥有" << numChef << "个厨师。";
        }
        return "拥有的厨师太少，请尝试关闭config.hpp中的AVOID_CHEF_1/2/"
               "3选项，如果仍出现此提示，得到的结果可能不理想，建议手动计算。";
    }
    NoChefException(int numChef = -1) throw() : numChef(numChef) {}
};
class NoRecipeException : public std::exception {
  public:
    virtual const char *what() const throw() {

        return "拥有的菜谱太少，得到的结果可能不理想，建议手动计算。";

    }
    NoRecipeException() throw() {}
};
class FileNotExistException : public std::exception {
  public:
    virtual const char *what() const throw() {
        return "json文件缺失。如果在网页端，请确认已经上传了文件；如果在"
               "本地，请确认已经下载了文件。";
    }
    FileNotExistException() throw() {}
};
#define RANDOM_SEARCH_TIMEOUT 100
#endif