#pragma once

#include <boost/format.hpp>
#include <boost/format/format_fwd.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "user_info.h"
#include "db_include.h"
#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/common.h"
#include "sql_util.h"
#include "common.h"

namespace nameless_carpool {
  class DbManager;
}


class nameless_carpool::DbManager {
  using string = std::string;
  template <typename T>
  using vector = std::vector<T>;


 private:
  Client client = Client(SessionOption::USER, "root",
                         SessionOption::PWD, "root",
                         SessionOption::HOST, "172.17.0.4",
                         SessionOption::PORT, 33060,
                         SessionOption::DB, SqlUtil::dbName);

 private:
  DbManager(); /* 默认构造 */

 public:
  DbManager(const DbManager& dbManager) = delete;             /* 拷贝构造 */
  DbManager& operator=(const DbManager& dbManager) = delete;  /* 拷贝赋值 */
  DbManager(const DbManager&& dbManager)           = delete;  /* 移动构造 */
  DbManager& operator=(const DbManager&& dbManager) = delete; /* 移动赋值 */
                                                              //~DbManager();                                                /* 析   构 */

 public:

  static DbManager& getInstance();
  static Client&    getClient();


 public:
  SqlResult executeSql(const string& sqlTmp);
  SqlResult executeSql(const stringstream& sqlTmp);

 public: /* model operate util */

  inline void throwUnknowColumn(const std::string& columnName) {
    throw std::logic_error(boost::str(boost::format("未知属性[%1%] , 暂时无法处理") % columnName));
  }

  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │                query                                                                │
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */
  /* 按照 数据库对象 Columns 中各个列名出现的顺序 , 填充 map<index , columnName> */
  template <typename Model>
  std::map<int, std::string> getModelColumnNameMap(const mysqlx::Columns& columns) {
    const std::vector<std::string>& nameVector = getModelNames<Model>().getColumnNameVector();
    std::map<int, std::string>      indexNameMap;

    int index = 0;
    for (auto item = columns.begin(); item != columns.end(); item++, index++) {
      auto&       column     = *item;
      std::string columnName = column.getColumnName();
      if (columnName.empty()) columnName = column.getColumnLabel();

      bool isContain = std::find(nameVector.cbegin(), nameVector.cend(), columnName) != nameVector.cend();

      if (isContain) indexNameMap[index] = columnName;
      else throwUnknowColumn(columnName);
    }

    return indexNameMap;
  }

  /* 从数据库对象 SqlResult 中读取所有 Model 对象并保存到 vector 中 */
  template <typename Model, typename ModeNames = typename Model::Names>
  std::vector<Model> getModelVector(SqlResult& sqlResult) {
    std::map<int, string> indexNameMap = getModelColumnNameMap<Model>(sqlResult.getColumns());

    const ModeNames& modeNames = getModelNames<Model>();

    std::map<int, string>::size_type      columnCount      = indexNameMap.size();
    std::vector<Model> result;
    for (mysqlx::Row row : sqlResult) {
      Model model;

      for (std::map<int, string>::size_type columnIndex = 0;
           columnIndex < columnCount; columnIndex++) {
        mysqlx::Value& value = row.get(columnIndex);
        const string& columnName = indexNameMap[columnIndex];

        /* ANCHOR - 这个 inflate 预计手动生成 */
        if (model.inflate(modeNames, columnName, value)) model; /* empty */
        else throwUnknowColumn(columnName);
      } /* endfor , 制造一个 model */

      result.push_back(model);
    }/* endfor , 制造一个 Vector<model> */

    return result;
  }

  /** @description: 用于针对特定的数据类型生成 select 语句的 select from 字句 , were 字句需要手动填充
   * @param {typename ModelNames} 描述某个确切的数据类型对应的字段描述类型
   * @param {const ModelNames& modelNames} 
   * @param {const std::string& whereStr} 
   * @return {*}
   */
  template <typename ModelNames>
  std::string queryModelSql(const ModelNames& modelNames, const std::string& whereStatements) {
    stringstream sqlTmp;

    sqlTmp << " SELECT " << modelNames.allFieldSql(modelNames.getColumnNameVector()) 
           << " FROM   " << SqlUtil::getDbAndTablename(modelNames.tableName) << " \n"
           << " WHERE  " << whereStatements << " ; ";
    return sqlTmp.str();
  }

