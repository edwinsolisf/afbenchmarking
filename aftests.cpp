#define NOMINMAX

#include <iostream>
#include <fstream>
#include <filesystem>

#include <arrayfire.h>
#include <benchmark/benchmark.h>

static std::filesystem::path assets = "../../assets";
static constexpr auto iterations = 20;
static constexpr auto repetitions = 1000;

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

bool approx_equal(const af::array& C,
                  const af::array& refC,
                  double max_abs_err = 1e-7,
                  double max_rel_err = 1e-6)
{
    auto diff = af::abs(C - refC);

    return af::allTrue<bool>((diff < max_abs_err) || (diff / (refC + max_abs_err) < max_rel_err));
}

void BM_af_matmul(benchmark::State& state)
{
    static bool first = true;
    static af::array A;
    static af::array B;
    if (first)
    {        
        // Get test values
        A = read_array(assets/"matmul_src.txt");
        B = read_array(assets/"matmul_weight.txt");
        
        auto C = af::matmul(A, B);
        auto ref = read_array(assets/"matmul_dst.txt");

        std::cout << "is matmul op correct? = " << std::boolalpha
            << approx_equal(C, ref) << std::endl;

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
                       ->Iterations(iterations)
                       ->Repetitions(repetitions)
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


void BM_af_convolve(benchmark::State& state)
{
    static bool first = true;
    static af::array image;
    static af::array kernel;

    const af::dim4 padding  = {1, 0, 0, 0};
    const af::dim4 dilation = {1, 1, 0, 0};
    const af::dim4 stride   = {2, 1, 0, 0};

    auto convolve = [padding, dilation, stride](const af::array& A, const af::array& B){
        auto C = af::convolve2NN(image, kernel, stride, padding, dilation);
        return C * 0.5 * (1. + af::erf(C / std::sqrt(2)));
    };

    if (first)
    {
        /*
            mini-batch = 1
            input channels = 512
            output channels = 512
            input width = 3000
            output width 1500
            kernel width 3
            stride width = 2
            dilation width = 0
            padding width = 1
            in = 1x512x3000x1 readas 512x3000x1x1 -> 3000x1x512x1
            kern=512x512x3x1 readas 512x3x512x1 -> 3x1x512x512
            ref=1x512x1500x1 readas 512x1500x1x1-> 1500x1x512x1
            in = af::reorder(in, 1, 2, 0);
        */

        // Get test values
        image = af::reorder(read_array(assets/"conv_src.txt"), 1, 2, 0);

        // Arrayfire requires flips the values of width on convolution computation
        kernel = af::flip(af::reorder(read_array(assets/"conv_weight.txt"), 1, 3, 0, 2), 0);

        auto ref = af::reorder(read_array(assets/"conv_dst.txt"), 1, 2, 0);
        auto C = convolve(image, kernel);

        std::cout << "is conv op correct? = " << std::boolalpha
            << approx_equal(C, ref) << std::endl;

        first = false;
    }

    // First compile
    auto temp = convolve(image, kernel);
    temp.eval();
    af::sync();
    benchmark::DoNotOptimize(temp);

    // Benchmark
    af::array C;
    for (auto _ : state)
    {
        C = convolve(image, kernel);
        af::eval(C);
        af::sync();
        benchmark::DoNotOptimize(C);
    }
}

BENCHMARK(BM_af_convolve)->Unit(benchmark::kMillisecond)
                       ->Iterations(iterations)
                       ->Repetitions(repetitions)
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

void BM_af_softmax(benchmark::State& state)
{
    static bool first = true;
    static af::array A;

    auto softmax = [](const af::array& in, int dim = -1)
    {
        af::array result = in;

        result -= af::max(in, dim);
        result = af::exp(result);
        result /= af::sum(result, dim);

        return result;
    };

    if (first)
    {
        // Get test values
        A = read_array(assets/"softmax_src.txt");

        auto ref = read_array(assets/"softmax_dst.txt");
        auto C = softmax(A, 1);

        std::cout << "is softmax op correct? = " << std::boolalpha
            << approx_equal(C, ref, 1e-5, 1e-5) << std::endl;

        first = false;
    }

    // First compile
    auto temp = softmax(A);
    temp.eval();
    af::sync();
    benchmark::DoNotOptimize(temp);

    // Benchmark
    af::array C;
    for (auto _ : state)
    {
        C = softmax(A);
        af::eval(C);
        af::sync();
        benchmark::DoNotOptimize(C);
    }
}

BENCHMARK(BM_af_softmax)->Unit(benchmark::kMillisecond)
                       ->Iterations(iterations)
                       ->Repetitions(repetitions)
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


int main(int argc, char** argv)
{
    int device = 0;
    af::Backend backend = af::Backend::AF_BACKEND_DEFAULT;
    if (argc > 1)
    {
        std::string backend_str = argv[1];
        std::transform(backend_str.begin(), backend_str.end(), backend_str.begin(), [](unsigned char c){
            return std::tolower(c);
        });

        if (backend_str == "cpu")
            backend = af::Backend::AF_BACKEND_CPU;
        else if (backend_str == "opencl" || backend_str == "cl")
            backend = af::Backend::AF_BACKEND_OPENCL;
        else if (backend_str == "cuda")
            backend = af::Backend::AF_BACKEND_CUDA;
        else if (backend_str == "oneapi" || backend_str == "one")
            backend = af::Backend::AF_BACKEND_ONEAPI;
        else
            std::cout << "No such Backend, using default\n";
        argc--;
        argv++;
    }
    af::setBackend(backend);

    if (argc > 1)
    {
        device = std::stoi(argv[1]);
        if (device >= af::devicecount())
        {
            std::cout << "No such Device, using default\n";
            device = 0;
        }
        argc--;
        argv++;
    }
    af::setDevice(device);
    af::info();

    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    ::benchmark::RunSpecifiedBenchmarks();
}                          