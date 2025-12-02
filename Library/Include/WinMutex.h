#pragma once

// WinMutex.h 

namespace Win {

	class Mutex {
	private:
		CRITICAL_SECTION _cs;
	public:
		inline Mutex(int countSpinLock_ = 16) noexcept
		{
			InitializeCriticalSectionEx(&_cs, countSpinLock_, 0);
		} // can throw bad_alloc 

		inline ~Mutex() noexcept { DeleteCriticalSection(&_cs); }

		Mutex(const Mutex&) = delete;
		Mutex& operator=(const Mutex&) = delete;
		Mutex(Mutex&&) = delete;
		Mutex& operator=(Mutex&&) = delete;

		inline void Lock() noexcept { EnterCriticalSection(&_cs); }
		inline bool TryLock() noexcept { return TryEnterCriticalSection(&_cs) != 0; }
		inline void Unlock() noexcept { LeaveCriticalSection(&_cs); }
	};

	class SharedMutex {
	private:
		SRWLOCK _srwLock;
	public:
		SharedMutex(const SharedMutex&) = delete;
		SharedMutex& operator=(const SharedMutex&) = delete;
		SharedMutex(SharedMutex&&) = delete;
		SharedMutex& operator=(SharedMutex&&) = delete;

		SharedMutex() { InitializeSRWLock(&_srwLock); }
		~SharedMutex() = default;

		inline void LockExclusive() noexcept { AcquireSRWLockExclusive(&_srwLock); }
		inline bool TryLockExclusive() noexcept { return TryAcquireSRWLockExclusive(&_srwLock) != 0; }
		inline void UnlockExclusive() noexcept { ReleaseSRWLockExclusive(&_srwLock); }

		inline void LockShared() noexcept { AcquireSRWLockShared(&_srwLock); }
		inline bool TryLockShared() noexcept { return TryAcquireSRWLockShared(&_srwLock) != 0; }
		inline void UnlockShared() noexcept { ReleaseSRWLockShared(&_srwLock); }
	};

	class LockGuard {
	private:
		Mutex& _mtx;
	public:
		explicit inline LockGuard(Mutex& m) noexcept : _mtx(m) { _mtx.Lock(); }
		inline ~LockGuard() noexcept { _mtx.Unlock(); }

		LockGuard(const LockGuard&) = delete;
		LockGuard& operator=(const LockGuard&) = delete;
		LockGuard(LockGuard&&) = delete;
		LockGuard& operator=(LockGuard&&) = delete;
	};

	class UniqueLock {
	private:
		Mutex* _mtx;
		bool _owns;
	public:
		inline UniqueLock() noexcept : _mtx(nullptr), _owns(false) {}
		explicit inline UniqueLock(Mutex& mtx) : _mtx(&mtx), _owns(false) { Lock(); }

		inline bool OwnsLock() const noexcept { return _owns; }

		UniqueLock(UniqueLock&& other) noexcept
			: _mtx(other._mtx), _owns(other._owns) {
			other._mtx = nullptr;
			other._owns = false;
		}
		inline UniqueLock& operator=(UniqueLock&& other) noexcept {
			if (this == &other) return *this;
			if (_owns && _mtx) _mtx->Unlock();

			_mtx = other._mtx;
			_owns = other._owns;
			other._mtx = nullptr;
			other._owns = false;

			return *this;
		}

		inline ~UniqueLock() noexcept { if (_owns && _mtx) _mtx->Unlock(); }
		inline void Lock() {
			if (_mtx && !_owns) {
				_mtx->Lock();
				_owns = true;
			}
		}
		inline void Unlock() {
			if (_mtx && _owns) {
				_mtx->Unlock();
				_owns = false;
			}
		}
		inline bool TryLock() {
			if (_mtx && !_owns) {
				if (_mtx->TryLock()) {
					_owns = true;
					return true;
				}
			}
			return false;
		}
		inline Mutex* Release() noexcept {
			Mutex* temp = _mtx;
			_mtx = nullptr;
			_owns = false;
			return temp;
		}
	};

	class SharedLockGuard {
	private:
		SharedMutex& _mtx;
	public:
		explicit inline SharedLockGuard(SharedMutex& m) noexcept : _mtx(m) { _mtx.LockShared(); }
		inline ~SharedLockGuard() noexcept { _mtx.UnlockShared(); }

		SharedLockGuard(const SharedLockGuard&) = delete;
		SharedLockGuard& operator=(const SharedLockGuard&) = delete;
		SharedLockGuard(SharedLockGuard&&) = delete;
		SharedLockGuard& operator=(SharedLockGuard&&) = delete;
	};

	class ExclusiveLockGuard {
	private:
		SharedMutex& _mtx;
	public:
		explicit inline ExclusiveLockGuard(SharedMutex& m) noexcept : _mtx(m) { _mtx.LockExclusive(); }
		inline ~ExclusiveLockGuard() noexcept { _mtx.UnlockExclusive(); }

		ExclusiveLockGuard(const ExclusiveLockGuard&) = delete;
		ExclusiveLockGuard& operator=(const ExclusiveLockGuard&) = delete;
		ExclusiveLockGuard(ExclusiveLockGuard&&) = delete;
		ExclusiveLockGuard& operator=(ExclusiveLockGuard&&) = delete;
	};

} // namespace Win
