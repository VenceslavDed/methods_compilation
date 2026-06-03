#include "parser.h"
#include <stdexcept>
#include <cstdlib>

Parser::Parser(Lexer& lex) : lex_(lex), memOffset_(0) {}

// ═══════════════════════════════════════════════════════════════════════════
// Вспомогательные методы
// ═══════════════════════════════════════════════════════════════════════════

Token Parser::peek()  { return lex_.peek(); }
Token Parser::next()  { return lex_.next(); }
bool  Parser::check(TokenType t) { return peek().type == t; }

Token Parser::expect(TokenType t) {
    Token tok = next();
    if (tok.type != t) {
        throw InterpError("Синтаксическая ошибка: строка " +
            std::to_string(tok.line) + ", позиция " + std::to_string(tok.col) +
            " — неожиданная лексема \"" + tok.value + "\"");
    }
    return tok;
}

int Parser::addConst(const std::string& s) {
    double v = std::stod(s);
    for (int i = 0; i < (int)res_.consts.size(); ++i)
        if (res_.consts[i] == v) return i;
    res_.consts.push_back(v);
    return (int)res_.consts.size() - 1;
}

int Parser::addString(const std::string& s) {
    for (int i = 0; i < (int)res_.strings.size(); ++i)
        if (res_.strings[i] == s) return i;
    res_.strings.push_back(s);
    return (int)res_.strings.size() - 1;
}

int Parser::findVar(const std::string& name, int line, int col) {
    for (int i = 0; i < (int)res_.vars.size(); ++i)
        if (res_.vars[i].name == name) return i;
    throw InterpError("Семантическая ошибка: переменная \"" + name +
        "\" не объявлена, строка " + std::to_string(line) +
        ", позиция " + std::to_string(col));
}

void Parser::emit(OpsKind kind, int value) {
    res_.ops.push_back({kind, value});
}

int Parser::emitLabel() {
    int pos = (int)res_.ops.size();
    res_.ops.push_back({KIND_LABEL, 0});
    return pos;
}

void Parser::fillLabel(int pos) {
    res_.ops[pos].value = (int)res_.ops.size();
}

// ═══════════════════════════════════════════════════════════════════════════
// Грамматические правила
// ═══════════════════════════════════════════════════════════════════════════

ParseResult Parser::parse() {
    parseProg();
    expect(TOK_EOF);
    return res_;
}

// Prog → KwVar DeclList KwBegin StmtList KwEnd
void Parser::parseProg() {
    expect(TOK_KW_VAR);
    parseDeclList();
    expect(TOK_KW_BEGIN);
    parseStmtList();
    expect(TOK_KW_END);
}

// DeclList → Decl DeclList | ε
void Parser::parseDeclList() {
    while (check(TOK_NAM))
        parseDecl();
}

// Decl → Nam Colon Type Semi
void Parser::parseDecl() {
    Token nameTok = expect(TOK_NAM);
    expect(TOK_COLON);

    VarInfo vi;
    vi.name = nameTok.value;

    if (check(TOK_KW_INT)) {
        next();
        vi.kind = VAR_INT; vi.size = 1;
    } else if (check(TOK_KW_REAL)) {
        next();
        vi.kind = VAR_REAL; vi.size = 1;
    } else if (check(TOK_KW_ARRAY)) {
        next();
        expect(TOK_LBR);
        Token szTok = expect(TOK_NUM);
        expect(TOK_RBR);
        int sz = std::atoi(szTok.value.c_str());
        if (check(TOK_KW_INT)) {
            next(); vi.kind = VAR_ARRAY_INT;
        } else {
            expect(TOK_KW_REAL); vi.kind = VAR_ARRAY_REAL;
        }
        vi.size = sz;
    } else {
        Token bad = peek();
        throw InterpError("Синтаксическая ошибка: ожидался тип (int/real/array), строка " +
            std::to_string(bad.line) + ", позиция " + std::to_string(bad.col));
    }

    vi.offset = memOffset_;
    memOffset_ += vi.size;
    res_.vars.push_back(vi);
    expect(TOK_SEMI);
}

