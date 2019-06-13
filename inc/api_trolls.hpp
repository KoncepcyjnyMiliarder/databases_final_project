#pragma once

#include <abstract_api.hpp>

class api_trolls
  : public abstract_api
{
    unsigned timestamp_;

  public:

    api_trolls(nlohmann::json& data, database_executor& db);

    virtual void handle() override;
};