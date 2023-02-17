#pragma once

#include <optional>
#include <string>
#include <vector>

namespace nameless_carpool {
  struct ColumnsDesc; /* 描述表的某一列 */
  enum ColumnsValType /* 描述属性的附加信息 */ {
    Date,
    String,
    Uint64_t,
    Int64_t,
    Float,
    Double,
    Bool,
  };

  class TableDesc;
  class SubSqlWhere;
}  // namespace nameless_carpool

struct nameless_carpool::ColumnsDesc {
  // ColumnsDesc(
  //     const std::string&    _columnsName,
  //     const ColumnsValType& _columnsValType)
  //     : columnsName(_columnsName),
  //       columnsValType(_columnsValType){};

  const std::string                columnsName;               /* 列名 */
  const ColumnsValType             columnsValType;            /* 列值类型 */
  const std::optional<std::string> whereValue = std::nullopt; /* 是否作为 where 字句中的查询字段 */
};

class nameless_carpool::TableDesc {
 public:
  const std::string tableName;

 public:
  explicit TableDesc(const std::string& _tableName) : tableName(_tableName){};

 public:
  std::string queryAllFieldSql() const ;
  std::string queryWhereSubSql() const ;


 public: /* virtual */
  /**  @description: 设计用户获取 表 信息
   * @return {*}
   */
  virtual std::vector<ColumnsDesc> getFieldVector() const  = 0;
};

class nameless_carpool::SubSqlWhere {

};