// #include <functional>
#include <vector>

#include "./fixed.hpp"
#include "./ios.hpp"
#include "./math.hpp"

// #include <typeinfo>
// #include <ostream>
#include <stdlib.h>
// #include <iostream>
#include <stack>
#include <set>
#include <map>
#include <queue>
#include <algorithm>

static int idx;
template<typename T>
class Node{
private:
    // inline  // Each Node has a ID allotted to it
    int node_idx; 
public:
    bool isPlaceholder;
    virtual T getValue()  = 0;
    virtual T diff(int) =0;
    virtual void register_parent( Node<T> *) =0;
    virtual std::vector<Node<T> *> const getParents() =0;
    std::vector<Node<T> *> &getInputs(){}
    Node(){
      node_idx = idx;
      idx++;
    }

    int get_unq_node_idx() {
      return node_idx;
    }
};

template <typename T>
class Placeholder : public Node<T>{
  public: 
    Placeholder() {
      this->isPlaceholder = true;
    }
};

template <typename TYPE, typename I>
class Constant: public Placeholder<TYPE>{
  private:
    TYPE value;
    std::string name;
    int grad =0;
    std::vector<Node<TYPE> *> parents; // Shouldn't this be Operator instead?
  public:

    Constant(I val){
      TYPE num{val};
      value = num;
    }

    Constant(TYPE val, std::string n){
      value = val;
      name = n;
    }

    TYPE getValue()  {
      return value;
    }

    template<typename curr_type>
    curr_type getValue()
    {
      return static_cast<curr_type>(this->value);
    }

    void setValue(float input)  {
      TYPE num{input};
      this->value = num;
    }

    int getGrad(){
      return grad;
    }

    TYPE diff(int wrt_id){
      return TYPE{0};
    }

    void register_parent( Node<TYPE> * parent) {
      parents.push_back(parent);
    }

    std::vector<Node<TYPE> *> const getParents()  {
      return parents;
    }
    // int getPlaceholderType(){ // {0 -> constant, 1-> variable}
      // return 0;
    // }
};

// template<typename T>
// std::ostream& operator<<(std::ostream& os,  Constant<T> &obj) {
//      os << obj.getValue();
//      return os;
// }



template<typename T>
class Operator : public Node<T>{
  private:
    
    // Placeholder<T> * output;
    T output;
    T diff;
    // static int operator_index; // Unique identifier of Operator instances, there to make future passes easier.
  protected:
    int OP_ID;
    // int unq_op_id;
  public:
    // bool isComputed=false;
    // bool isDiffComputed = false;
    std::vector<Node<T> *> inputs; //it's node * because you can have input as either operator or placeholder
    std::vector<Node<T> *> parents;
    Operator()
    {
      // std::cout <<"Default C\n";
      this->isPlaceholder = false;
      // operator_index++;
    }

    Operator(Node<T> &a, Node<T> &b){
      // this->isPlaceholder = false;
      // std::cout << "New: Node operator\n";
      inputs.push_back(&a);
      inputs.push_back(&b);
      // setOP_ID();
      this->isPlaceholder = false;
      a.register_parent(this);
      b.register_parent(this);
      // unq_op_id =  operator_index;
      // operator_index++;
    }

    std::vector<Node<T> *> &getInputs(){
      return inputs;
    }

    // void setOpID(int id) {op_id = id;}
    T getOutput() {return output;}
    T getDiff() {return diff;}

    void setOutput(T out){output = out;}
    void setDiff(T d){diff = d;}
    // virtual void setOP_ID() = 0;

    int getOP_ID(){
      return OP_ID;
    }
    void register_parent( Node<T> * parent) {
      parents.push_back(parent);
    }

    std::vector<Node<T> *> const getParents()  {
      return parents;
    }

};

template <typename TYPE>
class Variable: public Placeholder<TYPE>{
  private:
    
    // std::string name;
    TYPE gradient;
    
