//
// Created by Tom on 2023/3/24.
//

#pragma once

#include "base_time.h"
#include "enum_util.h"


namespace nameless_carpool {

  struct ServiceType; /*枚举 , 作为 FindXxx 的一个成员 */
  struct FindCar;
  struct FindCustomers;
  struct CarBindCustomers;
  struct GoodsInfo;
  struct Car;
}  // namespace nameless_carpool


DEFINE_ENUM(nameless_carpool::ServiceType,
            carpool,         /*拼车*/
            one_way_charter, /*单程包车*/
            full_charter     /*全程包车*/
)


/* 人找车 */
struct nameless_carpool::FindCar : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "find_car";

    const std::string id                         = "id";
    const std::string start_point                = "start_point";
    const std::string end_point                  = "end_point";
    const std::string way_of_using_car           = "way_of_using_car";
    const std::string departure_time_range_start = "departure_time_range_start";
    const std::string departure_time_range_end   = "departure_time_range_end";
    const std::string pick_up_point              = "pick_up_point";
    const std::string people_number              = "people_number";
    const std::string goods_info_id              = "goods_info_id";
    const std::string append_info                = "append_info";

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {start_point,
                    end_point,
                    way_of_using_car,
                    departure_time_range_start,
                    departure_time_range_end,
                    pick_up_point,
                    people_number,
                    goods_info_id,
                    append_info};
    }
  };
  GET_NAMES()


  std::optional<uint64_t>    id                         = std::nullopt; /* 外键 : 货物信息 ID */
  std::optional<std::string> start_point                = std::nullopt; /* 出发点信息 , 从 高德得到的数据 , 不想建表了  <{"adcode": "130128","district": "河北省石家庄市深泽县","location": "115.259604,38.216147","name": "小直要村",} */
  std::optional<std::string> end_point                  = std::nullopt; /* 终点信息 , 从 高德得到的数据 , 不想建表了  <{"adcode": "130128","district": "河北省石家庄市深泽县","location": "115.259604,38.216147","name": "小直要村",} */
  std::optional<std::string> way_of_using_car           = std::nullopt; /* 用车形式 拼车(carpool), 单程包车(one_way_charter), 全程包车(full_charter) */
  std::optional<std::string> departure_time_range_start = std::nullopt; /* 发车时间范围起点 */
  std::optional<std::string> departure_time_range_end   = std::nullopt; /* 发车时间范围终点 */
  std::optional<std::string> pick_up_point              = std::nullopt; /* 接送点 附近(nearby), 上门(door to door) */
  std::optional<uint8_t>     people_number              = std::nullopt; /* 跟车人数 */
  std::optional<uint64_t>    goods_info_id              = std::nullopt; /* 外键 : 货物信息 ID */
  std::optional<std::string> append_info                = std::nullopt; /* 附加信息 , 最多 255 个字  */

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FindCar,
                                              id,
                                              start_point,
                                              end_point,
                                              way_of_using_car,
                                              departure_time_range_start,
                                              departure_time_range_end,
                                              pick_up_point,
                                              people_number,
                                              goods_info_id,
                                              append_info)

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);
  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {start_point,
            end_point,
            way_of_using_car,
            departure_time_range_start,
            departure_time_range_end,
            pick_up_point,
            numOptionToStrOption(people_number),
            numOptionToStrOption(goods_info_id),
            append_info};
  }
};
/* 车找人 */
struct nameless_carpool::FindCustomers : public BaseTime {
  struct Names : virtual BaseTimeNames {
    const std::string tableName = "find_customers";

    const std::string id                         = "id";
    const std::string start_points_area_list     = "start_points_area_list";
    const std::string end_points_area_list       = "end_points_area_list";
    const std::string car_supply_form            = "car_supply_form";
    const std::string departure_time_range_start = "departure_time_range_start";
    const std::string departure_time_range_end   = "departure_time_range_end";
    const std::string pick_up_point              = "pick_up_point";
    const std::string people_number              = "people_number";
    const std::string goods_info_id              = "goods_info_id";
    const std::string car_id                     = "car_id";
    const std::string append_info                = "append_info";

    virtual const std::string              getTableName() const override { return tableName; }
    virtual const std::vector<std::string> getPrimaryKeyNameVector() const override { return {id}; }
    virtual std::vector<std::string>       getSubUnPrimaryKeyNameVector() const override {
      return {start_points_area_list,
                    end_points_area_list,
                    car_supply_form,
                    departure_time_range_start,
                    departure_time_range_end,
                    pick_up_point,
                    people_number,
                    goods_info_id,
                    car_id,
                    append_info};
    }
  };
  GET_NAMES()

