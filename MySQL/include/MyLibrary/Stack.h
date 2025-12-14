#ifndef MYLIBRARY_STACK_H
#define MYLIBRARY_STACK_H

#include "Vector.h"
#include <cmath>
#include <cstring>
#include <cctype>
#include <cstdio>

template <typename T>
class Stack : public Vector<T> {
public:
    using Vector<T>::size;  // 显式引入基类成员
    using Vector<T>::insert;
    using Vector<T>::remove;
    using Vector<T>::empty;
    using Vector<T>::operator[];
    
    void push(T const& e) { this->insert(this->size(), e); }
    T pop() { return this->remove(this->size() - 1); }
    T& top() { return (*this)[this->size() - 1]; }
};

/* ---------- 进制转换 / 括号匹配 ---------- */
template <typename T>
void convert(Stack<T>& S, long long n, int base) {
    static char digit[] = "0123456789ABCDEF";
    while (n > 0) { 
        S.push(digit[n % base]); 
        n /= base; 
    }
}

inline bool paren(const char exp[]) {
    Stack<char> S;
    for (int i = 0; exp[i]; ++i) {
        switch (exp[i]) {
            case '(': case '[': case '{': 
                S.push(exp[i]); 
                break;
            case ')': 
                if (S.empty() || S.pop() != '(') return false; 
                break;
            case ']': 
                if (S.empty() || S.pop() != '[') return false; 
                break;
            case '}': 
                if (S.empty() || S.pop() != '{') return false; 
                break;
            default: 
                break;
        }
    }
    return S.empty();
}

/* ---------- 字符串处理函数 ---------- */
inline float myAtof(char*& s) {
    float intp = 0.0f, frac = 0.0f, base = 0.1f;
    int sign = 1;
    
    // 跳过空格
    while (isspace(*s)) ++s;
    
    // 处理符号
    if (*s == '-') { 
        sign = -1; 
        ++s; 
    } else if (*s == '+') {
        ++s;
    }
    
    // 整数部分
    while (*s >= '0' && *s <= '9') { 
        intp = intp * 10 + (*s - '0'); 
        ++s; 
    }
    
    // 小数部分
    if (*s == '.') {
        ++s;
        while (*s >= '0' && *s <= '9') { 
            frac += base * (*s - '0'); 
            base *= 0.1f; 
            ++s; 
        }
    }
    
    return sign * (intp + frac);
}

inline int myLen(const char* p) { 
    int n = 0; 
    while (*p++) ++n; 
    return n; 
}

inline void myCat(char* dst, const char* src) {
    while (*dst) ++dst;
    while ((*dst = *src)) { 
        ++dst; 
        ++src; 
    }
}

/* ---------- RPN 相关函数 ---------- */
inline void append(char*& RPN, char op) {
    if (!RPN) return;
    
    size_t len = strlen(RPN);
    if (len < 255) {  // 假设缓冲区足够大
        RPN[len] = op;
        RPN[len + 1] = '\0';
    }
}

inline void append(char*& RPN, float num) {
    if (!RPN) return;
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f ", num);
    strcat(RPN, buffer);
}

inline void readNumber(char*& S, Stack<float>& opnd) {
    float number = myAtof(S);
    opnd.push(number);
}

/* ---------- 计算函数 ---------- */
inline float calcu(char op, float a) {
    switch (op) {
        case '!': { // 阶乘
            int n = static_cast<int>(a);
            if (n < 0) return 0;
            float result = 1;
            for (int i = 2; i <= n; ++i) result *= i;
            return result;
        }
        case 's': return sin(a);        // sin
        case 'c': return cos(a);        // cos  
        case 't': return tan(a);        // tan
        case 'q': return sqrt(a);       // sqrt
        case 'L': return log(a);        // ln
        case 'l': return log10(a);      // log10
        default: return a;
    }
}

inline float calcu(float a, char op, float b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) return 0;  // 避免除零错误
            return a / b;
        case '^': return pow(a, b);
        case 'M': return (a > b) ? a : b; // max
        case 'R': return (a < b) ? a : b; // min
        default: return 0;
    }
}

