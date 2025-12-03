#include "pch.h"

#include "profiler.h"

// Profiler.cpp 

void Profiler::Manager::PrintConsoleTick() const noexcept
{
	printf("----------------------------------\n");
	for (auto it = _records.begin(); it != _records.end(); ++it) {
		const char* functionName = it.key();

		const std::vector<Record>& records = it.value();

		if (records.empty()) continue;

		long long total_time_raw = 0;
		long long min_time_raw = LLONG_MAX;
		long long max_time_raw = LLONG_MIN;

		for (const auto& rec : records) {
			long long tick_row = rec.leaveTick - rec.enterTick;
			total_time_raw += tick_row;
			if (tick_row < min_time_raw) min_time_raw = tick_row;
			if (tick_row > max_time_raw) max_time_raw = tick_row;
		}

		printf("Function %s Calls : %zu\n", functionName, records.size());
		printf("Frequency    : %11lld ticks/sec \n", _frequency.QuadPart);
		printf("Total Ticks  : %11lld \n", total_time_raw);
		printf("Min Ticks    : %11lld \n", min_time_raw);
		printf("Max Ticks    : %11lld \n", max_time_raw);
		printf("----------------------------------\n");
	}
}


void Profiler::Manager::PrintConsoleTime(Unit unit) noexcept {
	long double multiplier = GetUnitMultiplier(unit);
	const char* unit_str = GetUnitStr(unit); 
	QueryPerformanceFrequency(&_frequency);
	long double frequency = static_cast<long double>(_frequency.QuadPart);

	if (frequency == 0.0L) {
		printf("Error: Performance counter frequency is zero. Cannot calculate time.\n");
		return;
	}

	printf("----------------------------------\n");
	for (auto it = _records.begin(); it != _records.end(); ++it) {
		const char* func_name = it.key();
		const std::vector<Record>& records = it.value();

		if (records.empty()) continue;

		long long total_time_raw = 0;
		long long min_time_raw = LLONG_MAX;
		long long max_time_raw = LLONG_MIN;

		for (const auto& rec : records) {
			long long tick_row = rec.leaveTick - rec.enterTick;
			total_time_raw += tick_row;
			if (tick_row < min_time_raw) min_time_raw = tick_row;
			if (tick_row > max_time_raw) max_time_raw = tick_row;
		}

		long double total_time = static_cast<long double>(total_time_raw) / frequency * multiplier;
		long double avg_time = total_time / records.size();
		long double min_time = static_cast<long double>(min_time_raw) / frequency * multiplier;
		long double max_time = static_cast<long double>(max_time_raw) / frequency * multiplier;

		printf("Function %s Calls : %zu\n", func_name, records.size());
		printf("Total Time   : %16.4Lf %s \n", total_time, unit_str);
		printf("Average Time : %16.4Lf %s \n", avg_time, unit_str);
		printf("Min Time     : %16.4Lf %s \n", min_time, unit_str);
		printf("Max Time     : %16.4Lf %s \n", max_time, unit_str);
		printf("----------------------------------\n");
	}
}

Profiler::SummaryData Profiler::Manager::GetFunctionSummary
	(const std::vector<Profiler::Record>& records) const noexcept {

	SummaryData summary; 
	summary.callCount = records.size(); 

	for (const auto& rec : records) {
		long long tick_row = rec.leaveTick - rec.enterTick;
		summary.totalTimeRaw += tick_row;
		if (tick_row < summary.minTimeRaw) summary.minTimeRaw = tick_row;
		if (tick_row > summary.maxTimeRaw) summary.maxTimeRaw = tick_row;
	}
	if (summary.callCount == 0) {
		summary.minTimeRaw = 0; 
		summary.maxTimeRaw = 0; 
	}
	return summary;
}

