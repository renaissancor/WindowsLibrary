#include "pch.h"
#include "GuardOverflow.h"

void test_guard_overflow() noexcept {
    printf("== Custom Alloc/Free Test ==\n");
    char* p = (char*)GuardOverflow::Alloc(sizeof(int) * 100);
    if (!p) { printf("Alloc failed\n"); return; }

    memset(p, 0xFF, sizeof(int) * 100);
    // memset(p, 0xFF, sizeof(int) * 101); // Throw 5 
    for (int i = 0; i < 100; i++)
		// for (int i = 0; i < 101; i++) // Throw 5
        printf("%d ", ((int*)p)[i]);
    printf("\n");

    GuardOverflow::Free(p);
    printf("Custom free done.\n");

    printf("== Standard malloc/free Test ==\n");
    char* c = (char*)malloc(sizeof(int) * 100); 
	if (c == NULL) { printf("malloc failed\n"); return; }
    if(c) memset(c, 0xFF, sizeof(int) * 100);

    for (int i = 0; i < 100; i++)
        printf("%d ", ((int*)c)[i]);
    printf("\n");

    free(c);
    printf("Standard free done.\n");
}