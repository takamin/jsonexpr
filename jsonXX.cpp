#include <sstream>
#include "jsonXX.h"
namespace json {
    using namespace std;
    var var::Null;

    //
    // class var
    //
    var::var()
        : entity(new Value())
    {
    }
    var::var(const std::string& json)
        : entity(new Value())
    {
        istringstream ss(json);
        ss >> *this;
    }
    var::~var()
    {
        delete entity;
    }
    var::Type var::getType() const
    {
        return this->entity->getType();
    }
    void var::setEntity(const VarEntity* entity)
    {
        assign(entity->clone());
    }
    void var::assign(VarEntity* entity)
    {
        delete this->entity;
        this->entity = entity;
    }
    var& var::operator = (const var& var)
    {
        this->assign(var.entity->clone());
        return *this;
    }
    var& var::operator = (double value)
    {
        Value* p = new Value();
        p->setNumber(value);
        this->assign(p);
        return *this;
    }
    var& var::operator = (const VarEntity& value)
    {
        this->assign(value.clone());
        return *this;
    }
    var& var::operator = (const char* value)
    {
        Value* p = new Value();
        p->setString(value);
        this->assign(p);
        return *this;
    }
    var& var::operator = (const std::string& value)
    {
        Value* p = new Value();
        p->setString(value);
        this->assign(p);
        return *this;
    }
    var::operator double() const
    {
        return this->entity->getNumber();
    }
    var::operator const std::string&() const
    {
        return this->entity->getString();
    }
    int var::length() const
    {
        return this->entity->size();
    }
    void var::push(double value)
    {
        Value data;
        data.setNumber(value);
        this->entity->push(data);
    }
    void var::push(const std::string& value)
    {
        Value data;
        data.setString(value);
        this->entity->push(data);
    }
    void var::push(const var& value)
    {
        this->entity->push(*value.getEntity());
    }
    var& var::operator [](int index)
    {
        return this->entity->get(index);
    }
    const var& var::operator [](int index) const
    {
        return this->entity->get(index);
    }
    bool var::exists(const std::string& key) const
    {
        return this->entity->exists(key);
    }
    var& var::operator [](const std::string& key)
    {
        if(this->entity->getType() != var::TypeObject) {
            this->assign(new Object());
        }
        if(!this->entity->exists(key)) {
            Value e;
            e.setString("");
            this->entity->set(key, e);
        }
        return this->entity->get(key);
    }
    const var& var::operator [](const std::string& key) const
    {
        return this->entity->get(key);
    }
    std::ostream& operator << (std::ostream& os, const var& var)
    {
        var.getEntity()->writeJson(os);
        return os;
    }
    std::istream& operator >> (std::istream& is, var& var)
    {
        var.parse(is);
        return is;
    }
}
