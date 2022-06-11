#include <type_traits>
#include "/home/ashitabh/Documents/arduino-1.8.19/libraries/AutoDiff/fixed.hpp"
#include <iostream>

template< typename T>
class TypeWrapper
{
    public:
        TypeWrapper(){}
        typedef T T_type;
};


template <typename A>
void exp_inner(A input)
{
    exp_mem(input::T_type{99});
}

template <class T_c,
         typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr>
  void exp_mem(T_c x)
  {
    std::cout << "Running std..." << std::endl;
    // return std::exp(x);
  }

  template <class T_c,
         typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr>
  void exp_mem(T_c x)
  {
    std::cout << "Running fixed..." << std::endl;
    // return fpm::exp(x);
  }



int main()
{
    TypeWrapper<float> t;
    exp_inner(t);
}