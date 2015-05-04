jsonXX
======

C++でJSONを扱うためのクラスライブラリです。

json::varクラスでjavascriptの基本データ型(数値、文字列、配列、オブジェクト)
を表現しており、JSONデータのストリーム入出力機能を提供します。

## データ型

1. 数値 - double型の数値
2. 文字列 - std::string型の文字列
3. 配列 - json::var型の配列
4. オブジェクト - 文字列からjson::var型へのディクショナリ

## 初期化

コンストラクタにJson形式の文字列を与えて初期化できます。

```
//数値の初期化(内部表現は全てdouble)
json::var real("-1.234e+5");  //浮動小数点
json::var dec("1234");        //10進整数
json::var hex("0x1234");      //16進整数
json::var oct("0644");        //8進整数

//文字列で初期化
json::var str("'string'");    //シングルクォート

//配列の初期化
json::var arr("[-1.234e+5,'string']");

//オブジェクトの初期化
json::var obj("{'key':'value', foo: 'bar', arr:[1,2,3,4]}");
```

## 参照

数値型は、doubleへのキャスト、文字列型は、std::string&へのキャストで値を参照できます。

配列の要素は、operator\[\]\(int\)、オブジェクトの値は、operator\[\]\(const std::string&\)によって参照します。

```
double v = (double)real;
std::string s = (const std::string&)str;
v = arr[3];             //配列要素の参照
v = obj["arr"][0];      //オブジェクト内の配列要素の参照
```

内部表現と矛盾する参照を行うと、例外が投入されます。


## 代入

```
data = 1.234;           //数値を代入
data = "string";        //文字列を代入
arr[3] = 1.234;         //配列要素の書き換え
obj["key"] = 1.234;     //オブジェクト要素の書き換え
obj["new key"] = arr;   //オブジェクトへ新たな項目を追加
```

## JSONデータのストリーム入出力

C++標準のストリームに対する入出力機能を提供しています。

### JSONの読み込み

```
json::var dataobj;
ifstream is("input.json");
is >> dataobj;
```

### JSONの出力

```
json::var dataobj;
ofstream is("output.json");
os << dataobj;
```

## 制限事項

javascriptの仕様に完全準拠しているわけではありません。

* nullを扱えません。
* NaNを扱えません。
* Functionを扱えません。
* 読み込むJSONにコメントが含まれていてはいけません。
* オブジェクトのキーは文字列だけです。
* 数値はdoubleだけです。

## サンプル

```
    // json 文字列からの構築
    json::var num("-1.2345e+3");
    json::var str("'this is string.'");
    json::var arr("[ \"key\", 'str', 'hex', 0xABCD, 0777 ]");
    json::var obj("{ foo : 'bar', 'boo':1.2345e-6, 'arr': [0,1,2,3]}");

    // 参照
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

    // 配列の扱い
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
    // 別の値(別の型)に書き換え
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
```
