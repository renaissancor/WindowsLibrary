#include <string>
#include <iostream>
#include <Windows.h>

using namespace std;

// 예제 전용 사용자 정의 예외 클래스,
// 공통 타입으로 예외를 던지고 잡는 구조를 보여주기 위해 작성

void strcpy_mine(char* src, char* dst);

void strcpy_mine(char* src, char* dst) {
    char* p, * q; 
    for (p = src, q = dst; *p != NULL; ++p, ++q) {
        *q = *p; 
    }
    q = NULL; 
}



static void func() {
    char* p = (char*)malloc(sizeof(char) * 1024 * 1024 * 100); 
    char* p2 = p; 
    printf("Alloc Initiate\n");
    Sleep(10000); 

    
    for (int i = 0; i < 1024 * 1024 * 100; ++i) {
        *p = 0; 
        p += 512; 
        Sleep(1); 

    }
    printf("Alloc Complete\n");

    while (p) {
        *p = 0; 
        p += 512;
        Sleep(1); 
    }
}

static void virtual_alloc() {
    char* p; 
    p = (char*)VirtualAlloc((void*)0x00a00000, 4096 * 2, MEM_COMMIT, PAGE_READWRITE);
}

class CTest {};

class A {
public: 
    long long a; 
    virtual ~A() {}
    virtual void print() = 0;
};

class B : public A {
public:
    long long b; 
    virtual ~B() {}
    virtual void print() override { cout << "B" << endl; }
};

class B2 : public A {
public:
    long long b2; 
    virtual ~B2(){}
    virtual void print() override { cout << "B2" << endl; }
};

class C : public B {
public:
    long long c; 
    virtual ~C() {}
    virtual void print() override { cout << "C" << endl; }
};

class D : public C {
public:
    long long d; 
    virtual ~D() {}
    virtual void print() override { cout << "D" << endl; }
};


class C2 : public B, public B2 {
public:
    long long c2; 
    virtual ~C2() {} 
    virtual void print() override { cout << "C2" << endl; }
};

class D2 : public C2 {
public:
    virtual ~D2() {}
    virtual void print() override { cout << "D2" << endl; }
};

void TestClass () {

    cout << "E :" << sizeof(CTest) << endl;
    cout << "A :" << sizeof(A) << endl;
    cout << "B :" << sizeof(B) << endl;
    cout << "B2:" << sizeof(B2) << endl;
    cout << "C :" << sizeof(C) << endl;
    cout << "D :" << sizeof(D) << endl;
    cout << "C2:" << sizeof(C2) << endl;
    cout << "D2:" << sizeof(D2) << endl;

    A* pA;
    B b_obj;
    C c_obj;
    D d_obj;
    pA = &b_obj;
    pA->print();  // "B" 출력

    pA = &c_obj;
    pA->print();  // "C" 출력

    pA = &d_obj;
    pA->print();  // "D" 출력

    B* pB;
    B2* pB2;

    C2 c2_obj;
    pB = &c2_obj;
    pB2 = &c2_obj;

    pB->print();   // "C2" 출력
    pB2->print();  // "C2" 출력

    // D2 객체로도 실험
    D2 d2_obj;
    pB = &d2_obj;
    pB->print();   // "D2" 출력


    char a[100];
    memset(a, NULL, 100);
    for (char c = 'a'; c <= 'z'; ++c) a[c - 'a'] = c;
    a[26] = NULL;
    char b[100];
    memset(b, NULL, 100);

    cout << a << endl;
    cout << b << endl;

    strcpy_mine(a, b);
    cout << a << endl;
    cout << b << endl;
}

int main() {
    

    return 0; 
}
