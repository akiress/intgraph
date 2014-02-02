#include <iostream>
using namespace std;
class B {
private:
    int x;
public:
    virtual int foo() { return this->bar(); }
    virtual int bar() { return 1; }
};
class C {
private:
    int x;
public:
    virtual int foo() { return 2; }
};
class D : public B, public C {
private:
    int y;
public:
    virtual int foo() { return B::foo(); }
};
class E : public D {
private:
    int z;
public:
    virtual int bar() { return 3; }
};

int main()
{
    B * p = new D();
    C * q = new D();
    B * r = new E();
    C * s = new E();
    int i = p->bar();
    int j = q->foo();
    int k = r->bar();
    int l = s->foo();
    cout << i << endl;
    cout << j << endl;
    cout << k << endl;
    cout << l << endl;
    char abc;
    cin >> abc;
}