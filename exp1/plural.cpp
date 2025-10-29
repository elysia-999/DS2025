#include <iostream>
#include <cmath>
#include <ctime>
#include "../MySQL/include/MyLibrary/Vector.h"  // 使用您提供的Vector头文件

// 复数类定义
class Complex {
private:
    double _real;  // 实部
    double _imag;  // 虚部

public:
    // 构造函数
    Complex(double r = 0.0, double i = 0.0) : _real(r), _imag(i) {}
    
    // 拷贝构造函数
    Complex(const Complex& other) : _real(other._real), _imag(other._imag) {}
    
    // 获取实部
    double real() const { return _real; }
    
    // 获取虚部
    double imag() const { return _imag; }
    
    // 计算模
    double magnitude() const { return sqrt(_real * _real + _imag * _imag); }
    
    // 重载比较运算符（基于模，模相同则比较实部）
    bool operator<(const Complex& other) const {
        double mag1 = magnitude();
        double mag2 = other.magnitude();
        if (fabs(mag1 - mag2) < 1e-9) {
            return _real < other._real;
        }
        return mag1 < mag2;
    }
    
    bool operator==(const Complex& other) const {
        return fabs(_real - other._real) < 1e-9 && fabs(_imag - other._imag) < 1e-9;
    }
    
    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }
    
    bool operator<=(const Complex& other) const {
        return (*this < other) || (*this == other);
    }
    
    bool operator>(const Complex& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const Complex& other) const {
        return !(*this < other);
    }
    
    // 重载输出运算符
    friend std::ostream& operator<<(std::ostream& os, const Complex& c) {
        os << "(" << c._real;
        if (c._imag >= 0) {
            os << "+" << c._imag << "i)";
        } else {
            os << c._imag << "i)";
        }
        return os;
    }
};

// 为了访问protected的排序方法，我们创建一个派生类
template <typename T>
class TestVector : public Vector<T> {
public:
    using Vector<T>::Vector;  // 继承构造函数
    
    // 公开protected的排序方法
    void testBubbleSort(Rank lo, Rank hi) {
        this->bubbleSort(lo, hi);
    }
    
    void testMergeSort(Rank lo, Rank hi) {
        this->mergeSort(lo, hi);
    }
};

// 随机生成复数向量的函数
TestVector<Complex> generateRandomComplexVector(int size) {
    TestVector<Complex> vec;
    for (int i = 0; i < size; ++i) {
        // 生成-1000到1000之间的随机实部和虚部，增加数据范围
        double real = (rand() % 2001 - 1000) / 10.0; // 使用小数增加复杂度
        double imag = (rand() % 2001 - 1000) / 10.0;
        vec.insert(Complex(real, imag));
    }
    return vec;
}

// 打印向量的函数 - 对于大向量只打印部分元素
template <typename T>
void printVector(const Vector<T>& vec, const std::string& name, int maxDisplay = 10) {
    std::cout << name << " (大小: " << vec.size() << "): ";
    if (vec.size() <= maxDisplay) {
        for (Rank i = 0; i < vec.size(); ++i) {
            std::cout << vec[i] << " ";
        }
    } else {
        // 只打印前几个和后几个元素
        for (Rank i = 0; i < 5; ++i) {
            std::cout << vec[i] << " ";
        }
        std::cout << "... ";
        for (Rank i = vec.size() - 5; i < vec.size(); ++i) {
            std::cout << vec[i] << " ";
        }
    }
    std::cout << std::endl;
}

// 测试特定排序算法的函数
template <typename T>
void testSpecificSort(TestVector<T>& vec, const std::string& sortName, void (TestVector<T>::*sortFunc)(Rank, Rank)) {
    TestVector<T> vecCopy = vec;
    
    clock_t start = clock();
    (vecCopy.*sortFunc)(0, vecCopy.size());  // 调用排序函数
    clock_t end = clock();
    
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    std::cout << sortName << "时间: " << elapsed << " ms" << std::endl;
    
    // 验证排序结果
    bool sorted = true;
    for (Rank i = 1; i < vecCopy.size(); ++i) {
        if (vecCopy[i] < vecCopy[i - 1]) {
            sorted = false;
            break;
        }
    }
    std::cout << sortName << "验证: " << (sorted ? "成功" : "失败") << std::endl;
}

// 测试排序效率的函数
template <typename T>
void testSortingEfficiency(TestVector<T>& vec, const std::string& state) {
    std::cout << "\n--- " << state << "向量排序效率测试 ---" << std::endl;
    std::cout << "向量大小: " << vec.size() << std::endl;
    
    // 分别测试起泡排序和归并排序
    testSpecificSort(vec, "起泡排序", &TestVector<T>::testBubbleSort);
    testSpecificSort(vec, "归并排序", &TestVector<T>::testMergeSort);
}

// 区间查找函数：查找模在[m1, m2)之间的所有元素
Vector<Complex> rangeSearchByMagnitude(const Vector<Complex>& sortedVec, double m1, double m2) {
    Vector<Complex> result;
    
    // 遍历有序向量，找到所有满足条件的元素
    for (Rank i = 0; i < sortedVec.size(); ++i) {
        double mag = sortedVec[i].magnitude();
        if (mag >= m1 && mag < m2) {
            result.insert(sortedVec[i]);
        }
    }
    
    return result;
}

