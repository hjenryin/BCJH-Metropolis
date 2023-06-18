#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <exception>

class NoChefException : public std::exception {
  public:
    virtual const char *what() const throw() {
        return "拥有的厨师太少，请尝试关闭config.hpp中的AVOID_CHEF_1/2/"

               "3选项，如果仍出现此提示，得到的结果可能不理想，建议手动计算。";

    }
    NoChefException() throw() {}
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
    virtual const char *what() const throw() { return "文件不存在。"; }
    FileNotExistException() throw() {}
};
#define RANDOM_SEARCH_TIMEOUT 100
#endif