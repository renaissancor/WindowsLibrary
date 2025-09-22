#include "pch.h"

#include "Profiler.h" 

struct ThreadParam {
	int id;
};

static void funcA() noexcept {
	Profiler::EnterSection profile("funcA");
	Sleep(1 + rand() % 2);
	printf("In funcA\n");
}

static void funcB() noexcept {
	Profiler::EnterSection profile("funcB");
	Sleep(1 + rand() % 2);
	profile.LeaveSection();
	printf("In funcB\n");
}

static void funcC() noexcept {

	Sleep(1 + rand() % 2);
	Profiler::EnterSection profile("funcC");
	profile.LeaveSection();
	printf("In funcC\n");
}

static unsigned int __stdcall ThreadFunc(void* arg) noexcept {
	ThreadParam* param = reinterpret_cast<ThreadParam*>(arg);
	int thread_id = param->id;
	// int thread_id = GetCurrentThreadId(); 
	srand(static_cast<unsigned int>(time(NULL) + thread_id));
	for (int i = 0; i < 100; ++i) {
		if (rand() % 3 == 0) funcA();
		else if (rand() % 2 == 0) funcB();
		else funcC();
	}

	// Make profile directory first 
	string save_data_txt_path = "profile\\profiler_results_txt_thread_" + std::to_string(thread_id) + ".txt";
	Profiler::Manager::GetInstance().SaveDataTXT(save_data_txt_path, Profiler::MILISEC);
	string save_data_csv_path = "profile\\profiler_results_csv_thread_" + std::to_string(thread_id) + ".csv";
	Profiler::Manager::GetInstance().SaveDataCSV(save_data_csv_path, Profiler::MILISEC);
	string save_func_csv_path = "profile\\profiler_funcall_csv_thread_" + std::to_string(thread_id) + ".csv";
	Profiler::Manager::GetInstance().SaveFuncCSV(save_func_csv_path);

	return 0;
}


int TestProfiler() noexcept {

	constexpr const size_t thread_size = 4;

	HANDLE threads[thread_size] = { 0, };
	unsigned int thread_ids[thread_size] = { 0, };
	ThreadParam params[thread_size] = { { 0 }, };

	for (size_t i = 0; i < thread_size; ++i) {
		params[i].id = static_cast<int>(i);
	}

	for (size_t i = 0; i < thread_size; ++i) {
		threads[i] = (HANDLE)_beginthreadex(
			NULL,
			0,
			&ThreadFunc,
			&params[i],
			0,
			&thread_ids[i]
		);
	}

	WaitForMultipleObjects(thread_size, threads, TRUE, INFINITE);
	// Get Current Directory
	printf("\nCurrent Directory: ");
	char buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer);
	printf("%s\n", buffer);

	for (size_t i = 0; i < thread_size; ++i) {
		std::string path = "profile\\profiler_results_txt_thread_" + std::to_string(i) + ".txt";
		// Load File and Print 
		FILE* fp = nullptr;
		errno_t err = fopen_s(&fp, path.c_str(), "r");
		if (err != 0 || fp == nullptr) {
			printf("Error: Unable to open file %s for reading.\n", path.c_str());
			continue;
		}
		printf("Contents of %s:\n", path.c_str());
		// char buffer[256] = { 0, }; 
		while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
			printf("%s", buffer);
		}
		fclose(fp);
	}

	for (size_t i = 0; i < thread_size; ++i) {
		if (threads[i]) {
			CloseHandle(threads[i]);
			threads[i] = NULL;
		}
	}

	return 0;
}