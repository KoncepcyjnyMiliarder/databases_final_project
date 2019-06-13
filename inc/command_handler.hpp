#pragma once

#include <database_executor.hpp>
#include <abstract_api.hpp>

class command_handler
{
  protected:

    database_executor& db_;
    command_handler(database_executor& db)
      : db_(db) {}

  public:

    virtual abstract_api::pointer from_input_line(const std::string& input_line) = 0;
    virtual ~command_handler() = default;
};