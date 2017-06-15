## 2017/06/08
乾，一開始沒做好直接用include去編譯搞得現在不知道哪裡缺什麼找不出來。
> 結果只是某個檔案多引用了

inline 貌似只能放在頭文件不能拆開到其他檔案去


## 2017/06/08
把創建煩惱交給編譯器處理

考慮一個fun()功能假設為 A in B out 而且他們的算法不能合併(一定要建第二個)
有兩種方式可行

```cpp
void fun(string& a, string& b);

string a="data";
string b(10);

fun(a, b);
```

這麼做有幾個壞處
1. 必須檢查B的有效性
2. 客戶要煩惱該創什麼型別

優點就是假如 b 是更早之前就已經存在的暫存就不用重建物件
或者說這個函式使用不只一次，這可以不用付出建構解構成本

```cpp
string fun(string& a){
    string b;
}

string a="data";
auto b = fun(a);
```

由於編譯器優化，這樣的宣告並，函式結束時轉出不會多一組無謂的複製行為。
這麼做既可以不管型別，又可以不檢查有效性。




```cpp
#include <iostream>
using namespace std;

class temp{
public:
    temp(){}
    ~temp(){cout << "dctor" << endl;}
    int num;
};
temp fun(){
    temp a;
    a.num=10;
    return a;
}
int main() {
    auto a = fun();
    return 0;
}
```

## 2017/06/07
WTF 那個 Imax 不是最大值

### 怎麼拆公式
> 學到的一點，一個公式怎麼打的基本技巧，至少要把不會動的拆到for迴圈外面先算好，不要浪費效能

### 什麼時候必須在 auto 後方指名推斷
剛剛用到一個 static 的函式發現如果不在後方推薦時態不給過，可能是因為static不能變吧
auto多少還有很微小的半可變性，有可能根據不同的main推斷改變。

```cpp
auto class_t::Mapping(vector<float> pix, float dmax, float b) -> decltype(pix){
    
}
```

## 2017/05/23
### 找不到函式
- WTF VC 居然不支持.c 檔案，找半天找不到錯
- 然後檔名也不能一樣(A.cpp a.cpp 會相衝)

### 例外
可以拋出就不管了，外面的人自己接不接就強制結束

常見例外：
- http://www.cplusplus.com/reference/exception/
- http://www.cplusplus.com/reference/stdexcept/
- http://www.cplusplus.com/reference/new/bad_alloc/

也可以拋出上面的例外，例如

```cpp
if(rgb>2) {throw range_error("range only 0~2");}
```

拋出去之後沒人接就強關，然後會顯示上面的信息。

小心使用 `exit(0);` 當厲害這不是很少，用gcc會彈出暫停(輸入的附帶命令控制的)
用VC因為沒有這個不會彈出什麼信息，直接就關了，不知道的人會找不到









