/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

std::string standardize_print(std::string original_line) {
    int bracket = 0;
    for (int i = 0; i < original_line.size(); i++) {
        if (original_line[i] == '(') {
            bracket++;
        }
        if (original_line[i] == ')') {
            bracket--;
            if (bracket < 0) {
                error("SYNTAX ERROR");
            }
        }
    }
    if (bracket) {
        error("SYNTAX ERROR");
    }
    int p = 4;
    while (original_line.substr(p - 4, 5) != "PRINT") {
        p++;
    }
    p++;
    while (original_line[p] == ' ') {
        p++;
    }
    int cur = p;
    while (p < original_line.length()) {
        if (original_line[p] == '=') {
            error("SYNTAX ERROR");
        }
        if (original_line[p] != ' ') {
            original_line[cur] = original_line[p];
            cur++;
        }
        p++;
    }
    return original_line.substr(0, cur);
}

std::string standardize_let(std::string original_line) {
    int p = 0;
    while (original_line[p] != '=') {
        p++;
    }
    p++;
    while (original_line[p] == ' ') {
        p++;
    }
    p++;
    int cur = p;
    while (p < original_line.size()) {
        if (original_line[p] != ' ') {
            original_line[cur] = original_line[p];
            cur++;
        }
        p++;
    }
    return original_line.substr(0,cur);
}

std::string standardize_if(std::string original_line) {
    int p = 1;
    while (original_line.substr(p - 1,2) != "IF") {
        p++;
    }
    p++;
    int cur = p;
    while (original_line[p] == ' ') {
        p++;
    }
    original_line[cur] = ' ';
    cur++;
    while (p < original_line.size() - 1 && original_line.substr(p,2) != " =" && original_line.substr(p,2) != " <" && original_line.substr(p,2) != " >") {
        if (original_line[p] != ' ') {
            original_line[cur] = original_line[p];
            cur++;
        }
        p++;
    }
    if (p == original_line.size() - 1) {
        error("SYNTAX ERROR");
    }

    original_line[cur++] = original_line[p++];
    original_line[cur++] = original_line[p++];

    while (original_line[p] == ' ') {
        p++;
    }
    original_line[cur++] = ' ';

    while (original_line.substr(p,5) != " THEN") {
        if (original_line[p] != ' ') {
            if (original_line[p] == '>' || original_line[p] == '=' || original_line[p] == '<') {
                error("SYNTAX ERROR");
            }
            original_line[cur] = original_line[p];
            cur++;
        }
        p++;
    }

    while (p < original_line.size()) {
        original_line[cur++] = original_line[p++];
    }

    return original_line.substr(0, cur);
}
void standardize_goto(std::string original_line) {
    int p = 4;
    while (original_line.substr(p - 4, 4) != "GOTO"){
        p++;
    }
    while (original_line[p] == ' ') {
        p++;
    }
    while (p < original_line.size()) {
        if (!isdigit(original_line[p])) {
            error("SYNTAX ERROR");
        }
        p++;
    }
}

void standardize_end(std::string original_line) {
    int p = 3;
    while (original_line.substr(p - 3, 3) != "END"){
        p++;
    }
    if (p != original_line.size()) {
        error("SYNTAX ERROR");
    }
}


void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);
    std::string token;
    TokenType type;

    token = scanner.nextToken();
    type = scanner.getTokenType(token);
    if (type == WORD) {
        if (token == "PRINT") {
            std::string tmp_line = standardize_print(line);
            line = tmp_line;
            PRINT(line).execute(state, program);
        }
        else if (token == "LET") {
            std::string tmp_line = standardize_let(line);
            line = tmp_line;
            LET(line).execute(state, program);
        }
        else if (token == "INPUT") {
            INPUT(line).execute(state, program);
        }
        else if (token == "QUIT") {
            if (line.size() != 4) {
                error("SYNTAX ERROR");
            }
            exit(0);
        }
        else if (token == "CLEAR") {
            if (line.size() != 5) {
                error("SYNTAX ERROR");
            }
            program.clear();
            state.Clear();
        }
        else if (token == "LIST") {
            if (line.size() != 4) {
                error("SYNTAX ERROR");
            }
            int line_number = program.getFirstLineNumber();
            while (line_number != -1) {
                std::cout << program.getOriginalLine(line_number) << '\n';
                line_number = program.getNextLineNumber(line_number);
            }
        }
        else if (token == "RUN") {
            if (line.size() != 3) {
                error("SYNTAX ERROR");
            }
            int line_number = program.getFirstLineNumber();
            while(true) {
                if (line_number == -1) {
                    break;
                }
                std::string cur_line = program.getSourceLine(line_number);
                scanner.setInput(cur_line);
                token = scanner.nextToken();
                while (scanner.getTokenType(token) != WORD) {
                    token = scanner.nextToken();
                }
                if (token == "REM") {
                    line_number = program.getNextLineNumber(line_number);
                }
                if (token == "END") {
                    break;
                }
                if (token == "PRINT") {
                    PRINT exe = cur_line;
                    exe.execute(state, program);
                    line_number = program.getNextLineNumber(line_number);
                }
                if (token == "LET") {
                    LET exe = cur_line;
                    exe.execute(state, program);
                    line_number = program.getNextLineNumber(line_number);
                }
                if (token == "INPUT") {
                    INPUT exe = cur_line;
                    exe.execute(state, program);
                    line_number = program.getNextLineNumber(line_number);
                }
                if (token == "IF") {
                    IF exe = cur_line;
                    exe.execute(state, program);
                    line_number = exe.next_line_number;
                }
                if (token == "GOTO") {
                    GOTO exe = cur_line;
                    exe.execute(state, program);
                    line_number = exe.next_line_number;
                }
            }
        }
        else if (token != "REM") {
            error("SYNTAX ERROR");
        }
    }
    else if (type == NUMBER) {
        int line_id = std::stoi(token);
        std::string original_line = line;
        if (!scanner.hasMoreTokens()) {
            program.removeSourceLine(line_id);
            return;
        }
        token = scanner.nextToken();
        if (token == "PRINT") {
            std::string tmp_line = standardize_print(line);
            line = tmp_line;
        }
        else if (token == "LET") {
            std::string tmp_line = standardize_let(line);
            line = tmp_line;
        }
        else if (token == "IF") {
            std::string tmp_line = standardize_if(line);
            line = tmp_line;
        }
        else if (token == "GOTO") {
            standardize_goto(line);
        }
        else if (token == "END") {
            standardize_end(line);
        }
        else if (token != "REM"  && token != "INPUT") {
            error("SYNTAX ERROR");
        }
        program.addSourceLine(line_id, line, original_line);
    }
    //todo
}

