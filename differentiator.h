//
// Created by mike on 22.11.18.
//

#ifndef DED_DIFFERENTIATOR_DIFFERENTIATOR_H
#define DED_DIFFERENTIATOR_DIFFERENTIATOR_H

#include <cstring>
#include <cctype>
#include <cstdlib>
#include <vector>

#include "exception.h"

enum NodeType {
  REAL_NUMBER,
  OPER_PLUS,
  OPER_MINUS,
  OPER_MUL,
  OPER_DIV,
  VARIABLE
};

NodeType getNodeTypeByOper(char oper) {
  switch (oper) {
    case '+':
      return OPER_PLUS;
    case '-':
      return OPER_MINUS;
    case '*':
      return OPER_MUL;
    case '/':
      return OPER_DIV;
    default:
      throw IncorrectArgumentException(std::string("no such operator provided: ") + oper,
                                       __PRETTY_FUNCTION__);
  }
}

struct Node {
  NodeType type;
  double result{0.0};
  char var_name{'0'};

  Node* left_son{nullptr};
  Node* right_son{nullptr};

  Node(NodeType type): type(type) {}

  Node(NodeType type, double result):
    type(type), result(result) {}

  Node(NodeType type, double result, char var_name):
    type(type), result(result), var_name(var_name) {}

  Node(NodeType type, Node* left_son, Node* right_son):
    type(type) {
    if ((type == REAL_NUMBER || type == VARIABLE) &&
       (left_son != nullptr || right_son != nullptr)) {
      throw IncorrectArgumentException("node of such type should be a leaf",
                                       __PRETTY_FUNCTION__);
    }
  }

  void setSons(Node* L, Node* R) {
    left_son = L;
    right_son = R;
  }

  void destroy() {
    delete left_son;
    delete right_son;
  }

  ~Node() {
    destroy();
  }
};

