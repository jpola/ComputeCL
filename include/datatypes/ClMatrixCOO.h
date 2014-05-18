#ifndef _CL_MATRIX_COO_H
#define _CL_MATRIX_COO_H

#include <CL/cl.hpp>
#include <boost/compute/container/vector.hpp>
#include <vector>

#include "ClMatrixBase.h"

namespace computecl
{

using namespace boost;

template <typename TYPE, StorageType STORAGE>
class ClMatrixCOO;

template <typename TYPE>
class ClMatrixCOO<TYPE, STORAGE_HOST>
        : public ClMatrixBase<TYPE, STORAGE_HOST>
{

    using BASE = ClMatrixBase<TYPE, STORAGE_HOST>;

    typedef std::vector<typename BASE::index_type> IdxVector;
    typedef std::vector<typename BASE::value_type> ValVector;

public:

    ClMatrixCOO() {};

    ClMatrixCOO (const ClMatrixCOO<TYPE, STORAGE_DEVICE>& matrix, compute::command_queue& queue)
    {

        //resize if necessary
        if (this->numEntries() != matrix.numEntries())
            resize(matrix.numRows(), matrix.numCols(), matrix.numEntries());

        compute::future<void> row_future = compute::copy_async
                (matrix.RowIndices().begin(), matrix.RowIndices().end(),
                 row_indices.begin(), queue);

        compute::future<void> col_future = compute::copy_async
                (matrix.ColIndices().begin(), matrix.ColIndices().end(),
                 col_indices.begin(), queue);

       compute::future<void> val_future = compute::copy_async
               (matrix.Values().begin(), matrix.Values().end(),
                values.begin(), queue);

        row_future.wait();
        col_future.wait();
        val_future.wait();
    }

    ClMatrixCOO (ClMatrixCOO<TYPE, STORAGE_DEVICE>&& matrix, compute::command_queue& queue)
    {

        //resize if necessary
        if (this->numEntries() != matrix.numEntries())
            resize(matrix.numRows(), matrix.numCols(), matrix.numEntries());

        compute::future<void> row_future = compute::copy_async
                (matrix.RowIndices().begin(), matrix.RowIndices().end(),
                 row_indices.begin(), queue);

        compute::future<void> col_future = compute::copy_async
                (matrix.ColIndices().begin(), matrix.ColIndices().end(),
                 col_indices.begin(), queue);

       compute::future<void> val_future = compute::copy_async
               (matrix.Values().begin(), matrix.Values().end(),
                values.begin(), queue);

        row_future.wait();
        col_future.wait();
        val_future.wait();
    }



    void resize(size_t num_rows, size_t num_cols, size_t num_entries)
    {
        this->numRows(num_rows);
        this->numCols(num_cols);
        this->numEntries(num_entries);

        row_indices.resize(num_entries);
        col_indices.resize(num_entries);
        values.resize(num_entries);

    }

    const IdxVector& RowIndices() const { return row_indices; }

    IdxVector& RowIndices() { return row_indices; }

    const IdxVector& ColIndices() const { return col_indices; }

    IdxVector& ColIndices() { return col_indices; }

    const ValVector& Values() const { return values; }

    ValVector& Values() { return values; }

private:

    IdxVector row_indices;
    IdxVector col_indices;
    ValVector values;

};

template<typename TYPE>
class ClMatrixCOO<TYPE, STORAGE_DEVICE>
        : public ClMatrixBase<TYPE, STORAGE_DEVICE>
{

    using BASE = ClMatrixBase<TYPE, STORAGE_DEVICE>;

    typedef compute::vector<typename BASE::index_type> IdxVector;
    typedef compute::vector<typename BASE::value_type> ValVector;

public:

    ClMatrixCOO(compute::command_queue& queue)
        : row_indices(queue.get_context()),
          col_indices(queue.get_context()),
          values(queue.get_context())
    {
//        std::cout << "row_idx ctx: " << row_indices.get_allocator().get_context().get() << std::endl;
    }

    ClMatrixCOO (const ClMatrixCOO<TYPE, STORAGE_HOST>& matrix, compute::command_queue& queue)
    {

        //resize if necessary
        if (this->numEntries() != matrix.numEntries())
            resize(matrix.numRows(), matrix.numCols(), matrix.numEntries(), queue);

        compute::future<void> row_future = compute::copy_async
                (matrix.RowIndices().begin(), matrix.RowIndices().end(),
                 row_indices.begin(), queue);
        compute::future<void> col_future = compute::copy_async
                (matrix.ColIndices().begin(), matrix.ColIndices().end(),
                 col_indices.begin(), queue);
        compute::future<void> val_future = compute::copy_async
               (matrix.Values().begin(), matrix.Values().end(),
                values.begin(), queue);

        row_future.wait();
        col_future.wait();
        val_future.wait();
    }

    ClMatrixCOO (ClMatrixCOO<TYPE, STORAGE_HOST>&& matrix, compute::command_queue& queue)
        : row_indices(queue.get_context()),
          col_indices(queue.get_context()),
          values(queue.get_context())
    {
        //resize if necessary
        if (this->numEntries() != matrix.numEntries())
            resize(matrix.numRows(), matrix.numCols(), matrix.numEntries(), queue);

        compute::future<void> row_future = compute::copy_async
                (matrix.RowIndices().begin(), matrix.RowIndices().end(),
                 row_indices.begin(), queue);
        compute::future<void> col_future = compute::copy_async
                (matrix.ColIndices().begin(), matrix.ColIndices().end(),
                 col_indices.begin(), queue);
        compute::future<void> val_future = compute::copy_async
               (matrix.Values().begin(), matrix.Values().end(),
                values.begin(), queue);

        row_future.wait();
        col_future.wait();
        val_future.wait();
    }

    void resize(size_t num_rows, size_t num_cols, size_t num_entries, compute::command_queue& queue)
    {
        this->numRows(num_rows);
        this->numCols(num_cols);
        this->numEntries(num_entries);

        row_indices.resize(num_entries, queue);
        col_indices.resize(num_entries, queue);
        values.resize(num_entries, queue);

    }

    const IdxVector& RowIndices() const { return row_indices; }

    IdxVector& RowIndices() { return row_indices; }

    const IdxVector& ColIndices() const { return col_indices; }

    IdxVector& ColIndices() { return col_indices; }

    const ValVector& Values() const { return values; }

    ValVector& Values() { return values; }


private:

    IdxVector row_indices;
    IdxVector col_indices;
    ValVector values;
};


template<typename T> using ClHostMatrixCOO = ClMatrixCOO<T, STORAGE_HOST>;
template<typename T> using ClDeviceMatrixCOO = ClMatrixCOO<T, STORAGE_DEVICE>;

}


#endif //_CL_MATRIX_COO_H
