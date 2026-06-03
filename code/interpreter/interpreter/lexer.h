#pragma once
#include "types.h"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(const std::string& src);

    // Вернуть следующую лексему (без продвижения)
    Token peek();
    // Вернуть следующую лексему и продвинуться
    Token next();

private:
    std::string src_;
    int pos_;
    int line_;
    int col_;

    // Состояния автомата
    enum State { S, Id, Int, Frac, Str, Col, Lt, Gt, Z, Zret, Err };

    Token readToken();
    char cur() const;
    char advance();
    void retract();

    static TokenType keyword(const std::string& s);
};
