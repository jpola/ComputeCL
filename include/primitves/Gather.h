#ifndef _GATHER_H_
#define _GATHER_H_

#include <iterator>

namespace computecl
{

namespace primitives
{
template< typename InputIterator1,
          typename InputIterator2,
          typename OutputIterator >
void gather(InputIterator1 mapfirst,
            InputIterator1 maplast,
            InputIterator2 input,
            OutputIterator result)
{
    std::cout<<"Serial code path ... \n";
    size_t numElements = static_cast< size_t >( std::distance( mapfirst, maplast ) );
    typedef typename  std::iterator_traits<InputIterator1>::value_type iType1;
    iType1 temp;
    for(size_t iter = 0; iter < numElements; iter++)
    {
        temp = *(mapfirst + (int)iter);
        *(result + (int)iter) = *(input + (int)temp);
    }
}

} //primitives
} //computecl
#endif
