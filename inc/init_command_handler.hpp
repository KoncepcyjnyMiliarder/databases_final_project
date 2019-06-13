#pragma once

#include <command_handler.hpp>

class init_command_handler
  : public command_handler
{
  public:

    init_command_handler(database_executor& db);

    virtual abstract_api::pointer from_input_line(const std::string& input_line) override;
};