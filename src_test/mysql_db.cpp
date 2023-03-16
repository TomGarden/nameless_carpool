
#include "./mysql_db.h"
#include <iostream>

using namespace mysqlx;
using namespace std;
using SqlStatement = internal::SQL_statement;
using Columns = mysqlx::Columns;

void src_test::MysqlDb::tryMysql()
{
  Client client(
      SessionOption::USER, "root",
      SessionOption::PWD, "root",
      SessionOption::HOST, "172.17.0.3",
      SessionOption::PORT, 33060,
      SessionOption::DB, "nameless_carpool");
  Session session = client.getSession();

  Schema defSchema = session.getDefaultSchema();
  std::string defSchemeName = defSchema.getName();

  {
    cout << "\n\n\nscheme 信息展示 ---------------------------------" << endl;

    Schema defSchema = session.getDefaultSchema();

    cout << "defSchema:" << defSchema.getName() << endl;

    vector<Schema> schemaVector = session.getSchemas();

    cout << "schemeVector.size = " << schemaVector.size() << endl;

    for (int i = 0; i < schemaVector.size(); i++)
    {
      cout << i << ':' << schemaVector[i].getName() << endl;
    }
  }

  vector<Table> tableVector = defSchema.getTables();
  {
    cout << "\n\n\nscheme 内表信息展示 ---------------------------------" << endl;
    vector<Table> tableVector = defSchema.getTables();
    cout << "tableVector.size() = " << tableVector.size() << endl;
    for (int i = 0; i < tableVector.size(); i++)
    {
      cout << i << ':' << tableVector[i].getName() << endl;
    }
  }

  // {
  //   cout << "\n\n\n插入表内数据 ---------------------------------" << endl;
  //   Table table = defSchema.getTable("user");
  //   std::string insertStr = "INSERT INTO user (name, register_time, register_time_tz, update_time, update_time_tz) VALUES (?,?,?,?,?)";
  //   SqlResult sqlResult =
  //       session.sql(insertStr)
  //           .bind("姓名", "2022-07-27 22:10:35", "8", "2022-07-27 22:10:35", "8")
  //           .execute();
  // }


  {
    cout << "\n\n\n查询表内数据 ---------------------------------" << endl;

    for (int i = 0; i < tableVector.size(); i++)
    {
      Table table = tableVector[i];
      std::string tableName = table.getName();

      // SqlStatement sqlStatement = session.sql(
      //   "SELECT * FROM nameless_carpool."+tableName);
      SqlStatement sqlStatement = session.sql(
        "SELECT id, DATE_FORMAT(`vc_update_time`,'%Y-%m-%d %H:%i:%S.%f') AS `vc_update_time` FROM nameless_carpool.telephone");
        // "SELECT id, `vc_update_time` FROM nameless_carpool.telephone");
      SqlResult sqlResult = sqlStatement.execute();

      //RowResult sqlResult = table.select("*").execute();
      cout << tableName << '(' << sqlResult.count() << ')' << endl;
      
      const Columns &columns = sqlResult.getColumns();
      col_count_t columnCount = sqlResult.getColumnCount();

      cout << tableName << '(' << sqlResult.count() << ')' << endl;
      for (int j = 0; j < columnCount; j++) {
        cout << "\t" << columns[j].getColumnName() << "\t|";
      }
      cout << endl;
      if (sqlResult.count() > 0) {
        vector<Row> rowVector = sqlResult.fetchAll();
        for (int k = 0; k < rowVector.size(); k++) {
          Row row = rowVector[k];
          for (int l = 0; l < row.colCount(); l++) {
            Value value = row.get(l);
            std::string strValue = value2Str(value);
            cout << "\t" << strValue << "\t|";
          }
          cout << endl;
        }
      } else {
        cout << tableName << " is empty" << endl;
      }
    }
  }


}


