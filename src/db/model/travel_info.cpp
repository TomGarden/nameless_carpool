//
// Created by Tom on 2023/3/24.
//

#include "travel_info.h"

namespace nameless_carpool {
  bool FindBase::inflate(const FindBase::Names& names, const std::string& name, const mysqlx::Value& value) {
    if /**/ (name == names.id)/*                        */ this->id = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.start_point_longitude)/*     */ this->start_point_longitude = SqlUtil::getOptional<double>(value);
    else if (name == names.start_point_latitude)/*      */ this->start_point_latitude = SqlUtil::getOptional<double>(value);
    else if (name == names.start_point_json)/*          */ this->start_point_json = SqlUtil::getOptional<nlohmann::json>(value);
    else if (name == names.end_point_longitude)/*       */ this->end_point_longitude = SqlUtil::getOptional<double>(value);
    else if (name == names.end_point_latitude)/*        */ this->end_point_latitude = SqlUtil::getOptional<double>(value);
    else if (name == names.end_point_json)/*            */ this->end_point_json = SqlUtil::getOptional<nlohmann::json>(value);
    else if (name == names.service_form)/*              */ this->service_form = SqlUtil::getOptional<std::string>(value);
    else if (name == names.departure_time_range_start)/**/ this->departure_time_range_start = SqlUtil::getOptional<std::string>(value);
    else if (name == names.departure_time_range_end)/*  */ this->departure_time_range_end = SqlUtil::getOptional<std::string>(value);
    else if (name == names.pick_up_point)/*             */ this->pick_up_point = SqlUtil::getOptional<std::string>(value);
    else if (name == names.intent)/*                    */ this->intent = SqlUtil::getOptional<std::string>(value);
    else if (name == names.people_number)/*             */ this->people_number = SqlUtil::getOptional<uint>(value);
    else if (name == names.goods_info_id)/*             */ this->goods_info_id = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.append_info)/*               */ this->append_info = SqlUtil::getOptional<std::string>(value);
    BASE_TIME_INFLATE
    return true;
  }

  bool UserFindCar::inflate(const UserFindCar::Names& names, const std::string& name, const mysqlx::Value& value) {
    if /**/ (name == names.user_id) this->user_id = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.find_car_id) this->find_car_id = SqlUtil::getOptional<uint64_t>(value);
    BASE_TIME_INFLATE

    return true;
  }

  bool FindCustomers::inflate(const FindCustomers::Names& names, const std::string& name, const mysqlx::Value& value) {
    if (name == names.car_id) {
      this->car_id = SqlUtil::getOptional<uint64_t>(value);
      return true;
    } else return FindBase::inflate(names,name,value);
  }

  bool UserFindCustomers::inflate(const UserFindCustomers::Names& names, const std::string& name, const mysqlx::Value& value) {
    if /**/ (name == names.user_id) this->user_id = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.find_customers_id) this->find_customers_id = SqlUtil::getOptional<uint64_t>(value);
    BASE_TIME_INFLATE

    return true;
  }

  bool CarBindCustomers::inflate(const CarBindCustomers::Names& names, const std::string& name, const mysqlx::Value& value) {
    if /**/ (name == names.find_car_id) /*       */
      this->find_car_id = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.find_customers_id) /* */ this->find_customers_id = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.car_request) /*       */ this->car_request = SqlUtil::getOptional<uint>(value);
    else if (name == names.car_response) /*      */ this->car_response = SqlUtil::getOptional<uint>(value);
    else if (name == names.customers_request) /* */ this->customers_request = SqlUtil::getOptional<uint>(value);
    else if (name == names.customers_response) /**/ this->customers_response = SqlUtil::getOptional<uint>(value);
    BASE_TIME_INFLATE

    return true;
  }

  bool GoodsInfo::inflate(const GoodsInfo::Names& names, const std::string& name, const mysqlx::Value& value) {
    if /**/ (name == names.id /*         */) this->id /*         */ = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.size_length /**/) this->size_length /**/ = SqlUtil::getOptional<double>(value);
    else if (name == names.size_width /* */) this->size_width /* */ = SqlUtil::getOptional<double>(value);
    else if (name == names.size_height /**/) this->size_height /**/ = SqlUtil::getOptional<double>(value);
    else if (name == names.size_unity /* */) this->size_unity /* */ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.volume /*     */) this->volume /*     */ = SqlUtil::getOptional<double>(value);
    else if (name == names.volume_unit /**/) this->volume_unit /**/ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.weight /*     */) this->weight /*     */ = SqlUtil::getOptional<double>(value);
    else if (name == names.weight_unit /**/) this->weight_unit /**/ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.number /*     */) this->number /*     */ = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.tag /*        */) this->tag /*        */ = SqlUtil::getOptional<nlohmann::json>(value);
    else if (name == names.desc /*       */) this->desc /*       */ = SqlUtil::getOptional<std::string>(value);

    BASE_TIME_INFLATE

    return true;
  }

  bool UserGoodsInfo::inflate(const UserGoodsInfo::Names& names, const std::string& name, const mysqlx::Value& value) {
    if /**/ (name == names.user_id) this->user_id = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.goods_info_id) this->goods_info_id = SqlUtil::getOptional<uint64_t>(value);
    BASE_TIME_INFLATE

    return true;
  }

  bool Car::inflate(const Car::Names& names, const std::string& name, const mysqlx::Value& value) {
    if /**/ (name == names.id /*                  */) this->id /*                  */ = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.plate /*               */) this->plate /*               */ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.brand /*               */) this->brand /*               */ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.model /*               */) this->model /*               */ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.age /*                 */) this->age /*                 */ = SqlUtil::getOptional<uint>(value);
    else if (name == names.passenger_capacity /*  */) this->passenger_capacity /*  */ = SqlUtil::getOptional<uint>(value);
    else if (name == names.size_length /*         */) this->size_length /*         */ = SqlUtil::getOptional<double>(value);
    else if (name == names.size_width /*          */) this->size_width /*          */ = SqlUtil::getOptional<double>(value);
    else if (name == names.size_height /*         */) this->size_height /*         */ = SqlUtil::getOptional<double>(value);
    else if (name == names.size_unity /*          */) this->size_unity /*          */ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.capacity_volume /*     */) this->capacity_volume /*     */ = SqlUtil::getOptional<double>(value);
    else if (name == names.capacity_volume_unit /**/) this->capacity_volume_unit /**/ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.capacity_weight /*     */) this->capacity_weight /*     */ = SqlUtil::getOptional<double>(value);
    else if (name == names.capacity_weight_unit /**/) this->capacity_weight_unit /**/ = SqlUtil::getOptional<std::string>(value);
    else if (name == names.main_transport_type /* */) this->main_transport_type /* */ = SqlUtil::getOptional<std::string>(value);

    BASE_TIME_INFLATE

    return true;
  }

  bool UserCar::inflate(const UserCar::Names& names, const std::string& name, const mysqlx::Value& value) {
    if /**/ (name == names.user_id) this->user_id = SqlUtil::getOptional<uint64_t>(value);
    else if (name == names.car_id) this->car_id = SqlUtil::getOptional<uint64_t>(value);
    BASE_TIME_INFLATE

    return true;
  }

}  // namespace nameless_carpool
