#pragma once

#include <boost/format.hpp>
#include <boost/format/format_fwd.hpp>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "common.h"
#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/common.h"
#include "libs/mysql_connector_arm_static/include/mysqlx/devapi/result.h"
#include "libs/mysql_connector_arm_static/include/mysqlx/xdevapi.h"
#include "sql_util.h"
#include "user_info.h"

namespace nameless_carpool {
  class DbManager;
}


class nameless_carpool::DbManager {

 private:
  mysqlx::Client client = mysqlx::Client(mysqlx::SessionOption::USER, "root",
                                         mysqlx::SessionOption::PWD, "root",
                                         mysqlx::SessionOption::HOST, "172.17.0.3",
                                         mysqlx::SessionOption::PORT, 33060,
                                         mysqlx::SessionOption::DB, SqlUtil::dbName);

 private:
  DbManager(); /* 默认构造 */

 public: /* constructor */
  DbManager(const DbManager& dbManager) = delete;             /* 拷贝构造 */
  DbManager& operator=(const DbManager& dbManager) = delete;  /* 拷贝赋值 */
  DbManager(const DbManager&& dbManager)           = delete;  /* 移动构造 */
  DbManager& operator=(const DbManager&& dbManager) = delete; /* 移动赋值 */
  //~DbManager();                                                /* 析   构 */

  static DbManager& getInstance();
  static mysqlx::Client&    getClient();


 public: /* executeSql */
  std::string             getSqlResultWarning(mysqlx::SqlResult* sqlResult);
  mysqlx::SqlResult       executeSql(const std::string& sqlTmp);
  mysqlx::SqlResult       executeSql(const std::stringstream& sqlTmp);
  mysqlx::SqlResult       executeTransactionSql(const std::vector<std::string>& sqlVector);

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

