#include <iostream>
#include <string>
#include <sstream>
#include "../jsonXX.h"

using namespace std;

int count_test = 0;
int count_pass = 0;
int count_fail = 0;
void report() {
    cerr << "----" << endl <<
        "test:" << count_test <<
        ", pass:" << count_pass <<
        ", fail:" << count_fail <<
        ", pass-ratio:" << 100.0 * count_pass / count_test << "%" << endl; 
}
bool assert_true(bool b, const char* test_name) {
    count_test++;
    cerr << "test " << count_test << ". " << test_name << " --- ";
    if(b) {
        count_pass++;
        cerr << "pass";
    } else {
        count_fail++;
        cerr << "fail";
    }
    cerr << endl;
    return b;
}

#define ASSERT_EQ(A,B) {\
        if(!assert_true(((A) == (B)), #A " == " #B)) {\
            std::cerr << "    LValue: " << (A) << std::endl;\
            std::cerr << "    RValue: " << (B) << std::endl;\
        }\
    }
#define ASSERT_NE(A,B) {\
        if(!assert_true(((A) != (B)), #A " != " #B)) {\
            std::cerr << "    LValue: " << (A) << std::endl;\
            std::cerr << "    RValue: " << (B) << std::endl;\
        }\
    }

int main(int argc, char* argv[]) {

    jsonXX::Value value;
    value.setString("A");
    ASSERT_EQ(value,  string("A"));

    value.setString("B");
    ASSERT_EQ(value,  string("B"));

    value.setNumber(1.0);
    ASSERT_EQ(value,  1.0);
    value.setNumber(123456.0);
    ASSERT_EQ(value,  123456.0);

    jsonXX::Array array;

    ASSERT_EQ(array.size(), 0);
    array.push(jsonXX::Value(1.0));
    array.push(jsonXX::Value("2.0"));
    array.push(jsonXX::Value(3.0));
    array.push(jsonXX::Value("4.0"));
    ASSERT_EQ(array.size(), 4);
    ASSERT_EQ(array[0], 1.0);
    ASSERT_EQ(array[1], string("2.0"));
    ASSERT_EQ(array[2], 3.0);
    ASSERT_EQ(array[3], string("4.0"));

    jsonXX::Object object;
    object.set("A", jsonXX::Value(1.0));
    object.set("B", jsonXX::Value("2.0"));
    object.set("C", array);
    object.set("D", object);
    ASSERT_EQ(object["A"], 1.0);
    ASSERT_EQ(object["B"], string("2.0"));
    ASSERT_EQ(object["C"][2], 3.0);
    ASSERT_EQ(object["D"]["C"][3], string("4.0"));
    
    // test toJson for Value class
    {
        //positive number
        jsonXX::Value value;
        stringstream ss;
        double f;

        value.setNumber(12345.0);
        value.writeJson(ss);
        ss >> f;
        ASSERT_EQ(f, 12345.0);
    }
    {
        //negative number
        jsonXX::Value value;
        stringstream ss;
        double f;

        value.setNumber(-1.2345);
        ss.str("");
        value.writeJson(ss);
        ss >> f;
        ASSERT_EQ(f, -1.2345);
    }
    {
        // ascii string
        jsonXX::Value value;
        stringstream ss;
        value.setString("ABC-0123-xyz");
        value.writeJson(ss);
        ASSERT_EQ(ss.str(), "\"ABC-0123-xyz\"");
    }
    {
        // escape chars
        jsonXX::Value value;
        stringstream ss;
        value.setString("\"A\nB\rC\t");
        value.writeJson(ss);
        ASSERT_EQ(ss.str(), "\"\\\"A\\nB\\rC\\t\"");
    }
    {
        // utf-8 string
        jsonXX::Value value;
        stringstream ss;
        value.setString("自営損");
        value.writeJson(ss);
        ASSERT_EQ(ss.str(), "\"自営損\"");
    }
    {
        // array
        jsonXX::Array array;
        for(int i = 0; i < 10; i++) {
            array.push(jsonXX::Value(i + i / 10.0));
        }
        stringstream ss;
        array.writeJson(ss);
        ASSERT_EQ(ss.str(), "[0,1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9]");
    }
    {
        // array
        jsonXX::Array array;
        for(int i = 0; i < 3; i++) {
            array.push(jsonXX::Value(i + i / 10.0));
            array.push(jsonXX::Value("and"));
        }
        stringstream ss;
        array.writeJson(ss);
        ASSERT_EQ(ss.str(), "[0,\"and\",1.1,\"and\",2.2,\"and\"]");
    }
    {
        // object
        jsonXX::Object object;
        object.set("number",jsonXX::Value(99.9));
        object.set("string",jsonXX::Value("hi!"));

        jsonXX::Array array;
        array.push(jsonXX::Value("string is here."));

        object.set("array",array);

        jsonXX::Object subobj;
        subobj.set("subobj",jsonXX::Value(-2.2));
        object.set("object",subobj);

        stringstream ss;
        object.writeJson(ss);
        ASSERT_NE(strstr(ss.str().c_str(),"\"number\":99.9"), 0); 
        ASSERT_NE(strstr(ss.str().c_str(),"\"string\":\"hi!\""), 0); 
        ASSERT_NE(strstr(ss.str().c_str(),"\"array\":[\"string is here.\"]"), 0); 
        ASSERT_NE(strstr(ss.str().c_str(),"\"object\":{\"subobj\":-2.2}"), 0); 
    }


    report();
    return (count_fail > 0) ? -1 : 0;
}
