//
// Created by mike on 22.11.18.
//

#ifndef DED_DIFFERENTIATOR_EXCEPTION_H
#define DED_DIFFERENTIATOR_EXCEPTION_H

#include <iostream>
#include <exception>
#include <string>

struct DifferentiatorException : public std::exception {
  std::string message;
  std::string function_name;

  DifferentiatorException(const std::string& message = "", const std::string& function_name = ""):
    message(message), function_name(function_name) {}
};

struct IncorrectArgumentException : public DifferentiatorException {
  IncorrectArgumentException(const std::string& message, const std::string& function_name = ""):
    DifferentiatorException(message, function_name) {}
};

struct IncorrectParsingException : public DifferentiatorException {
  IncorrectParsingException(const std::string& message, const std::string& function_name = ""):
    DifferentiatorException(message, function_name) {}
};

struct DivisionByZeroException : public DifferentiatorException {
  DivisionByZeroException(const std::string& message, const std::string& function_name = ""):
    DifferentiatorException(message, function_name) {}
};

std::ostream& operator<<(std::ostream& os, const DifferentiatorException& iaexception) {
  os << "!!! Exception: " << iaexception.message;
  if (!iaexception.function_name.empty()) {
    os << "(" << iaexception.function_name << ")";
  }
  os << '\n';
  return os;
}

#endif //DED_DIFFERENTIATOR_EXCEPTION_H
