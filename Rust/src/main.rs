use montecarlo::{bench, simulate_pi_par, simulate_pi_seq};

fn main() {
    let number_of_threads = num_cpus::get();
    let particles = 10000000;
    let bench_size = 10;

    println!(
        "Benchmarking sequential Monte Carlo Pi simulation with {} particles:",
        particles
    );
    let result = bench(|| simulate_pi_seq(particles), bench_size);
    println!("result: {:?}", result);

    println!(
        "Benchmarking parallel Monte Carlo Pi simulation with {} particles, {} threads:",
        particles, number_of_threads
    );
    let result = bench(|| simulate_pi_par(particles), bench_size);
    println!("result: {:?}", result);
}
