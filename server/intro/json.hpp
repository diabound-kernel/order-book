#pragma once

#include "nlohmann/json.hpp"

#include "metadata.hpp"

#include <iostream>

template<metadata::CanGetFields Object>
void to_json(nlohmann::json &j, const Object &obj)
{
    metadata::forEachField(obj, [&](auto field) {
        j[std::get<metadata::Field::Name>(field)] =
            std::get<metadata::Field::Invoke>(field)(obj);
    });
}

template<metadata::CanGetFields Object>
void from_json(const nlohmann::json &j, Object &obj)
{
    metadata::forEachField(obj, [&](auto field) {
        j.at(std::get<metadata::Field::Name>(field))
            .get_to(std::get<metadata::Field::Ref>(field));
    });
}

template<typename T>
T decodeJSON(const std::string &json)
{
    auto parsed = ::nlohmann::json::parse(json);

    return parsed[metadata::NameOf<T>::get()].template get<T>();
}

template<metadata::CanGetFields T>
std::ostream &operator<<(std::ostream &out, const T &t)
{
    std::cout << ::nlohmann::json(t).dump();
    return out;
}