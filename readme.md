HW03
===
This is the hw03 sample. Please follow the steps below.

# Build the Sample Program

1. Fork this repo to your own github account.

2. Clone the repo that you just forked.

3. Under the hw03 dir, use:

	* `make` to build.

	* `make clean` to clean the ouput files.

4. Extract `gnu-mcu-eclipse-qemu.zip` into hw03 dir. Under the path of hw03, start emulation with `make qemu`.

	See [Lecture 02 ─ Emulation with QEMU] for more details.

5. The sample is a minimal program for ARM Cortex-M4 devices, which enters `while(1);` after reset. Use gdb to get more details.

	See [ESEmbedded_HW02_Example] for knowing how to do the observation and how to use markdown for taking notes.

# Build Your Own Program

1. Edit main.c.

2. Make and run like the steps above.

3. Please avoid using hardware dependent C Standard library functions like `printf`, `malloc`, etc.

# HW03 Requirements

1. How do C functions pass and return parameters? Please describe the related standard used by the Application Binary Interface (ABI) for the ARM architecture.

2. Modify main.c to observe what you found.

3. You have to state how you designed the observation (code), and how you performed it.

	Just like how you did in HW02.

3. If there are any official data that define the rules, you can also use them as references.

4. Push your repo to your github. (Use .gitignore to exclude the output files like object files or executable files and the qemu bin folder)

[Lecture 02 ─ Emulation with QEMU]: http://www.nc.es.ncku.edu.tw/course/embedded/02/#Emulation-with-QEMU
[ESEmbedded_HW02_Example]: https://github.com/vwxyzjimmy/ESEmbedded_HW02_Example

--------------------

- [ ] **If you volunteer to give the presentation next week, check this.**

--------------------
### 題目 : 觀察 C Function 如何傳遞參數與如何處理回傳值 ?

### 參考 Procedure Call Standard for the ARM® Architecture 觀察各個暫存器用途
page 14.

![](https://i.imgur.com/1rywdnI.png)

#### 修改 main.c 如下
```clike=
int sum (int a, int b ) {
	int sum = a + b ;
	return sum ;
}

int sum_five_parameter ( int a, int b, int c, int d, int e ) {
	return a + b + c + d + e ;
}


void reset_handler(void)
{
	sum(1,2);
	sum_five_parameter(1,2,3,4,5);
}
```

#### 觀察 reset_handler 組合語言

```clike=
00000054 <reset_handler>:
  54:   b580            push    {r7, lr}
  56:   b082            sub     sp, #8
  58:   af02            add     r7, sp, #8
  5a:   2001            movs    r0, #1
  5c:   2102            movs    r1, #2
  5e:   f7ff ffd3       bl      8 <sum>
  62:   2305            movs    r3, #5
  64:   9300            str     r3, [sp, #0]
  66:   2001            movs    r0, #1
  68:   2102            movs    r1, #2
  6a:   2203            movs    r2, #3
  6c:   2304            movs    r3, #4
  6e:   f7ff ffdb       bl      28 <sum_five_parameter>
  72:   46bd            mov     sp, r7
  74:   bd80            pop     {r7, pc}
  76:   bf00            nop
```

* 0x54~0x58 : 分配 stack 空間與將 $lr (function call 後回歸地址) 放入 stack 中
* 0x5a~0x5c 可藉由上面暫存計表格觀察到傳遞參數時會放入 $r0 ~ $r3
    * ![](https://i.imgur.com/wYtyNHh.png)
    * 故 sum(1,2) 1 被放入 $r0 ， 2 被放入 $r1
* 0x5e 執行 sum 這個function 使用 Branch with Link 的指令
* 0x64 與剛剛相加兩個參數不同，因為超過4個，第五個參數會被放入 stack 中
* 0x66~0x6c 將 1~4 $r0 ~ $r3 中
* 0x6e 執行 sum_five_parameter 這個function 使用 Branch with Link 的指令

#### 設計兩個簡單 Function 

功能如下

1. 將兩個參數數相加 sum(a,b)
    * 使用 objdump 觀察其組合語言如下
    * ```clike=
        00000008 <sum>:
           8:   b480            push    {r7}
           a:   b085            sub     sp, #20
           c:   af00            add     r7, sp, #0
           e:   6078            str     r0, [r7, #4]
          10:   6039            str     r1, [r7, #0]
          12:   687a            ldr     r2, [r7, #4]
          14:   683b            ldr     r3, [r7, #0]
          16:   4413            add     r3, r2
          18:   60fb            str     r3, [r7, #12]
          1a:   68fb            ldr     r3, [r7, #12]
          1c:   4618            mov     r0, r3
          1e:   3714            adds    r7, #20
          20:   46bd            mov     sp, r7
          22:   f85d 7b04       ldr.w   r7, [sp], #4
          26:   4770            bx      lr      
      ``` 
     * 0x8 ~ 0xc 分配 stack 空間與將 $r7 設為 stack的開頭
     * 0xe ~ 0x10 將 $r0 , $r1 先存入 stack 等待使用
     * 0x12 ~ 0x16 將值讀出，並相加
     * 0x18 將計算完的值存入 stack
     * 0x1a 將計算完的值取出
     * 0x1c 計算完的值放入 $r0 回傳
     * 0x1e ~ 0x22 更新 sp 位置
     * 0X26 回到 $lr 所指之位置
      
3. 將五個參數相加 sum_five_parameter ( a, b, c, d, e )
    * 使用 objdump 觀察其組合語言如下
    * ```clike=
        00000028 <sum_five_parameter>:
          28:   b480            push    {r7}
          2a:   b085            sub     sp, #20
          2c:   af00            add     r7, sp, #0
          2e:   60f8            str     r0, [r7, #12]
          30:   60b9            str     r1, [r7, #8]
          32:   607a            str     r2, [r7, #4]
          34:   603b            str     r3, [r7, #0]
          36:   68fa            ldr     r2, [r7, #12]
          38:   68bb            ldr     r3, [r7, #8]
          3a:   441a            add     r2, r3
          3c:   687b            ldr     r3, [r7, #4]
          3e:   441a            add     r2, r3
          40:   683b            ldr     r3, [r7, #0]
          42:   441a            add     r2, r3
          44:   69bb            ldr     r3, [r7, #24]
          46:   4413            add     r3, r2
          48:   4618            mov     r0, r3
          4a:   3714            adds    r7, #20
          4c:   46bd            mov     sp, r7
          4e:   f85d 7b04       ldr.w   r7, [sp], #4
          52:   4770            bx      lr     
      ```
    * 與兩數相加無太大區別

#### 結果與討論
透過手冊可以清楚的知道每個暫存器之用途以及參數間是如何傳遞，以前總以為 Function Call 完，回到 caller 執行下一行很直覺，但經過這樣 trace 後才發現底層的暫存器相當忙碌。有時候執行起來有點多此一舉，例如存入 stack 後又立刻取出，不知開啟最佳化後，會不會減少一些程式碼，下次可以試試並觀察。
