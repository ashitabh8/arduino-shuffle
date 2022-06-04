#include "AD_math.hpp"


class Node_lite{
    public:
    Node_lite(){}
    bool isVar;
};


template<typename T>
class Var_lite:public Node_lite{

    private:
        T value;
        int id;
    public:
        Var_lite(float input)
        {
            T num{input};
            this->value = num;
            this->isVar = true;
        }
        void setValue(T input)
        {
            this->value = input;
        }
        T getValue()
        {
            return this->value;
        }
};


template<typename T>
class Op_lite: public Node_lite{
    private:
        T output_value;
        Var_lite *input[2];
        Var_lite *output_var;
        bool isComputed;
    public:
        Op_lite(){this->isVar = false;}

        Op_lite(Var_lite &a, Var_lite &b)
        {
            input[0] = &a;
            input[1] = &b;
        }
};


template<

