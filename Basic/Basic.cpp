/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

LET::LET(std::string current_line) {
    this -> current_line = current_line;
}

LET::~LET(){}

void LET::execute(EvalState &state, Program &program) {
    scanner.setInput(current_line);
    std::string token = scanner.nextToken();
    while (token != "LET") {
        token = scanner.nextToken();
    }
    token = scanner.nextToken();
    std::string var = scanner.nextToken();
    if (!isVarName(var)) {
        error("SYNTAX ERROR");
    }
    token = scanner.nextToken();
    token = scanner.nextToken();
    token = scanner.nextToken();
    Expression* exp;
    int val;
    try {
        exp = parseExp(scanner);
        val = exp -> eval(state);
        state.setValue(var, val);
        delete exp;
    } catch (ErrorException &ex) {
        std::cout << ex.getMessage() << std::endl;
        delete exp;
    }
}

bool LET::isVarName(std::string var_name) {
    std::string forbid_var_name[] = {"REM", "LET", "PRINT", "INPUT", "END", "GOTO", "IF",
"THEN", "RUN", "LIST", "CLEAR", "QUIT", "HELP"};
    for (int i = 0; i < 13; i++) {
        if (var_name == forbid_var_name[i]) {
            return false;
        }
    }
    return true;
}

PRINT::PRINT(std::string current_line) {
    this -> current_line = current_line;
}

PRINT::~PRINT(){}

void PRINT::execute(EvalState &state, Program &program) {
    scanner.setInput(current_line);
    std::string token = scanner.nextToken();
    while (token != "PRINT") {
        token = scanner.nextToken();
    }
    token = scanner.nextToken();
    int val;
    Expression* exp;
    try {
        exp = parseExp(scanner);
        val = exp -> eval(state);
        std::cout << val << '\n';
        delete exp;
    } catch (ErrorException &ex) {
        std::cout << ex.getMessage() << std::endl;
        delete exp;
    }
}

INPUT::INPUT(std::string current_line) {
    this -> current_line = current_line;
}

INPUT::~INPUT(){}

void INPUT::execute(EvalState &state, Program &program) {
    scanner.setInput(current_line);
    std::string token = scanner.nextToken();
    while (token != "INPUT") {
        token = scanner.nextToken();
    }
    token = scanner.nextToken();
    token = scanner.nextToken();
    std::string input;
    while(true) {
        std::cout << " ? ";
        std::getline(std::cin, input);
        bool flag = true;
        if (!(input[0] >= '0' && input[0] <= '9' || input[0] == '-' && input.size() > 1)) {
            flag = false;
        }
        for (int i = 1; i < input.size(); i++) {
            if (!(input[i] >= '0' && input[i] <= '9')) {
                flag = false;
            }
        }
        if (flag) {
            state.setValue(token,std::stoi(input));
            break;
        }
        std::cout << "INVALID NUMBER\n";
    }
}

IF::IF(std::string current_line) {
    this -> current_line = current_line;
    next_line_number = -1;
}

IF::~IF(){}

void IF::execute(EvalState &state, Program &program) {
    scanner.setInput(current_line);
    std::string token = scanner.nextToken();
    while(scanner.getTokenType(token) != NUMBER) {
        token = scanner.nextToken();
    }
    current_line_number = std::stoi(token);
    while (token != "IF") {
        token = scanner.nextToken();
    }
    token = scanner.nextToken();
    int start_lhs = scanner.getPosition();
    token = scanner.nextToken();
    while (token != " ") {
        token = scanner.nextToken();
    }
    int end_lhs = scanner.getPosition() - 1;
    std::string op = scanner.nextToken();
    token = scanner.nextToken();
    int start_rhs = scanner.getPosition();
    token = scanner.nextToken();
    while (token != " ") {
        token = scanner.nextToken();
    }
    int end_rhs = scanner.getPosition() - 1;
    int lhs = calculate(state,current_line.substr(start_lhs, end_lhs - start_lhs));
    int rhs = calculate(state,current_line.substr(start_rhs, end_rhs - start_rhs));

    bool flag = false;
    if (op == "=") {
        if (lhs == rhs) {
            flag = true;
        }
    }
    if (op == "<") {
        if (lhs < rhs) {
            flag = true;
        }
    }
    if (op == ">") {
        if (lhs > rhs) {
            flag = true;
        }
    }

    if (flag) {
        while(token != "THEN") {
            token = scanner.nextToken();
        }
        token = scanner.nextToken();
        token = scanner.nextToken();
        if (program.find_line_number(std::stoi(token))) {
            next_line_number = std::stoi(token);
            return;
        }
        error("LINE NUMBER ERROR");
    }
    next_line_number = program.getNextLineNumber(current_line_number);
    //todo
}

int IF::calculate(EvalState &state,std::string expr) {
    TokenScanner cal;
    cal.setInput(expr);
    Expression* exp;
    int val;
    try {
        exp = parseExp(cal);
        val = exp -> eval(state);
        delete exp;
    } catch (ErrorException &ex) {
        std::cout << ex.getMessage() << std::endl;
        delete exp;
    }
    return val;
}

GOTO::GOTO(std::string current_line) {
    this -> current_line = current_line;
}

GOTO::~GOTO(){}

void GOTO::execute(EvalState &state, Program &program) {
    scanner.setInput(current_line);
    std::string token = scanner.nextToken();
    current_line_number = std::stoi(token);
    token = scanner.nextToken();
    while(scanner.getTokenType(token) != NUMBER) {
        token = scanner.nextToken();
    }
    if (program.find_line_number(std::stoi(token))) {
        next_line_number = std::stoi(token);
    }
    else {
        error("LINE NUMBER ERROR");
    }
}

//todo
