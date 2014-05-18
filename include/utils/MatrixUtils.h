#ifndef _MATRIX_UTILS_H_
#define _MATRIX_UTILS_H_

#include <boost/compute/command_queue.hpp>
#include <boost/compute/algorithm/iota.hpp>
#include <boost/compute/algorithm/gather.hpp>
#include <boost/compute/algorithm/sort_by_key.hpp>

#include "primitves/Gather.h" //gather on cpu;
#include "io/Print.h"
#include "iterator/SortIterator.h"

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
        primitives::Gather(permutation.begin(), permutation.end(),
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
        primitives::Gather(permutation.begin(), permutation.end(),
                           temp.begin(),
                           cols.begin());

        io::print_vector(rows, "sorted rows");
        io::print_vector(cols, "sorted cols");

        {
            std::vector<ValueType>& values = matrix.Values();
            std::vector<ValueType> temp(values);
            primitives::Gather(permutation.begin(), permutation.end(),
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
        std::cerr << "sort on gpu (shoud be stable sort)" <<std::endl;

        compute::vector<IndexType>& cols = matrix.ColIndices();
        compute::vector<IndexType>& rows = matrix.RowIndices();

        compute::vector<IndexType> temp(cols);

        //somehow by zip_iterator
        sort_by_key(temp.begin(), temp.end(), permutation.begin(), queue);

        copy(rows.begin(), rows.end(), temp.begin(), queue);
        gather(permutation.begin(), permutation.end(), temp.begin(), rows.begin(), queue);
        sort_by_key(rows.begin(), rows.end(), permutation.begin(), queue);

        copy(cols.begin(), cols.end(), temp.begin(), queue);
        gather(permutation.begin(), permutation.end(), temp.begin(), cols.begin(), queue);

        {
            compute::vector<ValueType>&  values = matrix.Values();
            compute::vector<ValueType> temp(values);
            gather(permutation.begin(), permutation.end(), temp.begin(), values.begin(), queue);
        }

    }




}

} //computecl

#endif //_MATRIX_UTILS_H
