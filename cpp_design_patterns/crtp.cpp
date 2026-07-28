#include <iostream>

template <typename Derived>
class Base
{
    Base() = default;
    ~Base() = default;
    friend Derived;

public:

    void do_something()
    {
        static_cast<Derived&>(*this).do_something();
        std::cout << "Base Class\n";
    }
};

class Derived1 : public Base<Derived1>
{
public:
    void do_something()
    {
        std::cout << "Derived1\n";
    }
};


class Derived2 : public Base<Derived2>
{
public:
    void do_something()
    {
        std::cout << "Derived2\n";
    }
};

template <typename Derived>
void class_do_something(Base<Derived>& b)
{
    b.do_something();
}


int main()
{
    Derived1 d1;
    Derived2 d2;
    class_do_something(d1);
    class_do_something(d2);
}

