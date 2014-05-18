#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "datatypes/ClMatrixCOO.h"
#include "io/MatrixMarket.h"
#include "matrix_operations/Transpose.h"


using namespace computecl;

const std::string input_matrix_file = "/media/Storage/matrices/mm/orani678.mtx";

const std::string vendor = "NVIDIA Corporation";

BOOST_AUTO_TEST_CASE(MatrixTransposition)
{

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

    computecl::ClDeviceMatrixCOO<float> cl_matrix(queue);
    computecl::ClDeviceMatrixCOO<float> cl_Tmatrix(queue);

    computecl::readMatrixMarket(cl_matrix, input_matrix_file, queue);

    computecl::Transpose(cl_matrix, cl_Tmatrix, queue);

    computecl::writeMatrixMarket(cl_Tmatrix, "Transposed.mtx", queue);

}
