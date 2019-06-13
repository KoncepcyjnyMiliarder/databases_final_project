#pragma once

#include <abstract_api.hpp>

class api_actions
  : public abstract_api
{
    unsigned timestamp_;
    unsigned member_;
    std::string password_;
    bool is_type_set_;
    std::string type_;
    bool is_project_set_;
    unsigned project_;
    bool is_authority_set_;
    unsigned authority_;

  public:

    api_actions(nlohmann::json& data, database_executor& db);

    virtual void handle() override;
};