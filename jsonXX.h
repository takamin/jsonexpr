#ifndef JSONXX_H_INCLUDED
#define JSONXX_H_INCLUDED

#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>

namespace json {

    //
    // var class
    //
    class VarEntity;
    class var {
        public:
            enum Type {
                TypeNull = 0,
                TypeNumber = 2,
                TypeString = 3,
                TypeArray = 4,
                TypeObject = 5, };
            static var Null;
        public:
            var();
            var(const std::string& json);
            virtual ~var();
            void parse(std::istream& is);
            Type getType() const;
        public:
            operator double() const;
            operator const std::string&() const;
            var& operator = (const var& var);
            var& operator = (double value);
            var& operator = (const char* value);
            var& operator = (const std::string& value);
            var& operator = (const VarEntity& value);

            int length() const;
            void push(double value);
            void push(const std::string& value);
            void push(const var& value);
            var& operator [](int index);
            const var& operator [](int index) const;
            
            bool exists(const std::string& key) const;
            var& operator [](const std::string& key);
            const var& operator [](const std::string& key) const;
        public:
            const VarEntity* getEntity() const { return entity; }
            const void writeJson(std::ostream& os) const;
        private:
            void assign(VarEntity* entity);
            void parseObject(std::istream& is);
            void parseObjectKey(std::istream& is);
            void parseArray(std::istream& is);
            void parseIdentifier(std::istream& is);
            void parseString(std::istream& is);
            void parseNumber(std::istream& is);
        private:
            VarEntity* entity;
    };
    std::ostream& operator << (std::ostream& os, const var& var);
    std::istream& operator >> (std::istream& is, var& var);

    //
    // class VarEntity
    //
    class VarEntity {
        friend class var;
        protected:
            VarEntity(var::Type type) : type(type) { }
            void setType(var::Type type) { this->type = type; }
            var::Type getType() const { return this->type; }
            virtual ~VarEntity() {}
        public:
            virtual std::ostream& writeJson(std::ostream& os) const = 0;
        private:
            virtual VarEntity* clone() const = 0;
            virtual void setNumber(double value) { throw new std::runtime_error("setNumber called, but this is NOT json::Value."); }
            virtual double getNumber() const { throw new std::runtime_error("getNumber called, but this is NOT json::Value."); }
            virtual void setString(const std::string& value) { throw new std::runtime_error("setString called, but this is NOT json::Value."); }
            virtual const std::string& getString() const { throw new std::runtime_error("getString called, but this is NOT json::Value."); }
            virtual int size() { throw new std::runtime_error("size called, but this is NOT json::Array."); }
            virtual int push(const var& item) { throw new std::runtime_error("push called, but this is NOT json::Array."); }
            virtual const var& get(int index) const { throw new std::runtime_error("get(index) called, but this is NOT json::Array."); }
            virtual var& get(int index) { throw new std::runtime_error("get(index) called, but this is NOT json::Array."); }
            virtual int set(const std::string& key, const VarEntity& data) { throw new std::runtime_error("set called, but this is NOT json::Object."); }
            virtual bool exists(const std::string& key) const { throw new std::runtime_error("exists(key) called, but this is NOT json::Object."); }
            virtual const var& get(const std::string& key) const { throw new std::runtime_error("get(key) called, but this is NOT json::Object."); }
            virtual var& get(const std::string& key) { throw new std::runtime_error("get(key) called, but this is NOT json::Object."); }
        private:
            var::Type type;
    };

