#ifndef JSONXX_H_INCLUDED
#define JSONXX_H_INCLUDED

#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>

namespace jsonXX {

    //
    // Var class
    //
    class Data;
    class Var {
        public:
            static Var Null;
        public:
            Var();
            Var(const std::string& json);
            virtual ~Var();
            void setEntity(const Data* entity);
            Data* getEntity() { return entity; }
            const Data* getEntity() const { return entity; }
            operator double() const;
            operator const std::string&() const;
            Var& operator = (const Var& var);
            Var& operator = (double value);
            Var& operator = (const char* value);
            Var& operator = (const std::string& value);
            Var& operator = (const Data& value);
            Var& operator [](int index);
            const Var& operator [](int index) const;
            Var& operator [](const std::string& key);
            const Var& operator [](const std::string& key) const;
        private:
            void assign(Data* entity);
        public:
            void parse(std::istream& is);
        private:
            void parseObject(std::istream& is);
            void parseObjectKey(std::istream& is);
            void parseArray(std::istream& is);
            void parseIdentifier(std::istream& is);
            void parseString(std::istream& is);
            void parseNumber(std::istream& is);
        private:
            Data* entity;
    };
    std::ostream& operator << (std::ostream& os, const Var& var);
    std::istream& operator >> (std::istream& is, Var& var);

    //
    // class Data
    //
    class Data {
        friend class Var;
        public:
            enum Type {
                TypeNull = 0,
                TypeInteger = 1,
                TypeDouble = 2,
                TypeString = 3,
                TypeArray = 4,
                TypeObject = 5, };
            Data() : type(TypeNull) { }
            Data(const Data& that) : type(that.type) { }
            virtual ~Data() {}
            virtual Data* clone() const = 0;
            virtual std::ostream& writeJson(std::ostream& os) const = 0;
            
            //
            // features of Number
            //

        public:
            operator double() const { return getNumber(); }
            virtual void setNumber(double value) { throw new std::runtime_error("setNumber called, but this is NOT jsonXX::Value."); }
        private:
            virtual double getNumber() const { throw new std::runtime_error("getNumber called, but this is NOT jsonXX::Value."); }

            //
            // features of String
            //

        public:
            operator const std::string&() const { return getString(); }
            virtual void setString(const std::string& value) { throw new std::runtime_error("setString called, but this is NOT jsonXX::Value."); }
        private:
            virtual const std::string& getString() const { throw new std::runtime_error("getString called, but this is NOT jsonXX::Value."); }
            
            //
            // features of Array
            //

        public:
            virtual int size() { throw new std::runtime_error("size called, but this is NOT jsonXX::Array."); }
            virtual int push(const Data& item) { throw new std::runtime_error("push called, but this is NOT jsonXX::Array."); }
            /*!
             * An element specified by index of array.
             *
             * returns an element of array specified by the index.
             * if this is not type of json arrau, exception will be thrown.
             */
            Var& operator[](int index) { return this->get(index); }
            const Var& operator[](int index) const { return this->get(index); }
        private:
            virtual const Var& get(int index) const { throw new std::runtime_error("get(index) called, but this is NOT jsonXX::Array."); }
            virtual Var& get(int index) { throw new std::runtime_error("get(index) called, but this is NOT jsonXX::Array."); }

            //
            // features of Object
            //

        public:
            virtual int set(const std::string& key, const Data& data) { throw new std::runtime_error("set called, but this is NOT jsonXX::Object."); }
            virtual bool exists(const std::string& key) const { throw new std::runtime_error("exists(key) called, but this is NOT jsonXX::Object."); }

            /*!
             * A value identified by key.
             *
             * returns a value identified by the key of this object.
             * if this is not type of json object, exception will be thrown.
             */
            Var& operator[](const std::string& key);
            const Var& operator[](const std::string& key) const;
        private:
            virtual const Var& get(const std::string& key) const { throw new std::runtime_error("get(key) called, but this is NOT jsonXX::Object."); }
            virtual Var& get(const std::string& key) { throw new std::runtime_error("get(key) called, but this is NOT jsonXX::Object."); }

        protected:
            Data(Type type) : type(type) { }
            void setType(Type type) { this->type = type; }
        public:
            Type getType() const { return this->type; }
        private:
            Type type;
    };

    template<class T> bool operator ==(const Data& data1, T data2) { return (double)data1 == (double)data2; }
    template<class T> bool operator ==(double data2, const Data& data1) { return (double)data1 == (double)data2; }
    
    template<class T> bool operator !=(const Data& data1, double data2) { return (double)data1 != (double)data2; }
    template<class T> bool operator !=(double data2, const Data& data1) { return (double)data1 != (double)data2; }
    
