#include "interpreter.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

Interpreter::Interpreter(const ParseResult& pr) : pr_(pr) {
    int total = 0;
    for (auto& v : pr_.vars) total += v.size;
    mem_.assign(total, 0.0);
}

// ─── стек ──────────────────────────────────────────────────────────────────

void Interpreter::push(double v)    { stack_.push_back({false, 0, v}); }
void Interpreter::pushRef(int addr) { stack_.push_back({true, addr, 0.0}); }

Interpreter::SVal Interpreter::pop() {
    if (stack_.empty())
        throw InterpError("Ошибка выполнения: пустой стек");
    SVal v = stack_.back();
    stack_.pop_back();
    return v;
}

double Interpreter::popVal() {
    SVal v = pop();
    return v.isRef ? mem_[v.addr] : v.val;
}

int Interpreter::popRef() {
    SVal v = pop();
    if (!v.isRef)
        throw InterpError("Ошибка выполнения: ожидалась ссылка на переменную");
    return v.addr;
}

void Interpreter::checkAddr(int addr) const {
    if (addr < 0 || addr >= (int)mem_.size())
        throw InterpError("Ошибка выполнения: выход за границу массива "
            "(адрес " + std::to_string(addr) + ")");
}

// ─── главный цикл ──────────────────────────────────────────────────────────

void Interpreter::run() {
    int pc = 0;
    const int sz = (int)pr_.ops.size();

    while (pc < sz) {
        const OpsElem& e = pr_.ops[pc];
        ++pc;

        switch (e.kind) {

        // ── операнды ──────────────────────────────────────────────────────
        case KIND_VAR:
            pushRef(pr_.vars[e.value].offset);
            break;

        case KIND_CONST:
            push(pr_.consts[e.value]);
            break;

        case KIND_STRING:
            // Строка: храним как специальную метку (addr = -(idx+1))
            stack_.push_back({true, -(e.value + 1), 0.0});
            break;

        case KIND_LABEL:
            // Адрес перехода — кладём как обычное значение
            push((double)e.value);
            break;

        // ── операции ──────────────────────────────────────────────────────
        case KIND_OP:
            switch ((OpCode)e.value) {

            case OP_ADD: { double b = popVal(), a = popVal(); push(a + b); break; }
            case OP_SUB: { double b = popVal(), a = popVal(); push(a - b); break; }
            case OP_MUL: { double b = popVal(), a = popVal(); push(a * b); break; }
            case OP_DIV: {
                double b = popVal(), a = popVal();
                if (b == 0.0) throw InterpError("Ошибка выполнения: деление на ноль");
                push(a / b);
                break;
            }
            case OP_NEG: { double a = popVal(); push(-a); break; }

            case OP_ASSIGN: {
                double val = popVal();
                int    dst = popRef();
                checkAddr(dst);
                mem_[dst] = val;
                break;
            }

            case OP_IDX: {
                // стек: ... [baseRef] [idxVal]
                double idxD = popVal();
                int    idx  = (int)idxD;
                SVal   base = pop();
                if (!base.isRef || base.addr < 0)
                    throw InterpError("Ошибка выполнения: ожидалась ссылка на массив");
                int addr = base.addr + idx;
                checkAddr(addr);
                pushRef(addr);
                break;
            }

            case OP_LT: { double b=popVal(),a=popVal(); push(a< b?1.0:0.0); break; }
            case OP_GT: { double b=popVal(),a=popVal(); push(a> b?1.0:0.0); break; }
            case OP_LE: { double b=popVal(),a=popVal(); push(a<=b?1.0:0.0); break; }
            case OP_GE: { double b=popVal(),a=popVal(); push(a>=b?1.0:0.0); break; }
            case OP_EQ: { double b=popVal(),a=popVal(); push(a==b?1.0:0.0); break; }
            case OP_NE: { double b=popVal(),a=popVal(); push(a!=b?1.0:0.0); break; }

            // ОПС для jf: ... cond  LABEL(addr)  OP_JF
            // К моменту исполнения OP_JF на стеке лежит:
            //   top   = addr (положено KIND_LABEL)
            //   top-1 = cond (результат сравнения)
            case OP_JF: {
                double addr = popVal();   // адрес
                double cond = popVal();   // условие
                if (cond == 0.0) pc = (int)addr;
                break;
            }

            // ОПС для j: ... LABEL(addr)  OP_J
            case OP_J: {
                double addr = popVal();
                pc = (int)addr;
                break;
            }

            case OP_RD: {
                SVal top = pop();
                if (!top.isRef || top.addr < 0)
                    throw InterpError("Ошибка выполнения: read ожидал ссылку");
                checkAddr(top.addr);
                double v;
                if (!(std::cin >> v))
                    throw InterpError("Ошибка ввода: не удалось прочитать число");
                mem_[top.addr] = v;
                break;
            }

            case OP_PT: {
                SVal top = pop();
                if (top.isRef && top.addr < 0) {
                    // строковый литерал
                    int strIdx = -(top.addr + 1);
                    std::cout << pr_.strings[strIdx] << "\n";
                } else {
                    double v = top.isRef ? mem_[top.addr] : top.val;
                    if (v == (long long)v)
                        std::cout << (long long)v << "\n";
                    else
                        std::cout << v << "\n";
                }
                break;
            }

            } // switch OpCode
            break;

        } // switch OpsKind
    } // while
}
