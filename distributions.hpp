#include <type_traits>
#include <random>
#include <cmath>
#include <bits/stdc++.h>
#include "/home/ashitabh/Documents/arduino-1.8.19/libraries/AutoDiff/lite_ad_mixed.hpp"



// NOTE: Two places where some performance loss could have creeped in -- 1) Sampler 2) Sqrt function see if something can be done about
// this.


class DistributionBase{};


template <typename mu_ty, typename stddev_ty,typename input_ty, typename T, typename EXPRESSION_TYPE>
class Normal : public DistributionBase {
  mu_ty& mu;
  stddev_ty& std_dev;
  input_ty& x;
  Constant<T> one, one_by_sqrt_2_pi_constant,minus_half;
  std::string name;

  std::default_random_engine gen;
  std::uniform_real_distribution<float> distribution;
  
  EXPRESSION_TYPE expression = (one_by_sqrt_2_pi_constant * (one/std_dev))*(exp(minus_half*((x-mu)/std_dev)*((x-mu)/std_dev)));
  
  public:
    using type_ = T;
    Normal(mu_ty& mu_in, stddev_ty& stddev_in, input_ty& x_in, std::string name_in): one(1), one_by_sqrt_2_pi_constant(one_by_sqrt_2_pi),
              minus_half(-0.5),std_dev(stddev_in), mu(mu_in),x(x_in), name(name_in)
    {
          // TODO: Add static_assert that std_dev == 0 not allowed.
          // using param_type = typename std::normal_distribution<float>::param_type;
          // param_type temp_(-std_dev.value(),1);
          // distribution.param(temp_);
    }

    // Normal(mu_ty& mu_in, stddev_ty& stddev_in, T data_): one(1), one_by_sqrt_2_pi_constant(one_by_sqrt_2_pi),
    //           minus_half(-0.5),std_dev(stddev_in), mu(mu_in),x(data_)
    // {
    //       // TODO: Add static_assert that std_dev == 0 not allowed.
    //       // using param_type = typename std::normal_distribution<float>::param_type;
    //       // param_type temp_(-std_dev.value(),1);
    //       // distribution.param(temp_);
    // }

    auto inline pdf()
    {
      return expression;
    }

    auto inline pdf(T x_in)
    {
      x.set_value(x_in);
      return pdf().value();
    }

    inline void set(T mu_in, T stddev_in)
    {
      mu.set_value(mu_in);
      std_dev.set_value(stddev_in);
    }

    void set_input(T input_)
    {
      std::cout << "value to be inserted: " << input_ << "\n";
      x.set_value(input_);
    }

    void set_input(Constant<T> & input)
    {
      x.set_value(input.value());
    }

    void set_input(Variable<T> & input)
    {
      x.set_value(input.value());
    }

    inline void set_mu(T mu_in)
    {
      mu.set_value(mu_in);
    }

    inline void set_stddev(T stddev_in)
    {
      std_dev.set_value(stddev_in);
    }

    void see_input()
    {
        std::cout << x.value() <<"\n";
    }

    void see_params()
    {
      std::cout << "Params\n";
      std::cout << "Input: " << x.value() << "\n";
      std::cout << "mu: " << mu.value() << "\n";
      std::cout << "sd: " << std_dev.value() << "\n";
    }

    template < typename Tc = T, typename std::enable_if<std::is_base_of<fpm::fixedpoint_base, Tc>::value>::type* = nullptr> inline
    Tc sample()
    {
      auto sign_and_int_bits = Tc::integer_bits;

      Tc mu_value, stddev_value;
      if constexpr(std::is_base_of<ADBase, mu_ty>::value) {
        mu_value = Tc{mu.value()};
      }
      if constexpr(std::is_base_of<ADBase, stddev_ty>::value) {
        stddev_value = Tc{std_dev.value()};
      }
      if constexpr(std::is_base_of<DistributionBase, mu_ty>::value)
      {
        mu_value = Tc{mu.sample()};
      }
      if constexpr(std::is_base_of<DistributionBase, stddev_ty>::value)
      {
        stddev_value = Tc{std_dev.sample()};
      }

      uint32_t random32_1 = gen();
      random32_1 = random32_1 << sign_and_int_bits;
      random32_1 = random32_1 >> sign_and_int_bits;

      uint32_t random32_2 = gen();
      random32_2 = random32_2 << sign_and_int_bits;
      random32_2 = random32_2 >> sign_and_int_bits;

      uint32_t random32_3 = gen();
      random32_3 = random32_3 << sign_and_int_bits;
      random32_3 = random32_3 >> sign_and_int_bits;

      auto sum = random32_1 + random32_2 + random32_3;
      sum = sum << 1;

      auto shifted_mean = Tc::from_raw_value(sum) - Tc(3);
      return (shifted_mean *stddev_value) + mu_value;
    }

