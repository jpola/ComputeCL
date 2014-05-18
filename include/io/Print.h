#ifndef _PRINT_H_
#define _PRINT_H_

#include <boost/compute/container/vector.hpp>
#include <boost/compute/command_queue.hpp>

//using namespace boost;

namespace computecl
{
namespace io
{

template <typename TYPE>
void print_vector(std::vector<TYPE>& vector,
                  std::string title = "vec", const int begin = 0, const int end = 5)
{
    std::cout << "vector " << title << " : [ ";
        boost::copy(vector,
            std::ostream_iterator<TYPE>(std::cout, ", ")
        );
        std::cout << "]" << std::endl;

}

template <typename TYPE>
void print_vector(boost::compute::vector<TYPE>& vector,
                  boost::compute::command_queue& queue,
                  std::string title = "vec", const int begin = 0, const int end = 5)
{
    std::cout << "vector " << title << " : [ ";
        boost::compute::copy(
            vector.begin(), vector.end(),
            std::ostream_iterator<TYPE>(std::cout, ", "),
            queue
        );
        std::cout << "]" << std::endl;


}

}//io

}//computecl


#endif //_PRINT_H_
