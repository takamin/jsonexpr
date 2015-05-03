#include "jsonXX.h"
namespace jsonXX {
    Var Var::Null;
    Value Null;

    //
    // class Var
    //
    Var::Var()
        : entity(new Value())
    {
    }
    Var::~Var()
    {
        delete entity;
    }
    void Var::setEntity(const Data* entity)
    {
        assign(entity->clone());
    }
    void Var::assign(Data* entity)
    {
        delete this->entity;
        this->entity = entity;
    }
    Var& Var::operator = (double value)
    {
        this->assign(new Value(value));
        return *this;
    }
    Var& Var::operator = (const Data& value)
    {
        this->assign(value.clone());
        return *this;
    }
    Var& Var::operator = (const char* value)
    {
        this->assign(new Value(value));
        return *this;
    }
    Var& Var::operator = (const std::string& value)
    {
        this->assign(new Value(value));
        return *this;
    }
    Var::operator double() const
    {
        return this->entity->getNumber();
    }
    Var::operator const std::string&() const
    {
        return this->entity->getString();
    }
    Var& Var::operator [](int index)
    {
        return this->entity->operator[](index);
    }
    const Var& Var::operator [](int index) const
    {
        return this->entity->operator[](index);
    }
    Var& Var::operator [](const std::string& key)
    {
        return this->entity->operator[](key);
    }
    const Var& Var::operator [](const std::string& key) const
    {
        return this->entity->operator[](key);
    }
    std::ostream& operator << (std::ostream& os, const Var& var)
    {
        var.getEntity()->writeJson(os);
        return os;
    }
    std::istream& operator >> (std::istream& is, Var& var)
    {
        var.parse(is);
        return is;
    }
}
