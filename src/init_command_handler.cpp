#include <init_command_handler.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <init_leader.hpp>

init_command_handler::init_command_handler(database_executor& db)
  : command_handler(db)
{
  //check is init.sql file is accessible
  std::ifstream filein("resources/init.sql");
  if (!filein)
    throw std::runtime_error("resources/init.sql - file is missing");

  //execute init.sql
  std::stringstream ss;
  ss << filein.rdbuf();
  try
  {
    db.exec_query(ss.str());
  }
  catch (const std::exception& e)
  {
    throw std::runtime_error(std::string("init.sql - execution failed, error msg: ") + e.what());
  }
}

abstract_api::pointer init_command_handler::from_input_line(const std::string& input_line)
{
  nlohmann::json command_data = nlohmann::json::parse(input_line);
  if(command_data.size() != 1)
    throw std::invalid_argument("Every line should contain exactly one json object");
  const std::string& api_name = command_data.begin().key();
  if (api_name == "leader")
    return std::make_unique<init_leader>(command_data[api_name], db_);

  nlohmann::json error_json;
  error_json["status"] = "ERROR";
  error_json["debug"] = "No handler for api: " + api_name;
  std::cout << error_json << std::endl;
  return abstract_api::pointer();
}