void src_test::MysqlDb::tryMysql2() {
  Client client(
      SessionOption::USER, "root",
      SessionOption::PWD, "root",
      SessionOption::HOST, "172.17.0.3",
      SessionOption::PORT, 33060,
      SessionOption::DB, "nameless_carpool");
  Session session = client.getSession();

  Schema defSchema = session.getDefaultSchema();
  std::string defSchemeName = defSchema.getName();

  {
    cout << "\n\n\nscheme 信息展示 ---------------------------------" << endl;

    Schema defSchema = session.getDefaultSchema();

    cout << "defSchema:" << defSchema.getName() << endl;

    vector<Schema> schemaVector = session.getSchemas();

    cout << "schemeVector.size = " << schemaVector.size() << endl;

    for (int i = 0; i < schemaVector.size(); i++)
    {
      cout << i << ':' << schemaVector[i].getName() << endl;
    }
  }

  vector<Table> tableVector = defSchema.getTables();
  {
    cout << "\n\n\nscheme 内表信息展示 ---------------------------------" << endl;
    vector<Table> tableVector = defSchema.getTables();
    cout << "tableVector.size() = " << tableVector.size() << endl;
    for (int i = 0; i < tableVector.size(); i++)
    {
      cout << i << ':' << tableVector[i].getName() << endl;
    }
  }

  {
    cout << "\n\n\n查询表内数据 ---------------------------------" << endl;

    for (int i = 0; i < tableVector.size(); i++)
    {
      Table table = tableVector[i];
      std::string tableName = table.getName();

      // SqlStatement sqlStatement = session.sql("SELECT * FROM ?.?")
      //                                 .bind(defSchemeName, tableName);
      // SqlStatement sqlStatement = session.sql("SELECT * FROM "+defSchemeName + '.' + tableName);
                                      //.bind(defSchemeName + '.' + tableName);

      SqlStatement sqlStatement = session.sql(
        "SELECT id, DATE_FORMAT(vc_update_time,'%Y-%m-%d %H:%i:%S.%f') AS vc_update_time FROM nameless_carpool.telephone");
      SqlResult sqlResult = sqlStatement.execute();

      const Columns &columns = sqlResult.getColumns();
      col_count_t columnCount = sqlResult.getColumnCount();

      cout << tableName << '(' << sqlResult.count() << ')' << endl;
      for (int j = 0; j < columnCount; j++)
      {
        cout << "\t" << columns[j].getColumnName() << "\t|";
      }
      cout << endl;
      if (sqlResult.count() > 0) {
        vector<Row> rowVector = sqlResult.fetchAll();
        for (int k = 0; k < rowVector.size(); k++)
        {
          Row row = rowVector[k];
          for (int l; l < row.colCount(); l++)
          {
            Value value = row.get(l);
            std::string strValue = value2Str(value);
            cout << "\t" << strValue << "\t|";
          }
          cout << endl;
        }
      } else {
        cout << tableName << " is empty" << endl;
      }
    }
  }
}

std::string src_test::MysqlDb::value2Str(Value value) {
  Value::Type type = value.getType();
  switch (type) {
    case Value::Type::ARRAY:      return "Value::Type::ARRAY"/* bytes2Str(value.getRawBytes()) */;
    case Value::Type::DOCUMENT:   return "Value::Type::DOCUMENT"/* bytes2Str(value.getRawBytes()) */;
    case Value::Type::VNULL:      return "Value::Type::VNULL";
    
    default: { 
      switch (type) {
        case common::Value::VNULL:    return "common::Value::VNULL";
        case common::Value::UINT64:   return to_string(value.get<uint64_t>());
        case common::Value::INT64:    return to_string(value.get<int64_t>());
        case common::Value::FLOAT:    return to_string(value.get<float>());
        case common::Value::DOUBLE:   return to_string(value.get<double>());
        case common::Value::BOOL:     return to_string(value.get<bool>());
        case common::Value::STRING:   return value.get<std::string>();
        case common::Value::USTRING:  return value.get<mysqlx::string>();
        case common::Value::RAW:      return "common::Value::RAW"/* bytes2Str(value.getRawBytes()) */;
        case common::Value::EXPR:     return "common::Value::EXPR"/* bytes2Str(value.getRawBytes()) */;
        case common::Value::JSON:     return "common::Value::JSON"/* bytes2Str(value.getRawBytes()) */;
        
        default:                      return "default" /* bytes2Str(value.getRawBytes()) */;
      }
    }
  }


}


std::string src_test::MysqlDb::bytes2Str(mysqlx::bytes bytesObj) {

  // auto charP = bytesObj.begin();
  //mysqlx::abi2::r0::common::byte *charP = bytesObj.begin();
  //std::string result = std::string(charP);
  return "未知";
}