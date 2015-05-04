#include <sstream>
#include "jsonXX.h"
namespace jsonXX {
    using namespace std;
    Var Var::Null;
    Value Null;

    //
    // class Var
    //
    Var::Var()
        : entity(new Value())
    {
    }
    Var::Var(const std::string& json)
        : entity(new Value())
    {
        istringstream ss(json);
        ss >> *this;
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
    Var& Var::operator = (const Var& var)
    {
        this->assign(var.entity->clone());
        return *this;
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
    int Var::length() const
    {
        return this->entity->size();
    }
    void Var::push(double value)
    {
        Value data(value);
        this->entity->push(data);
    }
    void Var::push(const std::string& value)
    {
        Value data(value);
        this->entity->push(data);
    }
    void Var::push(const Var& value)
    {
        this->entity->push(*value.getEntity());
    }
    Var& Var::operator [](int index)
    {
        return this->entity->operator[](index);
    }
    const Var& Var::operator [](int index) const
    {
        return this->entity->operator[](index);
    }
    bool Var::exists(const std::string& key) const
    {
        return this->entity->exists(key);
    }
    Var& Var::operator [](const std::string& key)
    {
        if(this->entity->getType() != Data::TypeObject) {
            this->assign(new Object());
        }
        if(!this->entity->exists(key)) {
            Value empty("");
            this->entity->set(key, empty);
        }
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
