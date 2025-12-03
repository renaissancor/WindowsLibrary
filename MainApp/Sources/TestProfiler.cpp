#include "pch.h"
#include "Profiler.h"

static int getThreadRandom(int minVal, int maxVal) {
    return minVal + (rand() % (maxVal - minVal + 1));
}

static void funcA() noexcept {
    Profiler::Enter profile("funcA");
    Sleep(getThreadRandom(1, 2));
    printf("In funcA\n");
}

static void funcB() noexcept {
    Profiler::Enter profile("funcB");
    Sleep(getThreadRandom(1, 2));
    printf("In funcB\n");
}

static void funcC() noexcept {
    Sleep(getThreadRandom(1, 2));
    Profiler::Enter profile("funcC");
    printf("In funcC\n");
}

unsigned int __stdcall ThreadFunc(void* arg) noexcept {
    int thread_id = *reinterpret_cast<int*>(arg);

    // 스레드별로 srand 시드 설정 (시간+스레드ID)
    srand(static_cast<unsigned int>(time(NULL)) + thread_id);

    for (int i = 0; i < 100; ++i) {
        int choice = rand() % 3;
        switch (choice) {
        case 0: funcA(); break;
        case 1: funcB(); break;
        default: funcC(); break;
        }
    }

    // 프로파일 결과 저장 폴더 생성 (없으면)
    CreateDirectoryA(".\\profile", NULL);

    std::string basePath = ".\\profile\\profiler_results_thread_" + std::to_string(thread_id);
    auto& profiler = Profiler::Manager::GetInstance();
    profiler.SaveDataTXT(basePath + ".txt", Profiler::MILISEC);
    profiler.SaveDataCSV(basePath + ".csv", Profiler::MILISEC);
    profiler.SaveFuncCSV(basePath + "_func.csv");

    return 0;
}

int TestProfiler() noexcept {
    constexpr size_t threadCount = 4;
    HANDLE threads[threadCount] = { NULL };
    int threadIds[threadCount] = { 0 };

    for (size_t i = 0; i < threadCount; ++i)
        threadIds[i] = static_cast<int>(i);

    for (size_t i = 0; i < threadCount; ++i) {
        threads[i] = (HANDLE)_beginthreadex(nullptr, 0, &ThreadFunc, &threadIds[i], 0, nullptr);
    }

    WaitForMultipleObjects(static_cast<DWORD>(threadCount), threads, TRUE, INFINITE);

    char buffer[512];
    for (size_t i = 0; i < threadCount; ++i) {
        std::string path = ".\\profile\\profiler_results_thread_" + std::to_string(i) + ".txt";
        FILE* fp = nullptr;
        if (fopen_s(&fp, path.c_str(), "r") == 0 && fp) {
            printf("Contents of %s:\n", path.c_str());
            while (fgets(buffer, sizeof(buffer), fp)) {
                printf("%s", buffer);
            }
            fclose(fp);
        }
        else {
            printf("Failed to open %s\n", path.c_str());
        }
    }

    for (size_t i = 0; i < threadCount; ++i) {
        if (threads[i]) {
            CloseHandle(threads[i]);
        }
    }

    return 0;
}
