#include "Printer.hpp"
int Printer::print(const char *head, const char *conjunction, const char *end,
                   int quotationDepth) {
    if (!value)
        return 0;
    std::cout << head << name;
    if (quotationDepth) {
        std::cout << ": ";
        quotationDepth--;
    }
    if (!printNoValue) {
        std::cout << value;
        if (percent)
            std::cout << "%";
    }
    if (children.size() == 0) {
        return value;
    }
    if (!printNoValue) {
        std::cout << " (";
    }
    int valueCount = 0;
    for (auto &child : children) {
        int printed = child.print("", conjunction, "", quotationDepth);
        valueCount += printed;
        if (printed && valueCount < value)
            std::cout << conjunction;
    }
    if (!printNoValue) {
        std::cout << ") ";
    }
    std::cout << end;
    return true;
}
