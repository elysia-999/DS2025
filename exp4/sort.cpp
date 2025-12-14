#include <iostream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "../MySQL\Vector.h"

using namespace std;

// 边界框结构体
struct BoundingBox {
    int id;         // 边界框ID
    float x1, y1;   // 左上角坐标
    float x2, y2;   // 右下角坐标
    float confidence; // 置信度
    bool suppressed;  // 是否被抑制
    
    // 构造函数
    BoundingBox(int id = 0, float x1 = 0, float y1 = 0, float x2 = 0, float y2 = 0, float conf = 0) 
        : id(id), x1(x1), y1(y1), x2(x2), y2(y2), confidence(conf), suppressed(false) {}
    
    // 用于排序的比较运算符（按置信度降序）
    bool operator<(const BoundingBox& other) const {
        return confidence < other.confidence; // 升序排列
    }
    
    bool operator>(const BoundingBox& other) const {
        return confidence > other.confidence; // 降序排列
    }
    
    bool operator<=(const BoundingBox& other) const {
        return confidence <= other.confidence;
    }
    
    bool operator>=(const BoundingBox& other) const {
        return confidence >= other.confidence;
    }
    
    bool operator==(const BoundingBox& other) const {
        return id == other.id;
    }
    
    bool operator!=(const BoundingBox& other) const {
        return id != other.id;
    }
    
    // 计算面积
    float area() const {
        return (x2 - x1) * (y2 - y1);
    }
};

// 降序比较函数
struct CompareDesc {
    bool operator()(const BoundingBox& a, const BoundingBox& b) const {
        return a.confidence > b.confidence; // 降序
    }
};

// 计算IoU（交并比）
float calculateIoU(const BoundingBox& box1, const BoundingBox& box2) {
    // 计算相交区域
    float inter_x1 = max(box1.x1, box2.x1);
    float inter_y1 = max(box1.y1, box2.y1);
    float inter_x2 = min(box1.x2, box2.x2);
    float inter_y2 = min(box1.y2, box2.y2);
    
    // 计算相交面积
    float inter_width = max(0.0f, inter_x2 - inter_x1);
    float inter_height = max(0.0f, inter_y2 - inter_y1);
    float inter_area = inter_width * inter_height;
    
    // 计算并集面积
    float union_area = box1.area() + box2.area() - inter_area;
    
    if (union_area == 0) return 0;
    return inter_area / union_area;
}

// NMS算法
Vector<BoundingBox> basicNMS(Vector<BoundingBox>& boxes, float iou_threshold = 0.5) {
    if (boxes.empty()) return Vector<BoundingBox>();
    
    // 创建结果向量
    Vector<BoundingBox> result;
    
    // 重置抑制标记
    for (int i = 0; i < boxes.size(); i++) {
        boxes[i].suppressed = false;
    }
    
    // 遍历所有边界框
    for (int i = 0; i < boxes.size(); i++) {
        if (boxes[i].suppressed) continue;
        
        // 将当前框加入结果
        result.insert(boxes[i]);
        
        // 抑制与当前框IoU大于阈值的框
        for (int j = i + 1; j < boxes.size(); j++) {
            if (!boxes[j].suppressed) {
                float iou = calculateIoU(boxes[i], boxes[j]);
                if (iou > iou_threshold) {
                    boxes[j].suppressed = true;
                }
            }
        }
    }
    
    return result;
}

// 降序排序函数
void sortDescending(Vector<BoundingBox>& boxes, int left, int right) {
    // 使用选择排序实现降序
    for (int i = left; i < right - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < right; j++) {
            if (boxes[j].confidence > boxes[maxIdx].confidence) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            std::swap(boxes[i], boxes[maxIdx]);
        }
    }
}

// 随机分布生成边界框
Vector<BoundingBox> generateRandomBoxes(int n, int image_width = 1000, int image_height = 1000) {
    Vector<BoundingBox> boxes;
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    
    for (int i = 0; i < n; i++) {
        // 随机生成位置和大小
        float width = 50 + rand() % 100;  // 宽度50-150
        float height = 50 + rand() % 100; // 高度50-150
        float x = rand() % (image_width - (int)width);
        float y = rand() % (image_height - (int)height);
        
        // 随机生成置信度（0.5-1.0）
        float confidence = 0.5 + (rand() % 51) / 100.0;
        
        boxes.insert(BoundingBox(i, x, y, x + width, y + height, confidence));
    }
    
    return boxes;
}

// 聚集分布生成边界框
Vector<BoundingBox> generateClusteredBoxes(int n, int clusters = 10, int image_width = 1000, int image_height = 1000) {
    Vector<BoundingBox> boxes;
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    
    // 生成聚类中心
    float center_x[100], center_y[100];
    for (int c = 0; c < clusters; c++) {
        center_x[c] = rand() % image_width;
        center_y[c] = rand() % image_height;
    }
    
    // 在每个聚类中心周围生成边界框
    int boxes_per_cluster = n / clusters;
    int remaining_boxes = n % clusters;
    
    int box_id = 0;
    for (int c = 0; c < clusters; c++) {
        int boxes_to_generate = boxes_per_cluster + (c < remaining_boxes ? 1 : 0);
        
        for (int i = 0; i < boxes_to_generate; i++) {
            // 在聚类中心附近生成
            float offset_x = (rand() % 200) - 100;  // ±100像素偏移
            float offset_y = (rand() % 200) - 100;
            float width = 40 + rand() % 60;         // 宽度40-100
            float height = 40 + rand() % 60;        // 高度40-100
            
            float x = max(0.0f, min((float)image_width - width, center_x[c] + offset_x));
            float y = max(0.0f, min((float)image_height - height, center_y[c] + offset_y));
            
            // 置信度（聚类中心附近的置信度较高）
            float distance = sqrt(offset_x * offset_x + offset_y * offset_y);
            float confidence = max(0.3f, 1.0f - distance / 200.0f);
            
            boxes.insert(BoundingBox(box_id++, x, y, x + width, y + height, confidence));
        }
    }
    
    return boxes;
}

