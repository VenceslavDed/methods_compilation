#pragma once
#include "lexer.h"
#include "types.h"
#include <vector>
#include <string>
#include <unordered_map>

// ─── Описание переменной ────────────────────────────────────────────────────
enum VarKind { VAR_INT, VAR_REAL, VAR_ARRAY_INT, VAR_ARRAY_REAL };

struct VarInfo {
    std::string name;
    VarKind     kind;
    int         size;   // для массивов; для скаляров = 1
    int         offset; // смещение в памяти интерпретатора
};

// ─── Результат парсинга ─────────────────────────────────────────────────────
struct ParseResult {
    std::vector<OpsElem>    ops;
    std::vector<VarInfo>    vars;
    std::vector<double>     consts;
    std::vector<std::string> strings;
};

// ─── Парсер ─────────────────────────────────────────────────────────────────
class Parser {
public:
    explicit Parser(Lexer& lex);
    ParseResult parse();

private:
    Lexer&      lex_;
    ParseResult res_;
    int         memOffset_;  // текущее смещение для размещения переменных

    // ── вспомогательные ─────────────────────────────────────────────────────
    Token expect(TokenType t);
    Token peek();
    Token next();
    bool  check(TokenType t);

    int  addConst(const std::string& s);
    int  addString(const std::string& s);
    int  findVar(const std::string& name, int line, int col);
    void emit(OpsKind kind, int value);
    int  emitLabel();          // резервирует место под метку, возвращает позицию
    void fillLabel(int pos);   // заполняет метку текущим pc

    // ── правила грамматики ──────────────────────────────────────────────────
    void parseProg();
    void parseDeclList();
    void parseDecl();
    void parseStmtList();
    void parseStmt();
    void parseBlock();
    void parseAssignTail(int varIdx);  // после Nam: := Expr ; или [Expr] := Expr ;
    void parseIfStmt();
    void parseWhileStmt();
    void parseReadStmt();
    void parsePrintStmt();
    void parsePrintArg();
    void parseBoolExpr();
    OpCode parseRelOp();
    void parseExpr();
    void parseExprT();
    void parseTerm();
    void parseTermT();
    void parseFactor();
};
