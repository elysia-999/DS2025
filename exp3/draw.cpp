#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>
#include <string>
#include <map>
#include <set>

using namespace std;

#include "../MySQL/Vector.h"
#include "../MySQL/include/MyLibrary/Stack.h"
#include "../MySQL/include/MyLibrary/Queen.h"
#include "../MySQL/include/MyLibrary/Graph.h"
#include "../MySQL/include/MyLibrary/Vertex.h"

using namespace std;

// 图1的节点标签
const string NODE_LABELS_1[] = {"A", "B", "C", "D", "E", "F", "G", "H"};
// 图2的节点标签  
const string NODE_LABELS_2[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L"};

// 简单测试函数
void testGraph1() {
    cout << "========== Testing Graph 1 (Directed) ==========" << endl << endl;
    
    // 创建图1
    GraphMatrix<char, int> graph1;
    
    // 插入8个顶点
    for (int i = 0; i < 8; i++) {
        graph1.insert('A' + i);
    }
    
    // 添加边（根据描述）
    graph1.insert(4, 4, 0, 1);   // A->B, weight=4
    graph1.insert(6, 6, 0, 3);   // A->D, weight=6
    graph1.insert(7, 7, 0, 6);   // A->G, weight=7
    
    graph1.insert(12, 12, 1, 2); // B->C, weight=12
    graph1.insert(9, 9, 1, 4);   // B->E, weight=9
    
    graph1.insert(2, 2, 2, 5);   // C->F, weight=2
    graph1.insert(10, 10, 2, 7); // C->H, weight=10
    
    graph1.insert(13, 13, 3, 4); // D->E, weight=13
    graph1.insert(2, 2, 3, 6);   // D->G, weight=2
    
    graph1.insert(1, 1, 4, 2);   // E->C, weight=1
    graph1.insert(5, 5, 4, 5);   // E->F, weight=5
    graph1.insert(11, 11, 4, 6); // E->G, weight=11
    graph1.insert(8, 8, 4, 7);   // E->H, weight=8
    
    graph1.insert(3, 3, 5, 7);   // F->H, weight=3
    
    graph1.insert(14, 14, 6, 7); // G->H, weight=14
    
    // 打印邻接矩阵
    cout << "Adjacency Matrix of Graph 1:" << endl;
    cout << "  ";
    for (int i = 0; i < 8; i++) {
        cout << NODE_LABELS_1[i] << " ";
    }
    cout << endl;
    
    for (int i = 0; i < 8; i++) {
        cout << NODE_LABELS_1[i] << " ";
        for (int j = 0; j < 8; j++) {
            if (graph1.exists(i, j)) {
                cout << graph1.weight(i, j) << " ";
            } else {
                cout << "0 ";
            }
        }
        cout << endl;
    }
    cout << endl;
    
    // 测试BFS
    cout << "1. BFS traversal from node A:" << endl;
    graph1.bfs(0);
    
    cout << "BFS Results:" << endl;
    for (int i = 0; i < 8; i++) {
        cout << "Vertex " << NODE_LABELS_1[i] << ": ";
        cout << "status=" << graph1.status(i) << ", ";
        cout << "dTime=" << graph1.dTime(i) << ", ";
        cout << "parent=";
        int parent = graph1.parent(i);
        if (parent >= 0) {
            cout << NODE_LABELS_1[parent];
        } else {
            cout << "none";
        }
        cout << endl;
    }
    cout << endl;
    
    // 测试DFS
    cout << "2. DFS traversal from node A:" << endl;
    GraphMatrix<char, int> graph1_dfs;
    for (int i = 0; i < 8; i++) graph1_dfs.insert('A' + i);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (graph1.exists(i, j)) {
                graph1_dfs.insert(graph1.weight(i, j), graph1.weight(i, j), i, j);
            }
        }
    }
    graph1_dfs.dfs(0);
    
    cout << "DFS Results:" << endl;
    for (int i = 0; i < 8; i++) {
        cout << "Vertex " << NODE_LABELS_1[i] << ": ";
        cout << "status=" << graph1_dfs.status(i) << ", ";
        cout << "dTime=" << graph1_dfs.dTime(i) << ", ";
        cout << "fTime=" << graph1_dfs.fTime(i) << ", ";
        cout << "parent=";
        int parent = graph1_dfs.parent(i);
        if (parent >= 0) {
            cout << NODE_LABELS_1[parent];
        } else {
            cout << "none";
        }
        cout << endl;
    }
    cout << endl;
    
    // 测试Dijkstra最短路径算法
    cout << "3. Dijkstra shortest paths from node A:" << endl;
    GraphMatrix<char, int> graph1_dijkstra;
    for (int i = 0; i < 8; i++) graph1_dijkstra.insert('A' + i);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (graph1.exists(i, j)) {
                graph1_dijkstra.insert(graph1.weight(i, j), graph1.weight(i, j), i, j);
            }
        }
    }
    
    DijkstraPU<char, int> dijkstraPU;
    graph1_dijkstra.pfs(0, dijkstraPU);
    
    cout << "Shortest paths from A:" << endl;
    for (int i = 0; i < 8; i++) {
        if (i != 0 && graph1_dijkstra.priority(i) < INT_MAX) {
            cout << "To " << NODE_LABELS_1[i] << ": distance = " << graph1_dijkstra.priority(i);
            
            // 构建路径
            vector<int> path;
            int current = i;
            while (current != -1) {
                path.push_back(current);
                current = graph1_dijkstra.parent(current);
            }
            reverse(path.begin(), path.end());
            
            if (path.size() > 1) {
                cout << ", path: ";
                for (size_t j = 0; j < path.size(); j++) {
                    cout << NODE_LABELS_1[path[j]];
                    if (j < path.size() - 1) cout << " -> ";
                }
            }
            cout << endl;
        }
    }
    cout << endl;
    
    // 测试拓扑排序（如果有向图是DAG）
    cout << "4. Topological sort from node A:" << endl;
    GraphMatrix<char, int> graph1_topo;
    for (int i = 0; i < 8; i++) graph1_topo.insert('A' + i);
    // 创建一个简单的DAG用于拓扑排序
    graph1_topo.insert(1, 1, 0, 1); // A->B
    graph1_topo.insert(1, 1, 0, 3); // A->D
    graph1_topo.insert(1, 1, 1, 2); // B->C
    graph1_topo.insert(1, 1, 3, 4); // D->E
    graph1_topo.insert(1, 1, 4, 5); // E->F
    
    Stack<char>* topoResult = graph1_topo.tSort(0);
    if (topoResult && !topoResult->empty()) {
        cout << "Topological order: ";
        while (!topoResult->empty()) {
            cout << topoResult->pop();
            if (!topoResult->empty()) cout << " -> ";
        }
        cout << endl;
        delete topoResult;
    } else {
        cout << "Graph is not a DAG (no topological order exists)" << endl;
        if (topoResult) delete topoResult;
    }
    cout << endl;
}