    template<class T> bool operator <(const Data& data1, T data2) { return (double)data1 < (double)data2; }
    template<class T> bool operator <(double data2, const Data& data1) { return (double)data1 >= (double)data2; }
    
    template<class T> bool operator <=(const Data& data1, T data2) { return (double)data1 <= (double)data2; }
    template<class T> bool operator <=(double data2, const Data& data1) { return (double)data1 > (double)data2; }
    
    template<class T> bool operator >(const Data& data1, T data2) { return (double)data1 > (double)data2; }
    template<class T> bool operator >(double data2, const Data& data1) { return (double)data1 <= (double)data2; }
    
    template<class T> bool operator >=(const Data& data1, T data2) { return (double)data1 >= (double)data2; }
    template<class T> bool operator >=(double data2, const Data& data1) { return (double)data1 < (double)data2; }

    inline bool operator ==(const Data& data1, const std::string& data2) { return (const std::string&)data1 == data2; }
    inline bool operator ==(const std::string& data2, const Data& data1) { return (const std::string&)data1 == data2; }

    inline bool operator !=(const Data& data1, const std::string& data2) { return (const std::string&)data1 != data2; }
    inline bool operator !=(const std::string& data2, const Data& data1) { return (const std::string&)data1 != data2; }

    class Value : public Data {
        public:
            Value() : Data(TypeNull), value("") {}
            Value(const Value& that) : Data(that.getType()), value(that.value) { }
            Value(double value) : Data(Data::TypeDouble), value(Value::num2str(value)) { }
            Value(const std::string& value) : Data(Data::TypeString), value(value) { }

            Data* clone() const { return new Value(*this); }
            std::ostream& writeJson(std::ostream& os) const {
                switch(getType()) {
                    case Data::TypeDouble:
                        os << str2num(value);
                        break;
                    case Data::TypeString:
                        os << "\"";
                        os << escapeString(value);
                        os << "\"";
                        break;
                }
                return os;
            }
            void setString(const std::string& value) { this->value = value; setType(Data::TypeString); }
            void setNumber(double value) { this->value = num2str(value); setType(Data::TypeDouble); }

        public:
            static std::string escapeString(const std::string& s) {
                std::ostringstream ss;
                std::string::const_iterator c = s.begin();
                for(; c != s.end(); c++) {
                    switch(*c) {
                        case '\"': ss << '\\' << '"'; break;
                        case '\n': ss << '\\' << 'n'; break;
                        case '\r': ss << '\\' << 'r'; break;
                        case '\t': ss << '\\' << 't'; break;
                        default: ss << (char)*c; break;
                    }
                }
                return ss.str();
            }
        private:
            const std::string& getString() const { return value; }
            double getNumber() const { return Value::str2num(value); }
            static double str2num(const std::string& s) {
                double value = 0.0;
                std::istringstream(s) >> value;
                return value;
            }
            static const std::string num2str(double value) {
                std::ostringstream ss;
                ss << value;
                return ss.str();
            }
        private:
            std::string value;
    };

    class Array : public Data {
        public:
            Array() : Data(Data::TypeArray) { }
            Array(const Array& that) : Data(Data::TypeArray) {
                std::vector<Var*>::const_iterator item = that.value.begin();
                for(; item != that.value.end(); item++) {
                    this->push(*((*item)->getEntity()));
                }
            }
            ~Array() {
                std::vector<Var*>::const_iterator item = value.begin();
                for(; item != value.end(); item++) {
                    delete *item;
                }
            }
            Data* clone() const { return new Array(*this); }
            std::ostream& writeJson(std::ostream& os) const {
                os << "[";
                int n = value.size();
                for(int i = 0; i < n; i++) {
                    value[i]->getEntity()->writeJson(os);
                    if(i < n - 1) {
                        os << ',';
                    }
                }
                os << "]";
                return os;
            }
            int size() { return value.size(); }
            int push(const Data& item) {
                Var* var = new Var();
                var->setEntity(&item);
                value.push_back(var);
            }
        private:
            const Var& get(int index) const;
            Var& get(int index);
        private:
            void assertValidIndex(int index) const {
                if(index < 0 || value.size() <= index) {
                    throw new std::out_of_range("jsonXX::Array index out of range.");
                }
            }
        private:
            std::vector<Var*> value;
    };

