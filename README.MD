无名拼车

mysql_script : mysql 脚本文件们

src : 代码目录

src_test : 测试代码目录

makefiles : makeFile 文件目录 ， cMake 后面再了解 ， 继续使用了 makefile




## 调试方式
```
nameless_carpool 软件本地调用的格式说明

nameless_carpool \
    --debug                               \   关键字, 用于区分数据当做网络数据还是当做本地数据
    --help                                \   关键字, 包含此参数时候 , 其他参数被忽略
    --method=get,post,put,delete ···      \   http method
    --header={key:value, key1:value1 ···} \   http header
    --function=httpUrl                    \   http url
    --body={anyjsondata}                  \   http body

只接受 utf-8 编码的数据 , 几个参数顺序不分先后

help : 入参中只要包含了 `help` 就会打印本说明 , 且忽略其他入参 . 

debug : 只有包含 `debug` 才被当做一次性的调试调用 
    不包含 `debug` 参数 , 其他参数将被忽略 , 进入网络阻塞等待下一个网络呼叫 . 
```