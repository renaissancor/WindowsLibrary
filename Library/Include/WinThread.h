#pragma once
#include <functional>
#include <process.h>
#include <stdexcept>

// WinThread.h 

class Thread {
private:
    HANDLE _threadHandle = nullptr;
    DWORD _id = 0;

    static unsigned int __stdcall ThreadProc(void* param) {
        auto* func = static_cast<std::function<void()>*>(param);
        try {
            (*func)();
        }
        catch (...) {}
        delete func;
        return 0;
    }

public:
    Thread() noexcept = default;

    template<typename Func>
    explicit Thread(Func&& func) {
        auto* cmd = new std::function<void()>(std::forward<Func>(func));
        unsigned int id = 0;
        auto h = _beginthreadex(nullptr, 0, ThreadProc, cmd, 0, &id);
        if (h == 0) {
            delete cmd;
            throw std::runtime_error("Failed to create thread");
        }
        _threadHandle = reinterpret_cast<HANDLE>(h);
        _id = static_cast<DWORD>(id);
    }

    ~Thread() {
        if (Joinable()) std::terminate();
    }

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread(Thread&& other) noexcept
        : _threadHandle(other._threadHandle), _id(other._id) {
        other._threadHandle = nullptr;
        other._id = 0;
    }

    Thread& operator=(Thread&& other) noexcept {
        if (this != &other) {
            if (Joinable()) std::terminate();
            _threadHandle = other._threadHandle;
            _id = other._id;
            other._threadHandle = nullptr;
            other._id = 0;
        }
        return *this;
    }

    void Join() {
        if (!Joinable()) throw std::runtime_error("Not joinable");
        WaitForSingleObject(_threadHandle, INFINITE);
        CloseHandle(_threadHandle);
        _threadHandle = nullptr;
    }

    void Detach() {
        if (!Joinable()) throw std::runtime_error("Not joinable");
        CloseHandle(_threadHandle);
        _threadHandle = nullptr;
    }

    bool Joinable() const noexcept { return _threadHandle != nullptr; }
    DWORD GetId() const noexcept { return _id; }
    HANDLE GetHandle() const noexcept { return _threadHandle; }
};