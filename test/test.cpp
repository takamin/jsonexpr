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

void sample() {
    // json 文字列からの構築
    json::var num("-1.2345e+3");
    json::var str("'this is string.'");
    json::var arr("[ \"key\", 'str', 'hex', 0xABCD, 0777 ]");
    json::var obj("{ foo : 'bar', 'boo':1.2345e-6, 'arr': [0,1,2,3]}");

    //
    // 参照
    //
    double numval = num;
    string strval = str;
    cout << numval << endl;
    cout << strval << endl;

    numval = arr[3];
    cout << numval << endl;

    strval = (string)obj["foo"];
    cout << strval << endl;

    strval = (string)obj["arr"][2];
    cout << strval << endl;

    //
    // 配列の扱い
    //
    cout << "arr.length() = " << arr.length() << endl;
    arr.push(9.876);
    arr.push("string element");
    arr.push(obj);
    arr.push(arr);
    cout << "arr.length() = " << arr.length() << endl;

    //
    // オブジェクトへ新たなキーを追加
    //
    cout << "obj.exists('new key') = " << (obj.exists("new key")?"true":"false") << endl;
    obj["new key"] = json::var("[0,1,2,3]");
    cout << "obj.exists('new key') = " << (obj.exists("new key")?"true":"false") << endl;

    //
    // std::ostreamへのJSONの書き出し
    //
    ostringstream os;
    os << obj;

    //
    // std::istreamからJSONを読み出し
    //
    istringstream is("{ foo : 'bar', 'boo':1.2345e-6, 'arr': [9,8,7,6] }");
    is >> obj;

    //
    // 別の値(別の型に)に書き換え
    //
    num = 4.0;
    cout << num << endl;

    num = "overwrite string";
    cout << num << endl;

    num = obj;
    cout << num["boo"] << endl;

    num["boo"] = "change by reference";
    cout << num["boo"] << endl;

    num["arr"][1] = "change by reference";
    cout << num["arr"][1] << endl;
}
void test() {
    sample();

    json::var value;
    value = "A";
    ASSERT_EQ(value,  string("A"));

    value = "B";
    ASSERT_EQ(value,  string("B"));

    value = 1.0;
    ASSERT_EQ(value,  1.0);
    value = 123456.0;
    ASSERT_EQ(value,  123456.0);

    json::var array("[]");
    //json::Array array;

    ASSERT_EQ(array.length(), 0);
    array.push(1.0);
    array.push("2.0");
    array.push(3.0);
    array.push("4.0");
    ASSERT_EQ(array.length(), 4);
    ASSERT_EQ(array[0], 1.0);
    ASSERT_EQ(array[1], string("2.0"));
    ASSERT_EQ(array[2], 3.0);
    ASSERT_EQ(array[3], string("4.0"));

    json::var object("{}");
    object["A"] = 1.0;
    object["B"] = "2.0";
    object["C"] = array;
    object["D"] = object;
    ASSERT_EQ(object["A"], 1.0);
    ASSERT_EQ(object["B"], string("2.0"));
    ASSERT_EQ(object["C"][2], 3.0);
    ASSERT_EQ(object["D"]["C"][3], string("4.0"));
    
    // test out stream json
    {
        //positive number
        json::var value;
        stringstream ss;
        double f;

        value = 12345.0;
        ss << value;
        ss >> f;
        ASSERT_EQ(f, 12345.0);
    }
    {
        //negative number
        json::var value;
        stringstream ss;
        double f;

        value = -1.2345;
        ss << value;
        ss >> f;
        ASSERT_EQ(f, -1.2345);
    }
    {
        // ascii string
        json::var value;
        stringstream ss;
        value = "ABC-0123-xyz";
        ss << value;
        ASSERT_EQ(ss.str(), "\"ABC-0123-xyz\"");
    }
    {
        // escape chars
        json::var value;
        stringstream ss;
        value = "\"A\nB\rC\t";
        ss << value;
        ASSERT_EQ(ss.str(), "\"\\\"A\\nB\\rC\\t\"");
    }
    {
        // utf-8 string
        json::var value;
        stringstream ss;
        value = "自営損";
        ss << value;
        ASSERT_EQ(ss.str(), "\"自営損\"");
    }
    {
        // array
        json::var array("[]");
        for(int i = 0; i < 10; i++) {
            array.push(i + i / 10.0);
        }
        stringstream ss;
        ss << array;
        ASSERT_EQ(ss.str(), "[0,1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9]");
    }
    {
        // array
        json::var array("[]");
        for(int i = 0; i < 3; i++) {
            array.push(i + i / 10.0);
            array.push("and");
        }
        stringstream ss;
        ss << array;
        ASSERT_EQ(ss.str(), "[0,\"and\",1.1,\"and\",2.2,\"and\"]");
    }
    {
        // object
        json::var object("{}");
        object["number"] = 99.9;
        object["string"] = "hi!";

        json::var array("[]");
        array.push("string is here.");

        object["array"] = array;

        json::var subobj("{}");
        subobj["subobj"] = -2.2;
        object["object"] = subobj;

        stringstream ss;
        ss << object;
        ASSERT_NE(strstr(ss.str().c_str(),"\"number\":99.9"), 0); 
        ASSERT_NE(strstr(ss.str().c_str(),"\"string\":\"hi!\""), 0); 
        ASSERT_NE(strstr(ss.str().c_str(),"\"array\":[\"string is here.\"]"), 0); 
        ASSERT_NE(strstr(ss.str().c_str(),"\"object\":{\"subobj\":-2.2}"), 0); 
    }

    {
        //change the element by assignment operator
        json::var obj2("[]");

        obj2.push(1.0);
        obj2[0] = "ABC";
        ASSERT_EQ(obj2[0], "ABC");

        obj2.push("AAA");
        obj2[1] = 2.3;
        ASSERT_EQ(obj2[1], 2.3);
    }
    {
        //change the associated value
        json::var obj2;
        obj2["A"] = 3.0;
        obj2["A"] = 1.0;
        ASSERT_EQ(obj2["A"], 1.0);

        obj2["B"] = "4.0";
        obj2["B"] = 2.0;
        ASSERT_EQ(obj2["B"], 2.0);
    }
    {
        json::var array("[]");
        array.push(1.0);
        array.push("2.0");
        array.push(3.0);
        array.push("4.0");

        json::var object("{}");
        object["C"] = array;

        json::var obj2;
        obj2["D"] = array;
        obj2["D"] = object;
        ASSERT_EQ(obj2["D"]["C"][3], string("4.0"));
    }

    {
        // var::parse string
        json::var obj;
        istringstream ss("\"json\"");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("json"));
    }
    {
        // var::parse string
        json::var obj;
        istringstream ss("'json'");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("json"));
    }
    {
        // var::parse string
        json::var obj;
        istringstream ss("\"\\\"json\\\"\"");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("\"json\""));
    }
    {
        // var::parse string
        json::var obj;
        istringstream ss("\"'json'\"");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("'json'"));
    }
    {
        // var::parse string
        json::var obj;
        istringstream ss("'\"json\"'");
        obj.parse(ss);
        ASSERT_EQ((const string&)obj, string("\"json\""));
    }
    {
        // var::parse number
        json::var obj;
        istringstream ss("1");
        obj.parse(ss);
        ASSERT_EQ(obj, 1.0);
    }
    {
        // var::parse number
        json::var obj;
        istringstream ss("1.12345");
        obj.parse(ss);
        ASSERT_EQ(obj, 1.12345);
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss(".0");
        obj.parse(ss);
        ASSERT_EQ(obj, 0.0);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss("0.");
        obj.parse(ss);
        ASSERT_EQ(obj, 0.0);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss("+1.2E-6");
        obj.parse(ss);
        ASSERT_EQ(obj, 1.2E-6);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss("-1.2e6");
        obj.parse(ss);
        ASSERT_EQ(obj, -1.2e6);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss("-1.2E+6");
        obj.parse(ss);
        ASSERT_EQ(obj, -1.2E+6);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss("0xef01");
        obj.parse(ss);
        ASSERT_EQ(obj, (double)0xef01);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss("-0x89AB");
        obj.parse(ss);
        ASSERT_EQ(obj, (double)-0x89AB);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss("0123");
        obj.parse(ss);
        ASSERT_EQ(obj, (double)0123);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss("-0670");
        obj.parse(ss);
        ASSERT_EQ(obj, (double)-0670);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse array
        json::var obj;
        istringstream ss("[0,1,2,3]");
        obj.parse(ss);
        ASSERT_EQ(obj[0], 0.0);
        ASSERT_EQ(obj[1], 1.0);
        ASSERT_EQ(obj[2], 2.0);
        ASSERT_EQ(obj[3], 3.0);
        ASSERT_EQ(obj.length(), 4);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse object
        json::var obj;
        istringstream ss("{key:'value'}");
        obj.parse(ss);
        ASSERT_EQ(obj["key"], string("value"));
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse object
        json::var obj;
        istringstream ss("{\"key\":'value'}");
        obj.parse(ss);
        ASSERT_EQ(obj["key"], string("value"));
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse object
        json::var obj;
        istringstream ss("{'key':'value'}");
        obj.parse(ss);
        ASSERT_EQ(obj["key"], string("value"));
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse array
        json::var obj;
        istringstream ss("[0,'No.1',-2e2,{'last':'element',},]");
        obj.parse(ss);
        ASSERT_EQ(obj.length(), 4);
        ASSERT_EQ(obj[0], 0.0);
        ASSERT_EQ(obj[1], "No.1");
        ASSERT_EQ(obj[2], -2e2);
        ASSERT_EQ(obj[3]["last"], string("element"));
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse number
        json::var obj;
        istringstream ss(" { \"array\" : [0,1,2] , 'number key' : 1.23  } ");
        obj.parse(ss);
        ASSERT_EQ(obj["array"][0], 0.0);
        ASSERT_EQ(obj["array"][1], 1.0);
        ASSERT_EQ(obj["array"][2], 2.0);
        ASSERT_EQ(obj["number key"], 1.23);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse array
        json::var obj;
        istringstream ss("[0,'No.1',-2e2,{'last':'element',},]");
        ss >> obj;

        stringstream ss2;
        ss2 << obj;

        json::var obj2;
        ss2 >> obj2;
        ASSERT_EQ(obj2.length(), 4);
        ASSERT_EQ(obj2[0], 0.0);
        ASSERT_EQ(obj2[1], "No.1");
        ASSERT_EQ(obj2[2], -2e2);
        ASSERT_EQ(obj2[3]["last"], string("element"));
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        // var::parse object
        json::var obj;
        istringstream ss(" { \"array\" : [0,1,2] , 'number key' : 1.23  } ");
        ss >> obj;

        stringstream ss2;
        ss2 << obj;

        json::var obj2;
        ss2 >> obj2;
        ASSERT_EQ(obj2["array"][0], 0.0);
        ASSERT_EQ(obj2["array"][1], 1.0);
        ASSERT_EQ(obj2["array"][2], 2.0);
        ASSERT_EQ(obj2["number key"], 1.23);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    {
        json::var var;
        json::var object2("{}");
        var = object;

        json::var empty;
        object2["A"] = empty;
        var["A"] = object2;

        json::var& var2 = var["A"];
        ASSERT_EQ(var2.getType(), json::var::TypeObject);
    }
    {
        json::var var;
        var["a"] = 1.0;
        var["b"] = "AAA";
        ASSERT_EQ(var["a"], 1.0);
        ASSERT_EQ(var["b"], "AAA");
    }
    try {
        json::var array("[0,1,2,3]");
        ASSERT_EQ(array.getType(), json::var::TypeArray);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        json::var object("{a:0,b:'1',c:2,d:'3'}");
        ASSERT_EQ(object.getType(), json::var::TypeObject);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        json::var var;
        json::var array("[0,1,2,3]");
        var["c"] = array;
        ASSERT_EQ(var["c"][3], 3.0);
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    try {
        json::var var;
        json::var object("{a:0,b:'1',c:2,d:'3'}");
        var["d"] = object;
        ASSERT_EQ(var["d"]["d"], "3");
    } catch (exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
}
int main(int argc, char* argv[]) {
    try {
        test();
    } catch(exception& e) {
        cerr << "EXCEPTION CAUGHT: " << e.what() << endl;
    }
    report();
    return (count_fail > 0) ? -1 : 0;
}