  template <typename Model>
  std::vector<Model> query(const std::string& whereStatements) {

    std::string sqlStr = queryModelSql(getModelNames<Model>(), whereStatements); 

    SqlResult sqlResult = executeSql(sqlStr);

    std::vector<Model> result;

    if (sqlResult.hasData()) result = getModelVector<Model>(sqlResult); 

    return result;

  }

  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │                insert                                                               │
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */

  /** @description: 用于针对特定的数据类型生成 insert 语句
   * @param {typename Model} 描述某个确切的数据类型
   * @param {typename ModelNames} 描述某个确切的数据类型对应的字段描述类型
   * @param {const std::vector<Model>&} modelVector 
   * @param {const ModelNames& modelNames} modelNames 
   * @return {*}
   */
  template <typename Model>
  std::string insertModelSql(const std::vector<Model>& modelVector) {

    /* 
      -- INSERT INTO `nameless_carpool`.`telephone` (
      --                   `id`                  ,
      --                   `number`              ,
      --                   `vertify_code`        ,
      --                   `vc_update_time`      ,
      --                   `vc_update_time_tz`   ,
      --                   `create_time`         ,
      --                   `create_time_tz`      ,
      --                   `update_time`         ,
      --                   `update_time_tz`      ,
      --                   `del_time`            ,
      --                   `del_time_tz`         )
      --             VALUES (
      --                         NULL            ,
      --                         '17611111111'   ,
      --                         'vertify_code'  ,
      --                         '2022-09-22 19:28:00.123456'    ,
      --                         'Asia/Shanghai'             ,
      --                         '2022-09-22 19:28:00.123456'    ,
      --                         'Asia/Shanghai'             ,
      --                         '2022-09-22 19:28:00.123456'    ,
      --                         'Asia/Shanghai'             ,
      --                         NULL            ,
      --                         NULL            ),
      --                   (
      --                         NULL            ,
      --                         '17622222222'   ,
      --                         '======'  ,
      --                         '2022-09-22 19:59:35.123456'    ,
      --                         'Asia/Shanghai'             ,
      --                         '2022-09-22 19:59:35.123456'    ,
      --                         'Asia/Shanghai'             ,
      --                         '2022-09-22 19:59:35.123456'    ,
      --                         'Asia/Shanghai'             ,
      --                         NULL            ,
      --                         NULL            )
      --                         ;
     */

    if (modelVector.size() < 1) return "";

    const typename Model::Names& modelName = getModelNames<Model>();
    stringstream sqlTmp;

    sqlTmp
        << " INSERT INTO " << SqlUtil::getDbAndTablename(modelName.tableName) << " ( \n"
        << modelName.BaseTimeNames::allFieldSql(modelName.getColumnNameVector()) << " )\n"
        << " VALUES \n";

    for (typename std::vector<Model>::const_iterator modelIter = modelVector.cbegin();
         modelIter != modelVector.cend(); modelIter++) {
          
      const Model& model = *modelIter;
      sqlTmp << " \t(\n" << model.BaseTime::insertAllFieldSql(model.getColumnValVector()) << " \t) ";

      if (modelIter + 1 == modelVector.cend()) {
        sqlTmp << ';';
      } else {
        sqlTmp << ",\n";
      }
    } /* for 循环结束 */

    return sqlTmp.str();
  }

  template <typename Model>
  void insert(const std::vector<Model>& modelVector) {
    if (modelVector.size() < 1) return;

    const string& sqlTmp    = insertModelSql(modelVector);
    SqlResult     sqlResult = executeSql(sqlTmp);

    return;
  }

  template <typename Model>
  void insert(const Model& model) {
    return insert(std::vector<Model>{model});
  }

  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │ update 目前的局限性在于只支持唯一主键的单条或多条语句的一次性更新 ;                           │
  * │        具有联合主键的表的更新 , 考虑下 insert or update ;                                │
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */

