#include <iostream>
#include <iterator>
#include <string>

#include <boost/compute/command_queue.hpp>
#include <boost/compute/system.hpp>

#include "utils/cmd_options/CommandLineOptions.h"

#include "datatypes/ClMatrixCOO.h"
#include "io/MatrixMarket.h"

namespace compute = boost::compute;


int main(int argc, char* argv[])
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
    std::cout << "INIT: " << context.get() <<std::endl;
    std::cout << "QUEUE: " << queue.get_context().get() <<std::endl;


    //computecl::ClHostMatrixCOO<float> matrix;
    //computecl::readMatrixMarket(matrix, matrix_file);

    computecl::ClDeviceMatrixCOO<float> cl_matrix(queue);
    //computecl::readMatrixMarket(cl_matrix, matrix_file, queue);




    return 0;
}

