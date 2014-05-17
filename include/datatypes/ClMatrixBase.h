#ifndef _CL_MATRIX_BASE_H
#define _CL_MATRIX_BASE_H

#include "utils/StorageType.h"

namespace computecl
{

template <typename TYPE, StorageType STORAGE>
class ClMatrixBase
{
public:

    typedef int  index_type;
    typedef TYPE value_type;

    ClMatrixBase() : num_entries(0), num_rows(0), num_cols(0) {}

    inline index_type numRows() const { return num_rows; }

    void numRows(index_type num_rows) { this->num_rows = num_rows; }

    inline index_type numCols() const { return num_cols; }

    void numCols (index_type num_cols) { this->num_cols = num_cols; }

    inline index_type numEntries() const { return num_entries; }

    void numEntries(index_type num_entires) { this->num_entries = num_entires; }

private:

    index_type num_entries;
    index_type num_rows;
    index_type num_cols;
};

}//namespace computecl

#endif //_CL_MATRIX_BASE_H
