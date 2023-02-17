#pragma once

/* 预计使用 宏(模仿 nlohmann_json ) 生成 对于数据库的相关操作 */



#define NLOHMANN_JSON_EXPAND( x ) x


void test() {
    int flag;
    NLOHMANN_JSON_EXPAND( flag );
}