#include <iostream>
#include "../MySQL/Stack.h"

int main() {
    Calculator calc;
    
    std::cout << "=== Stack-Based Calculator ===" << std::endl;
    std::cout << "Supported operations: +, -, *, /, ^, sin, cos, tan, sqrt, log, ln" << std::endl;
    std::cout << "Enter 'quit' to exit" << std::endl << std::endl;
    
    // 测试用例 - 包括非法表达式
    const char* testExpressions[] = {
        "2 + 3",           // 合法
        "5 - 2",           // 合法  
        "3 * 4",           // 合法
        "1+2---8",         // 非法：连续运算符
        "1++1",            // 非法：连续运算符
        "2+++9",           // 非法：连续运算符
        "11w",             // 非法：无效字符
        "1@1",             // 非法：无效字符
        "3 + 4 * 2",       // 合法
        "(2 + 3) * 4",     // 合法
        "sin(0) + cos(0)", // 合法
        "5 / 0",           // 非法：除零
        "sqrt(-1)",        // 非法：负数开方
        "log(0)",          // 非法：非正数对数
        "2 + ",            // 非法：表达式以运算符结尾
        "(2 + 3",          // 非法：括号不匹配
        "2 + 3)",          // 非法：括号不匹配
        "2..3 + 4",        // 非法：多个小数点
        "sin2",            // 非法：函数后缺少括号
        "2 3 +"            // 非法：缺少运算符
    };
    
    std::cout << "Testing expressions:" << std::endl;
    int numTests = sizeof(testExpressions) / sizeof(testExpressions[0]);
    
    for (int i = 0; i < numTests; i++) {
        std::cout << "Expression: \"" << testExpressions[i] << "\" -> ";
        try {
            float result = calc.evaluate(testExpressions[i]);
            std::cout << result << " [VALID]" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
    
    std::cout << std::endl << "Interactive mode:" << std::endl;
    
    while (true) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        try {
            float result = calc.evaluate(input.c_str());
            std::cout << "=" << result << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    std::cout << "Goodbye!" << std::endl;
    return 0;
}