    inline std::string escapeString(const std::string& s) {
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
    class Value : public VarEntity {
        friend class var;
        private:
            Value() : VarEntity(var::TypeNull), value("") {}
        public:
            std::ostream& writeJson(std::ostream& os) const {
                switch(getType()) {
                    case var::TypeNumber:
                        os << str2num(value);
                        break;
                    case var::TypeString:
                        os << "\"";
                        os << escapeString(value);
                        os << "\"";
                        break;
                }
                return os;
            }
        private:
            VarEntity* clone() const { return new Value(*this); }
            void setString(const std::string& value) {
                this->value = value; setType(var::TypeString);
            }
            const std::string& getString() const {
                return value;
            }
            void setNumber(double value) {
                this->value = num2str(value);
                setType(var::TypeNumber); }
            double getNumber() const { return Value::str2num(value);
            }
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

    class Array : public VarEntity {
        friend class var;
        private:
            Array() : VarEntity(var::TypeArray) { }
            Array(const Array& that) : VarEntity(var::TypeArray) {
                std::vector<var*>::const_iterator item = that.value.begin();
                for(; item != that.value.end(); item++) {
                    this->push(**item);
                }
            }
            ~Array() {
                std::vector<var*>::const_iterator item = value.begin();
                for(; item != value.end(); item++) {
                    delete *item;
                }
            }
        public:
            std::ostream& writeJson(std::ostream& os) const {
                os << "[";
                int n = value.size();
                for(int i = 0; i < n; i++) {
                    os << *value[i];
                    if(i < n - 1) {
                        os << ',';
                    }
                }
                os << "]";
                return os;
            }
        private:
            VarEntity* clone() const { return new Array(*this); }
            int size() { return value.size(); }
            int push(const var& item) {
                var* v = new var();
                *v = item;
                value.push_back(v);
            }
            const var& get(int index) const {
                assertValidIndex(index);
                return *(this->value[index]);
            }
            var& get(int index) {
                assertValidIndex(index);
                return *(this->value[index]);
            }
            void assertValidIndex(int index) const {
                if(index < 0 || value.size() <= index) {
                    throw new std::out_of_range("json::Array index out of range.");
                }
            }
        private:
            std::vector<var*> value;
    };

    class Object : public VarEntity {
        friend class var;
        private:
            Object() : VarEntity(var::TypeObject) { }
            Object(const Object& that) : VarEntity(var::TypeObject) {
                std::map<std::string, var*>::const_iterator entry = that.value.begin();
                for(; entry != that.value.end(); entry++) {
                    this->set(entry->first, *(entry->second->getEntity()));
                }
            }
            ~Object() {
                std::map<std::string, var*>::const_iterator entry = value.begin();
                for(; entry != value.end(); entry++) {
                    delete entry->second;
                }
            }
        public:
            std::ostream& writeJson(std::ostream& os) const {
                os << "{";
                int i = 0, n = value.size();
                std::map<std::string, var*>::const_iterator kv = value.begin();
                for(; kv != value.end(); kv++, i++) {
                    os << "\"" << escapeString(kv->first) << "\":";
                    os << *kv->second;
                    if(i < n - 1) {
                        os << ',';
                    }
                }
                os << "}";
                return os;
            }
        private:
            VarEntity* clone() const { return new Object(*this); }
            bool exists(const std::string& key) const {
                return getValuePtr(key) != 0;
            }
            int set(const std::string& key, const VarEntity& data) {
                std::map<std::string, var*>::iterator it = value.find(key);
                if(it != value.end()) {
                    delete it->second;
                    value.erase(it);
                }
                var* v = new var();
                *v = data;
                std::pair<std::string, var*> item(key, v);
                value.insert(item);
            }
            const var& get(const std::string& key) const {
                const var* p = getValuePtr(key);
                return (p) ? *p : var::Null;
            }
            var& get(const std::string& key) {
                var* p = getValuePtr(key);
                return (p) ? *p : var::Null;
            }
            const var* getValuePtr(const std::string& key) const {
                ((Object*)this)->getValuePtr(key);
            }
            var* getValuePtr(const std::string& key) {
                std::map<std::string, var*>::iterator it = value.find(key);
                return (it == value.end()) ? 0 : it->second;
            }
        private:
            std::map<std::string, var*> value;
    };

    //
    // Compare templates with var
    //

    template<class T> bool operator ==(const var& data1, T data2)
        { return (double)data1 == (double)data2; }
    template<class T> bool operator ==(double data2, const var& data1)
        { return (double)data1 == (double)data2; }

    template<class T> bool operator !=(const var& data1, double data2)
        { return (double)data1 != (double)data2; }
    template<class T> bool operator !=(double data2, const var& data1)
        { return (double)data1 != (double)data2; }

    template<class T> bool operator <(const var& data1, T data2)
        { return (double)data1 < (double)data2; }
    template<class T> bool operator <(double data2, const var& data1)
        { return (double)data1 >= (double)data2; }

    template<class T> bool operator <=(const var& data1, T data2)
        { return (double)data1 <= (double)data2; }
    template<class T> bool operator <=(double data2, const var& data1)
        { return (double)data1 > (double)data2; }

    template<class T> bool operator >(const var& data1, T data2)
        { return (double)data1 > (double)data2; }
    template<class T> bool operator >(double data2, const var& data1)
        { return (double)data1 <= (double)data2; }

    template<class T> bool operator >=(const var& data1, T data2)
        { return (double)data1 >= (double)data2; }
    template<class T> bool operator >=(double data2, const var& data1)
        { return (double)data1 < (double)data2; }

    //
    // Compare between var and string
    //

    inline bool operator ==(const var& data1, const std::string& data2)
        { return (const std::string&)data1 == data2; }
    inline bool operator ==(const std::string& data2, const var& data1)
        { return (const std::string&)data1 == data2; }

    inline bool operator !=(const var& data1, const std::string& data2)
        { return (const std::string&)data1 != data2; }
    inline bool operator !=(const std::string& data2, const var& data1)
        { return (const std::string&)data1 != data2; }

    inline bool operator ==(const var& data1, const char* data2)
        { return (const std::string&)data1 == std::string(data2); }
    inline bool operator ==(const char* data2, const var& data1)
        { return (const std::string&)data1 == std::string(data2); }

    inline bool operator !=(const var& data1, const char* data2)
        { return (const std::string&)data1 != std::string(data2); }
    inline bool operator !=(const char* data2, const var& data1)
        { return (const std::string&)data1 != std::string(data2); }
}

#endif
