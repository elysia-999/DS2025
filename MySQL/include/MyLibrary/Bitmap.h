#ifndef MYLIBRARY_BITMAP_H
#define MYLIBRARY_BITMAP_H

#include <cstring>
#include <cstdio>

typedef int Rank;

class Bitmap {
private:
    unsigned char* M;
    Rank N, _sz;

protected:
    void init(Rank n) { 
        M = new unsigned char[N = (n + 7) / 8]; 
        memset(M, 0, N); 
        _sz = 0; 
    }

public:
    Bitmap(Rank n = 8) { init(n); }
    
    Bitmap(const char* file, Rank n = 8) {
        init(n);
        FILE* fp = fopen(file, "r"); 
        if (fp) {
            fread(M, sizeof(char), N, fp); 
            fclose(fp);
            // 重新计算有效位数
            _sz = 0;
            for (Rank k = 0; k < n; k++) 
                if (test(k)) _sz++;
        }
    }
    
    ~Bitmap() { delete[] M; M = NULL; _sz = 0; }
    
    Rank size() const { return _sz; }
    
    void set(Rank k) { 
        expand(k); 
        _sz++; 
        M[k >> 3] |= (0x80 >> (k & 0x07)); 
    }
    
    void clear(Rank k) { 
        expand(k); 
        _sz--; 
        M[k >> 3] &= ~(0x80 >> (k & 0x07)); 
    }
    
    // 添加 const 版本
    bool test(Rank k) const { 
        if (k >= 8 * N) return false; // 简化处理，不扩容
        return M[k >> 3] & (0x80 >> (k & 0x07)); 
    }
    
    // 非 const 版本用于内部使用
    bool test(Rank k) { 
        expand(k); 
        return M[k >> 3] & (0x80 >> (k & 0x07)); 
    }
    
    void dump(const char* file) {
        FILE* fp = fopen(file, "w"); 
        if (fp) {
            fwrite(M, sizeof(char), N, fp); 
            fclose(fp); 
        }
    }
    
    char* bits2string(Rank n) {
        expand(n - 1);
        char* s = new char[n + 1]; 
        s[n] = '\0';
        for (Rank i = 0; i < n; i++) 
            s[i] = test(i) ? '1' : '0';
        return s;
    }
    
    void expand(Rank k) {
        if (k < 8 * N) return;
        Rank oldN = N; 
        unsigned char* oldM = M;
        init(2 * k);
        memcpy(M, oldM, oldN);
        delete[] oldM;
    }
    
    Rank length() const { return 8 * N; }
};

#endif