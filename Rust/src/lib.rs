fn rand() -> (f64, f64) {
    #[cfg(feature = "fastrand")]
    {
        use fastrand::Rng;
        let mut rng = Rng::new();
        (rng.f64(), rng.f64())
    }
    #[cfg(feature = "rand")]
    {
        use rand::Rng;
        let mut rng = rand::thread_rng();
        (rng.random::<f64>(), rng.random::<f64>())
    }
}

pub fn simulate_pi_seq(particles: usize) -> f64 {
    let mut inside_circle = 0;
    (0..particles).for_each(|_| {
        let (x, y) = rand();
        if x * x + y * y <= 1.0 {
            inside_circle += 1;
        }
    });

    4.0 * (inside_circle as f64) / (particles as f64)
}

pub fn simulate_pi_par(particles: usize) -> f64 {
    let num_threads = num_cpus::get();
    let chunk_size = particles.div_ceil(num_threads);

    let inside_circle: usize = std::thread::scope(|s| {
        let handles: Vec<_> = (0..num_threads)
            .map(|i| {
                let start = i * chunk_size;
                let end = (start + chunk_size).min(particles);
                s.spawn(move || {
                    let mut local_inside = 0;
                    (start..end).for_each(|_| {
                        let (x, y) = rand();
                        if x * x + y * y <= 1.0 {
                            local_inside += 1;
                        }
                    });

                    local_inside
                })
            })
            .collect();

        handles.into_iter().map(|h| h.join().unwrap()).sum()
    });

    4.0 * (inside_circle as f64) / (particles as f64)
}

pub fn bench<F, R>(func: F, bench_size: usize) -> Vec<R>
where
    F: Fn() -> R,
{
    let mut results: Vec<R> = Vec::with_capacity(bench_size);
    let times = (0..bench_size)
        .map(|_| {
            let start = std::time::Instant::now();
            let result = func();
            let duration = start.elapsed().as_secs_f64();
            results.push(result);
            duration
        })
        .collect::<Vec<_>>();
    let min = times.iter().cloned().fold(f64::INFINITY, f64::min);
    let max = times.iter().cloned().fold(f64::NEG_INFINITY, f64::max);
    let mean = times.iter().sum::<f64>() / (bench_size as f64);
    let stddev = (times
        .iter()
        .map(|time| {
            let diff = *time - mean;
            diff * diff
        })
        .sum::<f64>()
        / (bench_size as f64))
        .sqrt();
    println!(
        "mean: {:.4}s, stddev: {:.4}, min: {:.4}s, max: {:.4}s",
        mean, stddev, min, max,
    );
    results
}
