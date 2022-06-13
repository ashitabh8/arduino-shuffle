#include <cmath>
// #include <iostream>
#include <type_traits>
#include "fixed.hpp"
#include "math.hpp"


static constexpr int counter = 0;

 int var_id_counter = 0;

template <typename M, typename T>
class Node2{
  public:
    using type_ = T;
    constexpr const M& cast() const {return static_cast<const M&>(*this);}
    constexpr type_ value() const {return cast().value();}
};

template <typename T>
struct Variable : public Node2<Variable<T>, T>{

  public:
    using type_ = T;
    mutable type_ value_;
    int Var_ID;

    constexpr Variable ():value_(0), Var_ID(var_id_counter++){}
    constexpr Variable(type_ val_):value_(val_), Var_ID(var_id_counter++){}

    constexpr type_ value() const
    {
      // std::cout <<" value in variable : " << value_ << "\n";
      return value_;
    }

    void set_value(type_ in_)
    {
        value_ = in_;
    }

    constexpr inline type_ diff(int wrt_v) const
    {
      if(wrt_v == Var_ID)
      {
        return type_{1};
      }
      else
      {
        return type_{0};
      }
    }
};

template <typename T>
struct Constant : public Node2<Constant<T>, T>{

  public:
    using type_ = T;
    mutable type_ value_;
    

    constexpr Constant ():value_(0){}
    constexpr Constant(type_ val_): value_(val_) {}

    constexpr inline type_ value() const
    {
      // std::cout <<" value in variable : " << value_ << "\n";
      return value_;
    }

    constexpr inline type_ diff(int wrt_v) const
    {
      // std::cout <<" value in variable : " << value_ << "\n";
      return type_{0};
    }
};

template<typename L, typename R, typename output>
struct ADD : public Node2<ADD<L,R,output>, output>{

  // const Node2<L,typename L::type_>& LHS;
  // const Node2<R,typename R::type_>& RHS;
  const L& LHS;
  const R& RHS;
  public:
  // mutable int counter_id = -99;
    static const int check = 8 ;
    // constexpr ADD(const L& Lin,const R& Rin): LHS(Lin), RHS(Rin){}
    constexpr ADD(const L& Lin, const R& Rin): LHS(Lin), RHS(Rin){}
    using type_ = output;
    constexpr inline type_ value() const {
      // std::cout << "calling value in ADD: " <<  "\n";
      return type_{LHS.value()} + type_{RHS.value()};
    }

