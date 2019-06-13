#include <nlohmann/json.hpp>
#include <database_executor.hpp>
#include <command_handler.hpp>
#include <init_command_handler.hpp>
#include <api_command_handler.hpp>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  try
  {
    //first line has to be "open" command
    std::string input_line;
    std::getline(std::cin, input_line);
    nlohmann::json db_init_args = nlohmann::json::parse(input_line);
    if (db_init_args["open"].empty())
      throw std::invalid_argument("First line should be \"open\" command");

    database_executor db{ db_init_args["open"]["database"], db_init_args["open"]["login"], db_init_args["open"]["password"] };

    nlohmann::json confirmation;
    confirmation["status"] = "OK";
    confirmation["debug"] = "opening DB connection succeed";
    std::cout << confirmation << std::endl;

    //instantiation of concrete factory used to translate jsons to command handlers
    std::unique_ptr<command_handler> handler;
    if (argc >= 2 && argv[1] == std::string("--init"))
      handler = std::make_unique<init_command_handler>(db);
    else
      handler = std::make_unique<api_command_handler>(db);

    //reading line after line
    while (std::getline(std::cin, input_line))
    {
      try
      {
        //try to translate json to handler
        auto api = handler->from_input_line(input_line);
        //if succesfull, execute associated handler
        if (api)
          api->handle();
      }
      catch (const std::exception& e)
      {
        nlohmann::json error_json;
        error_json["status"] = "ERROR";
        error_json["debug"] = e.what();
        std::cout << error_json << std::endl;
      }
    }
  }
  catch (const std::exception& e)
  {
    nlohmann::json error_json;
    error_json["status"] = "ERROR";
    error_json["debug"] = e.what();
    std::cout << error_json << std::endl;
    return 1;
  }
  return 0;
}
