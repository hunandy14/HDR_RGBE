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









