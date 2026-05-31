
# 1. Лексический анализатор

## 1.1. Список лексем

| №  | Лексема     | Пример             | Описание                             |
|----|-------------|--------------------|--------------------------------------|
| 1  | `Nam`       | `x`, `count`, `i1` | Имя (идентификатор)                  |
| 2  | `Num`       | `42`, `3.14`       | Число (целое или вещественное)       |
| 3  | `String`    | `"hello"`          | Строковый литерал                    |
| 4  | `Plus`      | `+`                | Сложение                             |
| 5  | `Minus`     | `-`                | Вычитание                            |
| 6  | `Star`      | `*`                | Умножение                            |
| 7  | `Slash`     | `/`                | Деление                              |
| 8  | `Assign`    | `:=`               | Присваивание                         |
| 9  | `Colon`     | `:`                | Двоеточие                            |
| 10 | `Semi`      | `;`                | Точка с запятой                      |
| 11 | `Comma`     | `,`                | Запятая                              |
| 12 | `Lpar`      | `(`                | Левая круглая скобка                 |
| 13 | `Rpar`      | `)`                | Правая круглая скобка                |
| 14 | `Lbr`       | `[`                | Левая квадратная скобка              |
| 15 | `Rbr`       | `]`                | Правая квадратная скобка             |
| 16 | `Less`      | `<`                | Меньше                               |
| 17 | `Greater`   | `>`                | Больше                               |
| 18 | `LessEq`    | `<=`               | Меньше или равно                     |
| 19 | `GreaterEq` | `>=`               | Больше или равно                     |
| 20 | `Equal`     | `=`                | Равно                                |
| 21 | `NotEqual`  | `<>`               | Не равно                             |
| 22 | `KwVar`     | `var`              | Служебное слово var                  |
| 23 | `KwInt`     | `int`              | Служебное слово int (тип)            |
| 24 | `KwReal`    | `real`             | Служебное слово real (тип)           |
| 25 | `KwArray`   | `array`            | Служебное слово array                |
| 26 | `KwBegin`   | `begin`            | Служебное слово begin                |
| 27 | `KwEnd`     | `end`              | Служебное слово end                  |
| 28 | `KwIf`      | `if`               | Служебное слово if                   |
| 29 | `KwThen`    | `then`             | Служебное слово then                 |
| 30 | `KwElse`    | `else`             | Служебное слово else                 |
| 31 | `KwWhile`   | `while`            | Служебное слово while                |
| 32 | `KwDo`      | `do`               | Служебное слово do                   |
| 33 | `KwRead`    | `read`             | Служебное слово read                 |
| 34 | `KwPrint`   | `print`            | Служебное слово print                |
| 35 | `Eof`       | `⊥`                | Конец ввода                          |

---

## 1.2. Классы входных символов

| Обозначение | Описание                                          |
|-------------|---------------------------------------------------|
| `<L>`       | Буква: a–z, A–Z, _                                |
| `<D>`       | Цифра: 0–9                                        |
| `<.>`       | Точка `.`                                         |
| `<sp>`      | Пробел, табуляция `\t`, перевод строки `\n`       |
| `"`         | Двойная кавычка                                   |
| `<sc>`      | Любой символ внутри строкового литерала (кроме `"` и `\n`) |
| `:`         | Двоеточие                                         |
| `<`         | Знак меньше                                       |
| `>`         | Знак больше                                       |
| `=`         | Знак равно                                        |
| `+` `-` `*` `/` `;` `,` `(` `)` `[` `]` | Одиночные символы-операторы |
| `⊥`         | Конец ввода                                       |
| `<other>`   | Прочие символы                                    |

---

## 1.3. Состояния конечного автомата

| Состояние | Описание                                                         |
|-----------|------------------------------------------------------------------|
| `S`       | Начальное состояние                                              |
| `Id`      | Накапливается имя или ключевое слово                             |
| `Int`     | Накапливается целая часть числа                                  |
| `Frac`    | Накапливается дробная часть числа                                |
| `Str`     | Накапливается содержимое строкового литерала (после открывающей `"`) |
| `Col`     | Прочитано `:`, ожидается `=`                                     |
| `Lt`      | Прочитано `<`, ожидается `=` или `>`                             |
| `Gt`      | Прочитано `>`, ожидается `=`                                     |
| `Z`       | Завершающее состояние (лексема готова)                           |
| `Z*`      | Завершение с возвратом: текущий символ принадлежит следующей лексеме |
| `Err`     | Состояние ошибки                                                 |