/* ---------- 扩展的优先级表 ---------- */
inline char orderBetween(char top, char cur) {
    // 定义运算符索引
    enum OpIndex {
        OP_PLUS, OP_MINUS, OP_MULT, OP_DIV, OP_POW, OP_FACT, 
        OP_SIN, OP_COS, OP_TAN, OP_SQRT, OP_LOG, OP_MAX, OP_MIN,
        OP_LPAR, OP_RPAR, OP_END, OP_COUNT
    };
    
    static bool initialized = false;
    static char priority[OP_COUNT][OP_COUNT];
    
    if (!initialized) {
        // 初始化优先级表
        const char defaultPriority[OP_COUNT][OP_COUNT] = {
            // +    -    *    /    ^    !    s    c    t    q    L    M    R    (    )   \0
            { '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // +
            { '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // -
            { '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // *
            { '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // /
            { '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // ^
            { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>' }, // !
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // s
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // c
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // t
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // q
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // L
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // M
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>' }, // R
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', ' ' }, // (
            { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }, // )
            { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '=' }  // \0
        };
        
        memcpy(priority, defaultPriority, sizeof(priority));
        initialized = true;
    }
    
    // 字符到索引的映射
    auto charToIndex = [](char c) -> int {
        switch (c) {
            case '+': return OP_PLUS;
            case '-': return OP_MINUS;
            case '*': return OP_MULT;
            case '/': return OP_DIV;
            case '^': return OP_POW;
            case '!': return OP_FACT;
            case 's': return OP_SIN;
            case 'c': return OP_COS;
            case 't': return OP_TAN;
            case 'q': return OP_SQRT;
            case 'L': return OP_LOG;
            case 'M': return OP_MAX;
            case 'R': return OP_MIN;
            case '(': return OP_LPAR;
            case ')': return OP_RPAR;
            case '\0': return OP_END;
            default: return -1;
        }
    };
    
    int topIdx = charToIndex(top);
    int curIdx = charToIndex(cur);
    
    if (topIdx == -1 || curIdx == -1) {
        return ' ';  // 未知运算符
    }
    
    return priority[topIdx][curIdx];
}

/* ---------- 函数名识别 ---------- */
inline bool isFunction(char* S) {
    if (strncmp(S, "sin", 3) == 0) return true;
    if (strncmp(S, "cos", 3) == 0) return true;
    if (strncmp(S, "tan", 3) == 0) return true;
    if (strncmp(S, "sqrt", 4) == 0) return true;
    if (strncmp(S, "log", 3) == 0) return true;
    if (strncmp(S, "ln", 2) == 0) return true;
    if (strncmp(S, "max", 3) == 0) return true;
    if (strncmp(S, "min", 3) == 0) return true;
    return false;
}

inline char getFunctionOp(char*& S) {
    if (strncmp(S, "sin", 3) == 0) { S += 3; return 's'; }
    if (strncmp(S, "cos", 3) == 0) { S += 3; return 'c'; }
    if (strncmp(S, "tan", 3) == 0) { S += 3; return 't'; }
    if (strncmp(S, "sqrt", 4) == 0) { S += 4; return 'q'; }
    if (strncmp(S, "log", 3) == 0) { S += 3; return 'l'; }
    if (strncmp(S, "ln", 2) == 0) { S += 2; return 'L'; }
    if (strncmp(S, "max", 3) == 0) { S += 3; return 'M'; }
    if (strncmp(S, "min", 3) == 0) { S += 3; return 'R'; }
    return '\0';
}

/* ---------- 主 evaluator ---------- */
inline float evaluate(char* S, char*& RPN) {
    Stack<float> opnd;
    Stack<char>  optr;
    optr.push('\0');
    
    // 初始化 RPN 字符串
    if (RPN) RPN[0] = '\0';
    
    while (!optr.empty()) {
        // 跳过空格
        while (isspace(*S)) ++S;
        
        if ((*S >= '0' && *S <= '9') || *S == '.' || 
            (*S == '-' && (*(S + 1) >= '0' && *(S + 1) <= '9'))) {
            // 处理数字
            readNumber(S, opnd);
            if (RPN) append(RPN, opnd.top());
        } else if (isalpha(*S)) {
            // 处理函数名
            char funcOp = getFunctionOp(S);
            if (funcOp != '\0') {
                switch (orderBetween(optr.top(), funcOp)) {
                    case '<':
                        optr.push(funcOp);
                        break;
                    default:
                        // 处理错误情况
                        break;
                }
            } else {
                // 跳过未知标识符
                ++S;
            }
        } else {
            // 处理运算符
            char topOp = optr.top();
            char curOp = *S ? *S : '\0';
            
            if (curOp == '\0' && topOp == '\0') break;
            
            char order = orderBetween(topOp, curOp);
            
            switch (order) {
                case '<':
                    optr.push(curOp);
                    if (*S) ++S;
                    break;
                case '=':
                    optr.pop();
                    if (*S) ++S;
                    break;
                case '>': {
                    char op = optr.pop();
                    if (RPN) append(RPN, op);
                    
                    if (op == '!' || op == 's' || op == 'c' || op == 't' || 
                        op == 'q' || op == 'L' || op == 'l' || op == 'M' || op == 'R') {
                        // 一元运算符
                        if (!opnd.empty()) {
                            float a = opnd.pop();
                            opnd.push(calcu(op, a));
                        }
                    } else {
                        // 二元运算符
                        if (opnd.size() >= 2) {
                            float b = opnd.pop();
                            float a = opnd.pop();
                            opnd.push(calcu(a, op, b));
                        }
                    }
                    break;
                }
                default:
                    // 跳过无法处理的字符
                    if (*S) ++S;
                    break;
            }
        }
    }
    
    return opnd.empty() ? 0 : opnd.pop();
}

/* ---------- 简化版求值函数（不生成RPN） ---------- */
inline float evaluate(char* S) {
    char* rpn = new char[1024];
    rpn[0] = '\0';
    float result = evaluate(S, rpn);
    delete[] rpn;
    return result;
}

#endif // MYLIBRARY_STACK_H