    constexpr inline type_ diff(int wrt) const
    {
      return type_{LHS.diff(wrt)} + type_{RHS.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr inline type_ diff(Variable<DIFFTY> wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};

template< typename I, typename output>
struct EXP : public Node2<EXP<I,output>, output>{
  private:
    const I& input_obj;
    template <class T_c =  output, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr>
    T_c exp_mem(T_c x) const
    {
      // std::cout << "Running std..." << std::endl;
      return std::exp(x);
    }

    template <class T_c = output, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr>
    T_c exp_mem(T_c x) const
    {
      // std::cout << "Running fixed..." << std::endl;
      return fpm::exp(x);
    }

  public:
    constexpr EXP(const I& in_): input_obj(in_) {}
    using type_ = output;
    constexpr inline type_ value() const {
      // std::cout << "calling value in ADD: " <<  "\n";
      return exp_mem(type_{input_obj.value()});
    }

    constexpr inline type_ diff(int wrt) const
    {
      return exp_mem(type_{input_obj.value()})*type_{input_obj.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr inline type_ diff(Variable<DIFFTY> wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};



template< typename I, typename output>
struct LOG10 : public Node2<LOG10<I,output>, output>{
  private:
    const I& input_obj;
    template <class T_c = output, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr>
    T_c log10_mem(T_c x) const
    {
        return std::log10(x);
    }

    template <class T_c = output, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr>
    T_c log10_mem(T_c x) const
    {
        return fpm::log10(x);
    }

  public:
    constexpr LOG10(const I& in_): input_obj(in_) {}
    using type_ = output;
    // constants
    type_ one = type_{1};
    // type_ one = type_{1};
    constexpr type_ value() const {
      return log10_mem(type_{input_obj.value()});
    }


    constexpr inline type_ diff(int wrt) const
    {
      return (one/type_{input_obj.value()})*type_{input_obj.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr inline type_ diff(Variable<DIFFTY> wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};


template< typename I>
struct SIN : public Node2<SIN<I>, typename I::type_>{
  private:
    const I& input_obj;
    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr> inline 
    T_c sin_mem(T_c x) const
    {
        return std::sin(x);
    }

    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr> inline 
    T_c sin_mem(T_c x) const
    {
        return fpm::sin(x);
    }

  public:
    constexpr SIN(const I& in_): input_obj(in_) {}
    using type_ = typename I::type_;
    constexpr inline type_ value() const {
      return sin_mem(input_obj.value());
    }
};


template< typename I>
struct COS : public Node2<COS<I>, typename I::type_>{
  private:
    const I& input_obj;
    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr> inline 
    T_c cos_mem(T_c x) const
    {
        return std::cos(x);
    }

    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr> inline 
    T_c cos_mem(T_c x) const
    {
        return fpm::cos(x);
    }

  public:
    constexpr COS(const I& in_): input_obj(in_) {}
    using type_ = typename I::type_;
    constexpr inline type_ value() const {
      return cos_mem(input_obj.value());
    }
};

template< typename I>
struct TAN : public Node2<TAN<I>, typename I::type_>{
  private:
    const I& input_obj;
    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr> inline 
    T_c tan_mem(T_c x) const
    {
        return std::tan(x);
    }

    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr> inline 
    T_c tan_mem(T_c x) const
    {
        return fpm::tan(x);
    }

  public:
    constexpr TAN(const I& in_): input_obj(in_) {}
    using type_ = typename I::type_;
    constexpr inline type_ value() const {
      return tan_mem(input_obj.value());
    }
};


template< typename I>
struct SQRT : public Node2<SQRT<I>, typename I::type_>{
  private:
    const I& input_obj;
    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr> inline 
    T_c sqrt_mem(T_c x) const
    {
        return std::sqrt(x);
    }

    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr> inline 
    T_c sqrt_mem(T_c x) const
    {
        return fpm::sqrt(x);
    }

  public:
    constexpr SQRT(const I& in_): input_obj(in_) {}
    using type_ = typename I::type_;
    constexpr inline type_ value() const {
      return sqrt_mem(input_obj.value());
    }
};

template<typename L, typename R, typename output>
struct MUL : public Node2<MUL<L,R, output>, output>{

  const L& LHS;
  const R& RHS;

  public:
    constexpr MUL(const L& Lin, const R& Rin): LHS(Lin), RHS(Rin){}
    using type_ = output;
    constexpr inline type_ value() const {
      return type_{LHS.value()} * type_{RHS.value()};
    }


    constexpr inline type_ diff(int wrt) const
    {
      return type_{LHS.value()}*type_{RHS.diff(wrt)} + type_{RHS.value()}*type_{LHS.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr inline type_ diff(Variable<DIFFTY> wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};

template<typename L, typename R, typename output>
struct DIV : public Node2<DIV<L,R,output>, output>{

  const L& NUM;
  const R& DEN;

  public:
    constexpr DIV(const L& Lin, const R& Rin): NUM(Lin), DEN(Rin){}
    using type_ = output;
    constexpr type_ value() const {
      return type_{NUM.value()} / type_{DEN.value()};
    }

    constexpr inline type_ diff(int wrt) const
    {
      auto temp = type_{DEN.value()};
      return (type_{NUM.diff(wrt)}*temp - type_{DEN.diff(wrt)}* type_{NUM.value()})/ (temp*temp);
    }

    template<typename DIFFTY>
    constexpr inline type_ diff(Variable<DIFFTY> wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};

template<typename L, typename R, typename output>
struct SUB : public Node2<SUB<L,R,output>, output>{

  const L& LHS;
  const R& RHS;

  public:
    constexpr SUB(const L& Lin, const R& Rin): LHS(Lin), RHS(Rin){}
    using type_ = output;
    constexpr type_ value() const {
      return type_{LHS.value()} - type_{RHS.value()};
    }

    constexpr inline type_ diff(int wrt) const
    {
      return type_{LHS.diff(wrt)} - type_{RHS.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr inline type_ diff(Variable<DIFFTY> wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};


// template <typename L, typename R> inline
// constexpr ADD<L,R> operator+(const L& lhs , const R& rhs) {
//   auto a = ADD<L,R>(lhs,rhs);
//   return a;
// }


template <typename out_t, typename L, typename R> inline
auto add_ty(const L& lhs_in, const R&rhs_in)
{
    return ADD<L,R,out_t>(lhs_in, rhs_in);
}

template <typename L, typename R> inline
constexpr auto operator+(const L& lhs , const R& rhs) {
  auto a = ADD<L,R,typename L::type_>(lhs,rhs);
  return a;
}

template <typename out_t, typename I_t> inline
constexpr const EXP<I_t, out_t> exp_ty(const I_t& in_)
{
    return EXP<I_t, out_t>(in_);
}

template <typename I> inline
constexpr const TAN<I> tan(const I& in_)
{
    return TAN<I>(in_);
}


template <typename out_t, typename I_t> inline
auto log10_ty(const I_t& in_)
{
    return LOG10<I_t, out_t>(in_);
}

template <typename I> inline
constexpr const SIN<I> sin(const I& in_)
{
    return SIN<I>(in_);
}

template <typename I> inline
constexpr const COS<I> cos(const I& in_)
{
    return COS<I>(in_);
}

template <typename I> inline
constexpr const SQRT<I> sqrt(const I& in_)
{
    return SQRT<I>(in_);
}


template <typename out_t, typename L, typename R> inline 
auto mul_ty(const L& lhs , const R& rhs){
  return MUL<L,R,out_t>(lhs,rhs);
}

template <typename L, typename R> inline 
constexpr auto operator*(const L& lhs , const R& rhs){
  return MUL<L,R, typename L::type_>(lhs,rhs);
}


template <typename out_t, typename L, typename R> inline 
auto div_ty(const L& lhs , const R& rhs){
  return DIV<L,R,out_t>(lhs,rhs);
}

template <typename L, typename R> inline 
constexpr auto operator/(const L& lhs , const R& rhs){
  return DIV<L,R, typename L::type_>(lhs,rhs);
}

template <typename out_t, typename L, typename R> inline 
auto sub_ty(const L& lhs , const R& rhs){
  return SUB<L,R,out_t>(lhs,rhs);
}

template <typename L, typename R> inline 
constexpr auto operator-(const L& lhs , const R& rhs){
  return SUB<L,R, typename L::type_>(lhs,rhs);
}


// int main()
// {

//     Variable<float> v(2);
//     // std::cout << "v value: " << v.value_ << "\n";
//     // v.set_value(2.5);
//     // std::cout << "v value: " << v.value_ << "\n";


//     // static constexpr const ADD<Variable<float>, Variable<float>> add = ADD<Variable<float>, Variable<float>>(v,v);
//     // static constexpr auto exp_v = EXP<Variable<float>>(v);
//     // std::cout << "Exp value: " << exp_v.value();
//     // static constexpr const Node2<Variable<float>, float> &n1 = v;
//     // auto v_p_exp_v = v + exp(v);
//     // std::cout << "v + exp(v): " << v_p_exp_v.value() << "\n";
//     // static constexpr ADD<Variable<float>, ADD<Variable<float>, Variable<float>>> add_2 = ADD<Variable<float>, ADD<Variable<float>, Variable<float>>>(v, add);
//     // static constexpr auto answer = add_2.value();


//     auto add_1 =  v + v;
//     std::cout<< "add v+v: " << add_1.value() << "\n";
//     v.set_value(2.5);
//     std::cout<< "add v+v: " << add_1.value() << "\n";

//     auto sin_v = sin(v);
//     std::cout << "sin(v): " << sin_v.value() << "\n";

//     auto log10_v = log10(v);
//     std::cout << "log(v): " <<  log10_v.value() << "\n";

//     auto cos_v = cos(v);
//     std::cout << "cos(v): " << cos_v.value() << "\n";

//     auto tan_v = tan(v);
//     std::cout << "tan(v): " << tan_v.value() << "\n";

//     auto sqrt_v = sqrt(v);
//     std::cout << "sqrt(v): " << sqrt_v.value() << "\n";
//     // std::cout << "sin - vanilla: " << std::sin(2.5) << "\n";
//     // static constexpr auto exp_v = exp(v);
//     // static constexpr auto add_2 = exp_v + v;
//     // static constexpr auto mul_v = add_2 * v;
//     // // static_assert(add_2.check ==8 && "check is this is static");
//     // std::cout << "add: " << mul_v.value() << "\n";
//     // std::cout << "add: " << answer << "\n";
//     // std::cout << "v type_: " << std::is_floating_point<v::value>::value << "\n";
//     return 0;
// }
