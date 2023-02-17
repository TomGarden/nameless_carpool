#include "table_desc.h"
#include "db_manager.h"

#include <iostream>
#include <vector>

namespace nameless_carpool/* TableDesc */ {
  std::string TableDesc::queryAllFieldSql() const {
    
    const std::vector<ColumnsDesc>& columnsDescVector = getFieldVector();

    std::stringstream sqlTmp;

    for (const ColumnsDesc& columnsDesc : columnsDescVector) {
      switch (columnsDesc.columnsValType) {
        case ColumnsValType::Date: sqlTmp << " \t  " << DbManager::dateSelectStatements(columnsDesc.columnsName) << ", \n";
        default: /*             */ sqlTmp << " \t  " << DbManager::backticks(columnsDesc.columnsName)            << ", \n";
      }
    }

    return sqlTmp.str();
  }

  std::string TableDesc:: queryWhereSubSql() const {
    const std::vector<ColumnsDesc>& columnsDescVector = getFieldVector();
    // ANCHOR - 阿斯顿发生
    for (const ColumnsDesc& columnsDesc : columnsDescVector) {}
  }
}