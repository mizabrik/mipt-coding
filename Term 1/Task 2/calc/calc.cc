#include "calc.h"

#include <cmath>

#include <string>
#include <stack>

#include <iostream>

double Calculate(std::string expression) {
  auto tokens = Parse(expression.c_str());

  std::stack<double> stack;
  std::stack<char> operators;

  bool got_number = false; // used to treat unary minus

  for (auto token : tokens) {
    if (token.type() == Token::Type::kNumber) {
      stack.push(token.number());
      got_number = true;
    } else {
      char op = token.op();
      if (op == '(') {
        got_number = false;
        operators.push(op);
      } else if (op == ')') {
        while (operators.top() != '(') {
          Execute(operators.top(), stack);
          operators.pop();
        }
        operators.pop();

      } else {
        // Dirty hack to support unary minus
        if (!got_number && op == '-')
          stack.push(0);

        while (operators.top() != '('
                && (OperatorPriority(op) > OperatorPriority(operators.top())
                    || (OperatorPriority(op) == OperatorPriority(operators.top())
                        && GetAssociationType(op) == Association::kLeft))) {
          Execute(operators.top(), stack);
          operators.pop();
        }
        operators.push(op);
        got_number = false;
      }
    }
  }
  
  return stack.top();
}

std::vector<Token> Parse(const char *expression) {
  enum class State { kDefault, kNumber, kFinished };

  State state = State::kDefault;
  std::vector<Token> tokens;
  double number;

  tokens.push_back(Token('('));
  //for (char c : expression) {
  while (state != State::kFinished) {
    char c = *expression;
    ++expression;

    switch (state) {
      case State::kDefault:
        if (c == ' ') {
          continue;
        } else if (OperatorPriority(c) != -1) {
          tokens.push_back(Token(c));
        } else if (GetDigit(c) != -1) {
          number = GetDigit(c);
          state = State::kNumber;
        } else if (c == '\0') {
          state = State::kFinished;
        } else {
          throw 1; // TODO: throw exception
        }
        break;

      case State::kNumber:
        if (GetDigit(c) != -1) {
          number *= 10;
          number += GetDigit(c);
        } else {
          tokens.push_back(Token(number));
          state = State::kDefault;

          if (c == ' ') {
            continue;
          } else if (OperatorPriority(c) != -1) {
            tokens.push_back(Token(c));
          } else if (c == '\0') {
            state = State::kFinished;
          } else {
            throw 1;// TODO: throw exception
          }
        }
        break;

      case State::kFinished:
        break;
    }
  }
  tokens.push_back(Token(')'));

  return tokens;
}

Token::Token(double number) : type_(Token::Type::kNumber), number_(number) {}
Token::Token(char op) : type_(Token::Type::kOperator), op_(op) {}

Token::Type Token::type() {
  return type_;
}

double Token::number() {
  if (type_ != Token::Type::kNumber)
    throw 0;

  return number_;
}

char Token::op() {
  if (type_ != Token::Type::kOperator)
    throw 0;

  return op_;
}

int OperatorPriority(char c) {
  switch (c) {
    case '(':
      return 100;
      break;

    case ')':
      return 0;
      break;
    case '^':
      return 1;
      break;

    case '*':
    case '/':
      return 5;
      break;

    case '+':
    case '-':
      return 10;
      break;

    default:
      return -1;
  }
}

int GetDigit(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  else
    return -1;
}

void Execute(char op, std::stack<double> &stack) {
  double operand2 = stack.top();
  stack.pop();
  double operand1 = stack.top();
  stack.pop();

  switch (op) {
    case '^':
      stack.push(std::pow(operand1, operand2));
      break;
    case '+':
      stack.push(operand1 + operand2);
      break;
    case '-':
      stack.push(operand1 - operand2);
      break;
    case '*':
      stack.push(operand1 * operand2);
      break;
    case '/':
      stack.push(operand1 / operand2);
      break;
  }
}

Association GetAssociationType(char op) {
  return op == '^' ? Association::kRight : Association::kLeft;
}
