#include <iostream>

using namespace std; 

void TryCatch() {
	try {
		int num = 0; 
		while (1) {
			num++;
			new int[10000][10000];
		}
	}
	catch (bad_alloc& bad) {

	}

}

class A {
	private:
		int a; 
		int b;
	public:
		void func() {
			int c = a + b; 
		}
};

class Base {
	public:
	virtual void func() {
		printf("Base Func\n"); 
	}

};

class HBase : public Base {
public:
	virtual void func() override {
		printf("HBase Func\n"); 
	}
};

class YBase : public Base {
};

class Nclass {

};



static void ClassFunc() {
	// Check Disassembly of Class Function 
	Base base;
	HBase hBase;
	YBase yBase;
	Base* pBase = nullptr;
	HBase* pHBase = nullptr;
	Base* pw = &base;
	pBase = dynamic_cast<Base*>(pw);
	pBase = dynamic_cast<HBase*>(&hBase); // valid
	pHBase = dynamic_cast<HBase*>(&hBase); 

	pHBase->func(); 

	if (pHBase == nullptr) {
		// Server Fail 
		__debugbreak();
	}
}

int main()
{
	ClassFunc(); 
	
	return 0;
}

