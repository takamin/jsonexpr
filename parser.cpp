#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "jsonXX.h"
namespace json {
    using namespace std;
    /*!
     * ignore stream to the end of line.
     */
    static void skip_to_eol(istream& is)
    {
        int c;
        while((c = is.get()) != EOF) {
            if(c == '\n') {
                break;
            }
        }
    }
    /*!
     * ignore stream to the end of C style comment.
     */
    static void skip_comment(istream& is)
    {
        int c;
        int prev = 0;
        while((c = is.get()) != EOF) {
            if(prev == '*' && c == '/') {
                break;
            }
            prev = c;
        }
    }
    /*!
     * skip whitespaces and comments in stream
     */
    static void skip_whitespace(istream& is)
    {
        int c;
        while((c = is.get()) != EOF) {
            if(c == '/') {
                switch(c = is.get()) {
                    case '/':
                        skip_to_eol(is);
                        break;
                    case '*':
                        skip_comment(is);
                        break;
                    default:
                        is.unget();
                        return;
                        break;
                }
            } else if(!isspace(c)) {
                is.unget();
                break;
            }
        }
    }
    void var::parse(istream& is)
    {
        int c;
        skip_whitespace(is);
        switch(c = is.get()) {
            case '{':
                skip_whitespace(is);
                this->assign(new Object());
                this->parseObject(is);
                skip_whitespace(is);
                break;
            case '[':
                skip_whitespace(is);
                this->assign(new Array());
                this->parseArray(is);
                skip_whitespace(is);
                break;
            case '\"':
            case '\'':
                is.unget();
                this->assign(new Value());
                this->parseString(is);
                skip_whitespace(is);
                break;
            default:
                is.unget();
                if(c == '_' || isalpha(c)) {
                    var id("");
                    id.parseIdentifier(is);
                    if(id.entity->getString() == "null") {
                        this->assign(new Value());
                    } else if(id.entity->getString() == "true") {
                        *this = true;
                    } else if(id.entity->getString() == "false") {
                        *this = false;
                    }
                } else {
                    this->assign(new Value());
                    this->parseNumber(is);
                }
                skip_whitespace(is);
                break;
        }
    }
    void var::parseObject(istream& is)
    {
        int c;
        bool flush = true;
        while((c = is.get()) != EOF) {
            switch(c) {
                case '}':
                    return;
                    break;
                case ',':
                    if(flush) {
                        throw invalid_argument("key-value empty.");
                    }
                    flush = true;
                    skip_whitespace(is);
                    break;
                default:
                    if(flush) {
                        var key, value;
                        // read key
                        if(c == '\"' || c == '\'') {
                            is.unget();
                            key.parseString(is);
                        } else {
                            is.unget();
                            key.parseIdentifier(is);
                        }
                        skip_whitespace(is);

                        // read delimitor
                        if((c = is.get()) != ':') {
                            stringstream ss;
                            ss << "key-value delimitor ':' not found.";
                            if(isprint(c)) {
                                ss << " but invalid char " << (char)c << " is there.";
                            }
                            throw invalid_argument(ss.str());
                        }

                        //read value
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
    void var::parseArray(istream& is)
    {
        int c;
        var value;
        bool flush = true;
        while((c = is.get()) != EOF) {
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
                    skip_whitespace(is);
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
                break;
            }
        }
        this->assign(new Value());
        this->entity->setString(ss.str());
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
            } else if('0' <= c && c <= '7') {
                ss << '0';
                radix = 8;
                is.unget();
            } else {
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

