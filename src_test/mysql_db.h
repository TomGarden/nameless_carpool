
#ifndef nameless_carpool_mysql_db
#define nameless_carpool_mysql_db

// #include "../libs/mysql_connector/include/mysqlx/xdevapi.h"
// #include "../libs/mysql_connector_tom/include/mysqlx/xdevapi.h"
#include "../libs/mysql_connector_arm_static/include/mysqlx/xdevapi.h"
#include "../libs/mysql_connector_arm_static/include/mysqlx/devapi/result.h"

namespace src_test
{

  class MysqlDb
  {

    public:
      void tryMysql();
      void tryMysql2();
      std::string value2Str(mysqlx::Value);
      std::string bytes2Str(mysqlx::bytes);
  };

}

#endif