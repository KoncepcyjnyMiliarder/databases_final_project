#pragma once

#include <nlohmann/json.hpp>
#include <database_executor.hpp>
#include <memory>

class abstract_api
{

    nlohmann::json& data_;

  protected:

    database_executor& db_;

    /*welll not the cleanest way with that raw bool pointer, but as a protected helper method it should do...*/
    inline unsigned fetch_number(const std::string& key_name, bool* optional = nullptr)
    {
      auto iter = data_.find(key_name);
      if (iter == data_.end())
      {
        if (optional)
        {
          *optional = false;
          return 0;
        }
        throw std::invalid_argument("Expected " + key_name + " in json");
      }
      if (!iter->is_number())
        throw std::invalid_argument("Expected " + key_name + " to be number");
      if (optional)
        *optional = true;
      return iter->get<unsigned>();
    }

    inline std::string fetch_string(const std::string& key_name, bool* optional = nullptr)
    {
      auto iter = data_.find(key_name);
      if (iter == data_.end())
      {
        if (optional)
        {
          *optional = false;
          return {};
        }
        throw std::invalid_argument("Expected " + key_name + " in json");
      }
      if (!iter->is_string())
        throw std::invalid_argument("Expected " + key_name + " to be string");
      if (optional)
        *optional = true;
      return iter->get<std::string>();
    }

  public:

    using pointer = std::unique_ptr<abstract_api>;

    abstract_api(nlohmann::json& data, database_executor& db)
      : data_(data), db_(db) {}
    virtual ~abstract_api() = default;

    virtual void handle() = 0;
};