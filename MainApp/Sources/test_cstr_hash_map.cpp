#include "pch.h"
#include "cstr_hash_map.h"
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

void test_cstr_hash_map_performance() noexcept {
    constexpr size_t N = 10000000;

    // 문자열 리터럴 시뮬레이션을 위해 실제 문자열 생성
    std::vector<std::string> key_storage(N);
    std::vector<const char*> keys(N);
    for (size_t i = 0; i < N; ++i) {
        key_storage[i] = "key_" + std::to_string(i);
        keys[i] = key_storage[i].c_str();
    }

    // 1) cstr_hash_map 테스트
    cstr_hash_map<int> cstr_map(N);
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        cstr_map.insert(keys[i], static_cast<int>(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "cstr_hash_map insert: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
        << " us\n";

    // 2) unordered_map<std::string, int> 테스트
    std::unordered_map<std::string, int> std_map;
    std_map.reserve(N);
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        std_map[keys[i]] = static_cast<int>(i);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "unordered_map insert: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
        << " us\n";

    // 3) 조회 성능 테스트
    volatile int sum1 = 0, sum2 = 0;

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        auto it = cstr_map.find(keys[i]);
        if (it != cstr_map.end()) sum1 += it.value();
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "cstr_hash_map find: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
        << " us\n";

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        auto it = std_map.find(keys[i]);
        if (it != std_map.end()) sum2 += it->second;
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "unordered_map find: "
        << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
        << " us\n";

    // 결과 확인
    std::cout << "cstr_hash_map middle key: " << cstr_map[keys[N / 2]] << "\n";
    std::cout << "unordered_map middle key: " << std_map[keys[N / 2]] << "\n";
}

void test_cstr_hash_map() noexcept {
    cstr_hash_map<std::string> my_map(128);

    // Insert some key-value pairs
    my_map.insert("Key1", "Value1");
    my_map.insert("Key2", "Value2");
    my_map.insert("Key3", "Value3");

    // Retrieve values using find
    auto it1 = my_map.find("Key1");
    if (it1 != my_map.end()) {
        std::cout << "Key1: " << it1.value() << "\n";
    }
    else {
        std::cout << "Key1 not found\n";
    }

    // Retrieve using operator[]
    std::cout << "Key2: " << my_map["Key2"] << "\n";
    std::cout << "Key3: " << my_map["Key3"] << "\n";

    // Erase a key
    my_map.erase("Key2");
    auto it2 = my_map.find("Key2");
    if (it2 != my_map.end()) {
        std::cout << "Key2: " << it2.value() << "\n";
    }
    else {
        std::cout << "Key2 not found after erase\n";
    }

    // Iterate over all elements
    std::cout << "All key-value pairs:\n";
    for (auto it = my_map.begin(); it != my_map.end(); ++it) {
        std::cout << it.key() << ": " << it.value() << "\n";
    }

    // Test operator[] auto-insert
    my_map["NewKey"] = "NewValue";
    std::cout << "NewKey: " << my_map["NewKey"] << "\n";

    // Size and empty
    std::cout << "Size: " << my_map.size() << "\n";
    std::cout << "Empty: " << (my_map.empty() ? "yes" : "no") << "\n";

    // Clear the map
    my_map.clear();
    std::cout << "After clear, size: " << my_map.size() << "\n";

    auto it3 = my_map.find("Key1");
    if (it3 != my_map.end()) {
        std::cout << "Key1: " << it3.value() << "\n";
    }
    else {
        std::cout << "Map cleared, Key1 not found\n";
    }
}


