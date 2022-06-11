#include "/home/ashitabh/Documents/arduino-1.8.19/libraries/AutoDiff/fixed.hpp"
#include "/home/ashitabh/Documents/arduino-1.8.19/libraries/AutoDiff/math.hpp"
#include "/home/ashitabh/Documents/arduino-1.8.19/libraries/AutoDiff/ios.hpp"

#include <iostream>

// template<typename T>
// struct TypeWrapper{
//     public:
//         TypeWrapper() {}
//         using type = T;
// };


// class BaseExpression{
//     public:
//     int  _dummy;
// };


// working version
template< template <typename T> typename A, typename T>
class Expression {
    const A<T>& cast() const {return static_cast<const A<T>&>(*this);}
    T value() const {return cast().value(); }
};


// template< typename A>
// class Expression {
//     const A& cast() const {return static_cast<const A&>(*this);}
//     // T value() const {return cast().value(); }
// };

// template< typename A>
// class Expression {

//     using T2 = typename A::T_type;
//     const A& cast() const {return static_cast<const A&>(*this);}
//     T2 value() const {return cast().value(); }
// };

template<class T>
class Var_lite: public Expression<Var_lite, T> {
    private:
        T val_;
    public:
        T value() const {return val_;}
        Var_lite () {}
        
        Var_lite(T input)
        {
            this->val_ = input;
        }
        using T_type = T;
        void operator = (T const input)
        {
            this->val_ = input;
        }
};


// exp(a); - Var_lite<float>
// template <class A, class T>
// struct Exp_lite: public Expression<Exp_lite<A>, T>{
//     public:
//         Exp_lite(const Expression<A,T> &in_v) : input(in_v.cast()) {}
//         // T value() const {return exp_mem(this->input.value();)}
    
//         void value() const {exp_mem(this->input.value();)}
//     private:
//         const A& input;
//         template <class T_c = T,
//          typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr>
//         void exp_mem(T_c x)
//         {
//             std::cout << "Running std..." << std::endl;
//             // return std::exp(x);
//         }

//         template <class T_c = T,
//                 typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr>
//         void exp_mem(T_c x)
//         {
//             std::cout << "Running fixed..." << std::endl;
//             // return fpm::exp(x);
//         }

// };


// template<template <typename K> typename A, typename K>
// void exp_trial(const Expression<)