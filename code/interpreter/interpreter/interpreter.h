#pragma once
#include "parser.h"
#include <vector>

class Interpreter {
public:
    explicit Interpreter(const ParseResult& pr);
    void run();

private:
    const ParseResult& pr_;

    // ── память переменных ────────────────────────────────────────────────
    std::vector<double> mem_;   // плоская память (скаляры + массивы)

    // ── стек интерпретатора ──────────────────────────────────────────────
    // Каждый элемент — либо значение (isRef=false),
    // либо ссылка на ячейку памяти (isRef=true, addr=индекс в mem_)
    struct SVal {
        bool   isRef;
        int    addr;    // если isRef
        double val;     // если !isRef
    };
    std::vector<SVal> stack_;

    void   push(double v);
    void   pushRef(int addr);
    SVal   pop();
    double popVal();  // снять значение (разыменовать ссылку)
    int    popRef();  // снять ссылку (нужна для присваивания)

    void checkAddr(int addr) const;
};
