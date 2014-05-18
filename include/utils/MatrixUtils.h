#ifndef _MATRIX_UTILS_H_
#define _MATRIX_UTILS_H_

#include <boost/compute/command_queue.hpp>
#include <boost/compute/algorithm/iota.hpp>
#include <boost/compute/algorithm/gather.hpp>

//cpu algorithms
#include "primitves/Gather.h"
#include "io/Print.h"

using namespace boost::compute; // = boost::compute;

namespace computecl {

namespace internal {

} //internal

template <typename MATRIX>
void SortByRowCol(MATRIX& matrix)
{
    typedef typename MATRIX::index_type IndexType;
    typedef typename MATRIX::value_type ValueType;

    size_t N = matrix.RowIndices().size();

    std::vector<IndexType> permutation(N);
    std::iota(permutation.begin(), permutation.end(), 0);

    {
        std::cerr << "sort on cpu" <<std::endl;

        //TODO: why auto here does not work?
        std::vector<IndexType>& cols = matrix.ColIndices();
        std::vector<IndexType>& rows = matrix.RowIndices();

        io::print_vector(rows, "rows");
        io::print_vector(cols, "cols");
        std::vector<IndexType> temp(cols);


        std::sort(make_sort_permute_iter(temp.begin(), permutation.begin()),
                  make_sort_permute_iter(temp.end(), permutation.end()),
                  sort_permute_iter_compare<
                  std::vector<int>::iterator,
                  std::vector<int>::iterator>()
                  );

        //sort_by_key(temp.begin(), temp.end(), permutation.begin(), queue);
        std::copy(rows.begin(), rows.end(), temp.begin());
        primitives::gather(permutation.begin(), permutation.end(),
                           temp.begin(),
                           rows.begin());


//        sort_by_key(rows.begin(), rows.end(), permutation.begin(), queue);
         std::sort(make_sort_permute_iter(rows.begin(), permutation.begin()),
                   make_sort_permute_iter(rows.end(), permutation.end()),
                   sort_permute_iter_compare<
                   std::vector<int>::iterator,
                   std::vector<int>::iterator>()
                   );

        std::copy(cols.begin(), cols.end(), temp.begin());
        primitives::gather(permutation.begin(), permutation.end(),
                           temp.begin(),
                           cols.begin());

        io::print_vector(rows, "sorted rows");
        io::print_vector(cols, "sorted cols");

        {
            std::vector<ValueType>& values = matrix.Values();
            std::vector<ValueType> temp(values);
            primitives::gather(permutation.begin(), permutation.end(),
                               temp.begin(),
                               values.begin());
        }

    }
}

template <typename MATRIX>
void SortByRowCol(MATRIX& matrix, command_queue& queue)
{
    typedef typename MATRIX::index_type IndexType;
    typedef typename MATRIX::value_type ValueType;

    size_t N = matrix.RowIndices().size();

    compute::vector<IndexType> permutation(N, queue.get_context());
    compute::iota(permutation.begin(), permutation.end(), 0, queue);

    {
        std::cerr << "Here should be stable sort by key" <<std::endl;

        auto cols = matrix.ColIndices();
        auto rows = matrix.RowIndices();

        compute::vector<IndexType> temp(cols);

        //somehow by zip_iterator
        sort_by_key(temp.begin(), temp.end(), permutation.begin(), queue);

        copy(rows.begin(), rows.end(), temp.begin(), queue);
        gather(permutation.begin(), permutation.end(), temp.begin(), rows.begin(), queue);
        sort_by_key(rows.begin(), rows.end(), permutation.begin(), queue);

        copy(cols.begin(), cols.end(), temp.begin(), queue);
        gather(permutation.begin(), permutation.end(), temp.begin(), cols.begin(), queue);

        {
            auto values = matrix.Values();
            compute::vector<ValueType> temp(values);
            gather(permutation.begin(), permutation.end(), temp.begin(), values.begin(), queue);
        }

    }




}
//CUSP_PROFILE_SCOPED();

//typedef typename Array1::value_type IndexType;
//typedef typename Array3::value_type ValueType;
//typedef typename Array1::memory_space MemorySpace;

//size_t N = rows.size();

//cusp::array1d<IndexType,MemorySpace> permutation(N);
//thrust::sequence(permutation.begin(), permutation.end());

//// compute permutation and sort by (I,J)
//{
//    cusp::array1d<IndexType,MemorySpace> temp(columns);
//    thrust::stable_sort_by_key(temp.begin(), temp.end(), permutation.begin());

//    cusp::copy(rows, temp);
//    thrust::gather(permutation.begin(), permutation.end(), temp.begin(), rows.begin());
//    thrust::stable_sort_by_key(rows.begin(), rows.end(), permutation.begin());

//    cusp::copy(columns, temp);
//    thrust::gather(permutation.begin(), permutation.end(), temp.begin(), columns.begin());
//}

//// use permutation to reorder the values
//{
//    cusp::array1d<ValueType,MemorySpace> temp(values);
//    thrust::gather(permutation.begin(), permutation.end(), temp.begin(), values.begin());
//}
} //computecl

#endif //_MATRIX_UTILS_H
