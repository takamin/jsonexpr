#include <iostream>
#include <ctype.h>
#include "jsonXX.h"
namespace jsonXX {
    using namespace std;
    enum parser_state_t {
        idle,
        parse_object,
        parse_array,
        parse_string,
        parse_number,
    };
    void Var::parse(istream& is){
        this->parse(is, idle);
    }
    void Var::parse(istream& is, parser_state_t state)
    {
        int c;
        while((c = is.get()) != EOF) {
            if(state == idle) {
                if(!isspace(c)) {
                    switch(c) {
                        case '{':
                            this->parse(is, parse_object);
                            break;
                        case '[':
                            this->parse(is, parse_array);
                            break;
                        case '\"':
                            this->parse(is, parse_string);
                            break;
                        default:
                            is.unget();
                            this->parse(is, parse_number);
                            break;
                    }
                }
            } else if(state == parse_object) {
                if(!isspace(c)) {
                    switch(c) {
                        case '}':
                            return;
                            break;
                        case '{':
                            this->parse(is, parse_object);
                            break;
                        case '[':
                            this->parse(is, parse_array);
                            break;
                        case '\"':
                            this->parse(is, parse_string);
                            break;
                        default:
                            is.unget();
                            this->parse(is, parse_number);
                            break;
                    }
                }
            } else if(state == parse_array) {
                if(!isspace(c)) {
                    switch(c) {
                        case ']':
                            return;
                            break;
                        case '{':
                            this->parse(is, parse_object);
                            break;
                        case '[':
                            this->parse(is, parse_array);
                            break;
                        case '\"':
                            this->parse(is, parse_string);
                            break;
                        default:
                            is.unget();
                            this->parse(is, parse_number);
                            break;
                    }
                }
            } else if(state == parse_string) {
                switch(c) {
                    case '\"':
                        return;
                        break;
                    case '\\':
                        c = is.get();
                        break;
                }
            } else if(state == parse_number) {
            }
        }
    }
}

