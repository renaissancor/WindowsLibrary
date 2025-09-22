#include <vector>
#include "NewTracer.h"

// STL 헤더 먼저 include 후 매크로 정의 
// 
#define new new(__FILE__, __LINE__)

int TestNewTracer() {
    int* a = new int[20];           // leak
    int* b = new int[10];
    delete[] b;

    int* c = new int;      // leak 

    std::vector<int>* v = new std::vector<int>(100);
    delete v;

    NewTracer::Manager::GetInstance().ReportLeaks();
    // NewTracer::Manager::GetInstance().CollectGarbage(); 

    return 0;
}