    class Object : public Data {
        public:
            Object() : Data(Data::TypeObject) { }
            Object(const Object& that) : Data(Data::TypeObject) {
                std::map<std::string, Var*>::const_iterator entry = that.value.begin();
                for(; entry != that.value.end(); entry++) {
                    this->set(entry->first, *(entry->second->getEntity()));
                }
            }
            ~Object() {
                std::map<std::string, Var*>::const_iterator entry = value.begin();
                for(; entry != value.end(); entry++) {
                    delete entry->second;
                }
            }
            Data* clone() const { return new Object(*this); }
            std::ostream& writeJson(std::ostream& os) const {
                os << "{";
                int i = 0, n = value.size();
                std::map<std::string, Var*>::const_iterator kv = value.begin();
                for(; kv != value.end(); kv++, i++) {
                    os << "\"" << Value::escapeString(kv->first) << "\":";
                    kv->second->getEntity()->writeJson(os);
                    if(i < n - 1) {
                        os << ',';
                    }
                }
                os << "}";
                return os;
            }
            bool exists(const std::string& key) const { return getValuePtr(key) != 0; }
            int set(const std::string& key, const Data& data) {
                std::map<std::string, Var*>::iterator it = value.find(key);
                if(it != value.end()) {
                    delete it->second;
                    value.erase(it);
                }
                Var* var = new Var();
                var->setEntity(&data);
                std::pair<std::string, Var*> item(key, var);
                value.insert(item);
            }
        private:
            const Var& get(const std::string& key) const {
                const Var* p = getValuePtr(key);
                return (p) ? *p : Var::Null;
            }
            Var& get(const std::string& key) {
                Var* p = getValuePtr(key);
                return (p) ? *p : Var::Null;
            }

        private:
            const Var* getValuePtr(const std::string& key) const { ((Object*)this)->getValuePtr(key); }
            Var* getValuePtr(const std::string& key) {
                std::map<std::string, Var*>::iterator it = value.find(key);
                return (it == value.end()) ? 0 : it->second;
            }
        private:
            std::map<std::string, Var*> value;
    };

    //
    // class Data
    //
    inline Var& Data::operator[](const std::string& key)
    {
        return this->get(key);
    }
    inline const Var& Data::operator[](const std::string& key) const
    {
        return this->get(key);
    }
    inline const Var& Array::get(int index) const
    {
        assertValidIndex(index);
        return *(this->value[index]);
    }
    inline Var& Array::get(int index)
    {
        assertValidIndex(index);
        return *(this->value[index]);
    }

    //
    // Compare templates with Var
    //

    template<class T> bool operator ==(const Var& data1, T data2)
        { return (double)data1 == (double)data2; }
    template<class T> bool operator ==(double data2, const Var& data1)
        { return (double)data1 == (double)data2; }

    template<class T> bool operator !=(const Var& data1, double data2)
        { return (double)data1 != (double)data2; }
    template<class T> bool operator !=(double data2, const Var& data1)
        { return (double)data1 != (double)data2; }

    template<class T> bool operator <(const Var& data1, T data2)
        { return (double)data1 < (double)data2; }
    template<class T> bool operator <(double data2, const Var& data1)
        { return (double)data1 >= (double)data2; }

    template<class T> bool operator <=(const Var& data1, T data2)
        { return (double)data1 <= (double)data2; }
    template<class T> bool operator <=(double data2, const Var& data1)
        { return (double)data1 > (double)data2; }

    template<class T> bool operator >(const Var& data1, T data2)
        { return (double)data1 > (double)data2; }
    template<class T> bool operator >(double data2, const Var& data1)
        { return (double)data1 <= (double)data2; }

    template<class T> bool operator >=(const Var& data1, T data2)
        { return (double)data1 >= (double)data2; }
    template<class T> bool operator >=(double data2, const Var& data1)
        { return (double)data1 < (double)data2; }

    //
    // Compare between Var and string
    //

    inline bool operator ==(const Var& data1, const std::string& data2)
        { return (const std::string&)data1 == data2; }
    inline bool operator ==(const std::string& data2, const Var& data1)
        { return (const std::string&)data1 == data2; }

    inline bool operator !=(const Var& data1, const std::string& data2)
        { return (const std::string&)data1 != data2; }
    inline bool operator !=(const std::string& data2, const Var& data1)
        { return (const std::string&)data1 != data2; }

    inline bool operator ==(const Var& data1, const char* data2)
        { return (const std::string&)data1 == std::string(data2); }
    inline bool operator ==(const char* data2, const Var& data1)
        { return (const std::string&)data1 == std::string(data2); }

    inline bool operator !=(const Var& data1, const char* data2)
        { return (const std::string&)data1 != std::string(data2); }
    inline bool operator !=(const char* data2, const Var& data1)
        { return (const std::string&)data1 != std::string(data2); }
}

#endif
