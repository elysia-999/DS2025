#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../MySQL/include/MyLibrary/Vector.h"
#include "../MySQL/include/MyLibrary/Stack.h"

using namespace std;

// 计算最大矩形面积的函数
int largestRectangleArea(Vector<int>& heights) {
    Stack<int> stack; // 用于存储柱子的索引
    int maxArea = 0;

    for (int i = 0; i < heights.size(); ++i) {
        // 如果当前柱子的高度小于栈顶柱子的高度，则计算面积
        while (!stack.empty() && heights[i] < heights[stack.top()]) {
            int height = heights[stack.pop()];
            int width = stack.empty() ? i : i - stack.top() - 1;
            maxArea = max(maxArea, height * width);
        }
        stack.push(i);
    }

    // 处理栈中剩余的柱子
    while (!stack.empty()) {
        int height = heights[stack.pop()];
        int width = stack.empty() ? heights.size() : heights.size() - stack.top() - 1;
        maxArea = max(maxArea, height * width);
    }

    return maxArea;
}
// 生成随机测试数据并测试
void testLargestRectangleArea() {
    srand(time(0)); // 设置随机种子
    for (int i = 0; i < 10; ++i) {
        Vector<int> heights;
        int n = rand() % 105 + 1; // 随机生成 1 到 105 个柱子
        // 随机生成柱子高度 (0 <= height <= 104)
        for (int j = 0; j < n; ++j) {
            heights.insert(j, rand() % 105);
        }
        // 计算最大矩形面积
        int maxArea = largestRectangleArea(heights);
        // 输出测试结果
        cout << "Test Case " << i + 1 << ":\n";
        cout << "Input: heights = [ ";
        for (int j = 0; j < heights.size(); ++j) {
            cout << heights[j] << " ";
        }
        cout << "]";
        cout << "\nOutput: " << maxArea << "\n\n";
    }
}
int main() {
    testLargestRectangleArea();
    return 0;
}