#include <windows.h>
int main() {
    SetConsoleOutputCP(65001);  // 设置控制台为 UTF-8 编码
    SetConsoleCP(65001);
    srand(time(0));  // 设置随机种子
    
    std::cout << "========== 复数向量测试 ==========" << std::endl;
    
    // 生成一个足够大的向量，确保clock()能检测到时间
    const int VECTOR_SIZE = 1000; // 使用1000个元素的大向量
    
    // 1. 生成随机复数向量（有重复项）
    std::cout << "\n1. 生成随机复数向量:" << std::endl;
    TestVector<Complex> vec = generateRandomComplexVector(VECTOR_SIZE);
    // 手动添加一些重复项
    for (int i = 0; i < 10; i++) {
        vec.insert(Complex(1.5, 1.5));
        vec.insert(Complex(2.5, 2.5));
    }
    printVector(vec, "原始向量");
    
    // 2. 测试置乱操作
    std::cout << "\n2. 测试置乱操作:" << std::endl;
    TestVector<Complex> vecShuffled = vec; // 创建副本用于置乱
    vecShuffled.unsort();
    printVector(vecShuffled, "置乱后向量");
    
    // 3. 测试查找操作
    std::cout << "\n3. 测试查找操作:" << std::endl;
    Complex searchTarget(1.5, 1.5);  // 查找重复元素
    Rank found = vec.find(searchTarget);
    if (found >= 0) {
        std::cout << "找到元素 " << searchTarget << " 在位置 " << found << std::endl;
    } else {
        std::cout << "未找到元素 " << searchTarget << std::endl;
    }
    
    // 4. 测试插入操作
    std::cout << "\n4. 测试插入操作:" << std::endl;
    Complex newElement(7.5, 8.5);
    TestVector<Complex> vecInserted = vec; // 创建副本用于插入
    vecInserted.insert(5, newElement);
    std::cout << "在位置5插入元素 " << newElement << std::endl;
    printVector(vecInserted, "插入后向量");
    
    // 5. 测试删除操作
    std::cout << "\n5. 测试删除操作:" << std::endl;
    TestVector<Complex> vecRemoved = vec; // 创建副本用于删除
    Complex removed = vecRemoved.remove(3);
    std::cout << "删除位置3的元素: " << removed << std::endl;
    printVector(vecRemoved, "删除后向量");
    
    // 6. 测试唯一化操作
    std::cout << "\n6. 测试唯一化操作:" << std::endl;
    TestVector<Complex> vecDedup = vec; // 创建副本用于唯一化
    std::cout << "唯一化前向量:" << std::endl;
    printVector(vecDedup, "有重复向量");
    
    int removedCount = vecDedup.deduplicate();
    std::cout << "唯一化后移除 " << removedCount << " 个重复元素" << std::endl;
    printVector(vecDedup, "唯一化后向量");
    
    // 7. 测试排序效率比较 - 使用原始向量的不同状态副本
    std::cout << "\n7. 排序效率比较测试:" << std::endl;
    
    // 创建顺序向量
    TestVector<Complex> orderedVec = vec;
    orderedVec.sort();
    
    // 创建乱序向量
    TestVector<Complex> randomVec = vec;
    randomVec.unsort();
    
    // 创建逆序向量
    TestVector<Complex> reversedVec = vec;
    reversedVec.sort();
    // 然后逆序
    for (Rank i = 0; i < reversedVec.size() / 2; ++i) {
        Complex temp = reversedVec[i];
        reversedVec[i] = reversedVec[reversedVec.size() - 1 - i];
        reversedVec[reversedVec.size() - 1 - i] = temp;
    }
    
    // 测试各种情况下的排序效率
    testSortingEfficiency(orderedVec, "顺序");
    testSortingEfficiency(randomVec, "乱序");
    testSortingEfficiency(reversedVec, "逆序");
    
    // 8. 测试区间查找
    std::cout << "\n8. 测试区间查找:" << std::endl;
    
    // 先对向量排序（使用归并排序）
    TestVector<Complex> sortedVec = vec;
    sortedVec.testMergeSort(0, sortedVec.size());
    printVector(sortedVec, "排序后向量");
    
    // 查找模在[50, 150)之间的元素
    double m1 = 50.0, m2 = 150.0;
    Vector<Complex> rangeResult = rangeSearchByMagnitude(sortedVec, m1, m2);
    
    std::cout << "模在 [" << m1 << ", " << m2 << ") 之间的元素:" << std::endl;
    if (rangeResult.size() > 0) {
        printVector(rangeResult, "区间查找结果");
        
        // 显示每个元素的模
        std::cout << "各元素的模: ";
        for (Rank i = 0; i < rangeResult.size(); ++i) {
            std::cout << rangeResult[i].magnitude() << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "没有找到模在 [" << m1 << ", " << m2 << ") 之间的元素" << std::endl;
    }
    
    std::cout << "\n========== 测试完成 ==========" << std::endl;
    
    return 0;
}