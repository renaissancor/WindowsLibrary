#pragma once 

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <conio.h>
#include <unordered_map> 
#include <wtypes.h> 
#include <string> 
#include <fstream>
#include <iomanip>
#include <windows.h> 
#include <cstring> 

// Profiler.hpp 

using std::vector;
using std::string;
using std::unordered_map;
using std::pair;
using std::cout;
using std::cerr;

namespace Profiler {
	enum Unit {
		NANOSEC, //  nanoseconds 
		MCROSEC, // microseconds 
		MILISEC, // milliseconds 
		SEC, //      seconds 
	};
	constexpr const char* UnitStr[] = { "ns", "us", "ms", " s" };
	static LARGE_INTEGER _frequency;

	static void InitProfiler() noexcept {
		QueryPerformanceFrequency(&_frequency);
	}

	inline static const long long Frequency() noexcept {
		return _frequency.QuadPart;
	}

	inline static const long double GetUnitMultiplier
	(Profiler::Unit unit) noexcept {
		switch (unit) {
		case Profiler::NANOSEC: return 1'000'000'000.0L;
		case Profiler::MCROSEC: return 1'000'000.0L;
		case Profiler::MILISEC: return 1'000.0L;
		case Profiler::SEC: return 1.0L;
		}
		return 1.0L;
	}

	// Singleton Manager 
	class Manager {
	private:
		Manager() noexcept { InitProfiler(); }
		~Manager() noexcept = default;
		Manager(const Manager&) = delete;
		Manager& operator=(const Manager&) = delete;

	private:
		typedef struct _Data {
			long long init;
			long long term;
		} Data;
		unordered_map<const char*, vector<Data>> _func_map;

	public:
		static Manager& GetInstance() noexcept
		{
			thread_local Manager instance;
			return instance;
		}

		inline void ClearData() noexcept {
			_func_map.clear();
		}

		void AddProfile(const char* section_name, long long init, long long term) noexcept;
		void PrintConsoleTick() const noexcept;
		void PrintConsoleTime(Unit unit) const noexcept;

		void SaveDataTXT(const string& filepath, Unit unit) noexcept;
		void SaveDataCSV(const string& filepath, Unit unit) noexcept;
		void SaveFuncCSV(const string& filepath, Unit unit) noexcept;
	};

	class EnterSection {
	private:
		bool _finish;
		const char* _section_name;
		LARGE_INTEGER _time_init;

	public:
		EnterSection(const char* section_name) noexcept;
		void LeaveSection() noexcept;
		~EnterSection() noexcept;
	};
}
