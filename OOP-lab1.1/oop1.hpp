class A {
public:
    A() {};

private:
    A(const A& clone) {};
    A(A&& another) {};
};
