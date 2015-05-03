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
            std::cerr << "    at " << __FILE__ << "(" << __LINE__ << "):" << endl;\
            std::cerr << "    LValue: " << (A) << std::endl;\
            std::cerr << "    RValue: " << (B) << std::endl;\
        }\
    }
#define ASSERT_NE(A,B) {\
        if(!assert_true(((A) != (B)), #A " != " #B)) {\
            std::cerr << "    at " << __FILE__ << "(" << __LINE__ << "):" << endl;\
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

    {
        //change the element by assignment operator
        jsonXX::Array obj2;

        obj2.push(jsonXX::Value(1.0));
        obj2[0] = "ABC";
        ASSERT_EQ(obj2[0], "ABC");

        obj2.push(jsonXX::Value("AAA"));
        obj2[1] = jsonXX::Value(2.3);
        ASSERT_EQ(obj2[1], 2.3);
    }
    {
        //change the associated value
        jsonXX::Object obj2;
        obj2.set("A", jsonXX::Value(3.0));
        obj2["A"] = jsonXX::Value(1.0);
        ASSERT_EQ(obj2["A"], 1.0);

        obj2.set("B", jsonXX::Value("4.0"));
        obj2["B"] = 2.0;
        ASSERT_EQ(obj2["B"], 2.0);
    }
    {
        jsonXX::Array array;
        array.push(jsonXX::Value(1.0));
        array.push(jsonXX::Value("2.0"));
        array.push(jsonXX::Value(3.0));
        array.push(jsonXX::Value("4.0"));

        jsonXX::Object object;
        object.set("C", array);

        jsonXX::Object obj2;
        obj2.set("D", array);
        obj2["D"] = object;
        ASSERT_EQ(obj2["D"]["C"][3], string("4.0"));
    }

    {
        // Var::parse string
        jsonXX::Var obj;
        istringstream ss("\"json\"");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("json"));
    }
    {
        // Var::parse string
        jsonXX::Var obj;
        istringstream ss("'json'");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("json"));
    }
    {
        // Var::parse string
        jsonXX::Var obj;
        istringstream ss("\"\\\"json\\\"\"");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("\"json\""));
    }
    {
        // Var::parse string
        jsonXX::Var obj;
        istringstream ss("\"'json'\"");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("'json'"));
    }
    {
        // Var::parse string
        jsonXX::Var obj;
        istringstream ss("'\"json\"'");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("\"json\""));
    }
    {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("1");
        obj.parse(ss);
        ASSERT_EQ(obj, 1.0);
    }
    {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("1.12345");
        obj.parse(ss);
        ASSERT_EQ(obj, 1.12345);
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss(".0");
        obj.parse(ss);
        ASSERT_EQ(obj, 0.0);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("0.");
        obj.parse(ss);
        ASSERT_EQ(obj, 0.0);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("+1.2E-6");
        obj.parse(ss);
        ASSERT_EQ(obj, 1.2E-6);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("-1.2e6");
        obj.parse(ss);
        ASSERT_EQ(obj, -1.2e6);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("-1.2E+6");
        obj.parse(ss);
        ASSERT_EQ(obj, -1.2E+6);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("0xef01");
        obj.parse(ss);
        ASSERT_EQ(obj, (double)0xef01);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("-0x89AB");
        obj.parse(ss);
        ASSERT_EQ(obj, (double)-0x89AB);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("0123");
        obj.parse(ss);
        ASSERT_EQ(obj, (double)0123);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss("-0670");
        obj.parse(ss);
        ASSERT_EQ(obj, (double)-0670);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse array
        jsonXX::Var obj;
        istringstream ss("[0,1,2,3]");
        obj.parse(ss);
        ASSERT_EQ(obj[0], 0.0);
        ASSERT_EQ(obj[1], 1.0);
        ASSERT_EQ(obj[2], 2.0);
        ASSERT_EQ(obj[3], 3.0);
        ASSERT_EQ(obj.getEntity()->size(), 4);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse object
        jsonXX::Var obj;
        istringstream ss("{key:'value'}");
        obj.parse(ss);
        ASSERT_EQ(obj["key"], string("value"));
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse object
        jsonXX::Var obj;
        istringstream ss("{\"key\":'value'}");
        obj.parse(ss);
        ASSERT_EQ(obj["key"], string("value"));
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse object
        jsonXX::Var obj;
        istringstream ss("{'key':'value'}");
        obj.parse(ss);
        ASSERT_EQ(obj["key"], string("value"));
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse array
        jsonXX::Var obj;
        istringstream ss("[0,'No.1',-2e2,{'last':'element',},]");
        obj.parse(ss);
        ASSERT_EQ(obj.getEntity()->size(), 4);
        ASSERT_EQ(obj[0], 0.0);
        ASSERT_EQ(obj[1], "No.1");
        ASSERT_EQ(obj[2], -2e2);
        ASSERT_EQ(obj[3]["last"], string("element"));
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse number
        jsonXX::Var obj;
        istringstream ss(" { \"array\" : [0,1,2] , 'number key' : 1.23  } ");
        obj.parse(ss);
        ASSERT_EQ(obj["array"][0], 0.0);
        ASSERT_EQ(obj["array"][1], 1.0);
        ASSERT_EQ(obj["array"][2], 2.0);
        ASSERT_EQ(obj["number key"], 1.23);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse array
        jsonXX::Var obj;
        istringstream ss("[0,'No.1',-2e2,{'last':'element',},]");
        ss >> obj;

        stringstream ss2;
        ss2 << obj;

        jsonXX::Var obj2;
        ss2 >> obj2;
        ASSERT_EQ(obj2.getEntity()->size(), 4);
        ASSERT_EQ(obj2[0], 0.0);
        ASSERT_EQ(obj2[1], "No.1");
        ASSERT_EQ(obj2[2], -2e2);
        ASSERT_EQ(obj2[3]["last"], string("element"));
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    try {
        // Var::parse object
        jsonXX::Var obj;
        istringstream ss(" { \"array\" : [0,1,2] , 'number key' : 1.23  } ");
        ss >> obj;

        stringstream ss2;
        ss2 << obj;

        jsonXX::Var obj2;
        ss2 >> obj2;
        ASSERT_EQ(obj2["array"][0], 0.0);
        ASSERT_EQ(obj2["array"][1], 1.0);
        ASSERT_EQ(obj2["array"][2], 2.0);
        ASSERT_EQ(obj2["number key"], 1.23);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
    {
        jsonXX::Var var;
        jsonXX::Object object2;
        var.setEntity(&object);

        jsonXX::Object object;
        jsonXX::Value empty;
        object2.set(string("A"), empty);
        var.getEntity()->set("A", object2);

        jsonXX::Var& var2 = var.getEntity()->operator[]("A");
        ASSERT_EQ(var2.getEntity()->getType(), jsonXX::Data::TypeObject);
    }
    {
        jsonXX::Var var;
        var["a"] = 1.0;
        var["b"] = "AAA";
        ASSERT_EQ(var["a"], 1.0);
        ASSERT_EQ(var["b"], "AAA");
    }
    try {
        jsonXX::Var array("[0,1,2,3]");
        ASSERT_EQ(array.getEntity()->getType(), jsonXX::Data::TypeArray);
    } catch (exception& e) {
        cerr << e.what() << endl;
    }
    try {
        jsonXX::Var object("{a:0,b:'1',c:2,d:'3'}");
        ASSERT_EQ(object.getEntity()->getType(), jsonXX::Data::TypeObject);
    } catch (exception& e) {
        cerr << e.what() << endl;
    }
    try {
        jsonXX::Var var;
        jsonXX::Var array("[0,1,2,3]");
        var["c"] = array;
        ASSERT_EQ(var["c"][3], 3.0);
    } catch (exception& e) {
        cerr << e.what() << endl;
    }
    try {
        jsonXX::Var var;
        jsonXX::Var object("{a:0,b:'1',c:2,d:'3'}");
        var["d"] = object;
        ASSERT_EQ(var["d"]["d"], "3");
    } catch (exception& e) {
        cerr << e.what() << endl;
    }
    report();
    return (count_fail > 0) ? -1 : 0;
}
