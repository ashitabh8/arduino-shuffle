#include <vector>
#include <iostream>
#include <random>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iterator>

template<typename op_ty, typename T>
auto sum_containers(std::vector<T>& vec)
{
    op_ty answer{0};
    for(auto elem: vec)
    {
        answer += elem.value();
    }
    return answer;
}

template<typename T>
void print_values(std::vector<T> &input)
{
    for(auto& elem: input)
    {
        std::cout << elem.value() << "\n";
    }
}

template<typename T, typename L>
void copy_vec(std::vector<T> &num_data, std::vector<L> &container_data)
{
    for(int i = 0; i < num_data.size(); i++)
    {
        container_data.at(i).set_value(num_data.at(i));
    }
}

template<typename T>
std::vector<std::vector<T>> reader(std::string filename)
{
    std::vector<std::vector<std::string>> content;
    std::vector<std::string> row;
    std::string line, word;

    std::fstream file (filename, std::ios::in);
    if(file.is_open())
    {
        while(getline(file, line))
        {
        row.clear();
        
        std::stringstream str(line);
        
        while(getline(str, word, ','))
        row.push_back(word);
        content.push_back(row);
        }
    }   
    else
        std::cout<<"Could not open the file\n";

    
    std::vector<std::vector<T>> data;
    int header = 0;
    int i = 0;
    for(auto row: content)
    {
        if(i==header){
            i++;
            continue;
        }
        std::vector<T> row_new;
        for(auto data: row)
        {
            row_new.push_back(T{std::stof(data)});
        }

        data.push_back(row_new);
    }
    // std::cout << "size: " << data.size() << "\n";
    return data;
}
template<typename L>
auto get_subset(std::vector<L> &input_vector, int subset_size)
{
    std::random_shuffle(std::begin(input_vector), std::end(input_vector));
    std::vector<L> new_vec;

    for(int i =0; i < subset_size; i++)
    {
        new_vec.push_back(input_vector.at(i));
    }
    return new_vec;
}

template<typename T>
auto get_column(int column_num, std::vector<std::vector<T>>& input_vec)
{
    std::vector<T> final_vec;
    for(auto row: input_vec)
    {
        final_vec.push_back(row.at(column_num));
    }
    return final_vec;
}

template<typename op_ty, typename T, typename LOSS_ty, typename surr_ty, typename us_ty, typename sigma_ty>
void fit(std::vector<T> &mu_container, LOSS_ty& loss, surr_ty& resultant_surrogate,
            us_ty& u_s, sigma_ty& sigma_s)
{
    int num_iters = 200;
    //  auto trial_surr = log10(surrogate_mu_0) + log10(surrogate_mu_1);
    //  auto trial_prior = log10(prior_mu_0) + log10(prior_mu_1);
    //  auto trial_sum = trial_prior - trial_surr;
    op_ty learning_rate{0.001};
    for(int i = 0 ; i < num_iters; i++)
    {
        // get L samples of mu
        std::cout <<" ********************************************* \n";
        std::cout << "Iteration: " << i << "\n";
        std::cout << "get L mu samples\n";
        // resultant_surrogate.see_params();
        for(auto& elem: mu_container)
        {
            auto curr = resultant_surrogate.sample();
            //std::cout << curr << "\n";
            elem.set_value(curr);
        }
        // print_values(mu_container);
        // std::cout << "Prior - Surr: " << trial_sum.value() << "\n";
         std::cout << "Loss: "<< loss.value() << "\n";
         std::cout << "Estimating loss.diff(u_s)\n";
        auto u_s_diff = loss.diff(u_s);
        std::cout << "Estimating loss.diff(sigma_s)\n";
        auto sigma_s_diff = loss.diff(sigma_s);
        u_s.set_value( - u_s_diff * learning_rate + u_s.value());
        sigma_s.set_value( - sigma_s_diff * learning_rate + sigma_s.value());

        
        std::cout << "u_s: " << u_s.value() <<" sigma_s: " << sigma_s.value() << " u_s_diff: " << u_s_diff * learning_rate << " sigma_s_diff: " << sigma_s_diff * learning_rate << "\n";
        //std::cout << "d(LOSS)/du: " << loss.diff(u_s) << "\n";
        //std::cout << "d(LOSS)/dsd: " << loss.diff(sigma_s) << "\n";
        //auto u_s_diff = loss.diff(u_s);
        //auto sigma_s_diff = loss.diff(sigma_s);
        //u_s.set_value(u_s_diff + u_s.value());
        //sigma_s.set_value(sigma_s_diff + sigma_s.value());
        
    }

    std::cout << "Final u_s: " << u_s.value() << "\n";
    std::cout << "Final sigma_s: " << sigma_s.value() << "\n";

}


float softplus_operator(float input, float beta = 1, float threshold = 20){
  if(input > threshold ){
    return input;
  }
  return std::log(1 + std::exp(input));
}


template<typename T>
T grad_clipping(T grad, T min_thresh  = -10, T max_thresh = 10)
{
    if(grad > max_thresh) return max_thresh;
    if(grad < min_thresh) return min_thresh;
    return grad;
}


template<typename op_ty, typename T,typename data_ty, typename LOSS_ty, typename surr_ty, typename data_container_ty, typename us_ty, typename sigma_ty>
void fit_stochastic(std::vector<T> &mu_container,std::vector<data_ty>& data, LOSS_ty& loss, surr_ty& resultant_surrogate, data_container_ty& data_container,
            us_ty& u_s, sigma_ty& sigma_s)
{
  int num_iters =10;
  float learning_rate = op_ty{0.001};
  size_t num_data = data.size();
  std::cout << "initial u_s value : " << u_s.value()  << "\n";
  std::cout << "initial sigma_s value : " << sigma_s.value() << "\n";
  for(int i = 0; i < num_iters ; i++){
    // std::cout << "Num iter: " << i << "\n";
    std::cout << "*********************************************\n";
    std::cout << "mu values in iter: " << i << "\n";
    for(auto& curr_mu: mu_container)
    {
      auto curr = resultant_surrogate.sample();
      curr_mu.set_value(curr);
    }
    print_values(mu_container);
    
    // std::cout << "Stochastic iteration over dataset\n";
    for( int i = 0; i < num_data; i++) {
    //   std::cout << "Data point "<< i << " : " << data.at(i) << "\n";
      data_container.set_value(data.at(i));
      auto loss_diff_wrt_u = loss.diff(u_s);
      if(std::isnan(loss_diff_wrt_u)) continue;
      auto loss_u = grad_clipping<op_ty>(loss_diff_wrt_u,-100,100);
      
      if(std::isinf(loss_diff_wrt_u)) continue;
      auto loss_diff_wrt_sigma = loss.diff(sigma_s);
      auto loss_sigma = grad_clipping<op_ty>(loss_diff_wrt_sigma, -40,40);
      if(std::isnan(loss_diff_wrt_sigma)) continue;
      if(std::isinf(loss_diff_wrt_sigma)) continue;
    //   std::cout << "Loss function value: " << loss.value() << "\n";
    //   std::cout << "HELPER.cpp: d(loss)/d(u_s): " << loss_diff_wrt_u << "\n";
    //   std::cout << "HELPER.cpp: d(loss)/d(sigma_s): " << loss_diff_wrt_sigma << "\n";

      u_s.set_value( - loss_diff_wrt_u * learning_rate + u_s.value());
      sigma_s.set_value( softplus_operator(- loss_diff_wrt_sigma * learning_rate + sigma_s.value()));

    }
    std::cout << "u_s: " << u_s.value()  << ", sigma_s: " << sigma_s.value() << "\n";
    
  }

}