  public:
    TYPE value;
    std::vector<Node<TYPE> *> parents;
    Node<TYPE> * child = nullptr;

    inline static int count;
    Variable () {}
    
    Variable(auto val){
      TYPE num{val};
      value = num;
      count++;
    }

    Variable(const Variable<TYPE> &input)
    {
      // Making sure it gets the child
      this->value = input.value;
      this->child = input.child;
      // this->setChild(const_cast<Node<TYPE> *>(input.getChild()));
      // register it as input to all the original nodes parents


      std::vector<Node<TYPE> *> parents = input.parents;
      for(auto parent :parents)
      {
        this->register_parent(parent);
      }
      // //Remove current input from Inputs of the parent and put this.

      for(auto parent : this->parents)
      {
        Operator<TYPE> *op_parent = static_cast<Operator<TYPE>*>(parent);
        std::vector<Node<TYPE> *> inputs_of_parent = op_parent->inputs;
        typename std::vector<Node<TYPE> *>::iterator position = std::find(inputs_of_parent.begin(), inputs_of_parent.end(), &input);
        inputs_of_parent.erase(position);
        inputs_of_parent.push_back(this);
      }
      

    }

    TYPE getValue() {
      // std::cout<<"Called get value" << std::endl;
      if(child)
      {
        return child->getValue();
      }
        return value;
    }

    template<typename old_type>
    void setValue(old_type input){
      TYPE num{input};
      value = num;
    }

    void setChild(Node<TYPE> *input)
    {
      this->child = input;
    }

    Node<TYPE> * getChild()
    {
      return this->child;
    }

    TYPE diff(int wrt_id){
      if(wrt_id == this->get_unq_node_idx()){
        return TYPE{1};
      }
      else{
        if(child)
        {
          return child->diff(wrt_id);
        }
        else
        {
          return TYPE{0};
        }     
      }
    }

    TYPE diff(Variable &V_wrt)
    {
      return this->diff(V_wrt.get_unq_node_idx());
    }

    void register_parent( Node<TYPE> * parent) {
      parents.push_back(parent);
    }

    std::vector<Node<TYPE> *> const getParents()  {
      return parents;
    }
    // int getPlaceholderType(){ // {0 -> constant, 1-> variable}
      // return 0;
    // }
};
template <typename T>
class add: public Operator<T> {
  public:
    add(Node<T> &a, Node<T> &b) : Operator<T>(a, b){this->OP_ID =1;}

    T getValue() {
      T temp{0};
      for(auto curr_input: this->getInputs())
      {
        temp+= curr_input->getValue();
      }
      // this->isComputed = true;
      // this->setOutput(temp);
      return temp;
    }


    T diff(int wrt_v){
      getValue();
      T temp{0};
      for(auto curr_input: this->getInputs()) 
      {
        temp+= curr_input->diff(wrt_v);
      }
      // this->isDiffComputed = true;
      // this->setDiff(temp);
      return temp;
      
    }
    int getOP_ID()
    {
      return 1;
    }
};

template <typename T>
class sub: public Operator<T> {
  int CHILD_ID = 2;
  public:
    sub(Node<T> &a, Node<T> &b) : Operator<T>(a, b){this->OP_ID = 2;}

    T getValue() {
      T temp =(this->getInputs()).at(0)->getValue() - (this->getInputs()).at(1)->getValue();
      return temp;
    }

    T diff(int wrt_v){
      getValue();
      T temp =(this->getInputs()).at(0)->diff(wrt_v) - (this->getInputs()).at(1)->diff(wrt_v);
      // this->isDiffComputed = true;
      this->setDiff(temp);
      return temp;
    }
};

template <typename T>
class mul: public Operator<T> {
  int CHILD_ID = 3;
  public:
    mul(Node<T> &a, Node<T> &b) : Operator<T>(a, b){setOP_ID();}

