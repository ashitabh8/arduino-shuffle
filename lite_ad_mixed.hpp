#include <cmath>
// #include <iostream>
#include <type_traits>
#include "fixed.hpp"
#include "math.hpp"


constexpr int counter = 0;

 int var_id_counter = 0;


class ADBase{};

template <typename M, typename T>
class Node2 : public ADBase{
  public:
    using type_ = T;
    using child_type = M;
    constexpr const M& cast() const {return static_cast<const M&>(*this);}
    constexpr type_ value() const {return cast().value();}
};

class AD_OP_Base{};

template <typename T>
struct Variable : public Node2<Variable<T>, T>{

  public:
    using type_ = T;
    mutable type_ value_;
    const int Var_ID;

    constexpr Variable ():value_(0), Var_ID(var_id_counter++){}
    constexpr Variable( const type_ val_):value_(val_), Var_ID(var_id_counter++){}

    constexpr type_ value() const
    {
      // std::cout <<" value in variable : " << value_ << "\n";
      return value_;
    }

    constexpr void set_value(type_ in_)
    {
      // std::cout << "variable set value \n";
        value_ = in_;
    }

    constexpr   type_ diff(int wrt_v) const
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

    constexpr   type_ value() const
    {
      return value_;
    }

    constexpr   type_ diff(int wrt_v) const
    {
      return type_{0};
    }

    void set_value(type_ in_)
    {
      // std::cout << "constant set value : " << in_  << "\n";
        value_ = in_;
    }
};

template<typename L, typename R, typename output>
struct ADD : public Node2<ADD<L,R,output>, output>, public AD_OP_Base{


  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, L>::value,
  L,
  const L& >::type Ltype;

  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, R>::value,
  R,
  const R& >::type Rtype;

  // typedef typename std::conditional<
  //   std::is_same<L, Variable<typename L::type_>>::value,
  //   const Variable<typename L::type_>&,
  //     typename std::conditional<
  //     std::is_same<L, Constant<typename L::type_>>::value,
  //     const Constant<typename L::type_>&,
  //     L>::type
  //   >::type Ltype;

  // typedef typename std::conditional<
  //   std::is_same<R, Variable<typename R::type_>>::value,
  //   const Variable<typename R::type_>&,
  //   typename std::conditional<
  //   std::is_same<R, Constant<typename R::type_>>::value,
  //   const Constant<typename R::type_>&,
  //   R>::type
  //   >::type Rtype;

   Ltype LHS;
   Rtype RHS;
  public:
    static const int check = 8 ;
   
    ADD( Ltype Lin,  Rtype Rin) : LHS{Lin}, RHS{Rin} {}
    
    using type_ = output;
    constexpr   type_ value() const {
      // std::cout << "calling value in ADD: " <<  "\n";
      return type_{LHS.value()} + type_{RHS.value()};
    }