    template < typename Tc = T, typename std::enable_if<std::is_base_of<fpm::fixedpoint_base, Tc>::value>::type* = nullptr> inline
    Tc sample_box() // Wont work right now...
    {
      
      float u, v;
      Tc u_fx,v_fx,s;
      Tc mu_value, stddev_value;
      if constexpr(std::is_base_of<ADBase, mu_ty>::value) {
        mu_value = Tc{mu.value()};
      }
      if constexpr(std::is_base_of<ADBase, stddev_ty>::value) {
        stddev_value = Tc{std_dev.value()};
      }
      if constexpr(std::is_base_of<DistributionBase, mu_ty>::value)
      {
        mu_value = Tc{mu.sample()};
      }
      if constexpr(std::is_base_of<DistributionBase, stddev_ty>::value)
      {
        stddev_value = Tc{std_dev.sample()};
      }

      do{
        u = distribution(gen); // Need to change this if it comes to it. 
        v = distribution(gen);
        u_fx = Tc{u};
        v_fx = Tc{v};
        s =u_fx*u_fx + v_fx*v_fx;
      } while(s == Tc{0} || s >= Tc{1});
        Tc mag = fpm::sqrt(Tc{-2} * fpm::log(s)/s);
      Tc z0 = u_fx* mag;
      // Tc z1 = v_fx* mag; // might not be needed, just do one and return.... test if this works too.
      
      z0 = z0 * stddev_value + mu_value;
      // z1 = z1 * stddev_value + mu_value;
      return z0;
    }

    T value() const
    {
      // std::cout << "Value estimation\n";
      // std::cout << "x: " << x.value() << "\n";
      // std::cout << "mu: " << mu.value() <<
      auto answer = T{expression.value()};
      // std::cout<< name << ", VALUE: " << answer << "\n";
      return answer ;
    }

    constexpr inline T diff(int wrt) const
    {
      return T{expression.diff(wrt)};
    }

    template<typename DIFFTY>
    constexpr inline T diff(Variable<DIFFTY> &wrt_diff)
    {
      return this->diff(wrt_diff.Var_ID);
    }

    template <typename Tc = T, typename std::enable_if<std::is_arithmetic<Tc>::value>::type* = nullptr> inline 
    Tc sample_box() // Wont work right now...
    {

      Tc u,v,s;

      Tc mu_value, stddev_value;
      if constexpr(std::is_base_of<ADBase, mu_ty>::value) {
        mu_value = Tc{mu.value()};
      }
      if constexpr(std::is_base_of<ADBase, stddev_ty>::value) {
        stddev_value = Tc{std_dev.value()};
      }
      if constexpr(std::is_base_of<DistributionBase, mu_ty>::value)
      {
        mu_value = Tc{mu.sample()};
      }
      if constexpr(std::is_base_of<DistributionBase, stddev_ty>::value)
      {
        stddev_value = Tc{std_dev.sample()};
      }
      do{
         u = distribution(gen);
         v = distribution(gen);
         s = u*u + v*v;
      } while(s == 0 || s >= 1);

      Tc z0 = u* std::sqrt(-2 * std::log(s)/s);
      // Tc z1 = v* std::sqrt(-2 * std::log(s)/s);
      z0 = z0 * stddev_value + mu_value;
      // z1 = z1 * stddev_value + mu_value;
      return z0;
    }

    template <typename Tc = T, typename std::enable_if<std::is_arithmetic<Tc>::value>::type* = nullptr> inline 
    Tc sample()
    {
      Tc u,v,s;

      Tc mu_value, stddev_value;
      if constexpr(std::is_base_of<ADBase, mu_ty>::value) {
        mu_value = Tc{mu.value()};
      }
      if constexpr(std::is_base_of<ADBase, stddev_ty>::value) {
        stddev_value = Tc{std_dev.value()};
      }
      if constexpr(std::is_base_of<DistributionBase, mu_ty>::value)
      {
        mu_value = Tc{mu.sample()};
      }
      if constexpr(std::is_base_of<DistributionBase, stddev_ty>::value)
      {
        stddev_value = Tc{std_dev.sample()};
      }
      std::normal_distribution<float> distr_normal(mu_value, stddev_value);

      return distr_normal(gen);
    }

    template <typename Tc = T> inline
    std::vector<Tc> sample(int num_samples)
    {
        std::vector<Tc> all_samples;
      
      while(all_samples.size() < num_samples)
      {
          all_samples.push_back(sample());
      }
      return all_samples;
    }
};

template<typename T, typename mu_ty, typename stddev_ty, typename input_ty >
inline auto normal_dist(mu_ty& mu_in, stddev_ty& stddev_in, input_ty& x_in, std::string name_in){
  Constant<T> one_by_sqrt_2_pi_constant,minus_half, one;
  using expr_type = decltype((one_by_sqrt_2_pi_constant * (one/stddev_in))*(exp(minus_half*((x_in-mu_in)/stddev_in)*((x_in-mu_in)/stddev_in))));
  return Normal<mu_ty, stddev_ty,input_ty, T,expr_type>(mu_in, stddev_in,x_in, name_in);
}


// template<typename T, typename mu_ty, typename stddev_ty >
// inline auto normal_dist(mu_ty& mu_in, stddev_ty& stddev_in, T data_){
//   Constant<T> one_by_sqrt_2_pi_constant,minus_half, x;
//   using expr_type = decltype((one_by_sqrt_2_pi_constant * stddev_in)*(exp(minus_half*((x-mu_in)/stddev_in)*((x-mu_in)/stddev_in))));
//   return Normal<mu_ty, stddev_ty, T,expr_type>(mu_in, stddev_in);
// }



//TODO: Define operators for distributions
