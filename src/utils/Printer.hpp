#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <iostream>
#include <vector>
class Printer {
    const char *name;
    std::vector<Printer> children;
    bool percent;
    int valueSum;
    bool dontPrintSum = false;

  public:
    void noValue(bool set = true) { dontPrintSum = set; }
    Printer(const char *name, int value, bool percent = true,
            bool dontPrintSum = false)
        : name(name), percent(percent), valueSum(value),
          dontPrintSum(dontPrintSum) {}
    Printer(const char *name, bool percent = true)
        : name(name), percent(percent), valueSum(0) {}
    void add(const Printer &p) {
        children.push_back(p);
        this->valueSum += p.valueSum;
    }
    void add(const std::vector<Printer> &p) {
        for (auto &i : p) {
            children.push_back(i);
            this->valueSum += i.valueSum;
        }
    }
    void add(const char *name, int value = 0, bool percent = true,
             bool dontPrintSum = false) {
        children.push_back(Printer(name, value, percent, dontPrintSum));
        this->valueSum += value;
    }
    int print(const char *head, const char *conjunction, const char *end,
              int quotationDepth = 1);
};
#endif