// 测试排序算法性能
void testSortPerformance(Vector<BoundingBox>& boxes, const string& sort_name, 
                        void (*sort_func)(Vector<BoundingBox>&, int, int)) {
    // 复制数据以避免修改原数据
    Vector<BoundingBox> boxes_copy = boxes;
    
    clock_t start = clock();
    
    // 使用指定的排序算法
    sort_func(boxes_copy, 0, boxes_copy.size());
    
    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;
    
    cout << sort_name << " 排序时间: " << elapsed << " 秒" << endl;
}

// 测试NMS性能
void testNMSPerformance(Vector<BoundingBox>& boxes, const string& sort_name,
                       void (*sort_func)(Vector<BoundingBox>&, int, int)) {
    // 复制数据
    Vector<BoundingBox> boxes_copy = boxes;

    clock_t start = clock();
    sort_func(boxes_copy, 0, boxes_copy.size());
    
    // 执行NMS
    Vector<BoundingBox> result = basicNMS(boxes_copy);
    
    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;
    
    cout << sort_name << " + NMS 总时间: " << elapsed << " 秒, 保留框数: " << result.size() << endl;
}

// 包装函数
void bubbleSortWrapper(Vector<BoundingBox>& boxes, int lo, int hi) {
    // 由于Vector的排序是升序，而我们需要降序，所以需要特殊处理
    boxes.bubbleSort(lo, hi);
    // 反转以获得降序
    for (int i = lo, j = hi - 1; i < j; i++, j--) {
        std::swap(boxes[i], boxes[j]);
    }
}

void selectionSortWrapper(Vector<BoundingBox>& boxes, int lo, int hi) {
    for (int i = lo; i < hi - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < hi; j++) {
            if (boxes[j].confidence > boxes[maxIdx].confidence) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            std::swap(boxes[i], boxes[maxIdx]);
        }
    }
}

void mergeSortWrapper(Vector<BoundingBox>& boxes, int lo, int hi) {
    boxes.mergeSort(lo, hi);
    
    // 反转以获得降序
    for (int i = lo, j = hi - 1; i < j; i++, j--) {
        std::swap(boxes[i], boxes[j]);
    }
}

void quickSortWrapper(Vector<BoundingBox>& boxes, int lo, int hi) {
    boxes.quickSort(lo, hi);
    
    // 反转以获得降序
    for (int i = lo, j = hi - 1; i < j; i++, j--) {
        std::swap(boxes[i], boxes[j]);
    }
}

#include <windows.h>
int main() {
    SetConsoleOutputCP(65001);  // 设置控制台为 UTF-8 编码
    SetConsoleCP(65001);
    srand(time(0));  // 设置随机种子

    srand(time(nullptr));
    
    cout << "NMS算法性能测试:" << endl ;
    // 测试不同数据规模
    int test_sizes[] = {100, 500, 1000, 5000};
    
    for (int size : test_sizes) {
        cout << "\n测试数据规模: " << size << endl;
        
        // 测试随机分布
        cout << "\n随机分布" << endl;
        Vector<BoundingBox> random_boxes = generateRandomBoxes(size);
        
        cout << "排序算法性能:" << endl;
        testSortPerformance(random_boxes, "起泡排序", bubbleSortWrapper);
        testSortPerformance(random_boxes, "选择排序", selectionSortWrapper);
        testSortPerformance(random_boxes, "归并排序", mergeSortWrapper);
        testSortPerformance(random_boxes, "快速排序", quickSortWrapper);
        
        cout << "\nNMS算法性能:" << endl;
        testNMSPerformance(random_boxes, "起泡排序", bubbleSortWrapper);
        testNMSPerformance(random_boxes, "选择排序", selectionSortWrapper);
        testNMSPerformance(random_boxes, "归并排序", mergeSortWrapper);
        testNMSPerformance(random_boxes, "快速排序", quickSortWrapper);
        
        // 测试聚集分布
        cout << "\n聚集分布" << endl;
        Vector<BoundingBox> clustered_boxes = generateClusteredBoxes(size);
        
        cout << "排序算法性能:" << endl;
        testSortPerformance(clustered_boxes, "起泡排序", bubbleSortWrapper);
        testSortPerformance(clustered_boxes, "选择排序", selectionSortWrapper);
        testSortPerformance(clustered_boxes, "归并排序", mergeSortWrapper);
        testSortPerformance(clustered_boxes, "快速排序", quickSortWrapper);
        
        cout << "\nNMS算法性能:" << endl;
        testNMSPerformance(clustered_boxes, "起泡排序", bubbleSortWrapper);
        testNMSPerformance(clustered_boxes, "选择排序", selectionSortWrapper);
        testNMSPerformance(clustered_boxes, "归并排序", mergeSortWrapper);
        testNMSPerformance(clustered_boxes, "快速排序", quickSortWrapper);
    }
    return 0;
}