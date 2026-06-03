#pragma once
#include <string>
#include <stdexcept>

// ─── Коды лексем ───────────────────────────────────────────────────────────
enum TokenType {
    TOK_NAM, TOK_NUM, TOK_STRING,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_ASSIGN, TOK_COLON, TOK_SEMI, TOK_COMMA,
    TOK_LPAR, TOK_RPAR, TOK_LBR, TOK_RBR,
    TOK_LESS, TOK_GREATER, TOK_LESSEQ, TOK_GREATEREQ, TOK_EQUAL, TOK_NOTEQUAL,
    TOK_KW_VAR, TOK_KW_INT, TOK_KW_REAL, TOK_KW_ARRAY,
    TOK_KW_BEGIN, TOK_KW_END,
    TOK_KW_IF, TOK_KW_THEN, TOK_KW_ELSE,
    TOK_KW_WHILE, TOK_KW_DO,
    TOK_KW_READ, TOK_KW_PRINT,
    TOK_EOF
};

// ─── Лексема ────────────────────────────────────────────────────────────────
struct Token {
    TokenType type;
    std::string value;   // текст лексемы
    int line;            // номер строки (для диагностики)
    int col;             // номер столбца
};

// ─── Коды операций ОПС ──────────────────────────────────────────────────────
enum OpCode {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_NEG,          // унарный минус
    OP_ASSIGN,
    OP_IDX,          // индексирование массива
    OP_LT, OP_GT, OP_LE, OP_GE, OP_EQ, OP_NE,
    OP_JF,           // условный переход (при false)
    OP_J,            // безусловный переход
    OP_RD,           // read
    OP_PT            // print
};

// ─── Тип элемента ОПС ───────────────────────────────────────────────────────
enum OpsKind {
    KIND_VAR,    // ссылка на переменную (индекс в varTable)
    KIND_CONST,  // ссылка на константу (индекс в constTable)
    KIND_STRING, // ссылка на строку    (индекс в strTable)
    KIND_LABEL,  // адрес перехода (индекс в ops)
    KIND_OP      // операция
};

// ─── Элемент ОПС ────────────────────────────────────────────────────────────
struct OpsElem {
    OpsKind kind;
    int     value;  // индекс или OpCode
};

// ─── Ошибки ─────────────────────────────────────────────────────────────────
struct InterpError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
