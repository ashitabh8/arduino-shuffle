#include "fixed.hpp"
#include "ios.hpp"
#include "math.hpp"
#include "shuffleAD.hpp"
#include <cstdlib>
#include <vector>
#include <ctime>
#include <random>


// x = x - f(x)/f'(x)
template <typename T>
T WeightUpdate(Variable<T> &expr,Variable<T> &var)
{
  T x_1 = var.getValue() - expr.getValue()/expr.diff(var);
  return x_1;
}

// // Generate (x - a)*(x + a)*(x - 1)
// template<typename T>
// Variable<T> * getPoly1()
// {

// }