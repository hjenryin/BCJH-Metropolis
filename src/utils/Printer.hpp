#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <iostream>
#include <vector>
class Printer {
    const char *name;
    std::vector<Printer> children;
    bool percent;
    int value;
    bool printNoValue = false;

  public:
    void noValue(bool set = true) { printNoValue = set; }
    Printer(const char *name, int value, bool percent = true,
            bool printNoValue = false)
        : name(name), percent(percent), value(value),
          printNoValue(printNoValue) {}
    Printer(const char *name, bool percent = true)
        : name(name), percent(percent), value(0) {}
    void add(const Printer &p) {
        children.push_back(p);
        this->value += p.value;
    }
    void add(const std::vector<Printer> &p) {
        for (auto &i : p) {
            children.push_back(i);
            this->value += i.value;
        }
    }
    void add(const char *name, int value = 0, bool percent = true,
             bool printNoValue = false) {
        children.push_back(Printer(name, value, percent, printNoValue));
        this->value += value;
    }
    int print(const char *head, const char *conjunction, const char *end,
              int quotationDepth = 1);
};
#endif
