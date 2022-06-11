#include "AD_math.hpp"
#include <iostream>

class Node_lite{
    public:
    Node_lite(){}
    bool isVar;
};

struct Parent
{
    struct Parent * nextParent;
    struct CurrParent;
};


template<typename T>
class Var_lite:public Node_lite{

    private:
        T value_;
        int id;
        // struct Parent * ParentList;
    public:
        using comp_type= T;

        Var_lite () {};
        Var_lite(T input)
        {
            this->value_ = input;
            this->isVar = true;
        }
        void setValue(T input)
        {
            this->value_ = input;
        }
        T value() const
        {
            return this->value_;
        }
};

template<typename L, typename R, typename T>
class add_lite: public Node_lite{
    private:

        L *Lhs;
        R *Rhs;
    public:
        add_lite(L &L_in,R &R_in)
        {
            Lhs = &L_in;
            Rhs = &R_in;
            std::cout << " Created object...\n";
        }

        T value()
        {
            return Lhs->value() + Rhs->value();
        }

};




// template<typename T>
template<typename K>
class op_lite{
    private:

        K cached_value;
        const op_lite<K> *LHS;
        const op_lite<K> *RHS;
        typedef K (*value_t) (const op_lite<K>* Lin, const op_lite<K>* Rin);
        
    public:
        const value_t value_in;
        op_lite(const op_lite<K>* LHS_in, const op_lite<K>* RHS_in, const value_t value_func):LHS(LHS_in), RHS(RHS_in), value_in(value_func){}

        op_lite(const op_lite<K>* LHS_in, const value_t value_func):LHS(LHS_in), value_in(value_func){} // Unary operations

        op_lite(K input_num, const value_t value_func): cached_value(input_num), value_in(value_func){}

        op_lite(){}

        K value()
        {
            return (*value_in)(LHS, RHS);
        }
};


// template<typename L, typename R, typename T>
// op_lite<L, R, T> op_wrapper(const L& lhs, const R& rhs, T(*value_ptr)(const L& lhs, const R& rhs))
// {
//     return op_lite<L, R, T>(lhs, rhs, value_ptr);
// }


// add_lite<float>, add_lite<float>

template<template <typename K> typename L, template <typename K> typename R, typename K>
add_lite<L<K>,R<K>,K> add_wrapper(L<K> &L_in, R<K> &R_in)
{
    // using T_curr = typename L::comp_type;
    return add_lite<L<K>,R<K>,K>(L_in, R_in);
}
// template<typename T>
// class Op_lite: public Node_lite{
//     private:
//         T output_value;
//         Var_lite *input[2];
//         Var_lite *output_var;
//         bool isComputed;
//     public:
//         Op_lite(){this->isVar = false;}

//         Op_lite(Var_lite &a, Var_lite &b)
//         {
//             input[0] = &a;
//             input[1] = &b;
//         }
// };


// template<

