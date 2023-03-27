//
// Created by Tom on 2023/3/24.
//

#pragma once

#include "travel_info.h"
#include "user_info.h"


namespace nameless_carpool {

  template <typename Model, typename ModelNames = typename Model::Names>
  inline ModelNames getModelNames() {
    const void* ptr = nullptr;

    // user_info
    if /**/ (std::is_same<User /*            */, Model>::value) ptr = &userNames;
    else if (std::is_same<WcUser /*          */, Model>::value) ptr = &wcUserNames;
    else if (std::is_same<Telephone /*       */, Model>::value) ptr = &telephoneNames;
    else if (std::is_same<UserTel /*         */, Model>::value) ptr = &userTelNames;
    else if (std::is_same<Session /*         */, Model>::value) ptr = &sessionNames;
    else if (std::is_same<UserSession /*     */, Model>::value) ptr = &userSessionNames;

    // travel_info
    else if (std::is_same<FindCar /*         */, Model>::value) ptr = &findCarNames;
    else if (std::is_same<FindCustomers /*   */, Model>::value) ptr = &findCustomersNames;
    else if (std::is_same<CarBindCustomers /**/, Model>::value) ptr = &carBindCustomersNames;
    else if (std::is_same<GoodsInfo /*       */, Model>::value) ptr = &goodsInfoNames;
    else if (std::is_same<Car /*             */, Model>::value) ptr = &carNames;

    else throw std::runtime_error("not implement");

    const ModelNames* result = reinterpret_cast<const ModelNames*>(ptr);

    return *result;
  }

  /* BaseMode , BaseModelNames , 的 getColumnXxxVector 获取的值应该是一一对应的
   将此一一对应的值做成 pair 放到 vector */
  template <typename Model,
            typename ModelNames = typename Model::Names,
            typename ResultItem = typename std::pair<std::string, std::optional<std::string>> /*  */>
  [[deprecated("ANCHOR - 此函数尚未真正使用 , 使用的时候移除此标记")]] inline std::vector<ResultItem> getColumnValNameMap(const Model& model) {
    const std::vector<std::string>&                nameVector = getModelNames<Model>().getColumnNameVector();
    const std::vector<std::optional<std::string>>& valVector  = model.getColumnValVector();
    if (nameVector.size() != valVector.size()) throw std::logic_error(
        boost::format("不符合预期 , 两个向量长度不同是异常的 ; nameVector.size = %1% , valVector.size = %2% ") % nameVector.size() % valVector.size());
    std::vector<ResultItem> result;

    for (int index = 0; index < valVector.size(); index++) {
      result.push_back(ResultItem{nameVector[index], valVector[index]});
    }

    return result;
  }
};  // namespace nameless_carpool