void Profiler::Manager::SaveDataTXT(const std::string& filepath, Unit unit) noexcept {
	FILE* file = nullptr;
	fopen_s(&file, filepath.c_str(), "w");
	if (!file) {
		std::cerr << "Error: Unable to open file " << filepath << " for writing.\n";
		return;
	}
	long double multiplier = GetUnitMultiplier(unit);
	const char* unit_str = GetUnitStr(unit);
	QueryPerformanceFrequency(&_frequency);
	long double frequency = static_cast<long double>(_frequency.QuadPart);
	if (frequency == 0.0L) {
		fprintf(file, "Error: Performance counter frequency is zero. Cannot calculate time.\n");
		fclose(file);
		return;
	}
	fprintf(file, "----------------------------------\n");
	for (auto it = _records.begin(); it != _records.end(); ++it) {
		const char* func_name = it.key();
		const std::vector<Record>& records = it.value();
		if (records.empty()) continue;
		long long total_time_raw = 0;
		long long min_time_raw = LLONG_MAX;
		long long max_time_raw = LLONG_MIN;
		for (const auto& rec : records) {
			long long tick_row = rec.leaveTick - rec.enterTick;
			total_time_raw += tick_row;
			if (tick_row < min_time_raw) min_time_raw = tick_row;
			if (tick_row > max_time_raw) max_time_raw = tick_row;
		}
		long double total_time = static_cast<long double>(total_time_raw) / frequency * multiplier;
		long double avg_time = total_time / records.size();
		long double min_time = static_cast<long double>(min_time_raw) / frequency * multiplier;
		long double max_time = static_cast<long double>(max_time_raw) / frequency * multiplier;
		fprintf(file, "Function %s Calls : %zu\n", func_name, records.size());
		fprintf(file, "Total Time   : %16.4Lf %s \n", total_time, unit_str);
		fprintf(file, "Average Time : %16.4Lf %s \n", avg_time, unit_str);
		fprintf(file, "Min Time     : %16.4Lf %s \n", min_time, unit_str);
		fprintf(file, "Max Time     : %16.4Lf %s \n", max_time, unit_str);
		fprintf(file, "----------------------------------\n");
	}
	fclose(file);
}

void Profiler::Manager::SaveDataCSV(const std::string& filepath, Unit unit) noexcept {
	FILE* file = nullptr;
	fopen_s(&file, filepath.c_str(), "w");
	if (!file) {
		std::cerr << "Error: Unable to open file " << filepath << " for writing.\n";
		return;
	}
	long double multiplier = GetUnitMultiplier(unit);
	const char* unit_str = GetUnitStr(unit);
	QueryPerformanceFrequency(&_frequency);
	long double frequency = static_cast<long double>(_frequency.QuadPart);
	if (frequency == 0.0L) {
		fprintf(file, "Error: Performance counter frequency is zero. Cannot calculate time.\n");
		fclose(file);
		return;
	}
	fprintf(file, "Function Name,Call Count,Total Time (%s),Average Time (%s),Min Time (%s),Max Time (%s)\n",
		unit_str, unit_str, unit_str, unit_str);
	for (auto it = _records.begin(); it != _records.end(); ++it) {
		const char* func_name = it.key();
		const std::vector<Record>& records = it.value();
		if (records.empty()) continue;
		long long total_time_raw = 0;
		long long min_time_raw = LLONG_MAX;
		long long max_time_raw = LLONG_MIN;
		for (const auto& rec : records) {
			long long tick_row = rec.leaveTick - rec.enterTick;
			total_time_raw += tick_row;
			if (tick_row < min_time_raw) min_time_raw = tick_row;
			if (tick_row > max_time_raw) max_time_raw = tick_row;
		}
		long double total_time = static_cast<long double>(total_time_raw) / frequency * multiplier;
		long double avg_time = total_time / records.size();
		long double min_time = static_cast<long double>(min_time_raw) / frequency * multiplier;
		long double max_time = static_cast<long double>(max_time_raw) / frequency * multiplier;
		fprintf(file, "%s,%zu,%.4Lf,%.4Lf,%.4Lf,%.4Lf\n",
			func_name,
			records.size(),
			total_time,
			avg_time,
			min_time,
			max_time
		);
	}
	fclose(file);
}

void Profiler::Manager::SaveFuncCSV(const std::string& filepath) noexcept {
	FILE* file = nullptr;
	fopen_s(&file, filepath.c_str(), "w");
	if (!file) {
		std::cerr << "Error: Unable to open file " << filepath << " for writing.\n";
		return;
	}
	fprintf(file, "Function Name,Call Count,Total Ticks,Min Ticks,Max Ticks\n");
	for (auto it = _records.begin(); it != _records.end(); ++it) {
		const char* func_name = it.key();
		const std::vector<Record>& records = it.value();
		if (records.empty()) continue;
		long long total_time_raw = 0;
		long long min_time_raw = LLONG_MAX;
		long long max_time_raw = LLONG_MIN;
		for (const auto& rec : records) {
			long long tick_row = rec.leaveTick - rec.enterTick;
			total_time_raw += tick_row;
			if (tick_row < min_time_raw) min_time_raw = tick_row;
			if (tick_row > max_time_raw) max_time_raw = tick_row;
		}
		fprintf(file, "%s,%zu,%lld,%lld,%lld\n",
			func_name,
			records.size(),
			total_time_raw,
			min_time_raw,
			max_time_raw
		);
	}
	fclose(file);
}