---

## 1.4. Таблица переходов конечного автомата

> `Z*` — переход в финальное состояние с возвратом текущего символа во входную ленту.

| Сост. ↓ \ Вход → | `<L>` | `<D>` | `<.>` | `<sp>` | `"` | `+` | `-` | `*` | `/` | `;` | `,` | `(` | `)` | `[` | `]` | `:` | `<` | `>` | `=` | `⊥`  | `<other>` |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| `S`   | Id  | Int | Err | S   | Str | Z   | Z   | Z   | Z   | Z   | Z   | Z   | Z   | Z   | Z   | Col | Lt  | Gt  | Z   | Z   | Err |
| `Id`  | Id  | Id  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  |
| `Int` | Z*  | Int | Frac| Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  |
| `Frac`| Z*  | Frac| Err | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  |
| `Str` | Str | Str | Str | Str | Z   | Str | Str | Str | Str | Str | Str | Str | Str | Str | Str | Str | Str | Str | Str | Err | Str |
| `Col` | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z   | Z*  | Z*  |
| `Lt`  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z   | Z   | Z*  | Z*  |
| `Gt`  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z*  | Z   | Z*  | Z*  |

**Примечания:**
- `Col` читает `:`: если следующий символ `=` → лексема `:=` (`Assign`), иначе → `:` (`Colon`) с возвратом.
- `Lt` читает `<`: если `=` → `<=` (`LessEq`), если `>` → `<>` (`NotEqual`), иначе → `<` (`Less`) с возвратом.
- `Gt` читает `>`: если `=` → `>=` (`GreaterEq`), иначе → `>` (`Greater`) с возвратом.
- `Str` накапливает символы до закрывающей `"`. Встреченная `⊥` или `\n` внутри строки — ошибка (незакрытый строковый литерал).

---

## 1.5. Семантические программы лексического анализатора

| №  | Название         | Описание действия                                                                                      |
|----|------------------|--------------------------------------------------------------------------------------------------------|
| P1 | **Accumulate**   | Добавить текущий символ к буферу лексемы                                                              |
| P2 | **Emit**         | Вернуть собранную лексему; буфер очистить                                                             |
| P3 | **Retract**      | Уменьшить позицию на 1 (текущий символ принадлежит следующей лексеме), затем зафиксировать            |
| P4 | **CheckKeyword** | Проверить буфер по таблице ключевых слов; вернуть соответствующий код или `Nam`                       |
| P5 | **EmitNum**      | Преобразовать буфер в числовое значение, вернуть лексему `Num`                                        |
| P6 | **EmitString**   | Вернуть лексему `String` со значением буфера (кавычки не включаются в значение)                       |
| P7 | **SkipSpace**    | Пробел / перевод строки: счётчик строки увеличить при `\n`, буфер не трогать                         |
| P8 | **LexError**     | Недопустимый символ — выдать сообщение с номером строки и позицией в строке                           |

### Привязка семантических программ к переходам

