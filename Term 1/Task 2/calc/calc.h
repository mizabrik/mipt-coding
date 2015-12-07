#ifndef CALC_H_
#define CALC_H_

#include <string>
#include <vector>
#include <stack>

// Defines where to put braces in sequence of same operators:
// around left or right operand
enum class Association { kLeft, kRight };

// Compute expression, given in string 'expression'
double Calculate(std::string expression);


// Used to store parsed expression, broken into numbers and operators
class Token {
 public:
  enum class Type {kOperator, kNumber};

  Token(double number);
  Token(char op);

  Token::Type type();
  
  // Return stored number, throws exception upon type mismatch
  double number();

  // Return stored operator, throws exception upon type mismatch
  char op();

 private:
  Token::Type type_;

  union {
    double number_;
    char op_;
  };
};

// Parse string into tokens
std::vector<Token> Parse(const char *expression);

// Returns operator priority (operators with lower priority are executed first)
// or -1 if char is not operator
int OperatorPriority(char c);

// Get digit from char or -1 if it's not digit
int GetDigit(char c);

Association GetAssociationType(char op);

// Execute operation, corresponding to operator
void Execute(char op, std::stack<double> &stack);

#endif // CALC_H_