  template <typename Model>
  std::string updateModelSql(const std::vector<Model>& modelVector) {

    const typename Model::Names& modelName  = getModelNames<Model>();
    std::vector<std::string> unPkVector = modelName.getUnPrimaryKeyNameVector();
    std::string primaryKey; {
      std::vector<std::string> pkVector   = modelName.getPrimaryKeyNameVector();
      if (pkVector.size() < 1) throw std::logic_error("预期至少有一个 id");
      else if (pkVector.size() > 1) throw std::logic_error("联合主键不兼容 , 使用 insert or update 吧");
      primaryKey = pkVector[0];
    }

    string updatePrefix = " UPDATE " + SqlUtil::getDbAndTablename(modelName.getTableName()) + " \n SET \n";

    /* 预计存储 
        `字段名`                   = CASE `id`                                                            
                                  WHEN '1' THEN '15111111111'                                                            
                                  WHEN '2' THEN '15122222222'  */
    // using Pair = pair<const string, stringstream>;
    using ColumnNamePair       = std::pair<const string, stringstream>;
    using ColumnNamePairPtr    = std::shared_ptr<ColumnNamePair>;
    using ColumnNamePairVector = std::vector<ColumnNamePairPtr>;
    ColumnNamePairVector columnNamePairVector;
    for(const std::string& name: unPkVector) {
      ColumnNamePairPtr pair = std::make_shared<std::pair<const string, stringstream>>(SqlUtil::backticks(name), "");
      columnNamePairVector.push_back(pair);
    }
    
    /* sql 后半段 */
    std::string updateSuffix = "\n WHERE " + SqlUtil::backticks(primaryKey) + " IN ();";

    /* 遍历所有要更新的 手机号 , 用这些数据填充 fieldPairVector*/
    for(auto modelIter = modelVector.cbegin(); modelIter != modelVector.cend(); modelIter++) {

      std::vector<std::string> columnValVector; /* telephone 的 所有变量 按顺序 放到 队列中 */
      for(const std::string& columnVal: unPkVector) columnValVector.push_back(SqlUtil::nullOrApostrophe(columnVal));

      ColumnNamePairVector::iterator           columnNamePairItr = columnNamePairVector.begin();
      std::vector<std::string>::const_iterator columnValItr      = columnValVector.cbegin();

      while(columnNamePairItr != columnNamePairVector.end() && columnValItr != columnValVector.cend()) {

        const std::string& columnName     = (**columnNamePairItr).first;
        std::stringstream& columnNameStr  = (**columnNamePairItr).second;

        int outputPosition = columnNameStr.tellg(); /* 输出到 std::stringstream 中的当前位置索引 */
        if(outputPosition == -1) {
          std::string errState = Common::GetErr::getErr(columnNameStr.rdstate());
          throw runtime_error("字符流状态异常:" + errState);
        }
        if(outputPosition <= 0) {
          columnNameStr << "\n\t" << columnName << " = CASE " << SqlUtil::backticks(telephoneNames.id) << "\n";
        }
          
        columnNameStr << "\t\t\t WHEN " << SqlUtil::nullOrApostrophe(primaryKey) << " THEN " << *columnValItr << '\n';

        if (modelIter + 1 == modelVector.cend()) {
          columnNameStr << "\t\t\t END ";

          if(columnNamePairItr+1 != columnNamePairVector.end() && columnValItr+1 != columnValVector.cend()) {
            columnNameStr << " , ";
          }
        }

        ++columnNamePairItr;
        ++columnValItr;
      }/* while end 为 每条 set 语句 添加一种情况*/
    
      updateSuffix.insert(updateSuffix.size() - 2, SqlUtil::nullOrApostrophe(modelIter->getPrimaryKeyVal()));
      if (modelIter + 1 != modelVector.end()) {
        updateSuffix.insert(updateSuffix.size() - 2, ", ");
      }

    }/* for end */

    std::stringstream sqlTmp; {
      sqlTmp << updatePrefix;
      for (const ColumnNamePairPtr& columnNamePair : columnNamePairVector) sqlTmp << columnNamePair->second.str();
      sqlTmp << updateSuffix;
    }
  
    return sqlTmp.str();
  }

