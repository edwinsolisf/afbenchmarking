#include <iostream>
#include <fstream>
#include <filesystem>

#include <arrayfire.h>
#include <benchmark/benchmark.h>

static std::filesystem::path assets = "../../assets";

af::array read_array(std::filesystem::path filename)
{
    std::ifstream file{ filename };

    if (file.bad() || !file.is_open())
    {
        std::cout << "Could not read file\n";
        return {};
    }

    af::dim4 in_dims;
    file >> in_dims[0] >> in_dims[1] >> in_dims[2] >> in_dims[3];
    size_t size = 1;
    af::dim4 dims(1);

    int nonzero = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (in_dims[i] > 0)
        {
            ++nonzero;
            size *= in_dims[i];
        }
    }

    dims[0] = nonzero > 0 ? in_dims[nonzero - 1] : 1;
    dims[1] = nonzero > 1 ? in_dims[nonzero - 2] : 1;
    dims[2] = nonzero > 2 ? in_dims[0] : 1;
    dims[3] = nonzero > 3 ? in_dims[1] : 1;

    std::vector<float> temp;
    temp.resize(size);

    size_t index = 0;
    float in_val = 0;
    while (file >> in_val)
        temp[index++] = in_val;
    
    auto out = af::array(dims, temp.data());

    return out.T();
}

bool approx_equal(const af::array& A,
                  const af::array& B,
                  const af::array& refC,
                  double max_abs_err = 1e-7,
                  double max_rel_err = 1e-6)
{
    auto C = af::matmul(A, B);

    auto diff = af::abs(C - refC);

    return af::allTrue<bool>((diff < max_abs_err) || (diff / (refC + max_abs_err) < max_rel_err));
}

void test_af_matmul_result()
{
    auto arr1 = read_array(assets/"in1.txt");
    auto arr2 = read_array(assets/"in2.txt");
    auto res = read_array(assets/"out.txt");

    std::cout << "equal? = " << std::boolalpha << approx_equal(arr1, arr2, res) << std::endl;
}

void BM_af_matmul(benchmark::State& state)
{
    static bool first = true;
    static af::array A;
    static af::array B;
    if (first)
    {
        af::setBackend(af::Backend::AF_BACKEND_CPU);
        af::setDevice(0);

        af::info();
        
        // Get test values
        A = read_array(assets/"in1.txt");
        B = read_array(assets/"in2.txt");

        test_af_matmul_result();

        first = false;
    }

    // First compile
    auto temp = af::matmul(A, B);
    temp.eval();
    af::sync();
    benchmark::DoNotOptimize(temp);

    // Benchmark
    af::array C;
    for (auto _ : state)
    {
        C = af::matmul(A, B);
        af::eval(C);
        af::sync();
        benchmark::DoNotOptimize(C);
    }
}

BENCHMARK(BM_af_matmul)->Unit(benchmark::kMillisecond)
                       ->Iterations(20)
                       ->Repetitions(1000)
                       ->ReportAggregatesOnly(true)
                       ->ComputeStatistics("max", [](const std::vector<double>& v) {
                            return *std::max_element(std::begin(v), std::end(v));
                       })
                       ->ComputeStatistics("min", [](const std::vector<double>& v) {
                            return *std::min_element(std::begin(v), std::end(v));
                       })
                       ->ComputeStatistics("99%", [](const std::vector<double>& v) {
                            std::vector<double> sorted(v.cbegin(), v.cend());
                            std::sort(sorted.begin(), sorted.end());
                            auto count = sorted.size();
                            auto result = (*(sorted.begin() + count * 0.99 - 1) +
                                                (size_t(0.99 * count) != count ?
                                                    *(sorted.begin() + count * 0.99) :
                                                    *(sorted.begin() + count * 0.99 - 1))
                                            ) / 2;

                            return result;
                       });
BENCHMARK_MAIN();