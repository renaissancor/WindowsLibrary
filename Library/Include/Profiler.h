#pragma once 

// Profiler.h 
#include "cstr_hash_map.h"

namespace Win {
namespace Profiler {
	
	enum Unit {
		NANOSEC = 0, //  nanoseconds 
		MCROSEC = 1, // microseconds 
		MILISEC = 2, // milliseconds 
		SEC     = 3, //      seconds 
	};

	struct Record {
		long long enterTick;
		long long leaveTick;
	};

	struct SummaryData {
		long long totalTimeRaw = 0;
		long long minTimeRaw = LLONG_MAX;
		long long maxTimeRaw = LLONG_MIN;
		size_t callCount = 0;
	};
			
	class Manager {
	private:
		static constexpr const char* _unit_str[4] = { "ns", "us", "ms", " s" };
		static constexpr long double _unit_div[4] = { 1'000'000'000.0L, 1'000'000.0L, 1'000.0L, 1.0L };
		
		Manager() noexcept {
			QueryPerformanceFrequency(&_frequency);
			_thread_id = GetCurrentThreadId();
		}
		~Manager() noexcept = default;
		LARGE_INTEGER _frequency = { 0 }; // _frequency.QuadPart gives counts per second
		DWORD _thread_id = 0; 
		cstr_hash_map<std::vector<Record>> _records; 

	public:
		Manager(const Manager&) = delete;
		Manager& operator=(const Manager&) = delete;

		inline static Manager& GetInstance() noexcept
		{
			thread_local Manager instance;
			return instance;
		}
		
		inline DWORD GetThreadId() const noexcept { return _thread_id; }
		inline long long Frequency() const noexcept { return _frequency.QuadPart; }
		inline void Clear() noexcept { _records.clear(); } 
		inline const char* GetUnitStr(Unit unit) noexcept { return _unit_str[static_cast<int>(unit)]; }
		inline long double GetUnitMultiplier(Unit unit) noexcept { return _unit_div[static_cast<int>(unit)]; }

		inline void Add(const char* sectionName, long long enterTick, long long leaveTick) noexcept
		{
			auto& vec = _records[sectionName];
			vec.push_back(Record{ enterTick, leaveTick });
		}

		void PrintConsoleTick() const noexcept; 
		void PrintConsoleTime(Unit unit = MCROSEC) noexcept;  

		SummaryData GetFunctionSummary(const std::vector<Record>& records) const noexcept;

		void SaveDataTXT(const std::string& filepath, Unit unit = MCROSEC) noexcept;
		void SaveDataCSV(const std::string& filepath, Unit unit = MCROSEC) noexcept;
		void SaveFuncCSV(const std::string& filepath) noexcept;
	};

	class Enter {
	private:
		const char* _sectionName = nullptr;
		LARGE_INTEGER _enterTick;
		bool _stopped = false;

		void Stop() noexcept {
			if (_stopped) return;
			_stopped = true;
			LARGE_INTEGER leaveTick;
			QueryPerformanceCounter(&leaveTick);
			Manager::GetInstance().Add(_sectionName, _enterTick.QuadPart, leaveTick.QuadPart);

		}
	public:
		explicit Enter(const char* sectionName) noexcept
			: _sectionName(sectionName)
		{
			QueryPerformanceCounter(&_enterTick);
		}

		inline void Leave() noexcept { Stop(); }
		inline ~Enter() noexcept { Stop(); } 
		
	};
} // End of namespace Profiler 
} // End of namespace Win 