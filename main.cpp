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

    std::vector<int> hi(5);
    std::vector<int> hj(5);

    hi[0] = 1; hi[1] = 6; hi[2] = 1; hi[3] = 7; hi[4] = 1;
    hj[0] = 1; hj[1] = 2; hj[2] = 2; hj[3] = 1; hj[4] = 2;

    compute::vector<int> i(5, context);
    compute::vector<int> j(5, context);

    compute::copy(hi.begin(), hi.end(), i.begin(), queue);
    compute::copy(hj.begin(), hj.end(), j.begin(), queue);

    auto it1 = make_sort_permute_iter(i.begin(), j.begin());


    //stable_sort()

//    compute::sort(make_sort_permute_iter(i.begin(), j.begin()),
//                  make_sort_permute_iter(i.end(), j.end()),
//                  sort_permute_iter_compare<compute::vector<int>::iterator,
//                  compute::vector<int>::iterator>(), queue);


    computecl::io::print_vector(i, queue, "i");
    computecl::io::print_vector(j, queue, "j");

//    BOOST_COMPUTE_FUNCTION(bool, compare_ij, (boost::tuple<int, int> indexes),
//    {
//        const int first = boost_tuple_get(indexes, 0);
//        const int second = boost_tuple_get(indexes, 1);

//        return second < first;
//    });



    computecl::ClHostMatrixCOO<float> matrix;
    computecl::readMatrixMarket(matrix, matrix_file);
    computecl::writeMatrixMarket(matrix, "test.mtx");

//    computecl::ClDeviceMatrixCOO<float> cl_matrix(queue);
//    computecl::readMatrixMarket(cl_matrix, matrix_file, queue);
//    computecl::writeMatrixMarket(cl_matrix, "test.mtx", queue);




    return 0;
}

