
struct RealEval {
    double dwarf, control3, control4, connection;
};

std::uniform_real_distribution<double> dist_spsa(-1, 1);
std::mt19937_64 merssenne(default_seed);

double binary_signum(double val) {
    if (val < 0) {
        return -1;
    } else {
        return 1;
    }
}

/**
 * @param iteration
 * @return a vector of random values in {-1,1}
 */
RealEval delta() {
    RealEval del{};
    del.dwarf = binary_signum(dist_spsa(merssenne));
    del.control3 = binary_signum(dist_spsa(merssenne));
    del.control4 = binary_signum(dist_spsa(merssenne));
    del.connection = binary_signum(dist_spsa(merssenne));
    return del;
}

RealEval times(const RealEval& eval, double factor) {
    RealEval res = {};
    res.dwarf = eval.dwarf * factor;
    res.control3 = eval.control3 * factor;
    res.control4 = eval.control4 * factor;
    res.connection = eval.connection * factor;
    return res;
}

RealEval invert(const RealEval& eval) {
    RealEval res = {};
    res.dwarf = 1 / eval.dwarf;
    res.control3 = 1 / eval.control3;
    res.control4 = 1 / eval.control4;
    res.connection = 1 / eval.connection;
    return res;
}

RealEval plus(const RealEval& summand1, const RealEval& summand2) {
    RealEval res = {};
    res.dwarf = summand1.dwarf + summand2.dwarf;
    res.control3 = summand1.control3 + summand2.control3;
    res.control4 = summand1.control4 + summand2.control4;
    res.connection = summand1.connection + summand2.connection;
    return res;
}

RealEval minus(const RealEval& summand1, const RealEval& summand2) {
    RealEval res = {};
    res.dwarf = summand1.dwarf - summand2.dwarf;
    res.control3 = summand1.control3 - summand2.control3;
    res.control4 = summand1.control4 - summand2.control4;
    res.connection = summand1.connection - summand2.connection;
    return res;
}

void print(const RealEval& real) {
    std::cout << "Dwarf: " << real.dwarf << ", control3: " << real.control3 << ", control4: " << real.control4 << ", connection: " << real.connection << std::endl;
}

EvalParameters from_real(const RealEval& real) {
    EvalParameters res = default_dwarf_eval;
    res.dwarf_factor = real.dwarf;
    res.control3 = real.control3;
    res.control4 = real.control4;
    res.dwarf_connection_factor = real.connection;
    return res;
}

RealEval current_dwarf = { default_dwarf_eval.dwarf_factor, default_dwarf_eval.control3, default_dwarf_eval.control4, default_dwarf_eval.dwarf_connection_factor };
RealEval current_troll = { default_troll_eval.dwarf_factor, default_troll_eval.control3, default_troll_eval.control4, default_troll_eval.dwarf_connection_factor };

std::mutex io_mutex;

int depth = 3;

double alpha = 0.602;
double spsa_gamma = 0.101;
/*
 Rk last = 0.002;
 ck last = 4;
 */
double a = 1, c = 10;

int iteration_count = 5000;
int A = iteration_count / 10;

double ak(int iterations) {
    return a / pow(iterations + A, alpha);
}

double ck(int iterations) {
    return c / pow(iterations + 1, spsa_gamma);
}
int iteration = 0;

void simple_spsa() {
    while (iteration < iteration_count) {
        io_mutex.lock();
        int local_iteration = iteration++;
        RealEval delta_dwarf = times(delta(), ck(local_iteration));
        //RealEval delta_troll = delta(local_iteration);
        EvalParameters plus_dwarf = from_real(plus(current_dwarf, delta_dwarf));
        //EvalParameters plus_troll = from_real(plus(current_troll, delta_troll));
        EvalParameters minus_dwarf = from_real(minus(current_dwarf, delta_dwarf));
        //EvalParameters minus_troll = from_real(minus(current_troll, delta_troll));
        io_mutex.unlock();

        auto result_plus = quiet_selfplay(depth, depth, plus_dwarf, default_troll_eval);
        auto result_minus = quiet_selfplay(depth, depth, minus_dwarf, default_troll_eval);
        auto result = result_plus - result_minus;
        //auto result = quiet_selfplay(2, 2, plus_dwarf, minus_troll) - quiet_selfplay(2, 2, minus_dwarf, plus_troll);

        io_mutex.lock();
        current_dwarf = plus(current_dwarf, times(invert(delta_dwarf), result * ak(local_iteration)));
        //current_troll = plus(current_troll, delta_troll, winner * apply_factor);

        std::cout << "Iteration " << local_iteration << " Result: " << result_plus << " ";
        print(current_dwarf);
        std::cout << "Iteration " << local_iteration << " Results: " <<  result_minus << " ";
        print(current_dwarf);
        //print(current_troll);
        io_mutex.unlock();
    }
}

void multi_thread_spsa(int num_threads) {
    std::vector<std::thread> threads(num_threads);

    for (int i = 0; i < num_threads; ++i) {
        threads[i] = std::thread(simple_spsa);
    }

    for (auto& th : threads) {
        th.join();
    }
}