// StmtList → Stmt { ; Stmt } [ ; ] | ε
// Точка с запятой — разделитель операторов (как в Pascal).
// Последний оператор перед end может не иметь ;
void Parser::parseStmtList() {
    TokenType t = peek().type;
    if (t == TOK_KW_END || t == TOK_EOF) return;

    parseStmt();

    while (true) {
        t = peek().type;
        if (t == TOK_KW_END || t == TOK_EOF) break;
        if (t == TOK_SEMI) {
            next(); // съедаем ;
            t = peek().type;
            if (t == TOK_KW_END || t == TOK_EOF) break; // trailing ;
            parseStmt();
        } else {
            Token bad = peek();
            throw InterpError("Синтаксическая ошибка: ожидался ';' или 'end', строка " +
                std::to_string(bad.line) + ", позиция " + std::to_string(bad.col) +
                " — неожиданная лексема \"" + bad.value + "\"");
        }
    }
}

// Stmt → AssignStmt | IfStmt | WhileStmt | ReadStmt | PrintStmt | Block
void Parser::parseStmt() {
    Token t = peek();
    switch (t.type) {
    case TOK_NAM: {
        Token nameTok = next();
        int idx = findVar(nameTok.value, nameTok.line, nameTok.col);
        emit(KIND_VAR, idx);
        parseAssignTail(idx);
        break;
    }
    case TOK_KW_IF:    parseIfStmt();    break;
    case TOK_KW_WHILE: parseWhileStmt(); break;
    case TOK_KW_READ:  parseReadStmt();  break;
    case TOK_KW_PRINT: parsePrintStmt(); break;
    case TOK_KW_BEGIN: parseBlock();     break;
    default:
        throw InterpError("Синтаксическая ошибка: неожиданная лексема \"" +
            t.value + "\" в строке " + std::to_string(t.line) +
            ", позиция " + std::to_string(t.col));
    }
}

// Block → KwBegin StmtList KwEnd
void Parser::parseBlock() {
    expect(TOK_KW_BEGIN);
    parseStmtList();
    expect(TOK_KW_END);
}

// StmtTail → := Expr | [ Expr ] := Expr
// (точку с запятой разделитель обрабатывает parseStmtList)
void Parser::parseAssignTail(int /*varIdx*/) {
    if (check(TOK_LBR)) {
        next();
        parseExpr();
        expect(TOK_RBR);
        emit(KIND_OP, OP_IDX);
        expect(TOK_ASSIGN);
        parseExpr();
        emit(KIND_OP, OP_ASSIGN);
    } else {
        expect(TOK_ASSIGN);
        parseExpr();
        emit(KIND_OP, OP_ASSIGN);
    }
}

// IfStmt → if BoolExpr then Block ElsePart
void Parser::parseIfStmt() {
    expect(TOK_KW_IF);
    parseBoolExpr();
    expect(TOK_KW_THEN);

    int jfLabelPos = emitLabel();
    emit(KIND_OP, OP_JF);

    parseBlock();

    if (check(TOK_KW_ELSE)) {
        int jLabelPos = emitLabel();
        emit(KIND_OP, OP_J);
        fillLabel(jfLabelPos);

        next(); // KwElse
        parseBlock();

        fillLabel(jLabelPos);
    } else {
        fillLabel(jfLabelPos);
    }
}

// WhileStmt → while BoolExpr do Block
void Parser::parseWhileStmt() {
    expect(TOK_KW_WHILE);

    int loopStart = (int)res_.ops.size();

    parseBoolExpr();
    expect(TOK_KW_DO);

    int jfLabelPos = emitLabel();
    emit(KIND_OP, OP_JF);

    parseBlock();

    emit(KIND_LABEL, loopStart);
    emit(KIND_OP, OP_J);

    fillLabel(jfLabelPos);
}