| Из сост. | Входной символ              | В сост. | Сем. программа | Пояснение                              |
|---|---|---|---|---|
| `S`    | `<L>`                       | `Id`   | P1          | Начало имени                                  |
| `S`    | `<D>`                       | `Int`  | P1          | Начало числа                                  |
| `S`    | `<sp>`                      | `S`    | P7          | Пропустить пробел                             |
| `S`    | `+ - * / ; , ( ) [ ]`      | `Z`    | P1 + P2     | Одиночный символ — накопить и вернуть         |
| `S`    | `=`                         | `Z`    | P1 + P2     | Лексема `=`                                   |
| `S`    | `:`                         | `Col`  | P1          | Начало `:` или `:=`                           |
| `S`    | `<`                         | `Lt`   | P1          | Начало `<`, `<=` или `<>`                    |
| `S`    | `>`                         | `Gt`   | P1          | Начало `>` или `>=`                           |
| `S`    | `"`                         | `Str`  | —           | Начало строкового литерала, кавычку не копим  |
| `Id`   | `<L>` или `<D>`             | `Id`   | P1          | Продолжение имени                             |
| `Id`   | прочие                      | `Z*`   | P3 + P4     | Конец имени: вернуть символ, определить тип   |
| `Int`  | `<D>`                       | `Int`  | P1          | Продолжение целого числа                      |
| `Int`  | `<.>`                       | `Frac` | P1          | Начало дробной части                          |
| `Int`  | прочие                      | `Z*`   | P3 + P5     | Конец числа — вернуть символ                  |
| `Frac` | `<D>`                       | `Frac` | P1          | Продолжение дробной части                     |
| `Frac` | прочие                      | `Z*`   | P3 + P5     | Конец вещественного числа                     |
| `Str`  | `<sc>`                      | `Str`  | P1          | Накапливать содержимое строки                 |
| `Str`  | `"`                         | `Z`    | P6          | Закрывающая кавычка — вернуть `String`        |
| `Str`  | `⊥` или `\n`                | `Err`  | P8          | Незакрытый строковый литерал — ошибка         |
| `Col`  | `=`                         | `Z`    | P1 + P2     | Лексема `:=`                                  |
| `Col`  | прочие                      | `Z*`   | P3 + P2     | Лексема `:`                                   |
| `Lt`   | `=`                         | `Z`    | P1 + P2     | Лексема `<=`                                  |
| `Lt`   | `>`                         | `Z`    | P1 + P2     | Лексема `<>`                                  |
| `Lt`   | прочие                      | `Z*`   | P3 + P2     | Лексема `<`                                   |
| `Gt`   | `=`                         | `Z`    | P1 + P2     | Лексема `>=`                                  |
| `Gt`   | прочие                      | `Z*`   | P3 + P2     | Лексема `>`                                   |

---

# 2. КС-грамматика языка

Описание исходной контекстно-свободной грамматики. `KwXxx` и отдельные символы — терминалы (лексемы), остальные — нетерминалы.

## 2.1. Программа и описание переменных

```text
Prog     → KwVar DeclList KwBegin StmtList KwEnd
DeclList → Decl DeclList | ε
Decl     → Nam Colon Type Semi
Type     → KwInt
Type     → KwReal
Type     → KwArray Lbr Num Rbr KwInt
Type     → KwArray Lbr Num Rbr KwReal
```

## 2.2. Список операторов и оператор

```text
StmtList → Stmt StmtList | ε
Stmt     → AssignStmt
Stmt     → IfStmt
Stmt     → WhileStmt
Stmt     → ReadStmt
Stmt     → PrintStmt
Stmt     → Block
```

## 2.3. Блок

```text
Block → KwBegin StmtList KwEnd
```

## 2.4. Присваивание

```text
AssignStmt → Var Assign Expr Semi
Var        → Nam
Var        → Nam Lbr Expr Rbr
```

## 2.5. Условный оператор

```text
IfStmt   → KwIf BoolExpr KwThen Block ElsePart
ElsePart → KwElse Block | ε
```

## 2.6. Цикл

```text
WhileStmt → KwWhile BoolExpr KwDo Block
```

## 2.7. Ввод и вывод

```text
ReadStmt  → KwRead  Lpar Var       Rpar Semi
PrintStmt → KwPrint Lpar PrintArg  Rpar Semi
PrintArg  → Expr
PrintArg  → String
```


## 2.8. Логическое выражение (условие)

```text
BoolExpr → Expr RelOp Expr
RelOp    → Less | Greater | LessEq | GreaterEq | Equal | NotEqual
```

## 2.9. Арифметическое выражение

```text
Expr   → Expr Plus  Term
Expr   → Expr Minus Term
Expr   → Term
Term   → Term Star  Factor
Term   → Term Slash Factor
Term   → Factor
Factor → Num
Factor → Var
Factor → Lpar Expr Rpar
Factor → Minus Factor
```

---

# 3. Устранение левой рекурсии

Левая рекурсия присутствует в правилах `Expr` и `Term`.

## 3.1. Нетерминал `Expr`

Исходные правила:
```text
Expr → Expr Plus  Term    ← левая рекурсия
Expr → Expr Minus Term    ← левая рекурсия
Expr → Term               ← база
```