  template <typename Model>
  void update(const std::vector<Model>& modelVector) {
    if (modelVector.size() < 1) return;
    const string& sqlTmp    = updateModelSql(modelVector);
    SqlResult     sqlResult = executeSql(sqlTmp);
    return;
  }

  template <typename Model>
  void update(const Model& model) {
    return update(std::vector<Model>{model});
  }

  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │ insert or update 为了解决 update 语句在联合主键情况下无法完成更新的问题而增加的能力           │
  * │                  https://dev.mysql.com/doc/refman/8.0/en/insert-on-duplicate.html   │
  * │                  https://stackoverflow.com/a/3466/7707781                           │
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */
  /* TODO - insertOrUpdateModelSql 尚未测试 , 测试过后预计还需要若干改进 */
  template <typename Model>
  std::string insertOrUpdateModelSql(const std::vector<Model>& modelVector) {
    /*
      INSERT INTO `nameless_carpool`.`user_tel` ( `user_id`,
                                                  `telephone_id`,
                                                  `desc`,
                                                  `flag`,
                                                  `create_time`,
                                                  `create_time_tz`,
                                                  `update_time`,
                                                  `update_time_tz`,
                                                  `del_time`,
                                                  `del_time_tz`
                                                )
      VALUES (    '1',
                  '1',
                  'desc111111111',
                  'flag',
                  '2023-02-22 17:19:53.000000',
                  'shangHai',
                  '2023-02-22 17:19:53.000000',
                  'shangHai',
                  NULL,
                  NULL
              )
      ON DUPLICATE KEY UPDATE         `desc`           =VALUES(`desc`          ),
                                      `flag`           =VALUES(`flag`          ),
                                      `create_time`    =VALUES(`create_time`   ),
                                      `create_time_tz` =VALUES(`create_time_tz`),
                                      `update_time`    =VALUES(`update_time`   ),
                                      `update_time_tz` =VALUES(`update_time_tz`),
                                      `del_time`       =VALUES(`del_time`      ),
                                      `del_time_tz`    =VALUES(`del_time_tz`   );
    */
    if (modelVector.empty() == true) return "";
    const typename Model::Names&    modelName  = getModelNames<Model>();
    const std::vector<std::string>& unPkVector = modelName.getUnPrimaryKeyNameVector();
    // const std::vector<std::string>& pkVector   = modelName.getPrimaryKeyNameVector();
    const std::vector<std::string>& colunmNameVector = modelName.getColumnNameVector();

    std::stringstream sqlTmp;
    sqlTmp << "INSERT INTO" << getDbAndTablename(modelName.tableName) << " ( \n";
    for (auto colunmNameItr = colunmNameVector.cbegin(); colunmNameItr != colunmNameVector.cend(); colunmNameItr++) {
      if (colunmNameItr + 1 != colunmNameVector.cend()) sqlTmp << SqlUtil::backticks(*colunmNameItr) << " , \n";
      else sqlTmp << SqlUtil::backticks(*colunmNameItr) << "  \n\t\t\t)";
    }
    sqlTmp << "VALUES \n";
    for (auto modelItr = modelVector.cbegin(); modelItr != modelVector.cend(); modelItr++) {
      sqlTmp << " \t(\n" << modelItr->BaseTime::insertAllFieldSql(modelItr->getColumnValVector()) << " \n\t) ";
      if (modelItr + 1 != modelVector.cend()) sqlTmp << ", ";
    }
    sqlTmp << "ON DUPLICATE KEY UPDATE \n";
    for (auto columnNameItr = unPkVector.cbegin(); columnNameItr != unPkVector.cend(); columnNameItr++) {
      sqlTmp << boost::format("`%1%` = VALUES(`%1%`)") % (*columnNameItr);
      if (columnNameItr + 1 != modelVector.cend()) sqlTmp << ", \n";
      else sqlTmp << "; ";
    }

    return sqlTmp.str();
  }

  template <typename Model>
  void insertOrUpdate(const std::vector<Model>& modelVector) {
    if (modelVector.empty()) return;
    const string& sqlTmp    = insertOrUpdateModelSql(modelVector);
    SqlResult     sqlResult = executeSql(sqlTmp);
    return;
  }

