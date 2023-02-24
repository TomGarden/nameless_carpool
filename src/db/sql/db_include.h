#include "libs/mysql_connector_arm_static/include/mysqlx/xdevapi.h"
#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/result.h"

namespace nameless_carpool {
  using Client = mysqlx::Client;
  // using Client = mysqlx::Client;
  using Session = mysqlx::Session;
  using Schema = mysqlx::Schema;
  using SessionOption = mysqlx::SessionOption;
  using Warning = mysqlx::Warning;

  using SqlStatement = mysqlx::internal::SQL_statement;
  using SqlResult = mysqlx::SqlResult;
  
  using Columns = mysqlx::Columns;
  using Column = mysqlx::Column;
  using Value = mysqlx::Value;
}