После устранения:
```text
Expr  → Term ExprT
ExprT → Plus  Term ExprT
ExprT → Minus Term ExprT
ExprT → ε
```

## 3.2. Нетерминал `Term`

Исходные правила:
```text
Term → Term Star  Factor    ← левая рекурсия
Term → Term Slash Factor    ← левая рекурсия
Term → Factor               ← база
```

После устранения:
```text
Term  → Factor TermT
TermT → Star  Factor TermT
TermT → Slash Factor TermT
TermT → ε
```

---

# 4. Грамматика в нестрогой нормальной форме Грейбах (ННФГ)

Требование ННФГ: каждая правая часть либо начинается с терминала, либо равна ε.

```text
Prog → KwVar DeclList KwBegin StmtList KwEnd

DeclList → Nam Colon Type Semi DeclList | ε

Type → KwInt
Type → KwReal
Type → KwArray Lbr Num Rbr KwInt
Type → KwArray Lbr Num Rbr KwReal

StmtList → Nam  StmtTail StmtList
StmtList → KwBegin StmtList KwEnd StmtList
StmtList → KwIf BoolExpr KwThen Block ElsePart StmtList
StmtList → KwWhile BoolExpr KwDo Block StmtList
StmtList → KwRead  Lpar VarTail Rpar Semi StmtList
StmtList → KwPrint Lpar PrintArg Rpar Semi StmtList
StmtList → ε

StmtTail → Assign Expr Semi
StmtTail → Lbr Expr Rbr Assign Expr Semi

Block → KwBegin StmtList KwEnd

ElsePart → KwElse Block | ε

PrintArg → String
PrintArg → Nam NamExprT
PrintArg → Num TermT ExprT
PrintArg → Lpar Expr Rpar TermT ExprT
PrintArg → Minus Factor TermT ExprT

BoolExpr → Nam BoolVarT
BoolExpr → Num TermT ExprT RelOp Expr
BoolExpr → Lpar Expr Rpar TermT ExprT RelOp Expr
BoolExpr → Minus Factor TermT ExprT RelOp Expr

BoolVarT → Lbr Expr Rbr TermT ExprT RelOp Expr
BoolVarT → TermT ExprT RelOp Expr

RelOp → Less | Greater | LessEq | GreaterEq | Equal | NotEqual

Expr → Nam NamExprT
Expr → Num TermT ExprT
Expr → Lpar Expr Rpar TermT ExprT
Expr → Minus Factor TermT ExprT

NamExprT → Lbr Expr Rbr TermT ExprT
NamExprT → TermT ExprT

ExprT → Plus  Nam NamExprT
ExprT → Plus  Num TermT ExprT
ExprT → Plus  Lpar Expr Rpar TermT ExprT
ExprT → Plus  Minus Factor TermT ExprT
ExprT → Minus Nam NamExprT
ExprT → Minus Num TermT ExprT
ExprT → Minus Lpar Expr Rpar TermT ExprT
ExprT → Minus Minus Factor TermT ExprT
ExprT → ε

TermT → Star  Nam NamTermT
TermT → Star  Num TermT
TermT → Star  Lpar Expr Rpar TermT
TermT → Star  Minus Factor TermT
TermT → Slash Nam NamTermT
TermT → Slash Num TermT
TermT → Slash Lpar Expr Rpar TermT
TermT → Slash Minus Factor TermT
TermT → ε

Factor → Num
Factor → Nam NamFactor
Factor → Lpar Expr Rpar
Factor → Minus Factor

NamFactor → Lbr Expr Rbr | ε

NamTermT → Lbr Expr Rbr TermT
NamTermT → TermT

VarTail → Nam
VarTail → Nam Lbr Expr Rbr
```

---

# 5. Семантические действия для генерации ОПС

ОПС генерируется одновременно с работой LL(1)-анализатора.

## 5.1. Обозначения семантических действий