  template <typename Model>
  void insertOrUpdate(const Model& model) {
    return insertOrUpdate(std::vector<Model>{model});
  }

  /*┌──────────────────────────────────────────────────────────────────────────────────────
  * │ delete
  * └────────────────────────────────────────────────────────────────────────────────────── */
  /* TODO - DELETE 动作尚需进行进一步测试方可确认无误 */
  template <typename Model>
  std::string delModelSql(const std::vector<Model>& modelVector) {
    /*  DELETE FROM `nameless_carpool`.`user_tel`
        WHERE (`user_id`, `telephone_id`) IN ( ('1', '1') );    */

    if (modelVector.empty() == true) return "";

    const typename Model::Names& modelName = getModelNames<Model>();
    // const std::vector<std::string>& unPkVector       = modelName.getUnPrimaryKeyNameVector();
    const std::vector<std::string>& pkVector         = modelName.getPrimaryKeyNameVector();
    // const std::vector<std::string>& colunmNameVector = modelName.getColumnNameVector();
    std::stringstream               sqlTmp;
    /* sub sql : DELETE FROM */
    sqlTmp << " DELETE FROM " << getDbAndTablename(modelName.tableName) << "\n WHERE (";
    /* sub sql : WHERE */
    for (auto primaryKeyItr = pkVector.cbegin(); primaryKeyItr != pkVector.cend(); primaryKeyItr++) {
      sqlTmp << SqlUtil::backticks(*primaryKeyItr);
      if(primaryKeyItr != pkVector.cend()) sqlTmp << ", ";
      else sqlTmp << ") IN ( \n";
    }
    /* sub sql : IN */
    for (auto modelItr = modelVector.cbegin(); modelItr != modelVector.cend(); modelItr++) {
      const std::vector<std::optional<std::string>>& pkValVector = modelItr->getPrimaryKeyValVector();

      sqlTmp << "\t(";
      for (auto pkValItr = pkValVector.cbegin(); pkValItr != pkValVector.cend(); pkValItr++) {
        sqlTmp << SqlUtil::nullOrApostrophe(*pkValItr);
        if (pkValItr + 1 != pkValVector.cend()) sqlTmp << ", ";
        else sqlTmp << ")";
      }

      if (modelItr + 1 != modelVector.cend()) sqlTmp << ", \n";
      else sqlTmp << " \n\t) ;";
    }

    return sqlTmp.str();
  }

  template <typename Model>
  void del(const std::vector<Model>& modelVector) {
    if (modelVector.empty()) return;
    const string& sqlTmp    = delModelSql(modelVector);
    SqlResult     sqlResult = executeSql(sqlTmp);
    return;
  }

  template <typename Model>
  void del(const Model& model) {
    return del(std::vector<Model>{model});
  }

 public: /* sql where sub statements */
  /**  @description:  where 对 where 字句补充 是否搜索删除字段的秒速 */
  template <typename ModelNames>
  inline std::string andDelFilter(const ModelNames& modelNames , const std::optional<bool> isDel = std::nullopt) {
    if (std::nullopt == isDel) return "";
    if (isDel) {
      auto delSql = boost::format("  AND  ( %1% != NULL  OR  %2% != NULL )") % modelNames.del_time % modelNames.del_time_tz;
      return boost::str(delSql);
    } else {
      auto unDelSql = boost::format("  AND  ( %1% == NULL  OR  %2% == NULL )") % modelNames.del_time % modelNames.del_time_tz;
      return boost::str(unDelSql);
    }
  }
  template <typename Model>
  inline std::string andDelFilter(const std::optional<bool> isDel = std::nullopt) {
    if (std::nullopt == isDel) return "";

    const typename Model::Names& modelNames = getModelNames<Model>();

    return andDelFilter(modelNames, isDel);
  }

  /** @description: 根据 telNum 查找 Telephone 的 where 字句
   * @param {string} telNum 
   * @param {bool} isDel    true, 查找已删除的 Telephone ; 默认 false
   * @return {*}   where 字句
   */
  const std::string whereTelByNum(const std::string telNum, const std::optional<bool> isDel = false);


};