bool isOperator(char ch) {
  return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

bool isVariable(char ch) {
  return ch >= 'a' && ch <= 'z';
}

bool isCharForDouble(char ch) {
  return isdigit(ch) || ch == '.';
}

class Tree {
 private:
  Node* root_{nullptr};

  double parseDouble(const std::string& str, size_t* pos) {
    size_t cur_pos = *pos;

    while (cur_pos + 1 < str.size() && isCharForDouble(str[cur_pos + 1])) {
      ++cur_pos;
    }
    double res_value = atof(str.substr(*pos, cur_pos - *pos + 1).c_str());
    *pos = cur_pos;
    return res_value;
  }

  Node* getParentNode(Node* left_son, Node* right_son, NodeType node_type) {
    Node* result_node = nullptr;

    switch (node_type) {
      case OPER_PLUS:
        result_node = new Node{OPER_PLUS};
        break;
      case OPER_MINUS:
        result_node = new Node{OPER_MINUS};
        break;
      case OPER_MUL:
        result_node = new Node{OPER_MUL};
        break;
      case OPER_DIV:
        result_node = new Node{OPER_DIV};
        break;
      default:
        throw IncorrectArgumentException("an operator was expected", __PRETTY_FUNCTION__);
    }
    result_node->setSons(left_son, right_son);

    return result_node;
  }

  Node* calcNode(Node* left_son, Node* right_son, NodeType node_type) {
    Node* result_node = new Node(REAL_NUMBER);
    double left_value = left_son->result;
    double right_value = right_son->result;

    switch (node_type) {
      case OPER_PLUS:
        result_node = new Node{REAL_NUMBER, left_value + right_value};
        break;
      case OPER_MINUS:
        result_node = new Node{REAL_NUMBER, left_value - right_value};
        break;
      case OPER_MUL:
        result_node = new Node{REAL_NUMBER, left_value * right_value};
        break;
      case OPER_DIV:
        if (right_value == 0.0) {
          throw DivisionByZeroException("", __PRETTY_FUNCTION__);
        }
        result_node = new Node{REAL_NUMBER, left_value / right_value};
        break;
      default:
        throw IncorrectArgumentException("an operator was expected", __PRETTY_FUNCTION__);
    }
    return result_node;
  }

  void makeNiceVariable(Node* node, Node* left_son, Node* right_son, bool is_swapped = false) {

    if ((node->type == OPER_PLUS || node->type == OPER_MINUS) &&
         right_son->result == 0.0) {
      *node = *left_son;
      node->left_son = node->right_son = nullptr;
      return;
    }
    if (node->type == OPER_MUL && right_son->result == 1.0) {
      *node = *left_son;
      node->left_son = node->right_son = nullptr;
      return;
    }
    if (node->type == OPER_MUL && right_son->result == 1.0) {
      *node = Node(REAL_NUMBER, 0.0);
      node->left_son = node->right_son = nullptr;
      return;
    }
    if (!is_swapped && node->type == OPER_DIV && right_son->result == 0.0) {
      *node = *left_son;
      node->left_son = node->right_son = nullptr;
    } else if (is_swapped && node->type == OPER_DIV && left_son->result == 0.0) {
      *node = *right_son;
      node->left_son = node->right_son = nullptr;
    }
  }

  void processOpers(std::vector<char>* was_opers, std::vector<Node*>* was_operands) {
    std::vector<char>& opers = *was_opers;
    std::vector<Node*>& operands = *was_operands;

    while (!opers.empty() && operands.size() >= 2) {
      Node* operand_b = operands.back(); operands.pop_back();
      Node* operand_a = operands.back(); operands.pop_back();
      char oper = opers.back(); opers.pop_back();

      operands.push_back(getParentNode(operand_a, operand_b,
                                       getNodeTypeByOper(oper)));
    }
  }

  void printPrefixRec(Node* cur_node) {
    if (cur_node == nullptr) {
      return;
    }

    switch (cur_node->type) {
      case VARIABLE:
        std::cout << cur_node->var_name << ' ';
        break;
      case REAL_NUMBER:
        std::cout << cur_node->result << ' ';
        break;
      case OPER_PLUS:
        std::cout << '+' << ' ';
        break;
      case OPER_MINUS:
        std::cout << '-' << ' ';
        break;
      case OPER_MUL:
        std::cout << '*' << ' ';
        break;
      case OPER_DIV:
        std::cout << '/' << ' ';
        break;
    }
    printPrefixRec(cur_node->left_son);
    printPrefixRec(cur_node->right_son);
  }

  void printInfixSubtree(Node* subtree_root) {
    if (subtree_root && subtree_root->type != REAL_NUMBER && subtree_root->type != VARIABLE) {
      std::cout << '(';
    }
    printInfixRec(subtree_root);
    if (subtree_root && subtree_root->type != REAL_NUMBER && subtree_root->type != VARIABLE) {
      std::cout << ')';
    }
  }

  void printInfixRec(Node* cur_node) {
    if (cur_node == nullptr) {
      return;
    }

    printInfixSubtree(cur_node->left_son);
    switch (cur_node->type) {
      case VARIABLE:
        std::cout << cur_node->var_name;
        break;
      case REAL_NUMBER:
        std::cout << cur_node->result;
        break;
      case OPER_PLUS:
        std::cout << '+';
        break;
      case OPER_MINUS:
        std::cout << '-';
        break;
      case OPER_MUL:
        std::cout << '*';
        break;
      case OPER_DIV:
        std::cout << '/';
        break;
    }
    printInfixSubtree(cur_node->right_son);
  }

  Tree(Node* root_node): root_(root_node) {}

  Tree recCopy(Node* node) const {
    if (!node) {
      return nullptr;
    }

    Node* result = new Node(node->type, node->result, node->var_name);
    Tree left_tree = recCopy(node->left_son);
    Tree right_tree = recCopy(node->right_son);

    return Tree(new Node(node->type, left_tree.root_, right_tree.root_));
  }

  void simplify(Node*& node) {
    if (node == nullptr) {
      return;
    }

    simplify(node->left_son);
    simplify(node->right_son);
    if (node->left_son != nullptr && node->right_son != nullptr) {
      if (node->left_son->type == REAL_NUMBER && node->right_son->type == REAL_NUMBER) {
        node = calcNode(node->left_son, node->right_son, node->type);
      } else if (node->left_son->type == VARIABLE) {
        makeNiceVariable(node, node->left_son, node->right_son);
      } else if (node->right_son->type == VARIABLE) {
        makeNiceVariable(node, node->right_son, node->left_son, true);
      }
    }
  }

  Tree diffirentiateRec(Node* node, char variable) {
    if (node == nullptr) {
      return Tree(node);
    }

    std::cout << "диффирнём немножечко \n";
    std::cout << node->type << ' ' << node->var_name << '\n';

    if (node->type == REAL_NUMBER || (node->type == VARIABLE && node->var_name != variable)) {
      std::cout << "differ is finished\n";
      return Tree(new Node(REAL_NUMBER, 0.0));
    } else if (node->type == VARIABLE && node->var_name == variable) {
      std::cout << "differ is finished\n";
      return Tree(new Node(REAL_NUMBER, 1.0));
    }

    switch (node->type) {
      case OPER_PLUS:
      {
        Tree left_diff = diffirentiateRec(node->left_son, variable).root_;
        Tree right_diff = diffirentiateRec(node->right_son, variable).root_;


        return left_diff + right_diff;
      }
      case OPER_MINUS:
      {
        Tree left_diff = diffirentiateRec(node->left_son, variable).root_;
        Tree right_diff = diffirentiateRec(node->right_son, variable).root_;

        return left_diff - right_diff;
      }
      case OPER_MUL:
      {
        Tree left_diff = diffirentiateRec(node->left_son, variable).root_;
        Tree right_diff = diffirentiateRec(node->right_son, variable).root_;
        Tree left_subtree = Tree(node->left_son).copy();
        Tree right_subtree = Tree(node->right_son).copy();

        return left_diff * right_subtree + left_subtree * right_diff;
      }
      default:
        throw IncorrectParsingException("unknown node type", __PRETTY_FUNCTION__);

    }
  }

  Tree applyOper(const Tree& another, NodeType oper_type) const {
    std::cout << "I'm trying to apply an operator\n";

    Tree left_son = this->copy();
    std::cout << "left_son was copied\n";
    Tree right_son = another.copy();
    std::cout << "right_son was copied\n";

    return Tree(new Node(oper_type, left_son.root_, right_son.root_));
  }

 public:

  Tree(const std::string& polish_notation) {
    //std::cout << "polish in " << polish_notation << '\n';

    std::vector<char> opers;
    std::vector<Node*> operands;

    for (size_t char_id = 0; char_id < polish_notation.size(); ++char_id) {
      char cur_ch = polish_notation[char_id];

      if (std::isspace(cur_ch)) {
        continue;
      }
      if (isOperator(cur_ch)) {
        opers.push_back(cur_ch);
      } else if (isCharForDouble(cur_ch)) {
        double value = parseDouble(polish_notation, &char_id);

        operands.push_back(new Node(REAL_NUMBER, value));
      } else if (isVariable(cur_ch)) {
        operands.push_back(new Node(VARIABLE, 1, cur_ch));
      } else {
        throw IncorrectArgumentException(std::string("incorrect operator or variable: ") + cur_ch,
                                         __PRETTY_FUNCTION__);
      }

      processOpers(&opers, &operands);
    }

    if (operands.size() != 1) {
      throw IncorrectParsingException("invalid number of tree roots: " + std::to_string(operands.size()),
                                      __PRETTY_FUNCTION__);
    }

    root_ = operands.back();
    simplify(root_);
  }

  Tree copy() const {
    Tree result(nullptr);

    return recCopy(root_);
  }

  void printPrefixNotation() {
    printPrefixRec(root_);
    std::cout << '\n';
  }

  void printInfixNotation() {
    printInfixRec(root_);
    std::cout << '\n';
  }

  Tree derivative(char variable = 'x') {
    return diffirentiateRec(root_, variable);
  }

  Tree operator+(const Tree& another) const {
    return applyOper(another, OPER_PLUS);
  }

  Tree operator-(const Tree& another) const {
    return applyOper(another, OPER_MINUS);
  }

  Tree operator*(const Tree& another) const {
    return applyOper(another, OPER_MUL);
  }

  Tree operator/(const Tree& another) const {
    return applyOper(another, OPER_DIV);
  }

  ~Tree() {
    if (root_ != nullptr) {
      root_->destroy();
      delete root_;
    }
  }
};

#endif //DED_DIFFERENTIATOR_DIFFERENTIATOR_H
