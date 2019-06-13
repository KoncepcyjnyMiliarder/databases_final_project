#pragma once

#include <abstract_api.hpp>

class api_projects
  : public abstract_api
{
    unsigned timestamp_;
    unsigned member_;
    std::string password_;
    bool is_authority_set_;
    unsigned authority_;

  public:

    api_projects(nlohmann::json& data, database_executor& db);

    virtual void handle() override;
};