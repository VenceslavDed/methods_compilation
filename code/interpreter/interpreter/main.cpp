#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

static void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif
}

int main() {
    setupConsole();

    std::cout << "========================================\n";
    std::cout << "  Транслятор-интерпретатор\n";
    std::cout << "========================================\n\n";

    std::cout << "Примеры тестов:\n\n";

    std::cout << "-- Тест 1: Формулы и вычисления --\n";
    std::cout << "var\n";
    std::cout << "    a : int;\n";
    std::cout << "    b : int;\n";
    std::cout << "    c : real;\n";
    std::cout << "begin\n";
    std::cout << "    a := 10;\n";
    std::cout << "    b := 3;\n";
    std::cout << "    print(\"a + b =\");\n";
    std::cout << "    print(a + b);\n";
    std::cout << "    print(\"a - b =\");\n";
    std::cout << "    print(a - b);\n";
    std::cout << "    print(\"a * b =\");\n";
    std::cout << "    print(a * b);\n";
    std::cout << "    c := 10.0 / 3.0;\n";
    std::cout << "    print(\"a / b =\");\n";
    std::cout << "    print(c)\n";
    std::cout << "end\n\n";

    std::cout << "-- Тест 2: Условный оператор if-then-else --\n";
    std::cout << "var\n";
    std::cout << "    x : int;\n";
    std::cout << "begin\n";
    std::cout << "    print(\"Введите число:\");\n";
    std::cout << "    read(x);\n";
    std::cout << "    if x > 0 then begin\n";
    std::cout << "        print(\"положительное\")\n";
    std::cout << "    end else begin\n";
    std::cout << "        if x < 0 then begin\n";
    std::cout << "            print(\"отрицательное\")\n";
    std::cout << "        end else begin\n";
    std::cout << "            print(\"ноль\")\n";
    std::cout << "        end\n";
    std::cout << "    end\n";
    std::cout << "end\n\n";

    std::cout << "-- Тест 3: Цикл while, сумма от 1 до n --\n";
    std::cout << "var\n";
    std::cout << "    n   : int;\n";
    std::cout << "    sum : int;\n";
    std::cout << "    i   : int;\n";
    std::cout << "begin\n";
    std::cout << "    print(\"Введите n:\");\n";
    std::cout << "    read(n);\n";
    std::cout << "    sum := 0;\n";
    std::cout << "    i := 1;\n";
    std::cout << "    while i <= n do begin\n";
    std::cout << "        sum := sum + i;\n";
    std::cout << "        i := i + 1\n";
    std::cout << "    end;\n";
    std::cout << "    print(\"Сумма 1..n =\");\n";
    std::cout << "    print(sum)\n";
    std::cout << "end\n\n";

    std::cout << "-- Тест 4: Ввод и вывод массива --\n";
    std::cout << "var\n";
    std::cout << "    n : int;\n";
    std::cout << "    M : array [10] int;\n";
    std::cout << "    i : int;\n";
    std::cout << "begin\n";
    std::cout << "    print(\"Введите размер (не более 10):\");\n";
    std::cout << "    read(n);\n";
    std::cout << "    i := 0;\n";
    std::cout << "    while i < n do begin\n";
    std::cout << "        print(\"Введите элемент:\");\n";
    std::cout << "        read(M[i]);\n";
    std::cout << "        i := i + 1\n";
    std::cout << "    end;\n";
    std::cout << "    print(\"Массив:\");\n";
    std::cout << "    i := 0;\n";
    std::cout << "    while i < n do begin\n";
    std::cout << "        print(M[i]);\n";
    std::cout << "        i := i + 1\n";
    std::cout << "    end\n";
    std::cout << "end\n\n";

    std::cout << "-- Тест 5: Пузырьковая сортировка --\n";
    std::cout << "var\n";
    std::cout << "    n   : int;\n";
    std::cout << "    M   : array [10] int;\n";
    std::cout << "    i   : int;\n";
    std::cout << "    j   : int;\n";
    std::cout << "    tmp : int;\n";
    std::cout << "begin\n";
    std::cout << "    print(\"Введите размер (не более 10):\");\n";
    std::cout << "    read(n);\n";
    std::cout << "    i := 0;\n";
    std::cout << "    while i < n do begin\n";
    std::cout << "        print(\"Введите элемент:\");\n";
    std::cout << "        read(M[i]);\n";
    std::cout << "        i := i + 1\n";
    std::cout << "    end;\n";
    std::cout << "    i := 0;\n";
    std::cout << "    while i < n do begin\n";
    std::cout << "        j := 0;\n";
    std::cout << "        while j < n - 1 do begin\n";
    std::cout << "            if M[j] > M[j + 1] then begin\n";
    std::cout << "                tmp      := M[j];\n";
    std::cout << "                M[j]     := M[j + 1];\n";
    std::cout << "                M[j + 1] := tmp\n";
    std::cout << "            end;\n";
    std::cout << "            j := j + 1\n";
    std::cout << "        end;\n";
    std::cout << "        i := i + 1\n";
    std::cout << "    end;\n";
    std::cout << "    print(\"Отсортированный массив:\");\n";
    std::cout << "    i := 0;\n";
    std::cout << "    while i < n do begin\n";
    std::cout << "        print(M[i]);\n";
    std::cout << "        i := i + 1\n";
    std::cout << "    end\n";
    std::cout << "end\n\n";

    std::cout << "-- Тест 6: Лексическая ошибка (символ '@') --\n";
    std::cout << "var\n";
    std::cout << "    x : int;\n";
    std::cout << "    y : int;\n";
    std::cout << "begin\n";
    std::cout << "    x := 10;\n";
    std::cout << "    y := x @ 5;\n";
    std::cout << "    print(y)\n";
    std::cout << "end\n\n";

    std::cout << "-- Тест 7: Синтаксическая ошибка (пропущена ';') --\n";
    std::cout << "var\n";
    std::cout << "    x : int;\n";
    std::cout << "    y : int;\n";
    std::cout << "begin\n";
    std::cout << "    x := 10\n";
    std::cout << "    y := x + 5;\n";
    std::cout << "    print(y)\n";
    std::cout << "end\n\n";

    std::cout << "-- Тест 8: Необъявленная переменная --\n";
    std::cout << "var\n";
    std::cout << "    x : int;\n";
    std::cout << "begin\n";
    std::cout << "    x := 5;\n";
    std::cout << "    print(z)\n";
    std::cout << "end\n\n";

    std::cout << "========================================\n";
    std::cout << "Введите программу строка за строкой.\n";
    std::cout << "Когда закончите - введите RUN на новой строке.\n";
    std::cout << "========================================\n";

    // Читаем программу до строки "RUN"
    std::string src, line;
    while (std::getline(std::cin, line)) {
        if (line == "RUN") break;
        src += line + "\n";
    }

    std::cout << "\n-- Результат --\n";

    try {
        Lexer       lex(src);
        Parser      par(lex);
        ParseResult pr = par.parse();
        Interpreter interp(pr);
        interp.run();
    }
    catch (const InterpError& e) {
        std::cout << "ОШИБКА: " << e.what() << "\n";
    }

    return 0;
}