  std::optional<std::string> id                         = std::nullopt; /* 主键 id 自增 */
  std::optional<std::string> start_points_area_list     = std::nullopt; /* 出发点 jons 数组 */
  std::optional<std::string> end_points_area_list       = std::nullopt; /* 到达点 json 数组  */
  std::optional<std::string> car_supply_form            = std::nullopt; /* 供车形式 拼车(carpool), 单程包车(one_way_charter), 全程包车(full_charter) */
  std::optional<std::string> departure_time_range_start = std::nullopt; /* 发车时间范围起点 */
  std::optional<std::string> departure_time_range_end   = std::nullopt; /* 发车时间范围终点 */
  std::optional<std::string> pick_up_point              = std::nullopt; /* 接送点 附近(nearby), 上门(door to door) */
  std::optional<uint8_t>     people_number              = std::nullopt; /* 客容量 */
  std::optional<uint64_t>    goods_info_id              = std::nullopt; /* 货容量 -> 外键 : 货物信息 ID */
  std::optional<uint64_t>    car_id                     = std::nullopt; /* 车辆信息 */
  std::optional<std::string> append_info                = std::nullopt; /* 附加信息 , 最多 255 个字  */


  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FindCustomers,
                                              id,
                                              start_points_area_list,
                                              end_points_area_list,
                                              car_supply_form,
                                              departure_time_range_start,
                                              departure_time_range_end,
                                              pick_up_point,
                                              people_number,
                                              goods_info_id,
                                              car_id,
                                              append_info)

  /*  */
  bool inflate(const Names& names, const std::string& name, const mysqlx::Value& value);
  /* ANCHOR -> 获取主键值组成的列表 */
  virtual const std::vector<std::optional<std::string>>
  getPrimaryKeyValVector() const override { return {numOptionToStrOption(id)}; };
  /* ANCHOR - 只获取当前子类的 un primary key , 归并的动作交给 BaseTime::getUnPrimaryKeyValVector 完成 */
  std::vector<std::optional<std::string>>
  getSubUnPrimaryKeyValVector() const override {
    return {start_points_area_list,
            end_points_area_list,
            car_supply_form,
            departure_time_range_start,
            departure_time_range_end,
            pick_up_point,
            numOptionToStrOption(people_number),
            numOptionToStrOption(goods_info_id),
            numOptionToStrOption(car_id),
            append_info};
  }
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
  std::optional<uint8_t>  car_request        = std::nullopt; /* 车主发起绑定 */
  std::optional<uint8_t>  car_response       = std::nullopt; /* 车主相应绑定 */
  std::optional<uint8_t>  customers_request  = std::nullopt; /* 乘客发起绑定 */
  std::optional<uint8_t>  customers_response = std::nullopt; /* 乘客相应绑定 */


  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(CarBindCustomers,
                                              find_car_id,
                                              find_customers_id,
                                              car_request,
                                              car_response,
                                              customers_request,
                                              customers_response)

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


  std::optional<uint64_t>    id          = std::nullopt; /* 主键 id 自增 */
  std::optional<double>      size_length = std::nullopt; /* 长 */
  std::optional<double>      size_width  = std::nullopt; /* 宽 */
  std::optional<double>      size_height = std::nullopt; /* 高 */
  std::optional<std::string> size_unity  = std::nullopt; /* 尺寸单位(cm/m) */
  std::optional<double>      volume      = std::nullopt; /* 体积 */
  std::optional<std::string> volume_unit = std::nullopt; /* 体积单位(L/m³) */
  std::optional<double>      weight      = std::nullopt; /* 重量 */
  std::optional<std::string> weight_unit = std::nullopt; /* 重量单位(KG/T) */
  std::optional<uint64_t>    number      = std::nullopt; /* 件数 */
  std::optional<std::string> tag         = std::nullopt; /* 形态; 性质: 易碎/贵重/时效; 等等 */
  std::optional<std::string> desc        = std::nullopt; /* 货物描述信息 */


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
                                              desc)

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
        tag,
        desc,
    };
  }
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
  std::optional<uint8_t>      age                  = std::nullopt; /* 车龄 */
  std::optional<uint8_t>      passenger_capacity   = std::nullopt; /* 客容量(不含司机) */
  std::optional<double>      size_length          = std::nullopt; /* 长 */
  std::optional<double>      size_width           = std::nullopt; /* 宽 */
  std::optional<double>      size_height          = std::nullopt; /* 高 */
  std::optional<std::string> size_unity           = std::nullopt; /* 尺寸单位(cm/m) */
  std::optional<double>      capacity_volume      = std::nullopt; /* 货容量:体积 */
  std::optional<std::string> capacity_volume_unit = std::nullopt; /* 货容量单位:体积(L/m³) */
  std::optional<double>      capacity_weight      = std::nullopt; /* 货重量:重量 */
  std::optional<std::string> capacity_weight_unit = std::nullopt; /* 货重量单位:重量(KG/T) */
  std::optional<std::string> main_transport_type  = std::nullopt; /* 主力运输类型(goods/passenger) */

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Car,
                                              id,
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
                                              main_transport_type)

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

