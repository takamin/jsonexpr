#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "jsonXX.h"
namespace json {
    using namespace std;

    struct jsonlex {
        enum type { ws, ident, punct, digit, str};
        type id;
        std::stringstream word;
        /**/
        jsonlex(type id) : id(id) { }
        jsonlex& operator += (int c) { this->word << (char)c; }
        jsonlex(const jsonlex& that) : id(that.id), word(that.word.str()) {}
        jsonlex& operator=(const jsonlex& that) { id = that.id; word.str(that.word.str()); } 
        operator std::string() { return this->word.str(); }
    };
    typedef std::vector<jsonlex> lexlist;
    void tokenize_whitespace(std::istream& is, lexlist& lexseq) {
        jsonlex tok(jsonlex::ws);
        int c;
        while((c = is.get()) != EOF) {
            if(isspace(c)) {
                tok += c;
            } else {
                is.unget();
                break;
            }
        }
        lexseq.push_back(tok);
    }
    void tokenize_digit(std::istream& is, lexlist& lexseq) {
        jsonlex tok(jsonlex::digit);
        int c;
        while((c = is.get()) != EOF) {
            if(isdigit(c)) {
                tok += c;
            } else {
                is.unget();
                break;
            }
        }
        lexseq.push_back(tok);
    }
    void tokenize_ident(std::istream& is, lexlist& lexseq) {
        jsonlex tok(jsonlex::ident);
        int c;
        while((c = is.get()) != EOF) {
            if(isalnum(c) || c == '_' || c == '$') {
                tok += c;
            } else {
                is.unget();
                break;
            }
        }
        lexseq.push_back(tok);
    }
    bool ishex(int c) {
        return isdigit(c) || isalpha(c) && toupper(c) <= 'F';
    }
    bool isoct(int c) {
        return isdigit(c) && c < '8';
    }
    bool read_hex_char_spec(std::istream& is, std::string* hex)
    {
        int c;
        int col = 0;
        stringstream ss;
        while((c = is.get()) != EOF) {
            if(ishex(c)) {
                ss << c;
                col++;
            } else {
                is.unget();
                break;
            }
        }
        *hex = ss.str();
        return (col > 0);
    }
    bool read_oct_char_spec(std::istream& is, std::string* oct)
    {
        int c;
        int col = 0;
        stringstream ss;
        while((c = is.get()) != EOF) {
            if(isoct(c)) {
                ss << (char)c;
                col++;
                if(col >= 3) {
                    break;
                }
            } else {
                is.unget();
                break;
            }
        }
        *oct = ss.str();
        return (col > 0);
    }
    void tokenize_string(std::istream& is, lexlist& lexseq) {
        bool terminate = false;
        int start_with = is.get();
        jsonlex tok(jsonlex::str);
        int c;
        while(!terminate && (c = is.get()) != EOF) {
            switch(c) {
                case '"':
                case '\'':
                    if(c == start_with) {
                        terminate = true;
                    }
                    break;
                case '\\':
                    switch(c = is.get()) {
                        case EOF:
                            terminate = true;
                            break;
                        case 'n': tok += '\n'; break;
                        case 'r': tok += '\r'; break;
                        case 't': tok += '\t'; break;
                        case 'x':
                            {
                                string hexcharspec;
                                if(read_hex_char_spec(is, &hexcharspec)) {
                                    char* endptr;
                                    long value = strtol(hexcharspec.c_str(), &endptr, 16);
                                    tok += (char)value;
                                } else {
                                    tok += 'x';
                                }
                            }
                            break;
                        default:
                            if(isoct(c)) {
                                is.unget();
                                string octcharspec;
                                if(read_oct_char_spec(is, &octcharspec)) {
                                    char* endptr;
                                    long value = strtol(octcharspec.c_str(), &endptr, 16);
                                    tok += (char)value;
                                }
                            } else {
                                tok += (char)c;
                            }
                            break;
                    }
                    break;
                default:
                    tok += c;
                    break;
            }
        }
        if(terminate) {
            lexseq.push_back(tok);
        }
    }
    void tokenize(std::istream& is, lexlist& lexseq) {
        int c;
        std::stringstream s;
        while((c = is.get()) != EOF) {
            if(isspace(c)) {
                is.unget();
                tokenize_whitespace(is, lexseq);
            } else if(isdigit(c)) {
                is.unget();
                tokenize_digit(is, lexseq);
            } else if(isalpha(c) || c == '_' || c == '$') {
                is.unget();
                tokenize_ident(is, lexseq);
            } else if(ispunct(c)) {
                switch(c) {
                    case '"':
                    case '\'':
                        is.unget();
                        tokenize_string(is, lexseq);
                        break;
                    case '/':
                        switch(c = is.get()) {
                            case '*':
                                break;
                            case '/':
                                break;
                            default:
                                {
                                    jsonlex tok(jsonlex::punct);
                                    tok += '/';
                                    lexseq.push_back(tok);
                                }
                                is.unget();
                                break;
                        }
                        break;
                    default:
                        {
                            jsonlex tok(jsonlex::punct);
                            tok += c;
                            lexseq.push_back(tok);
                        }
                        break;
                }
            }
        }
    }
}