| Символ   | Описание                                                                                   |
|----------|--------------------------------------------------------------------------------------------|
| `□`      | Пустое действие — символ только снимается с магазина анализатора                          |
| `v`      | Записать в ОПС ссылку на переменную (имя из входной цепочки)                               |
| `c`      | Записать в ОПС ссылку на константу (число из входной цепочки)                              |
| `s`      | Записать в ОПС ссылку на строковый литерал (из таблицы строк)                              |
| `+`      | Записать в ОПС операцию сложения                                                           |
| `-`      | Записать в ОПС операцию вычитания                                                          |
| `*`      | Записать в ОПС операцию умножения                                                          |
| `/`      | Записать в ОПС операцию деления                                                            |
| `:=`     | Записать в ОПС операцию присваивания                                                       |
| `-u`     | Записать в ОПС операцию унарного минуса                                                    |
| `idx`    | Записать в ОПС операцию индексирования массива                                             |
| `<` `>` `<=` `>=` `=` `<>` | Записать в ОПС соответствующую операцию сравнения                      |
| `rd`     | Записать в ОПС операцию чтения (`read`)                                                    |
| `pt`     | Записать в ОПС операцию вывода (`print`)                                                   |
| `1`–`5`  | Выполнить семантическую программу для меток и переходов                                    |

## 5.2. Семантические программы для меток и переходов

Используется счётчик `pc` — номер следующего свободного элемента ОПС, и магазин меток `lbStk`.

**Программа 1** — после разбора условия в `if`:
1. В `lbStk` записывается `pc`.
2. В ОПС — пустое место под адрес false-ветки.
3. В ОПС — операция `jf`.

**Программа 2** — в начале `else`:
1. По ссылке из `lbStk` записывается `pc + 2`.
2. В `lbStk` записывается `pc`.
3. В ОПС — пустое место под адрес безусловного перехода.
4. В ОПС — операция `j`.

**Программа 3** — в конце `if` или `if-else`:
1. По ссылке из `lbStk` записывается `pc`.

**Программа 4** — перед условием `while`:
1. В `lbStk` записывается `pc` — адрес начала цикла.

**Программа 5** — после тела `while`:
1. По ссылке из `lbStk` записывается `pc + 2`.
2. В ОПС — адрес начала цикла из `lbStk`.
3. В ОПС — операция `j`.

## 5.3. Таблица семантических действий

### Описания переменных

| Нетерминал | Правая часть                            | Сем. действия      |
|------------|-----------------------------------------|--------------------|
| `DeclList` | `Nam Colon Type Semi DeclList`          | `□ □ □ □ □`        |
| `Type`     | `KwInt`                                 | `□`                |
| `Type`     | `KwReal`                                | `□`                |
| `Type`     | `KwArray Lbr Num Rbr KwInt`             | `□ □ □ □ □`        |
| `Type`     | `KwArray Lbr Num Rbr KwReal`            | `□ □ □ □ □`        |

### Присваивание

| Нетерминал  | Правая часть                             | Сем. действия      |
|-------------|------------------------------------------|--------------------|
| `StmtList`  | `Nam StmtTail StmtList`                  | `v □ □`            |
| `StmtTail`  | `Assign Expr Semi`                       | `□ □ :=`           |
| `StmtTail`  | `Lbr Expr Rbr Assign Expr Semi`          | `□ □ idx □ □ :=`   |

### Выражения

