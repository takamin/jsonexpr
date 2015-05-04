jsonXX
======

これはC++でJSONを扱うためのクラスライブラリです。

This is a C++ class library to operate JSON.

## 機能概要

* javascriptの基本的なデータ表現(数値、文字列、配列、オブジェクト)
* JSON形式の文字列との相互変換

## 制限事項

javascriptの仕様に完全準拠しているわけではありません。

* nullを扱えません。
* NaNを扱えません。
* Functionを扱えません。
* 読み込むJSONにコメントが含まれていてはいけません。
* オブジェクトのキーは文字列だけです。
* 数値はdoubleだけです。

## 使い方サンプル

```
    // json 文字列からの構築
    jsonXX::Var num("-1.2345e+3");
    jsonXX::Var str("'this is string.'");
    jsonXX::Var arr("[ \"key\", 'str', 'hex', 0xABCD, 0777 ]");
    jsonXX::Var obj("{ foo : 'bar', 'boo':1.2345e-6, 'arr': [0,1,2,3]}");

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
    obj["new key"] = jsonXX::Var("[0,1,2,3]");
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

### 宣言



### JSONの読み込み

jsonを読み込むには、


namespace jsonXX

* class jsonXX::Data - abstract base class
* class jsonXX::Value : public jsonXX::Data - scalar data, number or string.(no Function)
* class jsonXX::Array : public jsonXX::Data - array of jsonXX::Data
* class jsonXX::Object : public jsonXX::Data - object containing jsonXX::Data mapped by string as key.


