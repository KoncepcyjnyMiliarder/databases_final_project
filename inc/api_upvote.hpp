#pragma once

#include <abstract_api.hpp>

class api_upvote
  : public abstract_api
{
    unsigned timestamp_;
    unsigned member_;
    std::string password_;
    unsigned action_;
    bool is_upvote_or_downvote_;

  public:

    api_upvote(nlohmann::json& data, database_executor& db, bool is_upvote_or_downvote);

    virtual void handle() override;
};