    constexpr   type_ diff(int wrt) const
    {
      return type_{LHS.diff(wrt)} + type_{RHS.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr   type_ diff(Variable<DIFFTY> &wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};


template< typename I, typename output>
struct EXP : public Node2<EXP<I,output>, output>,public AD_OP_Base{
  private:
  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, I>::value,
  I,
  const I& >::type Itype;
    Itype input_obj;
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
    constexpr EXP(Itype in_): input_obj(in_) {}
    using type_ = output;
    constexpr   type_ value() const {
      // std::cout << "calling value in ADD: " <<  "\n";
      return exp_mem(type_{input_obj.value()});
    }

    constexpr   type_ diff(int wrt) const
    {
      return exp_mem(type_{input_obj.value()})*type_{input_obj.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr   type_ diff(Variable<DIFFTY> wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};

template< typename I, typename output>
struct LOG10 : public Node2<LOG10<I,output>, output>, public AD_OP_Base{
  private:
    typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, I>::value,
  I,
  const I& >::type Itype;
    Itype input_obj;
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
    constexpr LOG10(Itype in_): input_obj(in_) {}
    using type_ = output;
    // constants
    type_ one = type_{1};
    // type_ one = type_{1};
    constexpr type_ value() const {
      auto answer = log10_mem(type_{input_obj.value()});
      // std::cout << " log answer: " <<answer << "\n";
      return answer;
    }


    constexpr   type_ diff(int wrt) const
    {
      auto answer = (one/type_{input_obj.value()})*type_{input_obj.diff(wrt)};
      auto input = type_{input_obj.value()};
      auto input_diff = type_{input_obj.diff(wrt)};
      // if(input_diff != 0)
      // {
        // std::cout << "Log elements diff(log(x)): \n";
        // std::cout <<  "x -> " << type_{input_obj.value()} << "\n";
        // std::cout << "diff(x)" << type_{input_obj.diff(wrt)} << "\n";
        // std::cout << "log diff: " << answer << "\n";
      // }
      
      return answer;
    }

    template<typename DIFFTY>
    constexpr   type_ diff(Variable<DIFFTY> &wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};


template< typename I>
struct SIN : public Node2<SIN<I>, typename I::type_>,public AD_OP_Base{
  private:
    const I& input_obj;
    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr>   
    T_c sin_mem(T_c x) const
    {
        return std::sin(x);
    }

    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr>   
    T_c sin_mem(T_c x) const
    {
        return fpm::sin(x);
    }

  public:
    constexpr SIN(const I& in_): input_obj(in_) {}
    using type_ = typename I::type_;
    constexpr   type_ value() const {
      return sin_mem(input_obj.value());
    }
};


template< typename I>
struct COS : public Node2<COS<I>, typename I::type_>,public AD_OP_Base{
  private:
    const I& input_obj;
    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr>   
    T_c cos_mem(T_c x) const
    {
        return std::cos(x);
    }

    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr>   
    T_c cos_mem(T_c x) const
    {
        return fpm::cos(x);
    }

  public:
    constexpr COS(const I& in_): input_obj(in_) {}
    using type_ = typename I::type_;
    constexpr   type_ value() const {
      return cos_mem(input_obj.value());
    }
};

template< typename I>
struct TAN : public Node2<TAN<I>, typename I::type_>,public AD_OP_Base{
  private:
    const I& input_obj;
    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr>   
    T_c tan_mem(T_c x) const
    {
        return std::tan(x);
    }

    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr>   
    T_c tan_mem(T_c x) const
    {
        return fpm::tan(x);
    }

  public:
    constexpr TAN(const I& in_): input_obj(in_) {}
    using type_ = typename I::type_;
    constexpr   type_ value() const {
      return tan_mem(input_obj.value());
    }
};


template< typename I>
struct SQRT : public Node2<SQRT<I>, typename I::type_>{
  private:
    const I& input_obj;
    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_arithmetic<T_c>::value>* = nullptr>   
    T_c sqrt_mem(T_c x) const
    {
        return std::sqrt(x);
    }

    template <class T_c = typename I::type_, typename std::enable_if_t<std::is_base_of<fpm::fixedpoint_base, T_c>::value>* = nullptr>   
    T_c sqrt_mem(T_c x) const
    {
        return fpm::sqrt(x);
    }

  public:
    constexpr SQRT(const I& in_): input_obj(in_) {}
    using type_ = typename I::type_;
    constexpr   type_ value() const {
      return sqrt_mem(input_obj.value());
    }
};

template<typename L, typename R, typename output>
struct MUL : public Node2<MUL<L,R, output>, output>,public AD_OP_Base{

  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, L>::value,
  L,
  const L& >::type Ltype;

  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, R>::value,
  R,
  const R& >::type Rtype;


   Ltype LHS;
   Rtype RHS;

  public:
    constexpr MUL(Ltype Lin, Rtype Rin): LHS(Lin), RHS(Rin){}
    using type_ = output;
    constexpr   type_ value() const {
      return type_{LHS.value()} * type_{RHS.value()};
    }


    constexpr   type_ diff(int wrt) const
    {
      return type_{LHS.value()}*type_{RHS.diff(wrt)} + type_{RHS.value()}*type_{LHS.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr   type_ diff(Variable<DIFFTY> &wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};

template<typename L, typename R, typename output>
struct DIV : public Node2<DIV<L,R,output>, output>,public AD_OP_Base{

  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, L>::value,
  L,
  const L& >::type Ltype;

  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, R>::value,
  R,
  const R& >::type Rtype;


   Ltype NUM;
   Rtype DEN;

  public:
    constexpr DIV(Ltype Lin, Rtype Rin): NUM(Lin), DEN(Rin){}
    using type_ = output;
    constexpr type_ value() const {
      return type_{NUM.value()} / type_{DEN.value()};
    }

    constexpr   type_ diff(int wrt) const
    {
      auto temp = type_{DEN.value()};
      return (type_{NUM.diff(wrt)}*temp - type_{DEN.diff(wrt)}* type_{NUM.value()})/ (temp*temp);
    }

    template<typename DIFFTY>
    constexpr   type_ diff(Variable<DIFFTY> &wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};

template<typename L, typename R, typename output>
struct SUB : public Node2<SUB<L,R,output>, output>,public AD_OP_Base{

  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, L>::value,
  L,
  const L& >::type Ltype;

  typedef typename std::conditional<
  std::is_base_of<AD_OP_Base, R>::value,
  R,
  const R& >::type Rtype;


   Ltype LHS;
   Rtype RHS;

  public:
    constexpr SUB(Ltype Lin, Rtype Rin): LHS(Lin), RHS(Rin){}
    using type_ = output;
    constexpr type_ value() const {
      return type_{LHS.value()} - type_{RHS.value()};
    }

    constexpr   type_ diff(int wrt) const
    {
      return type_{LHS.diff(wrt)} - type_{RHS.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr   type_ diff(Variable<DIFFTY> &wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }
};

template <typename out_t, typename L, typename R>  
auto add_ty(const L& lhs_in, const R&rhs_in)
{
    return ADD<L,R,out_t>(lhs_in, rhs_in);
}

template <typename L, typename R>  
auto operator+(const L& lhs , const R& rhs) { // TODO: Implement precedence of types
  return ADD<L,R,typename L::type_>(lhs,rhs);
}

template <typename out_t, typename I_t>  
constexpr const EXP<I_t, out_t> exp_ty(const I_t& in_)
{
    return EXP<I_t, out_t>(in_);
}

template <typename I_t>  
constexpr auto exp(const I_t& in_)
{
    return EXP<I_t, typename I_t::type_>(in_);
}


template <typename I>  
constexpr const TAN<I> tan(const I& in_)
{
    return TAN<I>(in_);
}


template <typename out_t, typename I_t>  
auto log10_ty(const I_t& in_)
{
    return LOG10<I_t, out_t>(in_);
}

template <typename I_t>  
auto log10(const I_t& in_)
{
    return LOG10<I_t, typename I_t::type_>(in_);
}

template <typename I>  
constexpr const SIN<I> sin(const I& in_)
{
    return SIN<I>(in_);
}

template <typename I>  
constexpr const COS<I> cos(const I& in_)
{
    return COS<I>(in_);
}

template <typename I>  
constexpr const SQRT<I> sqrt(const I& in_)
{
    return SQRT<I>(in_);
}


template <typename out_t, typename L, typename R>   
auto mul_ty(const L& lhs , const R& rhs){
  return MUL<L,R,out_t>(lhs,rhs);
}

template <typename L, typename R>   
constexpr auto operator*(const L& lhs , const R& rhs){
  return MUL<L,R, typename L::type_>(lhs,rhs);
}


template <typename out_t, typename L, typename R>   
auto div_ty(const L& lhs , const R& rhs){
  return DIV<L,R,out_t>(lhs,rhs);
}

template <typename L, typename R>   
constexpr auto operator/(const L& lhs , const R& rhs){
  return DIV<L,R, typename L::type_>(lhs,rhs);
}

template <typename out_t, typename L, typename R>   
auto sub_ty(const L& lhs , const R& rhs){
  return SUB<L,R,out_t>(lhs,rhs);
}

template <typename L, typename R>   
constexpr auto operator-(const L& lhs , const R& rhs){
  return SUB<L,R, typename L::type_>(lhs,rhs);
}



// Some useful constants

// 1/sqrt(2*pi)

#define one_by_sqrt_2_pi 0.3989422804014327

// Constant<float> one_by_sqrt_2_pi(0.3989422804014327);
