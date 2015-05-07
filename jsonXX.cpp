#include <iostream>
#include <sstream>
#include "jsonXX.h"
namespace json {
    using namespace std;

    //
    // class var
    //
    var::var()
        : entity(0)
    {
    }
    var::var(const std::string& json)
        : entity(0)
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
        if(this->entity == 0) {
            return TypeNull;
        }
        return this->entity->getType();
    }
    void var::assign(VarEntity* entity)
    {
        delete this->entity;
        this->entity = entity;
    }
    var& var::operator = (const var& var)
    {
        this->assign((var.entity) ? var.entity->clone() : 0);
        return *this;
    }
    var& var::operator = (bool value)
    {
        Value* p = new Value();
        p->setBool(value);
        this->assign(p);
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
    var::operator bool() const
    {
        this->assertEntityNotNull();
        Type type = this->getType();
        if(type != TypeBool && type != TypeNumber && type != TypeString ) {
            stringstream ss;
            ss << "entity is not number or string, but " << type;
            throw std::domain_error(ss.str());
        }
        return this->entity->getBool();
    }
    var::operator int() const
    {
        this->assertEntityNotNull();
        Type type = this->getType();
        if(type != TypeBool && type != TypeNumber && type != TypeString ) {
            stringstream ss;
            ss << "entity is not number or string, but " << type;
            throw std::domain_error(ss.str());
        }
        return (int)this->entity->getNumber();
    }
    var::operator double() const
    {
        this->assertEntityNotNull();
        Type type = this->getType();
        if(type != TypeBool && type != TypeNumber && type != TypeString ) {
            stringstream ss;
            ss << "entity is not number or string, but " << type;
            throw std::domain_error(ss.str());
        }
        return this->entity->getNumber();
    }
    var::operator const std::string&() const
    {
        this->assertEntityNotNull();
        Type type = this->getType();
        if(type != TypeBool && type != TypeNumber && type != TypeString ) {
            stringstream ss;
            ss << "entity is not number or string, but " << type;
            throw std::domain_error(ss.str());
        }
        return this->entity->getString();
    }
    int var::length() const
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeArray);
        return this->entity->size();
    }
    void var::push(double value)
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeArray);
        var data;
        data = value;
        this->entity->push(data);
    }
    void var::push(const std::string& value)
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeArray);
        var data;
        data = value;
        this->entity->push(data);
    }
    void var::push(const char* value) {
        this->push(std::string(value));
    }
    void var::push(const var& value)
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeArray);
        this->entity->push(value);
    }
    void var::remove(int index)
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeArray);
        this->entity->remove(index);
    }
    var& var::operator [](int index)
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeArray);
        return this->entity->get(index);
    }
    const var& var::operator [](int index) const
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeArray);
        return this->entity->get(index);
    }
    bool var::exists(const std::string& key) const
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeObject);
        return this->entity->exists(key);
    }
    var& var::operator [](const char* key)
    {
        return this->operator[](std::string(key));
    }
    const var& var::operator [](const char* key) const
    {
        return this->operator[](std::string(key));
    }
    var var::keys() const
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeObject);
        return this->entity->keys();
    }
    void var::remove(const std::string& key)
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeObject);
        this->entity->remove(key);
    }
    var& var::operator [](const std::string& key)
    {
        if(this->getType() != var::TypeObject) {
            this->assign(new Object());
        }
        if(!this->entity->exists(key)) {
            var e("''");
            this->entity->set(key, e);
        }
        return this->entity->get(key);
    }
    const var& var::operator [](const std::string& key) const
    {
        this->assertEntityNotNull();
        this->assertEntityTypeEquals(TypeObject);
        return this->entity->get(key);
    }
    var& var::operator [](const var& key)
    {
        switch(this->getType()) {
            case TypeArray:
                return this->operator[]((double)key);
                break;
            case TypeObject:
                return this->operator[]((const string&)key);
                break;
        }
        stringstream ss;
        ss << "entity type is not Array or Object, but " << this->getType();
        throw std::domain_error(ss.str());
        return *this;//dummy;
    }
    const var& var::operator [](const var& key) const
    {
        return ((var*)this)->operator[](key);
    }
    const void var::writeJson(std::ostream& os) const
    {
        if(this->entity == 0) {
            os << "null";
        } else {
            this->entity->writeJson(os);
        }
    }
    void var::assertEntityTypeEquals(var::Type type) const
    {
        if(this->getType() != type) {
            stringstream ss;
            ss << "entity type is not " << type << ", but " << this->getType();
            throw std::domain_error(ss.str());
        }
    }
    void var::assertEntityNotNull() const
    {
        if(this->entity == 0) {
            throw std::domain_error("reference null object.");
        }
    }
    std::ostream& operator << (std::ostream& os, const var& var)
    {
        var.writeJson(os);
        return os;
    }
    std::istream& operator >> (std::istream& is, var& var)
    {
        var.parse(is);
        return is;
    }
}
