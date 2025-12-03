#include "pch.h"
#include "indexed_heap.h"

// 1. 기본 기능 테스트
void test_basic() {
    printf("=== Basic Test ===\n");
    indexed_heap<float> pq(100);

    assert(pq.empty());
    assert(pq.size() == 0);
    assert(pq.pop() == -1);

    pq.push(5, 50.0f);
    pq.push(3, 30.0f);
    pq.push(7, 70.0f);
    pq.push(1, 10.0f);

    assert(pq.size() == 4);
    assert(pq.contains(5));
    assert(!pq.contains(99));

    assert(pq.pop() == 1);
    assert(pq.pop() == 3);
    assert(pq.pop() == 5);
    assert(pq.pop() == 7);
    assert(pq.empty());

    printf("PASSED\n\n");
}

// 2. decrease_key 테스트
void test_decrease_key() {
    printf("=== Decrease Key Test ===\n");
    indexed_heap<float> pq(100);

    pq.push(0, 100.0f);
    pq.push(1, 50.0f);
    pq.push(2, 75.0f);

    pq.decrease_key(0, 10.0f);
    assert(pq.pop() == 0);

    pq.push(3, 20.0f);
    pq.decrease_key(3, 100.0f);
    assert(pq.get_priority(3) == 20.0f);

    printf("PASSED\n\n");
}

// 3. 중복 push 테스트
void test_duplicate_push() {
    printf("=== Duplicate Push Test ===\n");
    indexed_heap<float> pq(100);

    pq.push(5, 100.0f);
    pq.push(5, 50.0f);
    pq.push(5, 200.0f);

    assert(pq.size() == 1);
    assert(pq.get_priority(5) == 50.0f);

    printf("PASSED\n\n");
}

// 4. 그리드 경로탐색 시뮬레이션
void test_grid_pathfinding() {
    printf("=== Grid Pathfinding Simulation ===\n");
    const int width = 100;
    const int height = 100;

    indexed_heap<float> pq(width * height);
    std::vector<float> dist(width * height, FLT_MAX);
    std::vector<bool> visited(width * height, false);

    int start = 0;
    int goal = width * height - 1;

    dist[start] = 0;
    pq.push(start, 0);

    int nodes_processed = 0;

    while (!pq.empty()) {
        int curr = pq.pop();
        if (visited[curr]) continue;
        visited[curr] = true;
        nodes_processed++;

        if (curr == goal) break;

        int cx = curr % width;
        int cy = curr / width;

        int dx[] = { 0, 0, 1, -1 };
        int dy[] = { 1, -1, 0, 0 };

        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

            int next = ny * width + nx;
            float new_dist = dist[curr] + 1.0f;

            if (new_dist < dist[next]) {
                dist[next] = new_dist;
                pq.push(next, new_dist);
            }
        }
    }

    printf("Goal distance: %.1f\n", dist[goal]);
    printf("Nodes processed: %d\n", nodes_processed);
    assert(dist[goal] == 198.0f);
    printf("PASSED\n\n");
}

