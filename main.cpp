#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>

#include "differentiator.h"
#include "common_classes.h"

StackAllocator<Node> Tree::allocator_ = StackAllocator<Node>();

int main(int argc, char* argv[]) {
  try {
    std::cout << "func dot file: " << argv[1] << '\n';
    std::cout << "derivative dot file: " << argv[2] << '\n';
    std::cout << "step-by-step solution file: " << argv[3] << '\n';

    SmartFile func_dot_file(argv[1], "w");
    SmartFile der_dot_file(argv[2], "w");
    SmartFile latex_file(argv[3], "w");

    std::string polish_notation;
    getline(std::cin, polish_notation);


    Tree t(polish_notation);
    t.printInfixNotation();
    t.draw(func_dot_file.getFile());

    Tree der_t = t.derivative('x', latex_file.getFile());

    std::cout << "Искомая производная посчитана:\n";
    der_t.printInfixNotation();
    der_t.draw(der_dot_file.getFile());
  } catch (DifferentiatorException& exc) {
    std::cerr << exc << '\n';
  }
  return 0;
}