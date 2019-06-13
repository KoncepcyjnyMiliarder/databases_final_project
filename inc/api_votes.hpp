#pragma once

#include <abstract_api.hpp>

class api_votes
  : public abstract_api
{
    unsigned timestamp_;
    unsigned member_;
    std::string password_;
    bool is_project_set_;
    unsigned project_;
    bool is_action_set_;
    unsigned action_;

  public:

    api_votes(nlohmann::json& data, database_executor& db);

    virtual void handle() override;
};