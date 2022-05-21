# SimpleCompileFramework

## 当前进度
- 词法分析器构建中。。。
- 修复了一些我也忘了的 bug
- 以 ops 表 存储有穷自动机，改进了 _lex_op_ll1()，统一了不同长度单词的词法分析方法。
- 还剩下 数字常量、字符串常量、'.'开头词未完成。
- 语义分析理论学习ing

## 过程发现
- _lex_push_char(scf_lex_t* lex,scf_lex_char_t* c)，是将c自己压入栈，将 c 压栈后不可以 free(c)，出入栈都会出现无法访问栈的问题。正确做法应该是将 c 指向 NULL，而不是清空 c 指向的内存空间。

## 感谢
编译器框架设计参考了 [@闲聊代码](https://author.baidu.com/home?from=bjh_article&app_id=1683021426724988) 老师的 [《用 C 语言写一个真正的词法分析分析器》](https://baijiahao.baidu.com/s?id=1696288054374712822&wfr=spider&for=pc) 系列分享