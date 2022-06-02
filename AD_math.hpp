#include "shuffleAD.hpp"


template<typename T>
Variable<T> &log10(Variable<T>& V_in)
{
    Variable<T> *out_v = new Variable<T>(0);
    Log10<T> *log_x = new Log10<T>(V_in);
    log_x->register_parent(out_v);
    out_v->setChild(log_x);
    return *out_v;
}

template<typename T>
Variable<T> &exp(Variable<T> &V_in)
{
    Variable<T> *out_v = new Variable<T>(0);
    Exp<T> *exp_out = new Exp<T>(V_in);
    exp_out->register_parent(out_v);
    out_v->setChild(exp_out);
    return *out_v;
}

typedef Variable<fpm::fixed_16_16> Var;
typedef Variable<float> Varf;
typedef Constant<fpm::fixed_16_16> const_fp;
typedef Constant<float> const_fl;

Constant<fpm::fixed_16_16> one_fp(1);
Constant<float> one_f(1);
Constant<fpm::fixed_16_16> zero_fp(0);
Constant<float> zero_f(0);



