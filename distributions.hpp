#include <type_traits>
#include <random>

#include "/home/ashitabh/Documents/arduino-1.8.19/libraries/AutoDiff/lite_ad_mixed.hpp"

template <typename mu_ty, typename stddev_ty, typename T >
class Normal {
  // Constant<fpm::fixed_16_16> one(1), one_by_sqrt_2_pi_constant(one_by_sqrt_2_pi), minus_half(-0.5);
  // Variable<T> std_dev,mu;
  mu_ty& mu;
  stddev_ty& std_dev;
  Constant<T> one, one_by_sqrt_2_pi_constant,minus_half,x;

  std::conditional_t<std::is_base_of<fpm::fixedpoint_base, T>::value,
                       void*, std::uniform_real_distribution<T>> distribution;
  
  std::default_random_engine gen;

  public:
    Normal(mu_ty& mu_in, stddev_ty& stddev_in): one(1),one_by_sqrt_2_pi_constant(one_by_sqrt_2_pi),
              minus_half(-0.5),std_dev(stddev_in), mu(mu_in)
    {
      std_dev.set_value(1);
      mu.set_value(0); // default parameters
    }

    auto inline pdf()
    {
      return (one_by_sqrt_2_pi_constant * std_dev)*(exp(minus_half*((x-mu)/std_dev)*((x-mu)/std_dev)));
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

    inline void set_mu(T mu_in)
    {
      mu.set_value(mu_in);
    }

    inline void set_stddev(T stddev_in)
    {
      std_dev.set_value(stddev_in);
    }

    template <typename Tc = T, typename std::enable_if<std::is_base_of<fpm::fixedpoint_base, Tc>::value>::type* = nullptr> inline
    Tc sample()
    {
      auto sign_and_int_bits = Tc::integer_bits;

      uint32_t random32_1 = gen();
      random32_1 = random32_1 << sign_and_int_bits;
      random32_1 = random32_1 >> sign_and_int_bits;

      auto random_value = Tc::from_raw_value(random32_1);
      return random_value * std_dev.value() + mu.value();
    }

    template <typename Tc = T, typename std::enable_if<std::is_arithmetic<Tc>::value>::type* = nullptr> inline 
    Tc sample()
    {
      Tc num = distribution(gen);
      //std::cout << "Using float sampler\n";
      return num * std_dev.value() + mu.value();
    }
};

template<typename T, typename mu_ty, typename stddev_ty>
inline auto normal_dist(mu_ty& mu_in, stddev_ty& stddev_in){
  return Normal<mu_ty, stddev_ty, T>(mu_in, stddev_in);
}