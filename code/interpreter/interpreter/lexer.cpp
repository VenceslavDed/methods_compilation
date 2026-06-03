#include "lexer.h"
#include <unordered_map>
#include <stdexcept>

Lexer::Lexer(const std::string& src)
    : src_(src), pos_(0), line_(1), col_(1) {}

// ─── вспомогательные ───────────────────────────────────────────────────────

char Lexer::cur() const {
    if (pos_ >= (int)src_.size()) return '\0';
    return src_[pos_];
}

char Lexer::advance() {
    char c = cur();
    ++pos_;
    if (c == '\n') { ++line_; col_ = 1; }
    else           { ++col_; }
    return c;
}

void Lexer::retract() {
    if (pos_ <= 0) return;
    --pos_;
    if (src_[pos_] == '\n') { --line_; col_ = 1; }
    else                    { --col_; }
}

// ─── таблица ключевых слов ─────────────────────────────────────────────────

TokenType Lexer::keyword(const std::string& s) {
    static const std::unordered_map<std::string, TokenType> kw = {
        {"var",   TOK_KW_VAR},
        {"int",   TOK_KW_INT},
        {"real",  TOK_KW_REAL},
        {"array", TOK_KW_ARRAY},
        {"begin", TOK_KW_BEGIN},
        {"end",   TOK_KW_END},
        {"if",    TOK_KW_IF},
        {"then",  TOK_KW_THEN},
        {"else",  TOK_KW_ELSE},
        {"while", TOK_KW_WHILE},
        {"do",    TOK_KW_DO},
        {"read",  TOK_KW_READ},
        {"print", TOK_KW_PRINT},
    };
    auto it = kw.find(s);
    return it != kw.end() ? it->second : TOK_NAM;
}

// ─── основной метод — конечный автомат ────────────────────────────────────

Token Lexer::readToken() {
    std::string buf;
    State state = S;

    while (true) {
        int tokLine = line_, tokCol = col_;
        char c = cur();

        switch (state) {

        // ── начальное состояние ──────────────────────────────────────────
        case S:
            if (c == '\0') {
                advance();
                return {TOK_EOF, "", line_, col_};
            }
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                advance(); break; // P7 — пропустить
            }
            tokLine = line_; tokCol = col_;
            if (isalpha(c) || c == '_') { buf += advance(); state = Id;   break; }
            if (isdigit(c))             { buf += advance(); state = Int;  break; }
            if (c == '"')               { advance(); state = Str; break; }
            if (c == ':')               { buf += advance(); state = Col;  break; }
            if (c == '<')               { buf += advance(); state = Lt;   break; }
            if (c == '>')               { buf += advance(); state = Gt;   break; }
            // одиночные символы
            {
                static const std::string singles = "+-*/;,()[]";
                if (singles.find(c) != std::string::npos) {
                    buf += advance();
                    TokenType t;
                    switch (c) {
                        case '+': t = TOK_PLUS;  break;
                        case '-': t = TOK_MINUS; break;
                        case '*': t = TOK_STAR;  break;
                        case '/': t = TOK_SLASH; break;
                        case ';': t = TOK_SEMI;  break;
                        case ',': t = TOK_COMMA; break;
                        case '(': t = TOK_LPAR;  break;
                        case ')': t = TOK_RPAR;  break;
                        case '[': t = TOK_LBR;   break;
                        default:  t = TOK_RBR;   break;
                    }
                    return {t, buf, tokLine, tokCol};
                }
            }
            if (c == '=') {
                buf += advance();
                return {TOK_EQUAL, buf, tokLine, tokCol};
            }
            // недопустимый символ — P8
            {
                advance();
                throw InterpError("Лексическая ошибка: недопустимый символ '" +
                    std::string(1, c) + "' в строке " + std::to_string(tokLine) +
                    ", позиция " + std::to_string(tokCol));
            }

        // ── имя / ключевое слово ─────────────────────────────────────────
        case Id:
            if (isalpha(c) || isdigit(c) || c == '_') { buf += advance(); break; }
            // P3 + P4 — возврат символа, определяем тип
            { TokenType t = keyword(buf); return {t, buf, tokLine, tokCol}; }

        // ── целое число ──────────────────────────────────────────────────
        case Int:
            if (isdigit(c)) { buf += advance(); break; }
            if (c == '.')   { buf += advance(); state = Frac; break; }
            return {TOK_NUM, buf, tokLine, tokCol};

        // ── дробная часть ────────────────────────────────────────────────
        case Frac:
            if (isdigit(c)) { buf += advance(); break; }
            if (c == '.') {
                throw InterpError("Лексическая ошибка: лишняя точка в числе "
                    "в строке " + std::to_string(tokLine) +
                    ", позиция " + std::to_string(tokCol));
            }
            return {TOK_NUM, buf, tokLine, tokCol};

        // ── строковый литерал ────────────────────────────────────────────
        case Str:
            if (c == '\0' || c == '\n') {
                throw InterpError("Лексическая ошибка: незакрытый строковый "
                    "литерал в строке " + std::to_string(tokLine) +
                    ", позиция " + std::to_string(tokCol));
            }
            if (c == '"') { advance(); return {TOK_STRING, buf, tokLine, tokCol}; }
            buf += advance(); break;

        // ── ':' или ':=' ────────────────────────────────────────────────
        case Col:
            if (c == '=') { buf += advance(); return {TOK_ASSIGN,   buf, tokLine, tokCol}; }
            return {TOK_COLON, buf, tokLine, tokCol};

        // ── '<', '<=' или '<>' ───────────────────────────────────────────
        case Lt:
            if (c == '=') { buf += advance(); return {TOK_LESSEQ,   buf, tokLine, tokCol}; }
            if (c == '>') { buf += advance(); return {TOK_NOTEQUAL, buf, tokLine, tokCol}; }
            return {TOK_LESS, buf, tokLine, tokCol};

        // ── '>' или '>=' ────────────────────────────────────────────────
        case Gt:
            if (c == '=') { buf += advance(); return {TOK_GREATEREQ, buf, tokLine, tokCol}; }
            return {TOK_GREATER, buf, tokLine, tokCol};

        default: break;
        }
    }
}

// ─── публичный интерфейс ───────────────────────────────────────────────────

Token Lexer::peek() {
    int savedPos  = pos_;
    int savedLine = line_;
    int savedCol  = col_;
    Token t = readToken();
    pos_  = savedPos;
    line_ = savedLine;
    col_  = savedCol;
    return t;
}

Token Lexer::next() {
    return readToken();
}
