#ifndef MYLIBRARY_STACK_H
#define MYLIBRARY_STACK_H

#include "Vector.h"
#include <cmath>
#include <cstring>
#include <cctype>
#include <string>
#include <stdexcept>
#include <sstream>

/* ---------- 基本栈 ---------- */
template <typename T>
class Stack : public Vector<T> {
public:
    void push(T const& e) { this->insert(this->size(), e); }
    T pop() { return this->remove(this->size() - 1); }
    T& top() { return (*this)[this->size() - 1]; }
    bool empty() const { return this->size() == 0; }
};

/* ---------- 计算器核心类 ---------- */
class Calculator {
private:
    Stack<float> numStack;    // 操作数栈
    Stack<char> opStack;      // 运算符栈
    
    // 检查字符是否为有效运算符
    bool isValidOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || 
               c == 's' || c == 'c' || c == 't' || c == 'q' || c == 'l' || c == 'L' ||
               c == '(' || c == ')';
    }
    
    // 检查字符是否为表达式有效字符
    bool isValidChar(char c) {
        return isdigit(c) || c == '.' || c == ' ' || c == '+' || c == '-' || 
               c == '*' || c == '/' || c == '^' || c == '(' || c == ')' ||
               c == 's' || c == 'i' || c == 'n' || c == 'c' || c == 'o' || 
               c == 't' || c == 'q' || c == 'r' || c == 'l' || c == 'g' || c == 'L';
    }
    
    // 运算符优先级
    int getPriority(char op) {
        switch (op) {
            case '+': case '-': return 1;
            case '*': case '/': return 2;
            case '^': return 3;
            case 's': case 'c': case 't': case 'q': case 'l': case 'L': return 4;
            case '(': case ')': return 0;
            default: return -1;
        }
    }
    
    // 执行运算
    float calculate(float a, char op, float b) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': 
                if (b == 0) throw std::runtime_error("Division by zero!");
                return a / b;
            case '^': return pow(a, b);
            default: throw std::runtime_error("Unknown operator!");
        }
    }
    
    float calculate(char func, float a) {
        switch (func) {
            case 's': return sin(a);
            case 'c': return cos(a);
            case 't': return tan(a);
            case 'q': 
                if (a < 0) throw std::runtime_error("Square root of negative number!");
                return sqrt(a);
            case 'l': 
                if (a <= 0) throw std::runtime_error("Log of non-positive number!");
                return log10(a);
            case 'L': 
                if (a <= 0) throw std::runtime_error("Ln of non-positive number!");
                return log(a);
            default: throw std::runtime_error("Unknown function!");
        }
    }
    
    // 处理运算符
    void processOperator(char op) {
        while (!opStack.empty() && getPriority(opStack.top()) >= getPriority(op)) {
            char topOp = opStack.pop();
            
            if (topOp == 's' || topOp == 'c' || topOp == 't' || topOp == 'q' || topOp == 'l' || topOp == 'L') {
                // 一元运算符
                if (!numStack.empty()) {
                    float a = numStack.pop();
                    numStack.push(calculate(topOp, a));
                }
            } else {
                // 二元运算符
                if (numStack.size() >= 2) {
                    float b = numStack.pop();
                    float a = numStack.pop();
                    numStack.push(calculate(a, topOp, b));
                }
            }
        }
        opStack.push(op);
    }
    
    // 读取数字 - 改进版本，检查格式
    float readNumber(const char*& expr) {
        std::string numStr;
        bool hasDecimal = false;
        int sign = 1;
        
        // 处理符号
        if (*expr == '-') {
            sign = -1;
            expr++;
        } else if (*expr == '+') {
            expr++;
        }
        
        // 读取数字部分
        while (isdigit(*expr) || *expr == '.') {
            if (*expr == '.') {
                if (hasDecimal) {
                    throw std::runtime_error("Multiple decimal points in number!");
                }
                hasDecimal = true;
            }
            numStr += *expr;
            expr++;
        }
        
        // 检查数字是否有效
        if (numStr.empty() || (numStr == ".")) {
            throw std::runtime_error("Invalid number format!");
        }
        
        // 转换为浮点数
        std::stringstream ss(numStr);
        float result;
        ss >> result;
        
        return sign * result;
    }
    
    // 读取函数名
    char readFunction(const char*& expr) {
        if (strncmp(expr, "sin", 3) == 0) { expr += 3; return 's'; }
        if (strncmp(expr, "cos", 3) == 0) { expr += 3; return 'c'; }
        if (strncmp(expr, "tan", 3) == 0) { expr += 3; return 't'; }
        if (strncmp(expr, "sqrt", 4) == 0) { expr += 4; return 'q'; }
        if (strncmp(expr, "log", 3) == 0) { expr += 3; return 'l'; }
        if (strncmp(expr, "ln", 2) == 0) { expr += 2; return 'L'; }
        throw std::runtime_error("Unknown function!");
    }
    
    // 验证表达式格式
    void validateExpression(const char* expression) {
        const char* expr = expression;
        bool expectOperand = true; // 开始时期望操作数
        int parenCount = 0;
        
        while (*expr) {
            // 跳过空格
            while (isspace(*expr)) expr++;
            if (!*expr) break;
            
            // 检查字符是否有效
            if (!isValidChar(*expr)) {
                std::string error = "Invalid character: ";
                error += *expr;
                throw std::runtime_error(error);
            }
            
            if (expectOperand) {
                // 期望操作数：数字、函数、左括号或一元运算符
                if (isdigit(*expr) || *expr == '.' || 
                    ((*expr == '+' || *expr == '-') && 
                     (expr == expression || *(expr-1) == '('))) {
                    // 验证数字格式
                    const char* temp = expr;
                    try {
                        readNumber(temp);
                    } catch (...) {
                        throw std::runtime_error("Invalid number format!");
                    }
                    expr = temp;
                    expectOperand = false;
                }
                else if (isalpha(*expr)) {
                    // 函数 - 修复：移除未使用的func变量
                    try {
                        readFunction(expr); // 直接调用，不使用返回值
                        expectOperand = true; // 函数后需要左括号
                    } catch (...) {
                        throw std::runtime_error("Invalid function!");
                    }
                }
                else if (*expr == '(') {
                    parenCount++;
                    expr++;
                }
                else {
                    throw std::runtime_error("Expected operand!");
                }
            }
            else {
                // 期望运算符：二元运算符或右括号
                if (*expr == '+' || *expr == '-' || *expr == '*' || *expr == '/' || *expr == '^') {
                    expr++;
                    expectOperand = true;
                }
                else if (*expr == ')') {
                    if (parenCount <= 0) {
                        throw std::runtime_error("Unmatched closing parenthesis!");
                    }
                    parenCount--;
                    expr++;
                }
                else {
                    throw std::runtime_error("Expected operator!");
                }
            }
        }
        
        if (expectOperand) {
            throw std::runtime_error("Expression ends with operator!");
        }
        
        if (parenCount != 0) {
            throw std::runtime_error("Unmatched parentheses!");
        }
    }

