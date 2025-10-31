module;

#include <algorithm>
#include <chrono>
#include <concepts>
#include <print>
#include <thread>
#include <random>
#include <vector>
#include <numeric>
#include <ranges>

export module montecarlo;

constexpr double NANOSECONDS_PER_SECOND = 1000000000.0;

export template<typename F>
requires std::invocable<F>
void bench(F func, size_t bench_size) {
    using R = std::conditional_t<std::is_void_v<std::invoke_result_t<F>>, int, std::invoke_result_t<F>>;
    std::vector<double> times(bench_size);
    std::vector<R> results(bench_size);

    for (auto&& [val, result]: std::views::zip(times, results)) {
        auto start_time = std::chrono::high_resolution_clock::now();
        if constexpr (std::is_void_v<std::invoke_result_t<F>>) {
            func();
            result = 0;
        } else {
            result = func();
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        auto elapsed = static_cast<double>(duration.count()) / NANOSECONDS_PER_SECOND;
        val = elapsed;
    }
    std::println("result: {}", results);

    double mean = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    double stddev = std::sqrt(std::accumulate(times.begin(), times.end(), 0.0, [mean](double acc, double x) {
        return acc + ((x - mean) * (x - mean));
    }) / times.size());
    double min = *std::ranges::min_element(times);
    double max = *std::ranges::max_element(times);

    std::println("mean: {:.4f}s, stddev: {:.4f}, min: {:.4f}s, max: {:.4f}s", mean, stddev, min, max);
}

double simulate_pi_seq(size_t particles) {
    size_t inside_circle = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution dis(0.0, 1.0);

    for (size_t i = 0; i < particles; ++i) {
        double x = dis(gen);
        double y = dis(gen);
        if (x * x + y * y <= 1.0) {
            ++inside_circle;
        }
    }

    return (4.0 * inside_circle) / particles;
}

double simulate_pi_par(size_t particles) {
    size_t inside_circle = 0;
    size_t num_threads = std::thread::hardware_concurrency();
    size_t chunk_size = (particles + num_threads - 1) / num_threads;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    std::vector<size_t> results(num_threads, 0);

    std::random_device rd;
    std::seed_seq seed{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
    std::vector<std::uint32_t> seeds(num_threads);
    seed.generate(seeds.begin(), seeds.end());

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * chunk_size;
        size_t end = std::min(start + chunk_size, particles);
        threads.emplace_back([i, start, end, &results, thread_seed = seeds[i]]() {
            std::mt19937 gen(thread_seed);
            std::uniform_real_distribution dis(0.0, 1.0);
            size_t local_inside_circle = 0;

            for (size_t j = start; j < end; ++j) {
                double x = dis(gen);
                double y = dis(gen);
                if (x * x + y * y <= 1.0) {
                    ++local_inside_circle;
                }
            }
            results[i] = local_inside_circle;
        });
    }

    for (auto& t : threads) {
        if(t.joinable()) {
            t.join();
        }
    }

    for (const auto& count : results) {
        inside_circle += count;
    }

    return (4.0 * inside_circle) / particles;
}

export double simulate_pi(size_t particles, bool parallel = true) {
    if (parallel) {
        return simulate_pi_par(particles);
    } else {
        return simulate_pi_seq(particles);
    }
}
