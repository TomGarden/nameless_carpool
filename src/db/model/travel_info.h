//
// Created by Tom on 2023/3/24.
//

#pragma once

#include "base_time.h"
#include "src/utils/macro/enum_util.h"


namespace nameless_carpool {


  struct FindBase; /* FindXxx 共用 enum */

  struct FindCar;
  struct UserFindCar;
  struct FindCustomers;
  struct UserFindCustomers;
  struct CarBindCustomers;
  struct GoodsInfo;
  struct UserGoodsInfo;
  struct Car;
  struct UserCar;
}  // namespace nameless_carpool

struct nameless_carpool::FindBase: public BaseTime {
  /* 枚举 , 作为 FindXxx 的一个成员 */
  ENUM_UTIL_DEFINE_ENUM(ServiceType,
                        carpool,         /*拼车*/
                        one_way_charter, /*单程包车*/
                        full_charter     /*全程包车*/
  )

  /* 上车地点选择 */
  ENUM_UTIL_DEFINE_ENUM(PickUpPoint,
                        nearby,      /* 附近上车 */
                        door_to_door /* 上门接 */
  )
  /* 上车地点选择 */
  ENUM_UTIL_DEFINE_ENUM(Intent,
                        goods, /* 货物 */
                        people /* 乘客 */
  )

  struct Names : public BaseTimeNames{
    const std::string id                         = "id";
    const std::string start_point_longitude      = "start_point_longitude";
    const std::string start_point_latitude       = "start_point_latitude";
    const std::string start_point_json           = "start_point_json";
    const std::string end_point_longitude        = "end_point_longitude";
    const std::string end_point_latitude         = "end_point_latitude";
    const std::string end_point_json             = "end_point_json";
    const std::string service_form               = "service_form";
    const std::string departure_time_range_start = "departure_time_range_start";
    const std::string departure_time_range_end   = "departure_time_range_end";
    const std::string pick_up_point              = "pick_up_point";
    const std::string intent                     = "intent";
    const std::string people_number              = "people_number";
    const std::string goods_info_id              = "goods_info_id";
    const std::string append_info                = "append_info";

    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {
          start_point_longitude,
          start_point_latitude,
          start_point_json,
          end_point_longitude,
          end_point_latitude,
          end_point_json,
          service_form,
          departure_time_range_start,
          departure_time_range_end,
          pick_up_point,
          intent,
          people_number,
          goods_info_id,
          append_info,
      };
    }
  };


  std::optional<uint64_t>       id                         = std::nullopt; /* 主键 id 自增 */
  std::optional<double>         start_point_longitude      = std::nullopt; /* 出发点 经度 */
  std::optional<double>         start_point_latitude       = std::nullopt; /* 出发点 纬度 */
  std::optional<nlohmann::json> start_point_json           = std::nullopt; /* 出发点信息 , 从 高德得到的数据 , 不想建表了  <{"adcode": "130128","district": "河北省石家庄市深泽县","location": "115.259604,38.216147","name": "小直要村",}> */
  std::optional<double>         end_point_longitude        = std::nullopt; /* 终点 经度 */
  std::optional<double>         end_point_latitude         = std::nullopt; /* 终点 纬度 */
  std::optional<nlohmann::json> end_point_json             = std::nullopt; /* 终点信息 , 从 高德得到的数据 , 不想建表了  <{"adcode": "130128","district": "河北省石家庄市深泽县","location": "115.259604,38.216147","name": "小直要村",}> */
  std::optional<std::string>    service_form               = std::nullopt; /* 服务形式 : 拼车(carpool), 单程包车(one_way_charter), 全程包车(full_charter) */
  std::optional<std::string>    departure_time_range_start = std::nullopt; /* 发车时间范围起点 */
  std::optional<std::string>    departure_time_range_end   = std::nullopt; /* 发车时间范围终点 */
  std::optional<std::string>    pick_up_point              = std::nullopt; /* 接送点 附近(nearby), 上门(door to door) */
  std::optional<std::string>    intent                     = std::nullopt; /* 运送意图 货物(goods), 乘客(people) */
  std::optional<uint>           people_number              = std::nullopt; /* 客容量 */
  std::optional<uint64_t>       goods_info_id              = std::nullopt; /* 货容量 -> 外键 : 货物信息 ID */
  std::optional<std::string>    append_info                = std::nullopt; /* 附加信息 , 最多 255 个字  */



  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);
  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {
        numOptionToStrOption(start_point_longitude),
        numOptionToStrOption(start_point_latitude),
        start_point_json->dump(),
        numOptionToStrOption(end_point_longitude),
        numOptionToStrOption(end_point_latitude),
        end_point_json->dump(),
        service_form,
        departure_time_range_start,
        departure_time_range_end,
        pick_up_point,
        intent,
        numOptionToStrOption(people_number),
        numOptionToStrOption(goods_info_id),
        append_info,
    };
  }



  ~FindBase() override = default;


};

