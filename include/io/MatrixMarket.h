#ifndef MATRIX_MARKET_H
#define MATRIX_MARKET_H

#include "datatypes/ClMatrixCOO.h"


#include <string.h>
#include <vector>
#include <fstream>
#include <algorithm>

/**
  * Functions to read matrix in matrix market (*.mtx) format
  */
namespace computecl
{

namespace internal
{
    inline
    void tokenize(std::vector<std::string>& tokens,
                  const std::string& str,
                  const std::string& delimiters = "\n\r\t ")
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

    struct matrix_market_banner
{
    std::string storage;    // "array" or "coordinate"
    std::string symmetry;   // "general", "symmetric", "hermitian", or "skew-symmetric"
    std::string type;       // "complex", "real", "integer", or "pattern"
};

    template <typename Stream>
    void read_matrix_market_banner(matrix_market_banner& banner, Stream& input)
{
  std::string line;
  std::vector<std::string> tokens;

  // read first line
  std::getline(input, line);
  tokenize(tokens, line);

  if (tokens.size() != 5 || tokens[0] != "%%MatrixMarket" || tokens[1] != "matrix")
  {
      std::cerr << "invalid MatrixMarket banner" << std::endl;
      return;
  }

  banner.storage  = tokens[2];
  banner.type     = tokens[3];
  banner.symmetry = tokens[4];

  if (banner.storage != "array" && banner.storage != "coordinate")
  {
      std::cerr << "invalid MatrixMarket storage format [" + banner.storage + "]";
      return;
  }

  if (banner.type != "complex" && banner.type != "real"
      && banner.type != "integer" && banner.type != "pattern")
  {
      std::cerr << "invalid MatrixMarket data type [" + banner.type + "]";
      return;
  }

  if (banner.symmetry != "general" && banner.symmetry != "symmetric"
      && banner.symmetry != "hermitian" && banner.symmetry != "skew-symmetric")
  {
      std::cerr << "invalid MatrixMarket symmetry [" + banner.symmetry + "]";
      return;
  }
}


    template <typename TYPE>
    void read (ClHostMatrixCOO<TYPE>& matrix, const std::string& filename)
    {
        typedef int     IndexType;
        typedef TYPE    ValueType;

        std::ifstream file(filename.c_str());
        if (!file)
        {
            std::cerr << "Problem with opening file ["<< filename <<"]"<< std::endl;
            return;
        }

        matrix_market_banner banner;
        read_matrix_market_banner(banner, file);
    //    std::cerr << "banner: "
    //              << banner.storage << " " << banner.type << " " << banner.symmetry
    //              << std::endl;

        if (banner.storage == "coordinate")
        {
            //read coordinate
            std::string line;

            //skip over banner and comments;
            do
            {
                std::getline(file, line);
            } while (line[0] == '%');

            //this line should contain size of the matrix;
            std::vector<std::string> tokens;
            tokenize(tokens, line);

            if (tokens.size() != 3)
            {
                std::cerr << "Invalid MatrixMarket coordinate format" <<std::endl;
                return;
            }

            IndexType num_rows, num_cols, num_entries;

            std::istringstream(tokens[0]) >> num_rows;
            std::istringstream(tokens[1]) >> num_cols;
            std::istringstream(tokens[2]) >> num_entries;

            std::vector<IndexType> rows (num_entries);
            std::vector<IndexType> cols (num_entries);
            std::vector<ValueType> vals (num_entries);

            IndexType num_entries_read = 0;

            if(banner.type == "pattern")
            {
                while (num_entries_read < num_entries && !file.eof())
                {
                    file >> rows[num_entries_read];
                    file >> cols[num_entries_read];
                    num_entries_read++;
                }

                std::fill(vals.begin(), vals.end(), ValueType(1));

            }
            else if (banner.type == "real" || banner.type == "integer")
            {
                while (num_entries_read < num_entries && !file.eof())
                {
                    double real;

                    file >> rows[num_entries_read];
                    file >> cols[num_entries_read];
                    file >> real;
                    vals[num_entries_read] = (ValueType)real; //easy conversion

                    num_entries_read++;
                }
            }
            else if (banner.type == "complex")
            {
                std::cerr
                        << "Complex Matrix Market format is not supported"
                        << std::endl;
                return;
            }
            else
            {
                std::cerr << "Invalid Matrix Market data type" << std::endl;

                return;
            }

            if (num_entries_read != num_entries)
            {
                std::cerr << "Unexpected EOF while reading MatrixMarket entries "
                          << "from file [" << file << "]" << std::endl;

                return;
            }

            //check validity
            if (num_entries > 0)
            {
                //JPA: redundand?! IndexType is unsigned int
                int min_row_index = *std::min_element(&rows[0],
                                                         &rows[num_entries-1]);
                int min_col_index = *std::min_element(&cols[0],
                                                         &cols[num_entries-1]);
                int max_row_index = *std::max_element(&rows[0],
                                                         &rows[num_entries-1]);
                int max_col_index = *std::max_element(&cols[0],
                                                         &cols[num_entries-1]);

                if (min_row_index < 1)
                {
                    std::cerr << "found invalid row index < 1"<< std::endl;
                    return;
                }
                if (min_col_index < 1)
                {
                    std::cerr << "found invalid col index < 1"<< std::endl;

                    return;
                }
                if (max_row_index > num_rows)
                {
                    std::cerr << "found invalid row_index > num_rows" << std::endl;
                    return;
                }
                if (max_col_index > num_cols)
                {
                    std::cerr << "found invalid col_index > num_rows" << std::endl;

                    return;
                }

                //convert to 0 base indices;
                for (IndexType i = 0; i < num_entries; i++)
                {
                    rows[i] -= 1;
                    cols[i] -= 1;
                }

                //expand symmetric formats to general format
                if (banner.symmetry != "general")
                {
                    IndexType off_diagonals = 0;

                    for (IndexType i = 0; i < num_entries; i++)
                        if (rows[i] != cols[i])
                            off_diagonals++;

                    //if matrix is not symmetrix the off_diagonals will be 0;
                    IndexType general_num_entries = num_entries + off_diagonals;


                    std::vector<IndexType> general_rows(general_num_entries);
                    std::vector<IndexType> general_cols(general_num_entries);
                    std::vector<ValueType> general_vals(general_num_entries);

                    if (banner.symmetry == "symmetric")
                    {
                        IndexType nnz = 0;

                        for (IndexType i = 0; i < num_entries; i++)
                        {
                            general_rows[nnz] = rows[i];
                            general_cols[nnz] = cols[i];
                            general_vals[nnz] = vals[i];
                            nnz++;

                            if (rows[i] != cols[i])
                            {
                                general_rows[nnz] = cols[i];
                                general_cols[nnz] = rows[i];
                                general_vals[nnz] = vals[i];
                                nnz++;
                            }
                        }
                    }
                    else if (banner.symmetry == "hermitian")
                    {
                        std::cerr << "Hermitian matrices are not supported" << std::endl;

                        return;
                    }
                    else if (banner.symmetry == "skew-symmetric")
                    {
                        std::cerr << "Skew-symmetric matrices are not supported" << std::endl;

                        return;
                    }

                    matrix.resize(num_rows, num_cols, general_num_entries);


                    std::copy(general_rows.begin(), general_rows.end(),
                                  matrix.RowIndices().begin());

                    std::copy(general_cols.begin(), general_cols.end(),
                              matrix.ColIndices().begin());

                    std::copy(general_vals.begin(), general_vals.end(),
                              matrix.Values().begin());

                }
                else
                {

                    matrix.resize(num_rows, num_cols, num_entries);

                    std::copy(rows.begin(), rows.end(),
                                  matrix.RowIndices().begin());

                    std::copy(cols.begin(), cols.end(),
                              matrix.ColIndices().begin());

                    std::copy(vals.begin(), vals.end(),
                              matrix.Values().begin());

                }

            }

        }
        else
        {
            std::cerr <<
            "Matrix storage type [array] is not supported." << std::endl;
            return;
        }

    //    sortByRowCol(queue, matrix);
    }


