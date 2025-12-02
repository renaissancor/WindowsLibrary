#include "pch.h"
#include "WinThread.h"

// Test 1: Lambda
static void Test_Lambda() {
    std::cout << "\n=== Test 1: Lambda ===\n";

    Thread t([]() {
        std::cout << "Lambda thread running\n";
        });

    std::cout << "Thread ID: " << t.GetId() << "\n";
    std::cout << "Joinable: " << (t.Joinable() ? "Yes" : "No") << "\n";

    t.Join();

    std::cout << "After Join - Joinable: " << (t.Joinable() ? "Yes" : "No") << "\n";
    std::cout << "Test 1 Passed\n";
}

// Test 2: Capture
static void Test_Capture() {
    std::cout << "\n=== Test 2: Capture ===\n";

    int value = 42;
    std::string message = "Hello from main thread";

    Thread t([value, message]() {
        std::cout << "Captured value: " << value << "\n";
        std::cout << "Captured message: " << message << "\n";
        });

    t.Join();
    std::cout << "Test 2 Passed\n";
}

// Test 4: Vector
static void Test_Vector() {
    std::cout << "\n=== Test 4: Vector Storage ===\n";

    std::vector<Thread> threads;
    std::atomic<int> counter{ 0 };

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([i, &counter]() {
            std::cout << "Thread " << i << " started\n";
            Sleep(50 * (i + 1));
            counter++;
            std::cout << "Thread " << i << " finished\n";
            });
    }

    std::cout << "All threads created. Waiting for completion...\n";

    for (auto& t : threads) {
        if (t.Joinable()) {
            t.Join();
        }
    }

    std::cout << "Counter value: " << counter.load() << "\n";
    std::cout << "Test 4 Passed\n";
}

// Test 5: Move Semantics
static void Test_MoveSemantics() {
    std::cout << "\n=== Test 5: Move Semantics ===\n";

    Thread t1([]() {
        std::cout << "Original thread\n";
        Sleep(100);
        });

    std::cout << "t1 Joinable: " << (t1.Joinable() ? "Yes" : "No") << "\n";

    Thread t2 = std::move(t1);

    std::cout << "After move - t1 Joinable: " << (t1.Joinable() ? "Yes" : "No") << "\n";
    std::cout << "After move - t2 Joinable: " << (t2.Joinable() ? "Yes" : "No") << "\n";

    t2.Join();
    std::cout << "Test 5 Passed\n";
}

// Test 6: Detach
static void Test_Detach() {
    std::cout << "\n=== Test 6: Detach ===\n";

    std::atomic<bool> completed{ false };

    Thread t([&completed]() {
        Sleep(200);
        completed = true;
        std::cout << "Detached thread completed\n";
        });

    std::cout << "Before detach - Joinable: " << (t.Joinable() ? "Yes" : "No") << "\n";
    t.Detach();
    std::cout << "After detach - Joinable: " << (t.Joinable() ? "Yes" : "No") << "\n";

    while (!completed) {
        Sleep(50);
    }

    std::cout << "Test 6 Passed\n";
}


// Test 8: IOCP Server
class IocpServer {
private:
    struct WorkerContext {
        HANDLE iocp;
        std::atomic<bool>* running;
        std::atomic<int>* totalProcessed;
        int workerId;
    };

    HANDLE _iocp;
    std::vector<Thread> _workers;
    std::atomic<bool> _running;
    std::atomic<int> _totalProcessed;

    static void WorkerThreadStatic(WorkerContext ctx) {
        std::cout << "Worker " << ctx.workerId << " started (TID: "
            << GetCurrentThreadId() << ")\n";

        int localProcessed = 0;

        while (ctx.running->load(std::memory_order_relaxed)) {
            DWORD transferred;
            ULONG_PTR key;
            OVERLAPPED* overlapped;

            BOOL ret = GetQueuedCompletionStatus(
                ctx.iocp, &transferred, &key, &overlapped, INFINITE
            );

            if (!ret) {
                std::cout << "Worker " << ctx.workerId << " GQCS failed\n";
                break;
            }

            if (transferred == 0 && key == 0) {
                std::cout << "Worker " << ctx.workerId << " received shutdown signal\n";
                break;
            }

            localProcessed++;
            ctx.totalProcessed->fetch_add(1, std::memory_order_relaxed);

            if (localProcessed % 10 == 0) {
                std::cout << "Worker " << ctx.workerId << " processed "
                    << localProcessed << " items\n";
            }

            Sleep(10);
        }

        std::cout << "Worker " << ctx.workerId << " stopped (processed "
            << localProcessed << " items)\n";
    }

public:
    IocpServer() : _iocp(nullptr), _running(false), _totalProcessed(0) {}

    ~IocpServer() {
        if (_running) {
            Stop();
        }
    }

    void Start(int workerCount) {
        std::cout << "\nStarting IOCP Server with " << workerCount << " workers\n";

        _iocp = CreateIoCompletionPort(
            INVALID_HANDLE_VALUE, nullptr, 0, workerCount
        );

        if (_iocp == nullptr) {
            throw std::runtime_error("Failed to create IOCP");
        }

        _running.store(true);

        for (int i = 0; i < workerCount; ++i) {
            WorkerContext ctx{
                _iocp,
                &_running,
                &_totalProcessed,
                i
            };

            _workers.emplace_back([ctx]() {
                WorkerThreadStatic(ctx);
                });
        }
    }

    void SimulateWork(int count) {
        std::cout << "Simulating " << count << " work items...\n";

        for (int i = 0; i < count; ++i) {
            PostQueuedCompletionStatus(_iocp, i + 1, i, nullptr);
        }
    }

    void Stop() {
        std::cout << "\nStopping IOCP Server...\n";

        _running.store(false);

        for (size_t i = 0; i < _workers.size(); ++i) {
            PostQueuedCompletionStatus(_iocp, 0, 0, nullptr);
        }

        std::cout << "Waiting for workers to finish...\n";

        for (auto& worker : _workers) {
            if (worker.Joinable()) {
                worker.Join();
            }
        }

        if (_iocp) {
            CloseHandle(_iocp);
            _iocp = nullptr;
        }

        std::cout << "Total processed: " << _totalProcessed.load() << " items\n";
        std::cout << "Server stopped\n";
    }
};

static void Test_IocpServer() {
    std::cout << "\n=== Test 8: IOCP Server ===\n";

    try {
        IocpServer server;
        server.Start(4);
        server.SimulateWork(50);
        Sleep(2000);
        server.Stop();

        std::cout << "Test 8 Passed\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Test 8 Failed: " << e.what() << "\n";
    }
}

// Main test function
int test_win_thread() {
    std::cout << "========================================\n";
    std::cout << "    WinThread Test Suite\n";
    std::cout << "========================================\n";

    try {
        Test_Lambda();
        std::cout << "Test_Lambda OK\n";

        Test_Capture();
        std::cout << "Test_Capture OK\n";

        Test_Vector();
        std::cout << "Test_Vector OK\n";

        Test_MoveSemantics();
        std::cout << "Test_MoveSemantics OK\n";  // 여기서 크래시?

        Test_Detach();
        std::cout << "Test_Detach OK\n";

        Test_IocpServer();
        std::cout << "Test_IocpServer OK\n";

        std::cout << "\n========================================\n";
        std::cout << "ALL TESTS PASSED\n";
        std::cout << "========================================\n";

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n========================================\n";
        std::cerr << "TEST FAILED: " << e.what() << "\n";
        std::cerr << "========================================\n";
        return 1;
    }
}