    T getValue() {
      T temp =(this->getInputs()).at(0)->getValue() * (this->getInputs()).at(1)->getValue();
    
      this->setOutput(temp);
      return temp;
    }
    void setOP_ID(){
      this->OP_ID = CHILD_ID;
    }

    T diff(int wrt_v) { 
      T curr_val = getValue();
        T temp =(this->getInputs()).at(0)->getValue() * (this->getInputs()).at(1)->diff(wrt_v) + (this->getInputs()).at(1)->getValue() * (this->getInputs()).at(0)->diff(wrt_v) ;
        this->setDiff(temp);
        return temp;
    }
    

};

template <typename T>
class division: public Operator<T> {
  int CHILD_ID = 4;
  public:
    division(Node<T> &a, Node<T> &b) : Operator<T>(a, b){setOP_ID();}
    T getValue() {
      T temp =(this->getInputs()).at(0)->getValue() / (this->getInputs()).at(1)->getValue();
      this->setOutput(temp);
      return temp;
    }

    void setOP_ID(){
      this->OP_ID = CHILD_ID;
    }

    T diff(int wrt_v) { 
      T curr_val = getValue();
        T temp =(this->getInputs()).at(1)->getValue() * (this->getInputs()).at(0)->diff(wrt_v) - (this->getInputs()).at(0)->getValue() * (this->getInputs()).at(1)->diff(wrt_v) ;
        temp = temp/((this->getInputs()).at(1)->getValue() * (this->getInputs()).at(1)->getValue());
        this->setDiff(temp);
        return temp;
    }
};


template <typename T>
Variable<T> &operator+( Node<T> &a,  Node<T> &b){
  Variable<T> *z = new Variable<T>(0);
  add<T> *temp = new add<T>(a,b);
  temp->register_parent(z);
  z->setChild(temp);
  return *z;
}

template <typename T>
Variable<T> &operator-( Node<T> &a,  Node<T> &b){
  Variable<T> *z = new Variable<T>(0);
  sub<T> *temp = new sub<T>(a,b);
  temp->register_parent(z);
  z->setChild(temp);
  return *z;
}

template <typename T>
Variable<T> &operator*( Node<T> &a,  Node<T> &b){
  Variable<T> *z = new Variable<T>(0);
  mul<T> *temp = new mul<T>(a,b);
  temp->register_parent(z);
  z->setChild(temp);
  return *z;
}

template <typename T>
Variable<T> &operator/( Node<T> &a,  Node<T> &b){
  Variable<T> *z = new Variable<T>(0);
  division<T> *temp = new division<T>(a,b);
  temp->register_parent(z);
  z->setChild(temp);
  return *z;
}

template<typename T>
class Expression{

  private:
  Node<T> *root_node;
  std::set<int> visited_nodes;
  std::vector<Node<T> *> ordering;
  public:
  Expression(Node<T> &rhs_node){root_node = &rhs_node;}

  Expression<T> * operator<<=( Node<T> &rhs_node){
    // std::cout<< " Assignment operator called\n";
    root_node = &rhs_node;
    // topological_sort(*root_node);
    return this;
  }



  void print_expr_top_sort(){
    
    if(visited_nodes.find(root_node->get_unq_node_idx()) == visited_nodes.end()) // If curr_node is not visited
    {
      visited_nodes.insert(root_node->get_unq_node_idx());

      if(root_node->isPlaceholder == false){

        Operator<T> * curr_op_parent =static_cast<Operator<T> *>(root_node);

        for(Node<T> *curr_child: curr_op_parent->getInputs())
        {
          topological_sort(*curr_child);
        }
      }
      // ordering.push_back(&curr_node);
      // std::cout << root_node->get_unq_node_idx() <<"\t";
    }
  }

  T getValue(){
    root_node->getValue();
  }

  Node<T> &getRootNode()
  {
    return root_node;
  }
};
// template <typename T>
// Node<T> &operator+( Node<T> &a,  Expression<T> &b){

//   add<T> *temp = new add<T>(a,b.getRootNode);
//   return *temp;
// }



