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
    Expression* exp = parseExp(scanner);
    int val = exp -> eval(state);
    state.setValue(token, val);
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
    Expression* exp = parseExp(scanner);
    std::cout << exp -> eval(state) << '\n';
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
    std::getline(std::cin, input);
    state.setValue(token,std::stoi(input));
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
    int end_lhs = scanner.getPosition();
    token = scanner.nextToken();
    std::string op = scanner.nextToken();
    token = scanner.nextToken();
    int start_rhs = scanner.getPosition();
    token = scanner.nextToken();
    int end_rhs = scanner.getPosition();
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
        next_line_number = std::stoi(token);
    } else {
        next_line_number = program.getNextLineNumber(current_line_number);
    }
    //todo
}

int IF::calculate(EvalState &state,std::string expr) {
    TokenScanner cal;
    cal.setInput(expr);
    Expression* exp = parseExp(cal);
    return exp -> eval(state);
}

GOTO::GOTO(std::string current_line) {
    this -> current_line = current_line;
}

GOTO::~GOTO(){}

void GOTO::execute(EvalState &state, Program &program) {
    scanner.setInput(current_line);
    std::string token = scanner.nextToken();
    while(scanner.getTokenType(token) != NUMBER) {
        token = scanner.nextToken();
    }
    next_line_number = std::stoi(token);
}

//todo
