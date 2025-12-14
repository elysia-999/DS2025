#ifndef MYLIBRARY_HUFFMAN_H
#define MYLIBRARY_HUFFMAN_H

#include "BinTree.h"
#include "Bitmap.h"
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <functional>

using namespace std;

struct HuffChar {
    char ch;
    int weight;
    
    HuffChar(char c = '^', int w = 0) : ch(c), weight(w) {}
    
    bool operator<(const HuffChar& hc) const { 
        return weight < hc.weight; 
    }
    
    bool operator==(const HuffChar& hc) const { 
        return ch == hc.ch && weight == hc.weight; 
    }
};

typedef BinNode<HuffChar> HuffNode;

class HuffTree {
private:
    HuffNode* _root;
    int _size;

public:
    HuffTree() : _root(NULL), _size(0) {}
    HuffTree(char c, int w) : _size(1) {
        _root = new HuffNode(HuffChar(c, w));
    }
    
    ~HuffTree() {
        if (_root) removeTree(_root);
    }
    
    HuffNode* root() const { return _root; }
    int size() const { return _size; }
    int weight() const { return _root ? _root->data.weight : 0; }
    
    static HuffTree* merge(HuffTree* t1, HuffTree* t2) {
        HuffTree* tree = new HuffTree();
        tree->_root = new HuffNode(HuffChar('^', t1->weight() + t2->weight()));
        tree->_root->lChild = t1->_root;
        tree->_root->rChild = t2->_root;
        if (t1->_root) t1->_root->parent = tree->_root;
        if (t2->_root) t2->_root->parent = tree->_root;
        tree->_size = t1->_size + t2->_size;
        
        // 不删除原树，因为现在它们属于新树
        return tree;
    }
    
    void removeTree(HuffNode* node) {
        if (!node) return;
        removeTree(node->lChild);
        removeTree(node->rChild);
        delete node;
    }
};

class HuffCode {
private:
    map<char, string> _codeTable;
    Bitmap* _bitmap;
    
public:
    HuffCode() : _bitmap(NULL) {}
    ~HuffCode() { if (_bitmap) delete _bitmap; }
    
    void generateCodeTable(HuffNode* root) {
        _codeTable.clear();
        if (!root) return;
        
        string currentCode = "";
        generateCodeRecursive(root, currentCode);
    }
    
    string getCode(char c) const {
        auto it = _codeTable.find(tolower(c));
        if (it != _codeTable.end()) return it->second;
        return "";
    }
    
    Bitmap* encode(const string& text) {
        string bitString = "";
        
        for (char c : text) {
            char lowerC = tolower(c);
            if (isalpha(lowerC)) {
                bitString += getCode(lowerC);
            }
        }
        
        if (_bitmap) delete _bitmap;
        _bitmap = new Bitmap(bitString.length());
        
        for (size_t i = 0; i < bitString.length(); ++i) {
            if (bitString[i] == '1') {
                _bitmap->set(i);
            }
        }
        
        return _bitmap;
    }
    
    string decode(const Bitmap* bitmap, HuffNode* root, int bitCount) const {
        string result = "";
        HuffNode* current = root;
        
        for (int i = 0; i < bitCount; ++i) {
            // 使用 const 版本的 test
            if (bitmap->test(i)) {
                current = current->rChild;
            } else {
                current = current->lChild;
            }
            
            if (!current->lChild && !current->rChild) {
                result += current->data.ch;
                current = root;
            }
        }
        
        return result;
    }
    
    void displayCodeTable() const {
        cout << "Huffman Code Table:" << endl;
        for (const auto& pair : _codeTable) {
            cout << "'" << pair.first << "': " << pair.second << endl;
        }
    }

private:
    void generateCodeRecursive(HuffNode* node, string& currentCode) {
        if (!node) return;
        
        if (!node->lChild && !node->rChild) {
            _codeTable[node->data.ch] = currentCode;
            return;
        }
        
        if (node->lChild) {
            currentCode += '0';
            generateCodeRecursive(node->lChild, currentCode);
            currentCode.pop_back();
        }
        
        if (node->rChild) {
            currentCode += '1';
            generateCodeRecursive(node->rChild, currentCode);
            currentCode.pop_back();
        }
    }
};

class HuffmanEncoder {
private:
    string _text;
    map<char, int> _freqTable;
    vector<HuffTree*> _forest;
    HuffTree* _huffTree;
    HuffCode _huffCode;

public:
    HuffmanEncoder(const string& text) : _text(text), _huffTree(NULL) {
        buildFrequencyTable();
        buildHuffmanTree();
        _huffCode.generateCodeTable(_huffTree->root());
    }
    
    ~HuffmanEncoder() {
        if (_huffTree) delete _huffTree;
    }
    
    void buildFrequencyTable() {
        _freqTable.clear();
        for (char c : _text) {
            if (isalpha(c)) {
                _freqTable[tolower(c)]++;
            }
        }
    }
    
    void buildHuffmanTree() {
        // 创建初始森林
        for (const auto& pair : _freqTable) {
            _forest.push_back(new HuffTree(pair.first, pair.second));
        }
        
        // 构建Huffman树
        while (_forest.size() > 1) {
            // 找到权重最小的两棵树
            int min1 = 0, min2 = 1;
            if (_forest.size() > 1 && _forest[min1]->weight() > _forest[min2]->weight()) {
                std::swap(min1, min2);  // 使用 std::swap
            }
            
            for (size_t i = 2; i < _forest.size(); ++i) {
                if (_forest[i]->weight() < _forest[min1]->weight()) {
                    min2 = min1;
                    min1 = i;
                } else if (_forest[i]->weight() < _forest[min2]->weight()) {
                    min2 = i;
                }
            }
            
            // 合并两棵树
            HuffTree* newTree = HuffTree::merge(_forest[min1], _forest[min2]);
            
            // 从森林中移除被合并的树
            if (min1 < min2) {
                _forest.erase(_forest.begin() + min2);
                _forest.erase(_forest.begin() + min1);
            } else {
                _forest.erase(_forest.begin() + min1);
                _forest.erase(_forest.begin() + min2);
            }
            
            // 添加新树
            _forest.push_back(newTree);
        }
        
        _huffTree = _forest.empty() ? NULL : _forest[0];
    }
    
    Bitmap* encodeText() {
        return _huffCode.encode(_text);
    }
    
    string encodeWord(const string& word) {
        string encoded = "";
        for (char c : word) {
            char lowerC = tolower(c);
            if (isalpha(lowerC)) {
                encoded += _huffCode.getCode(lowerC);
            }
        }
        return encoded;
    }
    
    void displayFrequencyTable() const {
        cout << "Character Frequency Table:" << endl;
        for (const auto& pair : _freqTable) {
            cout << "'" << pair.first << "': " << pair.second << endl;
        }
    }
    
    void displayCodeTable() const {
        _huffCode.displayCodeTable();
    }
    
    HuffTree* getHuffmanTree() const { return _huffTree; }
    HuffCode* getHuffCode() { return &_huffCode; }
};

#endif