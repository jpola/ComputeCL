#include <iostream>
#include <iterator>
#include <string>

#include <boost/compute/command_queue.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/algorithm/sort.hpp>

#include "iterator/SortIterator.h"
#include "io/Print.h"
#include "utils/cmd_options/CommandLineOptions.h"
#include "datatypes/ClMatrixCOO.h"
#include "io/MatrixMarket.h"
#include "matrix_operations/Transpose.h"
namespace compute = boost::compute;


int main(int argc, const char* argv[])
{


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
    compute::command_queue queue(context, gpu);


    computecl::ClHostMatrixCOO<float> matrix;
    computecl::readMatrixMarket(matrix, matrix_file);
    computecl::writeMatrixMarket(matrix, "test.mtx");

    computecl::ClDeviceMatrixCOO<float> cl_matrix(queue);
    computecl::readMatrixMarket(cl_matrix, matrix_file, queue);
    computecl::writeMatrixMarket(cl_matrix, "test2.mtx", queue);




    return 0;
}