| Нетерминал  | Правая часть                                | Сем. действия         |
|-------------|--------------------------------------------|-----------------------|
| `Expr`      | `Nam NamExprT`                              | `v □`                 |
| `Expr`      | `Num TermT ExprT`                           | `c □ □`               |
| `Expr`      | `Lpar Expr Rpar TermT ExprT`                | `□ □ □ □ □`           |
| `Expr`      | `Minus Factor TermT ExprT`                  | `□ □ -u □ □`          |
| `NamExprT`  | `Lbr Expr Rbr TermT ExprT`                  | `□ □ idx □ □`         |
| `NamExprT`  | `TermT ExprT`                               | `□ □`                 |
| `ExprT`     | `Plus  Nam NamExprT`                        | `□ v □ +`             |
| `ExprT`     | `Plus  Num TermT ExprT`                     | `□ c □ □ +`           |
| `ExprT`     | `Plus  Lpar Expr Rpar TermT ExprT`          | `□ □ □ □ □ □ +`       |
| `ExprT`     | `Plus  Minus Factor TermT ExprT`            | `□ □ □ -u □ □ +`      |
| `ExprT`     | `Minus Nam NamExprT`                        | `□ v □ -`             |
| `ExprT`     | `Minus Num TermT ExprT`                     | `□ c □ □ -`           |
| `ExprT`     | `ε`                                         | —                     |
| `TermT`     | `Star  Nam NamTermT`                        | `□ v □ *`             |
| `TermT`     | `Star  Num TermT`                           | `□ c □ *`             |
| `TermT`     | `Star  Lpar Expr Rpar TermT`                | `□ □ □ □ □ *`         |
| `TermT`     | `Star  Minus Factor TermT`                  | `□ □ □ -u □ *`        |
| `TermT`     | `Slash Nam NamTermT`                        | `□ v □ /`             |
| `TermT`     | `Slash Num TermT`                           | `□ c □ /`             |
| `TermT`     | `ε`                                         | —                     |
| `Factor`    | `Num`                                       | `c`                   |
| `Factor`    | `Nam NamFactor`                             | `v □`                 |
| `Factor`    | `Lpar Expr Rpar`                            | `□ □ □`               |
| `Factor`    | `Minus Factor`                              | `□ □ -u`              |
| `NamFactor` | `Lbr Expr Rbr`                              | `□ □ idx`             |
| `NamFactor` | `ε`                                         | —                     |

### Условие (логическое выражение)

| Нетерминал  | Правая часть                                          | Сем. действия          |
|-------------|------------------------------------------------------|------------------------|
| `BoolExpr`  | `Nam BoolVarT`                                        | `v □`                  |
| `BoolExpr`  | `Num TermT ExprT RelOp Expr`                          | `c □ □ □ □`            |
| `BoolExpr`  | `Lpar Expr Rpar TermT ExprT RelOp Expr`               | `□ □ □ □ □ □ □`        |
| `BoolExpr`  | `Minus Factor TermT ExprT RelOp Expr`                 | `□ □ -u □ □ □ □`       |
| `BoolVarT`  | `Lbr Expr Rbr TermT ExprT RelOp Expr`                | `□ □ idx □ □ □ □`      |
| `BoolVarT`  | `TermT ExprT RelOp Expr`                              | `□ □ □ □`              |
| `RelOp`     | `Less`                                                | `<`                    |
| `RelOp`     | `Greater`                                             | `>`                    |
| `RelOp`     | `LessEq`                                              | `<=`                   |
| `RelOp`     | `GreaterEq`                                           | `>=`                   |
| `RelOp`     | `Equal`                                               | `=`                    |
| `RelOp`     | `NotEqual`                                            | `<>`                   |

### Условный оператор if-then-else

| Нетерминал  | Правая часть                                               | Сем. действия          |
|-------------|-----------------------------------------------------------|------------------------|
| `StmtList`  | `KwIf BoolExpr KwThen Block ElsePart StmtList`             | `□ □ □ 1 □ □ 3 □`     |
| `ElsePart`  | `KwElse Block`                                             | `2 □`                  |
| `ElsePart`  | `ε`                                                        | —                      |

### Цикл while-do

| Нетерминал  | Правая часть                                    | Сем. действия        |
|-------------|------------------------------------------------|----------------------|
| `StmtList`  | `KwWhile BoolExpr KwDo Block StmtList`          | `4 □ □ □ 1 □ 5 □`   |

### Ввод и вывод

| Нетерминал  | Правая часть                                      | Сем. действия         |
|-------------|--------------------------------------------------|-----------------------|
| `StmtList`  | `KwRead  Lpar VarTail Rpar Semi StmtList`         | `□ □ □ rd □ □`        |
| `VarTail`   | `Nam`                                             | `v`                   |
| `VarTail`   | `Nam Lbr Expr Rbr`                                | `v □ □ idx`           |
| `StmtList`  | `KwPrint Lpar PrintArg Rpar Semi StmtList`        | `□ □ □ □ pt □ □`      |
| `PrintArg`  | `String`                                          | `s`                   |
| `PrintArg`  | `Nam NamExprT`                                    | `v □`                 |
| `PrintArg`  | `Num TermT ExprT`                                 | `c □ □`               |
| `PrintArg`  | `Lpar Expr Rpar TermT ExprT`                      | `□ □ □ □ □`           |
| `PrintArg`  | `Minus Factor TermT ExprT`                        | `□ □ -u □ □`          |

