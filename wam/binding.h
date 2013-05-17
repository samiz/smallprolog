#ifndef BINDING_H
#define BINDING_H

struct Binding
{
    uint var;
    shared_ptr<Term::Term> val;
    Binding(uint var, shared_ptr<Term::Term> val)
        :var(var), val(val)
    {
    }

    Binding() { }
};

#endif // BINDING_H
