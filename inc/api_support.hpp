#pragma once

#include <abstract_api.hpp>

class api_support
  : public abstract_api
{
    unsigned timestamp_;
    unsigned member_;
    std::string password_;
    unsigned action_;
    unsigned project_;
    bool authority_present_;
    unsigned authority_;
    bool is_support_or_protest_;

  public:

    api_support(nlohmann::json& data, database_executor& db, bool is_support_or_protest);

    virtual void handle() override;
};