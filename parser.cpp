#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "jsonXX.h"
namespace json {
    using namespace std;
    void var::parse(istream& is)
    {
        int c;
        while((c = is.get()) != EOF) {
            if(!isspace(c)) {
                break;
            }
        }
        switch(c) {
            case '{':
                this->assign(new Object());
                this->parseObject(is);
                break;
            case '[':
                this->assign(new Array());
                this->parseArray(is);
                break;
            case '\"':
            case '\'':
                is.unget();
                this->assign(new Value());
                this->parseString(is);
                break;
            default:
                is.unget();
                this->assign(new Value());
                this->parseNumber(is);
                break;
        }
    }
    void var::parseObject(istream& is)
    {
        int c;
        bool flush = true;
        while((c = is.get()) != EOF) {
            if(!isspace(c)) {
                switch(c) {
                    case '}':
                        return;
                        break;
                    case ',':
                        if(flush) {
                            throw invalid_argument("key-value empty.");
                        }
                        flush = true;
                        break;
                    default:
                        if(flush) {
                            var key, value;
                            is.unget();
                            key.parseObjectKey(is);
                            value.parse(is);
                            this->entity->set(key.entity->getString(), value);
                            flush = false;
                        } else {
                            throw invalid_argument("key-value value not end by comma.");
                        }
                        break;
                }
            }
        }
    }
    void var::parseObjectKey(istream& is)
    {
        int c;
        while((c = is.get()) != EOF) {
            if(!isspace(c)) {
                switch(c) {
                    case ':':
                        return;
                        break;
                    case '\"':
                    case '\'':
                        is.unget();
                        this->parseString(is);
                        break;
                    default:
                        is.unget();
                        this->parseIdentifier(is);
                        break;
                }
            }
        }
    }
    void var::parseArray(istream& is)
    {
        int c;
        var value;
        bool flush = true;
        while((c = is.get()) != EOF) {
            if(!isspace(c)) {
                switch(c) {
                    case ']':
                        return;
                        break;
                    case ',':
                        if(flush) {
                            // add null element
                            var null;
                            this->entity->push(null);
                        }
                        flush = true;
                        break;
                    default:
                        if(flush) {
                            flush = false;
                            is.unget();
                            value.parse(is);
                            this->entity->push(value);
                        } else {
                            throw invalid_argument("array element not end by comma.");
                        }
                        break;
                }
            }
        }
    }
    void var::parseIdentifier(istream& is)
    {
        int c;
        bool double_quote = false;
        stringstream ss;
        while((c = is.get()) != EOF) {
            if(c == '_' || isalpha(c) || isdigit(c)) {
                ss << (char)c;
            } else {
                is.unget();
                this->assign(new Value());
                this->entity->setString(ss.str());
                break;
            }
        }
    }
    void var::parseString(istream& is)
    {
        int c;
        bool double_quote = false;
        stringstream ss;
        switch(c = is.get()) {
            case '\"': double_quote = true; break;
            case '\'': double_quote = false; break;
            default:
               { throw invalid_argument("FATAL: parseString internal logic."); }
               break;
        }
        while((c = is.get()) != EOF) {
            switch(c) {
                case '\"':
                    if(double_quote) {
                        this->assign(new Value());
                        this->entity->setString(ss.str());
                        return;
                    } else {
                        ss << (char)c;
                    }
                    break;
                case '\'':
                    if(!double_quote) {
                        this->assign(new Value());
                        this->entity->setString(ss.str());
                        return;
                    } else {
                        ss << (char)c;
                    }
                    break;
                case '\\':
                    c = is.get();
                    switch(c) {
                        case 'n': ss << '\n'; break;
                        case 'r': ss << '\r'; break;
                        case 't': ss << '\t'; break;
                        case '\"': ss << '\"'; break;
                        default: ss << (char)c; break;
                    }
                    break;
                default:
                    ss << (char)c;
                    break;
            }
        }
    }
    void var::parseNumber(istream& is)
    {
        int radix = 10;
        stringstream ss;

        int c;
        c = is.get();
        if(c == '+' || c == '-') {
            ss << (char)c;
        } else {
            is.unget();
        }

        c = is.get();
        if(c == '0') {
            c = is.get();
            if(c == 'x' || c == 'X') {
                ss << '0';
                ss << "x";
                radix = 16;
            } else {
                ss << '0';
                radix = 8;
                is.unget();
            }
        } else {
            is.unget();
        }
        switch(radix) {
            case 10:
                while((c = is.get()) != EOF) {
                    if('0' <= c && c <= '9') {
                        ss << (char)c;
                    } else {
                        is.unget();
                        break;
                    }
                }
                c = is.get();
                if(c == '.') {
                    ss << (char)c;
                    while((c = is.get()) != EOF) {
                        if('0' <= c && c <= '9') {
                            ss << (char)c;
                        } else {
                            is.unget();
                            break;
                        }
                    }
                } else {
                    is.unget();
                }
                c = is.get();
                if(c == 'e' || c == 'E') {
                    ss << (char)c;

                    c = is.get();
                    if(c == '+' || c == '-') {
                        ss << (char)c;
                    } else {
                        is.unget();
                    }

                    while((c = is.get()) != EOF) {
                        if('0' <= c && c <= '9') {
                            ss << (char)c;
                        } else {
                            is.unget();
                            break;
                        }
                    }
                } else {
                    is.unget();
                }
                {
                    char* endptr;
                    double value = strtod(ss.str().c_str(), &endptr);
                    if(*endptr != 0) {
                        throw invalid_argument("FATAL: parseNumber decimal format error.");
                    }
                    this->entity->setNumber(value);
                }
                break;
            case 16:
                while((c = is.get()) != EOF) {
                    if('0' <= c && c <= '9' || 'a' <= c && c <= 'f' || 'A' <= c && c <= 'Z') {
                        ss << (char)c;
                    } else {
                        is.unget();
                        break;
                    }
                }
                {
                    char* endptr;
                    long value = strtol(ss.str().c_str(), &endptr, 0);
                    if(*endptr != 0) {
                        throw invalid_argument("FATAL: parseNumber hex format error.");
                    }
                    this->entity->setNumber((double)value);
                }
                break;
            case 8:
                while((c = is.get()) != EOF) {
                    if('0' <= c && c <= '7') {
                        ss << (char)c;
                    } else {
                        is.unget();
                        break;
                    }
                }
                {
                    char* endptr;
                    long value = strtol(ss.str().c_str(), &endptr, 0);
                    if(*endptr != 0) {
                        throw invalid_argument("FATAL: parseNumber octal format error.");
                    }
                    this->entity->setNumber((double)value);
                }
                break;
        }
    }
}

