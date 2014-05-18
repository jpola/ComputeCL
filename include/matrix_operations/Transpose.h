#ifndef _TRANSPOSE_H_
#define _TRANSPOSE_H_

#include <boost/compute/command_queue.hpp>
#include <boost/compute/algorithm/iota.hpp>
#include <boost/compute/algorithm/sort_by_key.hpp>
#include <boost/compute/algorithm/gather.hpp>

using namespace boost::compute;

namespace computecl
{

namespace internal
{

template<typename INDEX_VECTOR, typename VALUE_VECTOR>
void transpose(const INDEX_VECTOR& I,
               const INDEX_VECTOR& J,
               const VALUE_VECTOR& V,
               INDEX_VECTOR& tI,
               INDEX_VECTOR& tJ,
               VALUE_VECTOR& tV,
               command_queue& queue)
{
    assert(I.size() == tJ.size());
    assert(J.size() == tI.size());
    assert(V.size() == tV.size());

    typedef typename INDEX_VECTOR::value_type index_type;
    typedef typename VALUE_VECTOR::value_type value_type;

    INDEX_VECTOR permutation(J.size(), queue.get_context());

    iota(permutation.begin(), permutation.end(), 0, queue);

    copy(J.begin(), J.end(), tI.begin(), queue);

    sort_by_key(tI.begin(), tI.end(), permutation.begin(), queue);

    gather(permutation.begin(), permutation.end(), I.begin(), tJ.begin(), queue);

    gather(permutation.begin(), permutation.end(), V.begin(), tV.begin(), queue);

}

}//internal

template<typename MATRIX1, typename MATRIX2>
inline void Transpose(const MATRIX1& M, MATRIX2& tM, command_queue& queue)
{
    if (tM.numCols() != M.numRows()
            || tM.numRows() != M.numCols()
            || tM.numEntries() != M.numEntries())
        tM.resize(M.numCols(), M.numRows(), M.numEntries(), queue);

    internal::transpose(M.RowIndices(),
                        M.ColIndices(),
                        M.Values(),
                        tM.RowIndices(),
                        tM.ColIndices(),
                        tM.Values(), queue);
}



} //computecl

#endif //_TRANSPOSE_H_
