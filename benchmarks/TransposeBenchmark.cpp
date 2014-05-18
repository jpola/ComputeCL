#include <iostream>

#include "utils/cmd_options/CommandLineOptions.h"
#include "datatypes/ClMatrixCOO.h"
#include "io/MatrixMarket.h"
#include "matrix_operations/Transpose.h"
#include "utils/Timer.h"

#include <boost/compute/event.hpp>
#include <boost/compute/async/future.hpp>

using namespace computecl;

int main(int argc, const char* argv[])
{

    typedef double ValueType;
    typedef int    IndexType;

    std::string vendor;
    std::string matrix_file;

    if(!computecl::ParseCommandLine(argc, argv, vendor, matrix_file))
    {
        return 0;
    }

    std::vector<compute::device> devices = compute::system::devices();
    compute::device gpu;

    int index = 0;
    for (compute::device d : devices)
    {
        ///std::cout << "checking " << d.vendor() << std::endl;
        if (d.vendor() == vendor)
        {
            gpu = d;
            std::cout << "device[" << index << "]: "
                  << d.name() << " "
                  << d.vendor() << std::endl;
        }

        index++;
    }

    compute::context context(gpu);
    compute::command_queue queue(context, gpu, compute::command_queue::enable_profiling);

    /**
     * BENCHMARK BEGINS HERE
     */
    const int warmups = 50;
    const int benches = 100;

    ClDeviceMatrixCOO<ValueType> A(queue);
    readMatrixMarket(A, matrix_file, queue);

    std::cout << "Matrix - rows: " << A.numRows()
                 << " cols: " << A.numCols()
                 << " nnz: " << A.numEntries();

    //warmup
    {
        ClDeviceMatrixCOO<ValueType> At(queue);
        for (int i = 0; i < warmups; i++) Transpose(A, At, queue); //WARMUP
    }

    HTimer t;
    ClDeviceMatrixCOO<ValueType> At(queue);

    t.start();
        for (int i = 0; i < benches; i++) Transpose(A, At, queue);
    double avg_elapsed = t.stop() / benches;
    std::cout << " Transpose time = " << avg_elapsed << " ms" << std::endl;

    writeMatrixMarket(At, "transposed.mtx", queue);
}
