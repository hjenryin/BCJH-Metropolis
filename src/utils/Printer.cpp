#include "Printer.hpp"
int Printer::print(const char *head, const char *conjunction, const char *end,
                   int quotationDepth) {
    if (!valueSum)
        return 0;
    std::cout << head << name;
    if (quotationDepth) {
        std::cout << ": ";
        quotationDepth--;
    }
    if (!dontPrintSum) {
        std::cout << valueSum;
        if (percent)
            std::cout << "%";
    }
    if (children.size() == 0) {
        return valueSum;
    }
    if (!dontPrintSum) {
        std::cout << " (";
    }
    int valueCount = 0;
    for (auto &child : children) {
        int printed = child.print("", conjunction, "", quotationDepth);
        valueCount += printed;
        if (printed && valueCount != valueSum)
            std::cout << conjunction;
    }
    if (!dontPrintSum) {
        std::cout << ") ";
    }
    std::cout << end;
    return valueSum;
}
