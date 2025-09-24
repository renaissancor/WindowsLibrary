#include "pch.h"

#include "ro_hash_map.h"

void test_ro_hash_map_performance() noexcept {
    constexpr size_t N = 10000000;

    // 1) 런타임에 uintptr_t 키 생성
    std::vector<uintptr_t> raw_keys(N);
    for (size_t i = 0; i < N; ++i) {
		raw_keys[i] = (i * 2654435761) % 4294967291; // Knuth's multiplicative hash
    }

    // 2) ro_hash_map 테스트
    jhp::ro_hash_map<int> ro_map(N);
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        ro_map.insert(reinterpret_cast<const char*>(raw_keys[i]), static_cast<int>(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "ro_hash_map insert: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
        << " us\n";

    // 3) unordered_map 테스트
    std::unordered_map<const char*, int> std_map;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        std_map[reinterpret_cast<const char*>(raw_keys[i])] = static_cast<int>(i);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "unordered_map insert: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
        << " us\n";

    // 4) 간단 조회 테스트
    int* val = ro_map.find(reinterpret_cast<const char*>(raw_keys[N / 2]));
    std::cout << "Middle key in ro_hash_map: " << (val ? *val : -1) << "\n";

    auto it = std_map.find(reinterpret_cast<const char*>(raw_keys[N / 2]));
    std::cout << "Middle key in unordered_map: " << (it != std_map.end() ? it->second : -1) << "\n";
}

void test_ro_hash_map() noexcept {
    jhp::ro_hash_map<std::string> my_map(128);
    // Insert some key-value pairs
    std::string val1 = "Value1";
    std::string val2 = "Value2";
    std::string val3 = "Value3";
    my_map.insert("Key1", val1);
    my_map.insert("Key2", val2);
    my_map.insert("Key3", std::move(val3)); // Using move semantics
    // Retrieve values
    if (auto v = my_map.find("Key1")) {
        std::cout << "Key1: " << *v << "\n";
    } else {
        std::cout << "Key1 not found\n";
    }
    if (auto v = my_map["Key2"]) {
        std::cout << "Key2: " << *v << "\n";
    } else {
        std::cout << "Key2 not found\n";
    }
    if (auto v = my_map.at("Key3")) {
        std::cout << "Key3: " << *v << "\n";
    } else {
        std::cout << "Key3 not found\n";
    }
    // Erase a key
    my_map.erase("Key2");
    if (auto v = my_map.find("Key2")) {
        std::cout << "Key2: " << *v << "\n";
    } else {
        std::cout << "Key2 not found after erase\n";
    }
    // Iterate over all elements
    std::cout << "All key-value pairs:\n";
    for (auto it = my_map.begin(); it != my_map.end(); ++it) {
		auto kv = *it;         
        std::cout << kv.first << ": " << kv.second << "\n";
    }

    // Clear the map
    my_map.clear();
    if (auto v = my_map.find("Key1")) {
        std::cout << "Key1: " << *v << "\n";
    } else {
        std::cout << "Map cleared, Key1 not found\n";
	}
}