    template <typename TYPE>
    void write(const ClHostMatrixCOO<TYPE>& matrix,
               const std::string& filename)
    {
        typedef unsigned int IndexType;
        typedef TYPE         ValueType;

        std::ofstream file(filename.c_str());
        if (!file)
        {
            std::cerr << "Problem with opening file ["
                      << filename <<"]"
                      << std::endl;
            return;
        }

        file << "%%MatrixMarket matrix coordinate real general\n";
        file << matrix.numRows() << "\t"
             << matrix.numCols() << "\t"
             << matrix.numEntries() << "\n";


        for (unsigned int i = 0; i < matrix.numEntries(); i++)
        {
            file << (matrix.RowIndices()[i] + 1) << " ";
            file << (matrix.ColIndices()[i] + 1) << " ";
            file << (matrix.Values()[i]) << "\n";
        }
    }
} //internal

/**
 * Read matrix on host from matrix market file
 */
template <typename TYPE>
void readMatrixMarket (ClHostMatrixCOO<TYPE>& matrix,
                       const std::string& filename)
{
    internal::read (matrix, filename);
}

/**
 * Read matrix on on device from matrix market file
 */
template <typename TYPE>
void readMatrixMarket (ClDeviceMatrixCOO<TYPE>& matrix,
                       const std::string& filename,
                       compute::command_queue& queue)
{
    ClHostMatrixCOO<TYPE> host_matrix;

    internal::read(host_matrix, filename);

    matrix = ClDeviceMatrixCOO<TYPE>(std::move(host_matrix), queue);
}

/**
  * Write matrix to MTX file format
  */

template <typename TYPE>
void writeMatrixMarket (const ClHostMatrixCOO<TYPE>& matrix,
                        const std::string& filename)
{
    internal::write(matrix, filename);
}

template <typename TYPE>
void writeMatrixMarket (const ClDeviceMatrixCOO<TYPE>& matrix,
                        const std::string& filename,
                        compute::command_queue& queue)
{

    ClHostMatrixCOO<TYPE> host_matrix(matrix, queue);

    internal::write(host_matrix, filename);

}

} // namespace computecl
#endif //MATRIX_MARKET_H
