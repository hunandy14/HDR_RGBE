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









