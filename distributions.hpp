#include <type_traits>
#include <random>
#include <cmath>
#include <bits/stdc++.h>
#include "/home/ashitabh/Documents/arduino-1.8.19/libraries/AutoDiff/lite_ad_mixed.hpp"


class DistributionBase{};


template <typename mu_ty, typename stddev_ty, typename T, typename EXPRESSION_TYPE>
class Normal : public DistributionBase {
  mu_ty& mu;
  stddev_ty& std_dev;
  Constant<T> one, one_by_sqrt_2_pi_constant,minus_half,x;

  std::default_random_engine gen;
  std::uniform_real_distribution<float> distribution;
  EXPRESSION_TYPE expression = (one_by_sqrt_2_pi_constant * std_dev)*(exp(minus_half*((x-mu)/std_dev)*((x-mu)/std_dev)));
  
  public:
  using type_ = T;
    Normal(mu_ty& mu_in, stddev_ty& stddev_in): one(1), one_by_sqrt_2_pi_constant(one_by_sqrt_2_pi),
              minus_half(-0.5),std_dev(stddev_in), mu(mu_in),x(1)
    {
          using param_type = typename std::uniform_real_distribution<float>::param_type;
          param_type temp_(-1,1);
          distribution.param(temp_);
    }

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
      x.set_value(input_);
    }

    inline void set_mu(T mu_in)
    {
      mu.set_value(mu_in);
    }

    inline void set_stddev(T stddev_in)
    {
      std_dev.set_value(stddev_in);
    }

    template < typename Tc = T, typename std::enable_if<std::is_base_of<fpm::fixedpoint_base, Tc>::value>::type* = nullptr> inline
    Tc sample() // Wont work right now...
    {
      auto sign_and_int_bits = Tc::integer_bits;
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
      return T{expression.value()};
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
    Tc sample() // Wont work right now...
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

    // template <typename Tc = T> inline
    // std::vector<Tc> sample(int num_samples)
    // {
    //     int num_iters;
    //     std::vector<Tc> all_samples;
    //     if(num_samples%2)
    //     {
    //         num_iters = num_samples/2;
    //     }
    //     else{
    //         num_iters = (num_samples -1)/2;
    //         auto curr_pair_samples  = sample();
    //         all_samples.push_back(curr_pair_samples.first);
    //     }
      
    //   while(all_samples.size() < num_iters)
    //   {
    //       auto curr_pair_samples  = sample();
    //       all_samples.push_back(curr_pair_samples.first);
    //       all_samples.push_back(curr_pair_samples.second);

    //   }
    //   return all_samples;
    // }
};

template<typename T, typename mu_ty, typename stddev_ty >
inline auto normal_dist(mu_ty& mu_in, stddev_ty& stddev_in){
  Constant<T> one_by_sqrt_2_pi_constant,minus_half, x;
  using expr_type = decltype((one_by_sqrt_2_pi_constant * stddev_in)*(exp(minus_half*((x-mu_in)/stddev_in)*((x-mu_in)/stddev_in))));
  return Normal<mu_ty, stddev_ty, T,expr_type>(mu_in, stddev_in);
}



//TODO: Define operators for distributions