/* 人找车 */
struct nameless_carpool::FindCar : public FindBase {

  struct Names :public FindBase::Names  {
    const std::string tableName = "find_car";
    virtual const std::string              getTableName() const override { return tableName; }
  };
  GET_NAMES()

  ~FindCar() override = default;

  std::map<std::string,std::string> getModelNameValMap(const FindCar& model) {
    const FindCar::Names modelNames = FindCar::names();
    modelNames.getPrimaryKeyNameVector();
    modelNames.getUnPrimaryKeyNameVector();

    model.getPrimaryKeyValVector();
    model.getUnPrimaryKeyValVector();
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FindCar,
                                              id,
                                              start_point_longitude,
                                              start_point_latitude,
                                              start_point_json,
                                              end_point_longitude,
                                              end_point_latitude,
                                              end_point_json,
                                              service_form,
                                              departure_time_range_start,
                                              departure_time_range_end,
                                              pick_up_point,
                                              intent,
                                              people_number,
                                              goods_info_id,
                                              append_info,

                                              create_time,
                                              create_time_tz,
                                              update_time,
                                              update_time_tz,
                                              del_time,
                                              del_time_tz)

};
struct nameless_carpool::UserFindCar : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "user_find_car";

    const std::string user_id     = "user_id";
    const std::string find_car_id = "find_car_id";

    /* ANCHOR -> 返回表名称字段 */
    virtual const std::string getTableName() const override { return tableName; }
    /* ANCHOR -> 获取主键列名称组成的列表 , 主键可能不止一个 */
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {user_id, find_car_id}; }
    /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTimeNames::getUnPrimaryKeyNameVector 完成 */
    virtual std::vector<std::string> getSubUnPrimaryKeyNameVector() const override { return {}; }
  };
  GET_NAMES()

  std::optional<uint64_t> user_id     = std::nullopt;
  std::optional<uint64_t> find_car_id = std::nullopt;

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption({user_id, find_car_id})}; };

  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override { return {}; }
};
/* 车找人 */
struct nameless_carpool::FindCustomers : public FindBase {
  struct Names : public FindBase::Names {
    const std::string tableName = "find_customers";

    const std::string car_id                     = "car_id";

    virtual const std::string              getTableName() const override { return tableName; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      auto result(FindBase::Names::getSubUnPrimaryKeyNameVector());
      result.push_back(car_id);
      return result;
    }
  };
  GET_NAMES()

  std::optional<uint64_t>    car_id                     = std::nullopt; /* 车辆信息 */


  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FindCustomers,
                                              id,
                                              start_point_longitude,
                                              start_point_latitude,
                                              start_point_json,
                                              end_point_longitude,
                                              end_point_latitude,
                                              end_point_json,
                                              service_form,
                                              departure_time_range_start,
                                              departure_time_range_end,
                                              pick_up_point,
                                              intent,
                                              people_number,
                                              goods_info_id,
                                              append_info,
                                              car_id,

                                              create_time,
                                              create_time_tz,
                                              update_time,
                                              update_time_tz,
                                              del_time,
                                              del_time_tz)

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);/* ANCHOR -> 获取主键值组成的列表 */

  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    auto result(FindBase::getSubUnPrimaryKeyValVector());
    result.push_back(numOptionToStrOption(car_id));
    return result;
  }
};
struct nameless_carpool::UserFindCustomers : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "user_find_customers";

    const std::string user_id           = "user_id";
    const std::string find_customers_id = "find_customers_id";

    /* ANCHOR -> 返回表名称字段 */
    virtual const std::string getTableName() const override { return tableName; }
    /* ANCHOR -> 获取主键列名称组成的列表 , 主键可能不止一个 */
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {user_id, find_customers_id}; }
    /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTimeNames::getUnPrimaryKeyNameVector 完成 */
    virtual std::vector<std::string> getSubUnPrimaryKeyNameVector() const override { return {}; }
  };
  GET_NAMES()

  std::optional<uint64_t> user_id           = std::nullopt;
  std::optional<uint64_t> find_customers_id = std::nullopt;

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption({user_id, find_customers_id})}; };

  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override { return {}; }
};
/* 人车绑定 */
struct nameless_carpool::CarBindCustomers : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "car_bind_customers";

    const std::string find_car_id        = "find_car_id";
    const std::string find_customers_id  = "find_customers_id";
    const std::string car_request        = "car_request";
    const std::string car_response       = "car_response";
    const std::string customers_request  = "customers_request";
    const std::string customers_response = "customers_response";

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {find_car_id, find_customers_id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {car_request,
                    car_response,
                    customers_request,
                    customers_response};
    }
  };
  GET_NAMES()

  std::optional<uint64_t> find_car_id        = std::nullopt; /* 既是主键,又是外键 */
  std::optional<uint64_t> find_customers_id  = std::nullopt; /* 既是主键,又是外键 */
  std::optional<uint>   car_request        = std::nullopt; /* 车主发起绑定 */
  std::optional<uint>   car_response       = std::nullopt; /* 车主相应绑定 */
  std::optional<uint>   customers_request  = std::nullopt; /* 乘客发起绑定 */
  std::optional<uint>   customers_response = std::nullopt; /* 乘客相应绑定 */


