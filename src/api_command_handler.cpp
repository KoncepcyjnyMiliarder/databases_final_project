#include <api_command_handler.hpp>
#include <iostream>
#include <api_support.hpp>
#include <api_upvote.hpp>
#include <api_actions.hpp>
#include <api_projects.hpp>
#include <api_votes.hpp>
#include <api_trolls.hpp>

api_command_handler::api_command_handler(database_executor& db)
  : command_handler(db)
{
}

abstract_api::pointer api_command_handler::from_input_line(const std::string& input_line)
{
  nlohmann::json command_data = nlohmann::json::parse(input_line);
  if(command_data.size() != 1)
    throw std::invalid_argument("Every line should contain exactly one json object");
  const std::string& api_name = command_data.begin().key();
  //maybe should've used some hashmap
  if (api_name == "support")
    return std::make_unique<api_support>(command_data[api_name], db_, true);
  if (api_name == "protest")
    return std::make_unique<api_support>(command_data[api_name], db_, false);
  if (api_name == "upvote")
    return std::make_unique<api_upvote>(command_data[api_name], db_, true);
  if (api_name == "downvote")
    return std::make_unique<api_upvote>(command_data[api_name], db_, false);
  if (api_name == "actions")
    return std::make_unique<api_actions>(command_data[api_name], db_);
  if (api_name == "projects")
    return std::make_unique<api_projects>(command_data[api_name], db_);
  if (api_name == "votes")
    return std::make_unique<api_votes>(command_data[api_name], db_);
  if (api_name == "trolls")
    return std::make_unique<api_trolls>(command_data[api_name], db_);

  nlohmann::json error_json;
  error_json["status"] = "ERROR";
  error_json["debug"] = "No handler for api: " + api_name;
  std::cout << error_json << std::endl;
  return abstract_api::pointer();
}