---

# 6. Список операций ОПС

| №  | Операция                    | Обозначение | Арность |
|----|-----------------------------|-------------|---------|
| 1  | Сложение                    | `+`         | 2       |
| 2  | Вычитание                   | `-`         | 2       |
| 3  | Умножение                   | `*`         | 2       |
| 4  | Деление                     | `/`         | 2       |
| 5  | Унарный минус               | `-u`        | 1       |
| 6  | Присваивание                | `:=`        | 2       |
| 7  | Индексирование массива      | `idx`       | 2       |
| 8  | Меньше                      | `<`         | 2       |
| 9  | Больше                      | `>`         | 2       |
| 10 | Меньше или равно            | `<=`        | 2       |
| 11 | Больше или равно            | `>=`        | 2       |
| 12 | Равно                       | `=`         | 2       |
| 13 | Не равно                    | `<>`        | 2       |
| 14 | Условный переход (при false) | `jf`       | 2       |
| 15 | Безусловный переход         | `j`         | 1       |
| 16 | Ввод                        | `rd`        | 1       |
| 17 | Вывод                       | `pt`        | 1       |

---

# 7. Формат ОПС

ОПС — линейный массив элементов. Каждый элемент содержит два поля: **тип** (`type`) и **данные** (`value`).

## 7.1. Поле `type`

| Значение      | Описание                                               |
|---------------|--------------------------------------------------------|
| `TYPE_VAR`    | Ссылка на переменную в таблице переменных              |
| `TYPE_CONST`  | Ссылка на константу в таблице констант                 |
| `TYPE_STRING` | Ссылка на строковый литерал в таблице строк            |
| `TYPE_LABEL`  | Метка — номер элемента ОПС (адрес перехода)            |
| `TYPE_OP`     | Операция                                               |

## 7.2. Поле `value`

| `type`        | Содержимое `value`                   | Пример                    |
|---------------|--------------------------------------|---------------------------|
| `TYPE_VAR`    | Индекс в таблице переменных          | `3`                       |
| `TYPE_CONST`  | Индекс в таблице констант            | `0`                       |
| `TYPE_STRING` | Индекс в таблице строк               | `1`                       |
| `TYPE_LABEL`  | Номер элемента ОПС                   | `8`                       |
| `TYPE_OP`     | Код операции                         | `OP_ADD`, `OP_JF`         |

## 7.3. Виды содержимого в магазине интерпретатора

| Вид                        | Описание                                              |
|----------------------------|-------------------------------------------------------|
| Ссылка на переменную       | Индекс в таблице переменных                           |
| Ссылка на константу        | Индекс в таблице констант                             |
| Ссылка на строку           | Индекс в таблице строк                                |
| Числовое значение          | Результат вычисления (целое или вещественное)         |
| Ссылка на элемент массива  | Вычисленный адрес `base + size * index`              |

---

# 8. Примеры генерации и исполнения ОПС

## Пример 1. Арифметическое присваивание

**Входная цепочка:**
```
res := a + b * 3;
```

**Сгенерированная ОПС:** `res  a  b  3  *  +  :=`

| Индекс | Тип           | Данные          |
|--------|---------------|-----------------|
| 0      | `TYPE_VAR`    | ссылка на `res` |
| 1      | `TYPE_VAR`    | ссылка на `a`   |
| 2      | `TYPE_VAR`    | ссылка на `b`   |
| 3      | `TYPE_CONST`  | ссылка на `3`   |
| 4      | `TYPE_OP`     | `OP_MUL`        |
| 5      | `TYPE_OP`     | `OP_ADD`        |
| 6      | `TYPE_OP`     | `OP_ASSIGN`     |

**Вычисление** (при `a=2`, `b=4`):

| Шаг | Магазин        | Операция |
|-----|----------------|----------|
| 1   | res, a, b, 3   | `*`      |
| 2   | res, a, 12     | `+`      |
| 3   | res, 14        | `:=`     |

Результат: `res = 14`.

---

## Пример 2. Индексирование массива

**Входная цепочка:**
```
M[i] := t * M[i];
```

**Сгенерированная ОПС:** `M  i  idx  t  M  i  idx  *  :=`

