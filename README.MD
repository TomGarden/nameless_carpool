我们暂时放弃了本项目的 C++ 实现 , 原因如下

0. 我们计划用 Java 实现项目 : https://github.com/TomGarden/nameless_carpool_java
1. C++ 实现过程中脚手架不如广为流行的 Java 的脚手架更丰富简约 , 有太多的细节我们需要去关注了 . 这些轮子 在 Java 社区有更成熟的解决方案 . 
2. 过去我们对 C++ 总是有很多迷思 , 那是因为不够了解他 , 经过 > 6 个月的持续了解 , 我们认为 , 是时候停下来了 . 
3. 我们认为 , 将来使用 C++ 进行恰当项目的开发是有能力 . 只是当前这个项目 Java 看起来好像更合适 . 
4. 直接导致我们方式 C++ 的原因是
   1. 我们在处理 数据库数据的时候 , C++ STL 本身没有提供 类似 bigdecimal 这样的类型 , 我们不得不寻求第三方解决方案 . 
   2. 像这样的三方内容我们以前用的有 howardhinnant/date / nlohmann_json / drogon : 
      - 不可否认这些三方工具都很好用 . 
      - 同样不可否认的是这些三方工具的寻找过程要花费比在 Java 社区相同工具更多的时间 . 
      - 因为 C++ 用于 写 Web 后端终究是少数 , 那种可以服众的三方库(我的意思是众望所归) , 很多时候不存在 . 所以开发者必须花大量时间甄别选用 . 
      - 值得注意的是 , 花了大量时间甄别出来的库对比 同类型的 Java 库 又远不够贴心和舒适 . 
   3. 不可否认 C++ 有更佳的性能优势 , Java 底层使用的也是 C++ , 但是对于这个项目 , 性能优势并非我们优先考虑的内容
5. 将来有机会考虑性能问题的时候 , 如上所述 , 我们已经具备了使用 C++ 开发的能力 . 



无名拼车

mysql_script : mysql 脚本文件们

src : 代码目录

src_test : 测试代码目录

makefiles : makeFile 文件目录 
CMake : 根目录


关于 宽 窄 字符的选择 : 
1. nlohmann_json 库只支持 utf8 编码也就是窄字符 : https://github.com/nlohmann/json#character-encoding
2. 我们自己的业务逻辑 , 除了自己假设的需求外目前也没有大面积的 其他字符的需求 
3. 我们在项目中大面积使用 窄字符 处理字符串 , 当有特殊编码需求的字符操作的时候 , 先完成转码 , 然后再做操作 . 



## 调试方式
```
/*方式一: 入参配置信息在 doc 目录中可以查看 */
clear && make --file=makefiles/cmake_src.mk clean all clean_log reload_cgi run

/*方式二: clion 直接编译运行*/
```