  /* 从数据库对象 mysqlx::SqlResult 中读取所有 Model 对象并保存到 vector 中 */
  template <typename Model, typename ModeNames = typename Model::Names>
  std::vector<Model> getModelVector(mysqlx::SqlResult& sqlResult) {
    std::map<int, std::string> indexNameMap = getModelColumnNameMap<Model>(sqlResult.getColumns());

    const ModeNames& modeNames = getModelNames<Model>();

    std::map<int, std::string>::size_type      columnCount      = indexNameMap.size();
    std::vector<Model> result;
    for (mysqlx::Row row : sqlResult) {
      Model model;

      for (std::map<int, std::string>::size_type columnIndex = 0;
           columnIndex < columnCount; columnIndex++) {
        mysqlx::Value& value = row.get(columnIndex);
        const std::string& columnName = indexNameMap[columnIndex];

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
  inline std::string queryModelSql(const std::vector<std::string>& modelNames,
                                   const std::string&              tableName,
                                   const std::string&              whereStatements) {
    std::stringstream sqlTmp;

    sqlTmp << " SELECT " << SqlUtil::allFieldSql(modelNames) 
           << " FROM   " << SqlUtil::getDbAndTablename(tableName) << " \n"
           << " WHERE  " << whereStatements << " ; ";
    return sqlTmp.str();
  }

  template <typename Model>
  std::vector<Model> query(const std::string& whereStatements) {

    const typename Model::Names& modelNames = getModelNames<Model>();

    std::string sqlStr = queryModelSql(modelNames.getColumnNameVector(), modelNames.tableName, whereStatements);

    mysqlx::SqlResult sqlResult = executeSql(sqlStr);

    std::vector<Model> result;

    if (sqlResult.hasData()) result = getModelVector<Model>(sqlResult); 

    return result;

  }

  template <typename Model>
  inline Model querySingleResult(const std::string& whereStatements) {
    std::vector<Model> result = query<Model>(whereStatements);
    if (result.size() != 1) throw std::logic_error(
        "调用这个函数的你应该知道 , 你期望只获得一个结果 , 如果事实上结果有多个 , 那可能有两个原因  "
        " 1. 这个函数不该出现在你调用的地方"
        " 2. 其他地方的逻辑异常 , 导致这里有了多于一个结果");
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
    std::stringstream sqlTmp;

    sqlTmp
        << " INSERT INTO " << SqlUtil::getDbAndTablename(modelName.tableName) << " ( \n"
        << SqlUtil::allFieldSql(modelName.getColumnNameVector()) << " )\n"
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

  /** @description: sql 插入指令 , 返回多行中首行的首个自增长数字类型的数据 . 我们预期的是多行插入具有原子性 . */
  template <typename Model>
  uint64_t insert(const std::vector<Model>& modelVector) {
    if (modelVector.size() < 1) throw std::logic_error("不接受空向量 , 需要开发者自行筛选后 再入参");

    const std::string& sqlTmp    = insertModelSql(modelVector);
    mysqlx::SqlResult     sqlResult = executeSql(sqlTmp);

    return sqlResult.getAutoIncrementValue();
  }

  template <typename Model>
  uint64_t insert(const Model& model) {
    return insert(std::vector<Model>{model});
  }




  /*┌─────────────────────────────────────────────────────────────────────────────────────┐
  * │ update  : https://stackoverflow.com/a/75627308/7707781
  * └─────────────────────────────────────────────────────────────────────────────────────┘ */
  template <typename Model>
  std::string updateModelSql(const std::vector<Model>& modelVector) {

    const typename Model::Names& modelName      = getModelNames<Model>();
    std::vector<std::string>     unPkNameVector = modelName.getUnPrimaryKeyNameVector();
    std::vector<std::string>     pkNameVector   = modelName.getPrimaryKeyNameVector();

    std::string updatePrefix = " UPDATE " + SqlUtil::getDbAndTablename(modelName.getTableName()) + " \n SET \n";

    /* 预计存储 
        `字段名`                   = CASE `id`                                                            
                                  WHEN '1' THEN '15111111111'                                                            
                                  WHEN '2' THEN '15122222222'  */
    // using Pair = pair<const std::string, std::stringstream>;
    using ColumnNamePair       = std::pair<const std::string, std::stringstream>;
    using ColumnNamePairPtr    = std::shared_ptr<ColumnNamePair>;
    using ColumnNamePairVector = std::vector<ColumnNamePairPtr>;
    ColumnNamePairVector unPkNamePairVector;
    for(const std::string& name: unPkNameVector) {
      ColumnNamePairPtr pair = std::make_shared<std::pair<const std::string, std::stringstream>>(SqlUtil::backticks(name), "");
      unPkNamePairVector.push_back(pair);
    }
    
    /* sql 后半段 */
    std::stringstream updateSuffix;
    updateSuffix << "\n WHERE (" ;
    for (std::vector<std::string>::const_iterator pkNameItr = pkNameVector.cbegin();
         pkNameItr != pkNameVector.cend(); pkNameItr++) {
      updateSuffix << SqlUtil::backticks(*pkNameItr);
      if (pkNameItr + 1 == pkNameVector.cend()) updateSuffix << ")";
      else updateSuffix << ", ";
    }
    updateSuffix << " IN ( ";

    /* 遍历所有要更新的 手机号 , 用这些数据填充 columnNamePairVector*/
    for(auto modelIter = modelVector.cbegin(); modelIter != modelVector.cend(); modelIter++) {

      const std::vector<std::optional<std::string>>& pkValVector   = modelIter->getPrimaryKeyValVector();
      const std::vector<std::optional<std::string>>& unPkValVector = modelIter->getUnPrimaryKeyValVector();

      std::vector<std::string>::const_iterator                pkNameVectorItr = pkNameVector.cbegin();
      std::vector<std::optional<std::string>>::const_iterator pkValVectorItr  = pkValVector.cbegin();
      while (pkNameVectorItr != pkNameVector.cend() && pkValVectorItr != pkValVector.cend()) {
        /* sql 后段逻辑 */
        if (pkValVectorItr == pkValVector.cbegin()) updateSuffix << "( ";
        updateSuffix << SqlUtil::nullOrApostrophe(*pkValVectorItr);
        if (pkNameVectorItr + 1 == pkNameVector.cend() || pkValVectorItr + 1 == pkValVector.cend()) {
          updateSuffix << ") ";
        } else {
          updateSuffix << ", ";
        }

        ++pkNameVectorItr;
        ++pkValVectorItr;
      }


      ColumnNamePairVector::iterator                          unPkNamePairItr = unPkNamePairVector.begin();
      std::vector<std::optional<std::string>>::const_iterator unPkValItr      = unPkValVector.cbegin();

      while(unPkNamePairItr != unPkNamePairVector.end() && unPkValItr != unPkValVector.cend()) {

        const std::string& columnName     = (**unPkNamePairItr).first;
        std::stringstream& columnNameStr  = (**unPkNamePairItr).second;

        int outputPosition = columnNameStr.tellg(); /* 输出到 std::stringstream 中的当前位置索引 */
        if(outputPosition == -1) {
          std::string errState = Common::GetErr::getErr(columnNameStr.rdstate());
          throw std::runtime_error("字符流状态异常:" + errState);
        }
        if (outputPosition <= 0) columnNameStr << "\t" << columnName << " = CASE \n\t\t\t WHEN (";

        std::vector<std::string>::const_iterator                pkNameVectorItr = pkNameVector.cbegin();
        std::vector<std::optional<std::string>>::const_iterator pkValVectorItr  = pkValVector.cbegin();
        while (pkNameVectorItr != pkNameVector.cend() && pkValVectorItr != pkValVector.cend()) {
          /* sql 中段逻辑 */
          columnNameStr << SqlUtil::backticks(*pkNameVectorItr) << "=" << SqlUtil::nullOrApostrophe(*pkValVectorItr);
          if (pkNameVectorItr + 1 == pkNameVector.cend() || pkValVectorItr + 1 == pkValVector.cend()) {
            columnNameStr << ") ";
          } else {
            columnNameStr << ", ";
          }

          ++pkNameVectorItr;
          ++pkValVectorItr;
        }
        columnNameStr << " THEN " << SqlUtil::nullOrApostrophe(*unPkValItr) << " \n ";
        columnNameStr << " \t\t\t END ";

        if (unPkNamePairItr + 1 != unPkNamePairVector.end() && unPkValItr + 1 != unPkValVector.cend()) {
          columnNameStr << " , ";
        }

        ++unPkNamePairItr;
        ++unPkValItr;
      }/* while end 为 每条 set 语句 添加一种情况*/

    }/* for end */

    updateSuffix << " )";

    std::stringstream sqlTmp; {
      sqlTmp << updatePrefix;
      for (const ColumnNamePairPtr& columnNamePair : unPkNamePairVector) sqlTmp << columnNamePair->second.str();
      sqlTmp << updateSuffix.str();
    }
  
    return sqlTmp.str();
  }

  template <typename Model>
  void update(const std::vector<Model>& modelVector) {
    if (modelVector.size() < 1) return;
    const std::string& sqlTmp    = updateModelSql(modelVector);
    mysqlx::SqlResult     sqlResult = executeSql(sqlTmp);
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
    const std::string& sqlTmp    = insertOrUpdateModelSql(modelVector);
    mysqlx::SqlResult     sqlResult = executeSql(sqlTmp);
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

  template <typename Model,
            typename PkOptional        = std::optional<std::string>,
            typename PkOptVector       = std::vector<std::optional<std::string>>,
            typename PkOptDoubleVector = std::vector<std::vector<std::optional<std::string>>>>
  std::string delSqlByPkDoubleVector(const PkOptDoubleVector& pkValVectorVector) {
    /*  DELETE FROM `nameless_carpool`.`user_tel`
        WHERE (`user_id`, `telephone_id`) IN ( ('1', '1') );    */

    if (pkValVectorVector.empty() == true) return "";

    const typename Model::Names& modelName = getModelNames<Model>();
    // const std::vector<std::string>& unPkVector       = modelName.getUnPrimaryKeyNameVector();
    const std::vector<std::string>& pkVector = modelName.getPrimaryKeyNameVector();
    // const std::vector<std::string>& colunmNameVector = modelName.getColumnNameVector();
    std::stringstream sqlTmp;
    /* sub sql : DELETE FROM */
    sqlTmp << " DELETE FROM " << SqlUtil::getDbAndTablename(modelName.tableName) << "\n WHERE (";
    /* sub sql : WHERE */
    for (auto primaryKeyItr = pkVector.cbegin(); primaryKeyItr != pkVector.cend(); primaryKeyItr++) {
      sqlTmp << SqlUtil::backticks(*primaryKeyItr);
      if (primaryKeyItr != pkVector.cend()) sqlTmp << ", ";
      else sqlTmp << ") IN ( \n";
    }
    /* sub sql : IN */
    for (auto modelItr = pkValVectorVector.cbegin(); modelItr != pkValVectorVector.cend(); modelItr++) {
      const PkOptVector& pkValVector = *modelItr;

      sqlTmp << "\t(";
      for (auto pkValItr = pkValVector.cbegin(); pkValItr != pkValVector.cend(); pkValItr++) {
        sqlTmp << SqlUtil::nullOrApostrophe(*pkValItr);
        if (pkValItr + 1 != pkValVector.cend()) sqlTmp << ", ";
        else sqlTmp << ")";
      }

      if (modelItr + 1 != pkValVectorVector.cend()) sqlTmp << ", \n";
      else sqlTmp << " \n\t) ;";
    }

    return sqlTmp.str();
  }

  template <typename Model,
            typename PkOptional        = std::optional<std::string>,
            typename PkOptVector       = std::vector<std::optional<std::string>>,
            typename PkOptDoubleVector = std::vector<std::vector<std::optional<std::string>>>>
  std::string delSqlByModelVector(const std::vector<Model>& modelVector) {
    std::vector<std::vector<std::optional<std::string>>> pkOptDoubleVector;
    for (const Model& model : modelVector) pkOptDoubleVector.push_back(model.getPrimaryKeyValVector());
    return delSqlByPkDoubleVector<Model>(pkOptDoubleVector);
  }

  template <typename Model,
            typename PkOptional        = std::optional<std::string>,
            typename PkOptVector       = std::vector<std::optional<std::string>>,
            typename PkOptDoubleVector = std::vector<std::vector<std::optional<std::string>>>>
  std::string delSqlByModel(const Model& model) { return delSqlByModelVector({model}); }

  template <typename Model,
            typename PkOptional        = std::optional<std::string>,
            typename PkOptVector       = std::vector<std::optional<std::string>>,
            typename PkOptDoubleVector = std::vector<std::vector<std::optional<std::string>>>>
  void delByDoubleleVector(const PkOptDoubleVector& modelVector) {
    if (modelVector.empty()) return;
    const std::string& sqlTmp    = delSqlByPkDoubleVector<Model>(modelVector);
    mysqlx::SqlResult  sqlResult = executeSql(sqlTmp);
    return;
  }

  template <typename Model,
            typename PkOptional        = std::optional<std::string>,
            typename PkOptVector       = std::vector<std::optional<std::string>>,
            typename PkOptDoubleVector = std::vector<std::vector<std::optional<std::string>>>>
  void delBySingleVector(const PkOptVector& model) {
    return delByDoubleleVector<Model>(PkOptDoubleVector{model});
  }

  template <typename Model,
            typename PkOptional        = std::optional<std::string>,
            typename PkOptVector       = std::vector<PkOptional>,
            typename PkOptDoubleVector = std::vector<PkOptVector>>
  void del(const std::vector<Model>& modelVector) {
    if (modelVector.empty()) return;

    PkOptDoubleVector pkOptDoubleVector;
    for(const Model& model: modelVector) pkOptDoubleVector.push_back(model.getPrimaryKeyValVector());

    return delByDoubleleVector<Model>(pkOptDoubleVector);
  }

  template <typename Model>
  void del(const Model& model) {
    return del<Model>(std::vector<Model>{model});
  }

  /*┌──────────────────────────────────────────────────────────────────────────────────────
  * │ 事务 应该使用 session.startTransaction() 执行
  * └────────────────────────────────────────────────────────────────────────────────────── */
  // inline std::string transactionSql(const std::initializer_list<std::string>& subSqls){
  //   if (subSqls.size() < 1) return "";

  //   std::stringstream strStream;
  //   strStream << "START TRANSACTION ; \n";
  //   for (const std::string& subSql : subSqls) {
  //     strStream << subSql;
  //     if (!subSql.ends_with(";")) strStream << " ; ";
  //     strStream << "\n";
  //   }
  //   strStream << "COMMIT ; ";

  //   return strStream.str();
  // }

 public: /* sql where sub statements */
  /**  @description:  where 对 where 字句补充 是否搜索删除字段的秒速
   * @param {bool} isDel    true, 查找已删除的 Model 
   */
  inline std::string andDelFilter(const std::optional<bool> isDel = std::nullopt ) {
    if (std::nullopt == isDel) return "";

    /* 由于都是 BaseTime 的基类 , 所以这里实际上是那个 ModelNames 都不是紧要的 */
    const User::Names& modelNames = userNames;

    if (isDel.value()) {
      auto delSql = boost::format("  AND  ( %1% IS NOT NULL  OR  %2% IS NOT NULL )") % SqlUtil::backticks(modelNames.del_time) % SqlUtil::backticks(modelNames.del_time_tz);
      return boost::str(delSql);
    } else {
      auto unDelSql = boost::format("  AND  ( %1% IS NULL  AND  %2% IS NULL )") % SqlUtil::backticks(modelNames.del_time) % SqlUtil::backticks(modelNames.del_time_tz);
      return boost::str(unDelSql);
    }
  }

  /** @description: 单列查询 where 语句 
  * @param {bool} isDel    true, 查找已删除的 Model ; 默认 false
  */
  inline std::string where(const std::string& key, const std::string& val, const std::optional<bool>& isDel = false) {
    std::string result = boost::str(boost::format("%1%=%2%") % key % val);
    return result.append(andDelFilter(isDel));
  }

  /** @description: 单列查询 where 语句 , key value 默认包裹
  * @param {bool} isDel    true, 查找已删除的 Model ; 默认 false
  */
  template <typename Type>
  inline std::string whereWithPack(const std::string& key, const std::optional<Type>& val, const std::optional<bool>& isDel = false) {
    return where(SqlUtil::backticks(key), SqlUtil::nullOrApostrophe(val), isDel);
  }


  /** @description: 多个查询条件 , 用 And 连接
   * @param {initializer_list<std::string>} listFilter
   * @param {bool} isDel    true, 查找已删除的 Model 
   * @return {*}
   */
  inline std::string whereWithAnd(const std::initializer_list<std::string> listFilter, const std::optional<bool>& isDel = false) {
    std::stringstream strStream;
    std::initializer_list<std::string>::iterator it;
    for (it = listFilter.begin(); it != listFilter.end(); it++) {
      strStream << *it;
      if (it + 1 != listFilter.end()) strStream << " AND ";
    }

    return strStream.str().append(andDelFilter(isDel));
  }


 public: /* other insert opt */
  void insert(const Telephone& telephone, User& user, const UserTel& userTel);
  void insert(Session& session, UserSession& userSession);

 public:  /* 测试函数 */
  void testSqlResult(mysqlx::SqlResult& sqlResult) ;
  void testMultipleThread();

};