//  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(CarBindCustomers,
//                                              find_car_id,
//                                              find_customers_id,
//                                              car_request,
//                                              car_response,
//                                              customers_request,
//                                              customers_response,
//
//                                              create_time,
//                                              create_time_tz,
//                                              update_time,
//                                              update_time_tz,
//                                              del_time,
//                                              del_time_tz)

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);
  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption({find_car_id, find_customers_id})}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {
        numOptionToStrOption(find_car_id),
        numOptionToStrOption(find_customers_id),
        numOptionToStrOption(car_request),
        numOptionToStrOption(car_response),
        numOptionToStrOption(customers_request),
        numOptionToStrOption(customers_response),
    };
  }
};
/* 货物信息 */
struct nameless_carpool::GoodsInfo : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "goods_info";

    const std::string id          = "id";
    const std::string size_length = "size_length";
    const std::string size_width  = "size_width";
    const std::string size_height = "size_height";
    const std::string size_unity  = "size_unity";
    const std::string volume      = "volume";
    const std::string volume_unit = "volume_unit";
    const std::string weight      = "weight";
    const std::string weight_unit = "weight_unit";
    const std::string number      = "number";
    const std::string tag         = "tag";
    const std::string desc        = "desc";

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {
          size_length,
          size_width,
          size_height,
          size_unity,
          volume,
          volume_unit,
          weight,
          weight_unit,
          number,
          tag,
          desc,
      };
    }
  };
  GET_NAMES()


  std::optional<uint64_t>       id          = std::nullopt; /* 主键 id 自增 */
  std::optional<double>         size_length = std::nullopt; /* 长 */
  std::optional<double>         size_width  = std::nullopt; /* 宽 */
  std::optional<double>         size_height = std::nullopt; /* 高 */
  std::optional<std::string>    size_unity  = std::nullopt; /* 尺寸单位(cm/m) */
  std::optional<double>         volume      = std::nullopt; /* 体积 */
  std::optional<std::string>    volume_unit = std::nullopt; /* 体积单位(L/m³) */
  std::optional<double>         weight      = std::nullopt; /* 重量 */
  std::optional<std::string>    weight_unit = std::nullopt; /* 重量单位(KG/T) */
  std::optional<uint64_t>       number      = std::nullopt; /* 件数 */
  std::optional<nlohmann::json> tag         = std::nullopt; /* 形态; 性质: 易碎/贵重/时效; 等等 */
  std::optional<std::string>    desc        = std::nullopt; /* 货物描述信息 */

  ~GoodsInfo()  override = default;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(GoodsInfo,
                                              id,
                                              size_length,
                                              size_width,
                                              size_height,
                                              size_unity,
                                              volume,
                                              volume_unit,
                                              weight,
                                              weight_unit,
                                              number,
                                              tag,
                                              desc,

                                              create_time,
                                              create_time_tz,
                                              update_time,
                                              update_time_tz,
                                              del_time,
                                              del_time_tz)

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);
  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {
        numOptionToStrOption(size_length),
        numOptionToStrOption(size_width),
        numOptionToStrOption(size_height),
        size_unity,
        numOptionToStrOption(volume),
        volume_unit,
        numOptionToStrOption(weight),
        weight_unit,
        numOptionToStrOption(number),
        tag->dump(),
        desc,
    };
  }


  /*********************************************************************************************/
  /** true,是预置的物品
   * 我们预定义了几个标准行李箱, 这些数据也会下发到客户端供用户选择, 当数据被回传的时候需要确定是否预置数据 */
  bool inline isPresetItem() { return id.has_value() && id >= 1 && id <= 9; }
};
struct nameless_carpool::UserGoodsInfo : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "user_goods_info";

    const std::string user_id       = "user_id";
    const std::string goods_info_id = "goods_info_id";

    /* ANCHOR -> 返回表名称字段 */
    virtual const std::string getTableName() const override { return tableName; }
    /* ANCHOR -> 获取主键列名称组成的列表 , 主键可能不止一个 */
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {user_id, goods_info_id}; }
    /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTimeNames::getUnPrimaryKeyNameVector 完成 */
    virtual std::vector<std::string> getSubUnPrimaryKeyNameVector() const override { return {}; }
  };
  GET_NAMES()

  std::optional<uint64_t> user_id     = std::nullopt;
  std::optional<uint64_t> goods_info_id = std::nullopt;

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption({user_id, goods_info_id})}; };

  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override { return {}; }
};
/* 汽车信息 */
struct nameless_carpool::Car : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "car";

    const std::string id                   = "id";
    const std::string plate                = "plate";
    const std::string brand                = "brand";
    const std::string model                = "model";
    const std::string age                  = "age";
    const std::string passenger_capacity   = "passenger_capacity";
    const std::string size_length          = "size_length";
    const std::string size_width           = "size_width";
    const std::string size_height          = "size_height";
    const std::string size_unity           = "size_unity";
    const std::string capacity_volume      = "capacity_volume";
    const std::string capacity_volume_unit = "capacity_volume_unit";
    const std::string capacity_weight      = "capacity_weight";
    const std::string capacity_weight_unit = "capacity_weight_unit";
    const std::string main_transport_type  = "main_transport_type";

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {
          plate,
          brand,
          model,
          age,
          passenger_capacity,
          size_length,
          size_width,
          size_height,
          size_unity,
          capacity_volume,
          capacity_volume_unit,
          capacity_weight,
          capacity_weight_unit,
          main_transport_type,
      };
    }
  };
  GET_NAMES()

  std::optional<uint64_t>    id                   = std::nullopt; /* 主键 id 自增 */
  std::optional<std::string> plate                = std::nullopt; /* 车牌号 */
  std::optional<std::string> brand                = std::nullopt; /* 品牌 */
  std::optional<std::string> model                = std::nullopt; /* 型号 */
  std::optional<uint>      age                  = std::nullopt; /* 车龄 */
  std::optional<uint>      passenger_capacity   = std::nullopt; /* 客容量(不含司机) */
  std::optional<double>      size_length          = std::nullopt; /* 长 */
  std::optional<double>      size_width           = std::nullopt; /* 宽 */
  std::optional<double>      size_height          = std::nullopt; /* 高 */
  std::optional<std::string> size_unity           = std::nullopt; /* 尺寸单位(cm/m) */
  std::optional<double>      capacity_volume      = std::nullopt; /* 货容量:体积 */
  std::optional<std::string> capacity_volume_unit = std::nullopt; /* 货容量单位:体积(L/m³) */
  std::optional<double>      capacity_weight      = std::nullopt; /* 货重量:重量 */
  std::optional<std::string> capacity_weight_unit = std::nullopt; /* 货重量单位:重量(KG/T) */
  std::optional<std::string> main_transport_type  = std::nullopt; /* 主力运输类型(goods/passenger) */

