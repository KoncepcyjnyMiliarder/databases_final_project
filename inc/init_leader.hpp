#pragma once

#include <abstract_api.hpp>

class init_leader
  : public abstract_api
{
    unsigned timestamp_;
    unsigned member_;
    std::string password_;

  public:

    init_leader(nlohmann::json& data, database_executor& db);

    virtual void handle() override;
};