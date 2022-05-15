# 汇编拾遗

## 函数传参用到的寄存器

- 32 位汇编
	依次从 " 右向左 " 放入栈中, 通过[栈帧](https://baike.baidu.com/item/%E6%A0%88%E5%B8%A7/5662951) 基地址偏移取值.
- 64 位汇编 
	当参数少于 7 个时, 参数从左到右放入寄存器: rdi, rsi, rdx, rcx, r8, r9, 当参数为 7 个及以上时, 从第 7 个参数开始依次从 " 右向左 " 放入栈中, 即和32位汇编一样

## 64 / 32 / 16 / 8 位寄存器名称

|64-bit|32-bit|16-bit|8-bit|
|:-|:-|:-|:-|
|rax|eax|ax|al|
|rbx|ebx|bx|bl|
|rcx|ecx|cx|cl|
|rdx|edx|dx|dl|
|rsi|esi|si|sil|
|rdi|edi|di|dil|
|rbp|ebp|bp|bpi|
|rsp|esp|sp|spi|
|r8|r8d|r8w|r8b|

```
上述表格中前 8 个通用寄存器寄存器以及 r8 - r15 是常用寄存器
https://www.cnblogs.com/volva/p/11814998.html
https://blog.csdn.net/WTB15527135550/article/details/50974207
```

## c/c++ 寄存器作用

汇编代码中各个寄存器都有各自的作用.
|寄存器|名称|c/c++ 作用|
|:-|:-|:-|
|ax ( accumulator )|累加器|1. 函数的返回值; 2. 存放 add 指令结果|
|bx ( base )|基址寄存器|?|
|cx ( count )|计数寄存器|函数的第四个参数|
|dx ( data )|数据寄存器|1. 函数的第三个参数; 2. 存放 mov 指令结果|
|sp ( stack pointer )|栈指针寄存器|栈帧的起始地址|
|bp ( base pointer )|基指针寄存器|栈帧的底部地址|
|di ( destination index)|目的变址寄存器|函数第一个参数|
|si ( source inder )|源变址寄存器|函数第二个参数|
|ip ( instruction pointer )|指令指针寄存器|指向程序当前执行的代码, 即程序状态机的 pc|

