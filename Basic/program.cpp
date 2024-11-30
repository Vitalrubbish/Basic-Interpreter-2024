/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() = default;

void Program::clear() {
    sentence.clear();
    line_number.clear();
    // Replace this stub with your own code
    //todo
}

void Program::addSourceLine(int lineNumber, const std::string &line, const std::string &original_line) {
    // Replace this stub with your own code
    //todo
    sentence[lineNumber] = line;
    original[lineNumber] = original_line;
    if (line_number.find(lineNumber) == line_number.end()){
        line_number.insert(lineNumber);
    }
}

void Program::removeSourceLine(int lineNumber) {
    if (line_number.find(lineNumber) != line_number.end()) {
        line_number.erase(lineNumber);
        sentence.erase(lineNumber);
    }
    // Replace this stub with your own code
    //todo
}

std::string Program::getSourceLine(int lineNumber) {
    return sentence[lineNumber];
    // Replace this stub with your own code
    //todo
}

int Program::getFirstLineNumber() {
    // Replace this stub with your own code
    if (line_number.size() == 0) {
        return -1;
    }
    return *line_number.begin();
    //todo
}

int Program::getNextLineNumber(int lineNumber) {
    auto p = line_number.find(lineNumber);
    p++;
    if (p == line_number.end()) {
        return -1;
    }
    return *p;
    // Replace this stub with your own code
    //todo
}

bool Program::find_line_number(int number) {
    if (line_number.find(number) != line_number.end()) {
        return true;
    }
    return false;
}

std::string Program::getOriginalLine(int lineNumber) {
    return original[lineNumber];
}
//more func to add
//todo