public:
    // 表达式求值主函数
    float evaluate(const char* expression) {
        // 验证表达式格式
        validateExpression(expression);
        
        // 清空栈
        while (!numStack.empty()) numStack.pop();
        while (!opStack.empty()) opStack.pop();
        
        const char* expr = expression;
        bool expectOperand = true;
        
        while (*expr) {
            // 跳过空格
            while (isspace(*expr)) expr++;
            
            if (!*expr) break;
            
            if (expectOperand) {
                if (isdigit(*expr) || *expr == '.' || 
                    ((*expr == '+' || *expr == '-') && 
                     (expr == expression || *(expr-1) == '('))) {
                    // 读取数字
                    float num = readNumber(expr);
                    numStack.push(num);
                    expectOperand = false;
                }
                else if (isalpha(*expr)) {
                    // 读取函数
                    char func = readFunction(expr);
                    opStack.push(func);
                }
                else if (*expr == '(') {
                    opStack.push('(');
                    expr++;
                }
                else {
                    throw std::runtime_error("Syntax error!");
                }
            }
            else {
                if (*expr == '+' || *expr == '-' || *expr == '*' || *expr == '/' || *expr == '^') {
                    processOperator(*expr);
                    expr++;
                    expectOperand = true;
                }
                else if (*expr == ')') {
                    // 处理括号内的所有运算
                    while (!opStack.empty() && opStack.top() != '(') {
                        char op = opStack.pop();
                        if (op == 's' || op == 'c' || op == 't' || op == 'q' || op == 'l' || op == 'L') {
                            if (!numStack.empty()) {
                                float a = numStack.pop();
                                numStack.push(calculate(op, a));
                            }
                        } else {
                            if (numStack.size() >= 2) {
                                float b = numStack.pop();
                                float a = numStack.pop();
                                numStack.push(calculate(a, op, b));
                            }
                        }
                    }
                    if (!opStack.empty() && opStack.top() == '(') {
                        opStack.pop();
                    }
                    expr++;
                }
                else {
                    throw std::runtime_error("Syntax error!");
                }
            }
        }
        
        // 处理剩余的运算符
        while (!opStack.empty()) {
            char op = opStack.pop();
            if (op == 's' || op == 'c' || op == 't' || op == 'q' || op == 'l' || op == 'L') {
                if (!numStack.empty()) {
                    float a = numStack.pop();
                    numStack.push(calculate(op, a));
                }
            } else {
                if (numStack.size() >= 2) {
                    float b = numStack.pop();
                    float a = numStack.pop();
                    numStack.push(calculate(a, op, b));
                }
            }
        }
        
        if (numStack.empty()) {
            throw std::runtime_error("No result!");
        }
        
        return numStack.pop();
    }
};

/* ---------- 工具函数 ---------- */

// 括号匹配检查
inline bool checkParentheses(const char* expr) {
    Stack<char> stack;
    for (int i = 0; expr[i]; i++) {
        if (expr[i] == '(') {
            stack.push('(');
        } else if (expr[i] == ')') {
            if (stack.empty()) return false;
            stack.pop();
        }
    }
    return stack.empty();
}

#endif // MYLIBRARY_STACK_H