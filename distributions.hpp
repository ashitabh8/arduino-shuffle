#include <type_traits>
#include <random>
#include <cmath>
#include <bits/stdc++.h>
#include "/home/ashitabh/Documents/arduino-1.8.19/libraries/AutoDiff/lite_ad_mixed.hpp"

template <typename mu_ty, typename stddev_ty, typename T>
class Normal {
  // Constant<fpm::fixed_16_16> one(1), one_by_sqrt_2_pi_constant(one_by_sqrt_2_pi), minus_half(-0.5);
  // Variable<T> std_dev,mu;
  mu_ty& mu;
  stddev_ty& std_dev;
  Constant<T> one, one_by_sqrt_2_pi_constant,minus_half,x;
//   std::random_device dev;
  std::default_random_engine gen;
//   std::mt19937 rng;
//   RandomEngine& gen_fx;
  // std::conditional_t<std::is_base_of<fpm::fixedpoint_base, T>::value,
  //                      std::uniform_int_distribution<int_fast16_t>,
  //                       std::uniform_real_distribution<T>> distribution;
  
  
  std::uniform_real_distribution<float> distribution;
//   std::uniform_real_distribution<T> distribution(-1,1);

  public:
    Normal(mu_ty& mu_in, stddev_ty& stddev_in): one(1), one_by_sqrt_2_pi_constant(one_by_sqrt_2_pi),
              minus_half(-0.5),std_dev(stddev_in), mu(mu_in)
    {
    //   std_dev.set_value(1);
    //   mu.set_value(0); // default parameters

      // if constexpr(!std::is_base_of<fpm::fixedpoint_base, T>::value)
      // {
          using param_type = typename std::uniform_real_distribution<float>::param_type ;
          param_type temp_(-1,1);
          distribution.param(temp_);
      // }
      // if constexpr(std::is_base_of<fpm::fixedpoint_base, T>::value)
      // {
      //   //   using param_type = typename std::uniform_real_distribution<T>::param_type ;
      //   //   param_type temp_(-1,1);
      //   //   distribution.param(temp_);
      //   // rng(gen());
      //   using param_type = typename std::uniform_int_distribution<int_fast16_t>::param_type ;
      //   param_type temp_(INT16_MIN,INT16_MAX);
      //   distribution.param(temp_);
      // }
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

    template < typename Tc = T, typename std::enable_if<std::is_base_of<fpm::fixedpoint_base, Tc>::value>::type* = nullptr> inline
    auto sample()
    {
      auto sign_and_int_bits = Tc::integer_bits;
      // int32_t random32_1, random32_2;
      float u, v;
      Tc u_fx,v_fx,s;
      do{
        u = distribution(gen); // Need to change this if it comes to it. 
        v = distribution(gen);
        u_fx = Tc{u};
        v_fx = Tc{v};
        // int sign_u = std::signbit(u);
        // int sign_v = std::signbit(v);
        // u = u << sign_and_int_bits;
        // u = u >> sign_and_int_bits;

        // v = v << sign_and_int_bits;
        // v = v >> sign_and_int_bits;
        // random32_1 = distribution(gen_fx);
        // int sign_1 = std::signbit(random32_1);
        // random32_1 = random32_1 << sign_and_int_bits;
        // random32_1 = random32_1 >> sign_and_int_bits;

        // random32_2 = distribution(gen_fx);
        // int sign_2 = std::signbit(random32_2);
        // random32_2 = random32_2 << sign_and_int_bits;
        // random32_2 = random32_2 >> sign_and_int_bits;
        
        // u_fx = Tc::from_raw_value(u);
        // v_fx = Tc::from_raw_value(v);

        
        // u_fx = fpm::fixed_16_16::from_raw_value(u);
        // v_fx = fpm::fixed_16_16::from_raw_value(v);
        // u_fx = (sign_u) ?  - temp_u :  temp_u;
        // v_fx = (sign_v) ?  - temp_v :  temp_v;
        s =u_fx*u_fx + v_fx*v_fx;
      } while(s == Tc{0} || s >= Tc{1});

      
        // float sf = static_cast<float>(s);
        // float uf = static_cast<float>(u_fx);
        // float vf = static_cast<float>(v_fx);
        // float stddev_fl = static_cast<float>(std_dev.value());
        // float mu_fl = static_cast<float>(mu.value());
        // // float temp = static_cast<float>(fpm::log(s)/s);

        // float temp = std::log(sf)/sf;


        // float z0 = uf* std::sqrt(-2 * temp );
        // float z1 = vf* std::sqrt(-2 * temp);
        // z0 = z0 * stddev_fl + mu_fl;
        // z1 = z1 * stddev_fl + mu_fl;
        Tc mag = fpm::sqrt(Tc{-2} * fpm::log(s)/s);

    //   std::cout << "s: " << s  << ", u: " << u_fx  << ", v: " << v_fx  << ", mag: " << mag << "\n";
      Tc z0 = u_fx* mag;
      Tc z1 = v_fx* mag;
      
      z0 = z0 * std_dev.value() + mu.value();
      z1 = z1 * std_dev.value() + mu.value();
    //   std::cout << "z0: " << z0  << "\n";
    //   std::cout << "z1: " << z1 << "\n";
      return std::make_pair(z0,z1);
    }

    

    template <typename Tc = T, typename std::enable_if<std::is_arithmetic<Tc>::value>::type* = nullptr> inline 
    std::pair<Tc, Tc> sample()
    {

      Tc u,v,s;
      do{
         u = distribution(gen);
         v = distribution(gen);
         s = u*u + v*v;
      } while(s == 0 || s >= 1);

      Tc z0 = u* std::sqrt(-2 * std::log(s)/s);
      Tc z1 = v* std::sqrt(-2 * std::log(s)/s);
      z0 = z0 * std_dev.value() + mu.value();
      z1 = z1 * std_dev.value() + mu.value();
    //   u = u * std_dev.value() + mu.value();
    //   v = v * std_dev.value() + mu.value();
      
      //std::cout << "Using float sampler\n";
      return std::make_pair(z0,z1);
    }

    template <typename Tc = T> inline
    std::vector<Tc> sample(int num_samples)
    {
        int num_iters;
        std::vector<Tc> all_samples;
        if(num_samples%2)
        {
            num_iters = num_samples/2;
        }
        else{
            num_iters = (num_samples -1)/2;
            auto curr_pair_samples  = sample();
            all_samples.push_back(curr_pair_samples.first);
        }
      
      while(all_samples.size() < num_iters)
      {
          auto curr_pair_samples  = sample();
          all_samples.push_back(curr_pair_samples.first);
          all_samples.push_back(curr_pair_samples.second);

      }
      return all_samples;
    }


    // template <typename Tc = T, typename std::enable_if<std::is_arithmetic<Tc>::value>::type* = nullptr> inline 
    // auto sample(int num_samples)
    // {
    
    //   std::vector <Tc> samples;
    //   while(samples.size() < num_samples)
    //   {
    //       Tc u,v,s;
    //       do{
    //         u = distribution(gen);
    //         v = distribution(gen);
    //         s = u*u + v*v;
    //     } while(s == 0 || s >= 1);

    //     Tc z0 = u* std::sqrt(-2 * std::log(s)/s);
    //     Tc z1 = v* std::sqrt(-2 * std::log(s)/s);
    //     z0 = z0 * std_dev.value() + mu.value();
    //     z1 = z1 * std_dev.value() + mu.value();
    //     samples.push_back(z0);
    //     samples.push_back(z1);
    //   }
    //   return samples;
    // }
};

template<typename T, typename mu_ty, typename stddev_ty >
inline auto normal_dist(mu_ty& mu_in, stddev_ty& stddev_in){
  return Normal<mu_ty, stddev_ty, T>(mu_in, stddev_in);
}