---

## Пример 3. Вывод строки и значения переменной

**Входная цепочка:**
```
print("result =");
print(res);
```

**Сгенерированная ОПС:**

| Индекс | Тип            | Данные               |
|--------|----------------|----------------------|
| 0      | `TYPE_STRING`  | ссылка на `"result ="` |
| 1      | `TYPE_OP`      | `OP_PT`              |
| 2      | `TYPE_VAR`     | ссылка на `res`      |
| 3      | `TYPE_OP`      | `OP_PT`              |

**Вычисление:**

| Шаг | Магазин    | Операция | Вывод      |
|-----|------------|----------|------------|
| 1   | "result =" | `pt`     | result =   |
| 2   | res        | `pt`     | 14         |

---

## Пример 4. Условный оператор if-then-else

**Входная цепочка:**
```
if x > y then begin z := x end else begin z := y end
```

**Сгенерированная ОПС:**
```
x  y  >  m1  jf  z  x  :=  m2  j  z  y  :=
              ↑                   ↑
             m1                  m2
```

| Индекс | Тип           | Данные      |
|--------|---------------|-------------|
| 0      | `TYPE_VAR`    | `x`         |
| 1      | `TYPE_VAR`    | `y`         |
| 2      | `TYPE_OP`     | `OP_GT`     |
| 3      | `TYPE_LABEL`  | `10` (m1)   |
| 4      | `TYPE_OP`     | `OP_JF`     |
| 5      | `TYPE_VAR`    | `z`         |
| 6      | `TYPE_VAR`    | `x`         |
| 7      | `TYPE_OP`     | `OP_ASSIGN` |
| 8      | `TYPE_LABEL`  | `12` (m2)   |
| 9      | `TYPE_OP`     | `OP_J`      |
| 10     | `TYPE_VAR`    | `z`         |
| 11     | `TYPE_VAR`    | `y`         |
| 12     | `TYPE_OP`     | `OP_ASSIGN` |

**Вычисление** (при `x=5`, `y=2`):

| Шаг | Магазин  | Операция |
|-----|----------|----------|
| 1   | x, y     | `>`      |
| 2   | true, m1 | `jf`     |
| 3   | z, x     | `:=`     |
| 4   | m2       | `j`      |

Переход на m2, ветка else пропущена. Результат: `z = 5`.

---

## Пример 5. Цикл while-do

**Входная цепочка:**
```
while n > 0 do begin n := n - 1 end
```

**Сгенерированная ОПС:**
```
n  0  >  m1  jf  n  n  1  -  :=  m0  j
↑                                  ↑
m0                                m1
```

| Индекс | Тип           | Данные      |
|--------|---------------|-------------|
| 0      | `TYPE_VAR`    | `n` (← m0)  |
| 1      | `TYPE_CONST`  | `0`         |
| 2      | `TYPE_OP`     | `OP_GT`     |
| 3      | `TYPE_LABEL`  | `11` (m1)   |
| 4      | `TYPE_OP`     | `OP_JF`     |
| 5      | `TYPE_VAR`    | `n`         |
| 6      | `TYPE_VAR`    | `n`         |
| 7      | `TYPE_CONST`  | `1`         |
| 8      | `TYPE_OP`     | `OP_SUB`    |
| 9      | `TYPE_OP`     | `OP_ASSIGN` |
| 10     | `TYPE_LABEL`  | `0` (m0)    |
| 11     | `TYPE_OP`     | `OP_J`      |

**Вычисление** (при `n=3`):

| Шаг | Магазин    | n |
|-----|------------|---|
| 1   | n=3, 0, `>`→ true, m1, `jf` → не прыгаем | 3 |
| 2   | n, n, 1, `-`, `:=` | 2 |
| 3   | m0, `j` → переход на 0 | 2 |
| … | … | … |
| — | n=0, `>`→ false, `jf` → прыжок на m1=11, выход | 0 |

---

## Пример 6. Ввод и вывод с массивом

**Входная цепочка:**
```
read(x);
read(M[x]);
print("M[x] + x =");
print(M[x] + x);
```

**Сгенерированная ОПС:**
```
x  rd  M  x  idx  rd  "M[x] + x ="  pt  M  x  idx  x  +  pt
```