// ReadStmt → read ( VarTail )
void Parser::parseReadStmt() {
    expect(TOK_KW_READ);
    expect(TOK_LPAR);
    Token nameTok = expect(TOK_NAM);
    int idx = findVar(nameTok.value, nameTok.line, nameTok.col);
    emit(KIND_VAR, idx);
    if (check(TOK_LBR)) {
        next();
        parseExpr();
        expect(TOK_RBR);
        emit(KIND_OP, OP_IDX);
    }
    emit(KIND_OP, OP_RD);
    expect(TOK_RPAR);
}

// PrintStmt → print ( PrintArg )
void Parser::parsePrintStmt() {
    expect(TOK_KW_PRINT);
    expect(TOK_LPAR);
    parsePrintArg();
    emit(KIND_OP, OP_PT);
    expect(TOK_RPAR);
}

// PrintArg → String | Expr
void Parser::parsePrintArg() {
    if (check(TOK_STRING)) {
        Token s = next();
        int idx = addString(s.value);
        emit(KIND_STRING, idx);
    } else {
        parseExpr();
    }
}

// BoolExpr → Expr RelOp Expr
void Parser::parseBoolExpr() {
    // BoolExpr → Expr RelOp Expr
    // Оба операнда должны быть в ОПС ДО операции сравнения.
    // Поэтому сохраняем тип оператора, разбираем второй операнд, потом эмитируем.
    parseExpr();
    OpCode op = parseRelOp();
    parseExpr();
    emit(KIND_OP, op);
}

OpCode Parser::parseRelOp() {
    Token t = next();
    switch (t.type) {
    case TOK_LESS:      return OP_LT;
    case TOK_GREATER:   return OP_GT;
    case TOK_LESSEQ:    return OP_LE;
    case TOK_GREATEREQ: return OP_GE;
    case TOK_EQUAL:     return OP_EQ;
    case TOK_NOTEQUAL:  return OP_NE;
    default:
        throw InterpError("Синтаксическая ошибка: ожидался оператор сравнения, "
            "строка " + std::to_string(t.line) +
            ", позиция " + std::to_string(t.col));
    }
}

// Expr → Term ExprT
void Parser::parseExpr() {
    parseTerm();
    parseExprT();
}

// ExprT → + Term ExprT | - Term ExprT | ε
void Parser::parseExprT() {
    while (check(TOK_PLUS) || check(TOK_MINUS)) {
        TokenType op = next().type;
        parseTerm();
        emit(KIND_OP, op == TOK_PLUS ? OP_ADD : OP_SUB);
    }
}

// Term → Factor TermT
void Parser::parseTerm() {
    parseFactor();
    parseTermT();
}

// TermT → * Factor TermT | / Factor TermT | ε
void Parser::parseTermT() {
    while (check(TOK_STAR) || check(TOK_SLASH)) {
        TokenType op = next().type;
        parseFactor();
        emit(KIND_OP, op == TOK_STAR ? OP_MUL : OP_DIV);
    }
}

// Factor → Num | Nam NamFactor | ( Expr ) | - Factor
void Parser::parseFactor() {
    Token t = peek();

    if (t.type == TOK_NUM) {
        next();
        int idx = addConst(t.value);
        emit(KIND_CONST, idx);
        return;
    }

    if (t.type == TOK_NAM) {
        next();
        int idx = findVar(t.value, t.line, t.col);
        emit(KIND_VAR, idx);
        if (check(TOK_LBR)) {
            next();
            parseExpr();
            expect(TOK_RBR);
            emit(KIND_OP, OP_IDX);
        }
        return;
    }

    if (t.type == TOK_LPAR) {
        next();
        parseExpr();
        expect(TOK_RPAR);
        return;
    }

    if (t.type == TOK_MINUS) {
        next();
        parseFactor();
        emit(KIND_OP, OP_NEG);
        return;
    }

    throw InterpError("Синтаксическая ошибка: ожидалось выражение, строка " +
        std::to_string(t.line) + ", позиция " + std::to_string(t.col));
}
