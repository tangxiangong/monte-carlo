#include <print>
#include <thread>

import montecarlo;

int main() {
    size_t number_of_threads = std::thread::hardware_concurrency();
    size_t particles = 10000000;
    size_t bench_size = 10;
    std::println("Benchmarking sequential Monte Carlo Pi simulation with {} particles:", particles);
    auto results = bench([=](){ return simulate_pi(particles, false); }, bench_size);
    std::println("result: {}", results);

    std::println("Benchmarking parallel Monte Carlo Pi simulation with {} particles, {} threads:", particles, number_of_threads);
    results = bench([=](){ return simulate_pi(particles); }, bench_size);
    std::println("result: {}", results);
    return 0;
}