// 5. 성능 비교
void test_performance() {
    printf("=== Performance Test ===\n");
    const int N = 100000;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> id_dist(0, N - 1);
    std::uniform_real_distribution<float> priority_dist(0.0f, 10000.0f);

    std::vector<std::pair<int, float>> operations(N);
    for (int i = 0; i < N; i++) {
        operations[i].first = id_dist(gen);
        operations[i].second = priority_dist(gen);
    }

    // indexed_heap
    auto start = std::chrono::high_resolution_clock::now();
    {
        indexed_heap<float> pq(N);
        for (int i = 0; i < N; i++) {
            pq.push(operations[i].first, operations[i].second);
        }
        while (!pq.empty()) {
            pq.pop();
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    long long indexed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    printf("indexed_heap: %lld us\n", indexed_time);

    // std::priority_queue
    // std::priority_queue
    start = std::chrono::high_resolution_clock::now();
    {
        std::priority_queue<
            std::pair<float, int>,
            std::vector<std::pair<float, int>>,
            std::greater<std::pair<float, int>> > pq;
        std::vector<bool> visited(N, false);
        for (int i = 0; i < N; i++) {
            pq.push(std::make_pair(operations[i].second, operations[i].first));
        }
        while (!pq.empty()) {
            std::pair<float, int> top = pq.top();
            pq.pop();
            if (!visited[top.second]) visited[top.second] = true;
        }
    }

    end = std::chrono::high_resolution_clock::now();
    long long stl_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    printf("std::priority_queue: %lld us\n", stl_time);
    printf("Ratio: %.2fx\n", (float)stl_time / indexed_time);
    printf("\n");
}

// 6. 스트레스 테스트
void test_stress() {
    printf("=== Stress Test ===\n");
    const int N = 10000;
    indexed_heap<int> pq(N);
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int round = 0; round < 100; round++) {
        for (int i = 0; i < N / 2; i++) {
            int id = gen() % N;
            int priority = gen() % 10000;
            pq.push(id, priority);
        }

        for (int i = 0; i < N / 10; i++) {
            int id = gen() % N;
            if (pq.contains(id)) {
                int curr_priority = pq.get_priority(id);
                int new_priority = gen() % (curr_priority + 1);
                pq.decrease_key(id, new_priority);
            }
        }

        size_t pop_count = pq.size() / 2;
        for (size_t i = 0; i < pop_count; i++) {
            pq.pop();
        }
    }

    printf("PASSED\n\n");
}


void test_dijkstra_simulation() {
    printf("=== Dijkstra Simulation (decrease_key heavy) ===\n");
    const int N = 10000;
    const int EDGES_PER_NODE = 10;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> node_dist(0, N - 1);
    std::uniform_real_distribution<float> weight_dist(1.0f, 100.0f);

    // 랜덤 그래프 생성
    std::vector<std::vector<std::pair<int, float>>> graph(N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < EDGES_PER_NODE; j++) {
            int to = node_dist(gen);
            float w = weight_dist(gen);
            graph[i].push_back(std::make_pair(to, w));
        }
    }

    // indexed_heap 버전
    auto start = std::chrono::high_resolution_clock::now();
    {
        indexed_heap<float> pq(N);
        std::vector<float> dist(N, FLT_MAX);
        dist[0] = 0;
        pq.push(0, 0);

        while (!pq.empty()) {
            int u = pq.pop();
            for (size_t i = 0; i < graph[u].size(); i++) {
                int v = graph[u][i].first;
                float w = graph[u][i].second;
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push(v, dist[v]);  // 내부에서 decrease_key 처리
                }
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    long long indexed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    printf("indexed_heap: %lld us\n", indexed_time);

    // std::priority_queue 버전 (중복 push)
    start = std::chrono::high_resolution_clock::now();
    {
        std::priority_queue<
            std::pair<float, int>,
            std::vector<std::pair<float, int>>,
            std::greater<std::pair<float, int>> > pq;
        std::vector<float> dist(N, FLT_MAX);
        std::vector<bool> visited(N, false);
        dist[0] = 0;
        pq.push(std::make_pair(0.0f, 0));

        while (!pq.empty()) {
            std::pair<float, int> top = pq.top();
            pq.pop();
            int u = top.second;
            if (visited[u]) continue;
            visited[u] = true;

            for (size_t i = 0; i < graph[u].size(); i++) {
                int v = graph[u][i].first;
                float w = graph[u][i].second;
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push(std::make_pair(dist[v], v));  // 중복 삽입
                }
            }
        }
    }
    end = std::chrono::high_resolution_clock::now();
    long long stl_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    printf("std::priority_queue: %lld us\n", stl_time);
    printf("Ratio: %.2fx\n", (float)stl_time / indexed_time);
    printf("\n");
}


void test_indexed_heap() {
    test_basic();
    test_decrease_key();
    test_duplicate_push();
    test_grid_pathfinding();
    test_performance();
    test_stress();
	test_dijkstra_simulation(); 
    printf("=== All Tests Passed ===\n");
}