//  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Car,
//                                              id,
//                                              plate,
//                                              brand,
//                                              model,
//                                              age,
//                                              passenger_capacity,
//                                              size_length,
//                                              size_width,
//                                              size_height,
//                                              size_unity,
//                                              capacity_volume,
//                                              capacity_volume_unit,
//                                              capacity_weight,
//                                              capacity_weight_unit,
//                                              main_transport_type,
//
//                                              create_time,
//                                              create_time_tz,
//                                              update_time,
//                                              update_time_tz,
//                                              del_time,
//                                              del_time_tz)

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);
  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {
        plate,
        brand,
        model,
        numOptionToStrOption(age),
        numOptionToStrOption(passenger_capacity),
        numOptionToStrOption(size_length),
        numOptionToStrOption(size_width),
        numOptionToStrOption(size_height),
        size_unity,
        numOptionToStrOption(capacity_volume),
        capacity_volume_unit,
        numOptionToStrOption(capacity_weight),
        capacity_weight_unit,
        main_transport_type,
    };
  }
};
struct nameless_carpool::UserCar : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "user_goods_info";

    const std::string user_id       = "user_id";
    const std::string car_id = "car_id";

    /* ANCHOR -> 返回表名称字段 */
    virtual const std::string getTableName() const override { return tableName; }
    /* ANCHOR -> 获取主键列名称组成的列表 , 主键可能不止一个 */
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {user_id, car_id}; }
    /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTimeNames::getUnPrimaryKeyNameVector 完成 */
    virtual std::vector<std::string> getSubUnPrimaryKeyNameVector() const override { return {}; }
  };
  GET_NAMES()

  std::optional<uint64_t> user_id     = std::nullopt;
  std::optional<uint64_t> car_id = std::nullopt;

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);

  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption({user_id, car_id})}; };

  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  virtual std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override { return {}; }
};
