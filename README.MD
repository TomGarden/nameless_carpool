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