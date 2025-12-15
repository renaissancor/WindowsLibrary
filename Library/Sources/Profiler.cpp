#include "pch.h"

#include "profiler.h"

// Profiler.cpp 

void Win::Profiler::Manager::PrintConsoleTick() const noexcept
{
	printf("----------------------------------\n");
	for (auto it = _records.begin(); it != _records.end(); ++it) {
		const char* functionName = it.key();
		const std::vector<Record>& records = it.value();

		if (records.empty()) continue;

		SummaryData summary = GetFunctionSummary(records);

		printf("Function %s Calls : %zu\n", functionName, summary.callCount);
		printf("Frequency    : %11lld ticks/sec \n", _frequency.QuadPart);
		printf("Total Ticks  : %11lld \n", summary.totalTimeRaw);
		printf("Min Ticks    : %11lld \n", summary.minTimeRaw);
		printf("Max Ticks    : %11lld \n", summary.maxTimeRaw);
		printf("----------------------------------\n");
	}
}


void Win::Profiler::Manager::PrintConsoleTime(Unit unit) noexcept {
	long double multiplier = GetUnitMultiplier(unit);
	const char* unit_str = GetUnitStr(unit);
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
		SummaryData summary = GetFunctionSummary(records);

		long double total_time = static_cast<long double>(summary.totalTimeRaw) / frequency * multiplier;
		long double avg_time = total_time / summary.callCount;
		long double min_time = static_cast<long double>(summary.minTimeRaw) / frequency * multiplier;
		long double max_time = static_cast<long double>(summary.maxTimeRaw) / frequency * multiplier;

		printf("Function %s Calls : %zu\n", func_name, summary.callCount);
		printf("Total Time   : %16.4Lf %s \n", total_time, unit_str);
		printf("Average Time : %16.4Lf %s \n", avg_time, unit_str);
		printf("Min Time     : %16.4Lf %s \n", min_time, unit_str);
		printf("Max Time     : %16.4Lf %s \n", max_time, unit_str);
		printf("----------------------------------\n");
	}
}


Win::Profiler::SummaryData Win::Profiler::Manager::GetFunctionSummary
	(const std::vector<Win::Profiler::Record>& records) const noexcept {

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

void Win::Profiler::Manager::SaveDataTXT(const std::string& filepath, Unit unit) noexcept {
	FILE* file = nullptr;
	fopen_s(&file, filepath.c_str(), "w");
	if (!file) {
		std::cerr << "Error: Unable to open file " << filepath << " for writing.\n";
		return;
	}
	long double multiplier = GetUnitMultiplier(unit);
	const char* unit_str = GetUnitStr(unit);
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

		SummaryData summary = GetFunctionSummary(records);

		long double total_time = static_cast<long double>(summary.totalTimeRaw) / frequency * multiplier;
		long double avg_time = total_time / summary.callCount;
		long double min_time = static_cast<long double>(summary.minTimeRaw) / frequency * multiplier;
		long double max_time = static_cast<long double>(summary.maxTimeRaw) / frequency * multiplier;

		fprintf(file, "Function %s Calls : %zu\n", func_name, summary.callCount);
		fprintf(file, "Total Time   : %16.4Lf %s \n", total_time, unit_str);
		fprintf(file, "Average Time : %16.4Lf %s \n", avg_time, unit_str);
		fprintf(file, "Min Time     : %16.4Lf %s \n", min_time, unit_str);
		fprintf(file, "Max Time     : %16.4Lf %s \n", max_time, unit_str);
		fprintf(file, "----------------------------------\n");
	}
	fclose(file);
}

void Win::Profiler::Manager::SaveDataCSV(const std::string& filepath, Unit unit) noexcept {
	FILE* file = nullptr;
	fopen_s(&file, filepath.c_str(), "w");
	if (!file) {
		std::cerr << "Error: Unable to open file " << filepath << " for writing.\n";
		return;
	}
	long double multiplier = GetUnitMultiplier(unit);
	const char* unit_str = GetUnitStr(unit);
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

		SummaryData summary = GetFunctionSummary(records);

		long double total_time = static_cast<long double>(summary.totalTimeRaw) / frequency * multiplier;
		long double avg_time = total_time / summary.callCount;
		long double min_time = static_cast<long double>(summary.minTimeRaw) / frequency * multiplier;
		long double max_time = static_cast<long double>(summary.maxTimeRaw) / frequency * multiplier;

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


void Win::Profiler::Manager::SaveFuncCSV(const std::string& filepath) noexcept {
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

		SummaryData summary = GetFunctionSummary(records);

		fprintf(file, "%s,%zu,%lld,%lld,%lld\n",
			func_name,
			summary.callCount, 
			summary.totalTimeRaw,
			summary.minTimeRaw,
			summary.maxTimeRaw
		);
	}
	fclose(file);
}