void testGraph2() {
    cout << "========== Testing Graph 2 (Undirected) ==========" << endl << endl;
    
    // 创建图2（无向图）
    GraphMatrix<char, int> graph2;
    
    // 插入12个顶点
    for (int i = 0; i < 12; i++) {
        graph2.insert('A' + i);
    }
    
    // 添加无向边
    graph2.insert(1, 1, 0, 1); graph2.insert(1, 1, 1, 0); // A-B
    graph2.insert(1, 1, 0, 4); graph2.insert(1, 1, 4, 0); // A-E
    
    graph2.insert(1, 1, 1, 5); graph2.insert(1, 1, 5, 1); // B-F
    graph2.insert(1, 1, 1, 9); graph2.insert(1, 1, 9, 1); // B-J
    
    graph2.insert(1, 1, 2, 3); graph2.insert(1, 1, 3, 2); // C-D
    graph2.insert(1, 1, 2, 5); graph2.insert(1, 1, 5, 2); // C-F
    graph2.insert(1, 1, 2, 6); graph2.insert(1, 1, 6, 2); // C-G
    
    graph2.insert(1, 1, 3, 7); graph2.insert(1, 1, 7, 3); // D-H
    
    graph2.insert(1, 1, 4, 5); graph2.insert(1, 1, 5, 4); // E-F
    graph2.insert(1, 1, 4, 8); graph2.insert(1, 1, 8, 4); // E-I
    
    graph2.insert(1, 1, 5, 6); graph2.insert(1, 1, 6, 5); // F-G
    graph2.insert(1, 1, 5, 9); graph2.insert(1, 1, 9, 5); // F-J
    
    graph2.insert(1, 1, 6, 10); graph2.insert(1, 1, 10, 6); // G-K
    
    graph2.insert(1, 1, 7, 10); graph2.insert(1, 1, 10, 7); // H-K
    
    graph2.insert(1, 1, 8, 9); graph2.insert(1, 1, 9, 8); // I-J
    
    graph2.insert(1, 1, 10, 11); graph2.insert(1, 1, 11, 10); // K-L
    
    // 打印邻接矩阵
    cout << "Adjacency Matrix of Graph 2 (showing upper triangle):" << endl;
    cout << "  ";
    for (int i = 0; i < 12; i++) {
        cout << NODE_LABELS_2[i] << " ";
    }
    cout << endl;
    
    for (int i = 0; i < 12; i++) {
        cout << NODE_LABELS_2[i] << " ";
        for (int j = 0; j < 12; j++) {
            if (graph2.exists(i, j)) {
                cout << "1 ";
            } else {
                cout << "0 ";
            }
        }
        cout << endl;
    }
    cout << endl;
    
    // 测试BFS
    cout << "1. BFS traversal from node A:" << endl;
    graph2.bfs(0);
    
    cout << "BFS Results:" << endl;
    for (int i = 0; i < 12; i++) {
        cout << "Vertex " << NODE_LABELS_2[i] << ": ";
        cout << "status=" << graph2.status(i) << ", ";
        cout << "dTime=" << graph2.dTime(i) << ", ";
        cout << "parent=";
        int parent = graph2.parent(i);
        if (parent >= 0) {
            cout << NODE_LABELS_2[parent];
        } else {
            cout << "none";
        }
        cout << endl;
    }
    cout << endl;
    
    // 测试DFS
    cout << "2. DFS traversal from node A:" << endl;
    GraphMatrix<char, int> graph2_dfs;
    for (int i = 0; i < 12; i++) graph2_dfs.insert('A' + i);
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if (graph2.exists(i, j)) {
                graph2_dfs.insert(1, 1, i, j);
            }
        }
    }
    graph2_dfs.dfs(0);
    
    cout << "DFS Results:" << endl;
    for (int i = 0; i < 12; i++) {
        cout << "Vertex " << NODE_LABELS_2[i] << ": ";
        cout << "status=" << graph2_dfs.status(i) << ", ";
        cout << "dTime=" << graph2_dfs.dTime(i) << ", ";
        cout << "fTime=" << graph2_dfs.fTime(i) << ", ";
        cout << "parent=";
        int parent = graph2_dfs.parent(i);
        if (parent >= 0) {
            cout << NODE_LABELS_2[parent];
        } else {
            cout << "none";
        }
        cout << endl;
    }
    cout << endl;
    
    // 测试Prim最小生成树算法
    cout << "3. Prim's Minimum Spanning Tree from node A:" << endl;
    GraphMatrix<char, int> graph2_prim;
    for (int i = 0; i < 12; i++) graph2_prim.insert('A' + i);
    for (int i = 0; i < 12; i++) {
        for (int j = i + 1; j < 12; j++) {
            if (graph2.exists(i, j)) {
                graph2_prim.insert(1, 1, i, j);
                graph2_prim.insert(1, 1, j, i);
            }
        }
    }
    
    PrimPU<char, int> primPU;
    graph2_prim.pfs(0, primPU);
    
    int totalWeight = 0;
    cout << "MST Edges:" << endl;
    for (int i = 1; i < 12; i++) {
        int parent = graph2_prim.parent(i);
        if (parent >= 0 && graph2_prim.exists(parent, i)) {
            int weight = graph2_prim.weight(parent, i);
            cout << NODE_LABELS_2[parent] << " -- " << NODE_LABELS_2[i] 
                 << " (weight: " << weight << ")" << endl;
            totalWeight += weight;
        }
    }
    cout << "Total MST weight: " << totalWeight << endl << endl;
    
    // 测试双连通分量（BCC）
    cout << "4. Testing Biconnected Components:" << endl;
    // 创建一个简单图来测试BCC
    GraphMatrix<char, int> graph2_bcc;
    for (int i = 0; i < 5; i++) graph2_bcc.insert('A' + i);
    
    // 创建一个包含关节点的图
    graph2_bcc.insert(1, 1, 0, 1); graph2_bcc.insert(1, 1, 1, 0);
    graph2_bcc.insert(1, 1, 1, 2); graph2_bcc.insert(1, 1, 2, 1);
    graph2_bcc.insert(1, 1, 2, 3); graph2_bcc.insert(1, 1, 3, 2);
    graph2_bcc.insert(1, 1, 3, 4); graph2_bcc.insert(1, 1, 4, 3);
    graph2_bcc.insert(1, 1, 1, 3); graph2_bcc.insert(1, 1, 3, 1); // 添加一条边创建环
    
    cout << "Testing BCC algorithm on a simple graph..." << endl;
    graph2_bcc.bcc(0);
    
    cout << "BCC Results (checking for articulation points):" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "Vertex " << char('A' + i) << ": ";
        cout << "status=" << graph2_bcc.status(i) << ", ";
        cout << "dTime=" << graph2_bcc.dTime(i) << ", ";
        cout << "fTime=" << graph2_bcc.fTime(i) << endl;
    }
    cout << endl;
}

int main() {
    cout << "==================================================" << endl;
    cout << "      Graph Algorithms Implementation Test" << endl;
    cout << "==================================================" << endl << endl;
    
    testGraph1();
    testGraph2();
    
    cout << "==================================================" << endl;
    cout << "      All Tests Completed Successfully!" << endl;
    cout << "==================================================" << endl;
    
    return 0;
}