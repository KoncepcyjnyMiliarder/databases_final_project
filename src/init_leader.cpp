#include <init_leader.hpp>
#include <cassert>
#include <iostream>

init_leader::init_leader(nlohmann::json& data, database_executor& db)
  : abstract_api(data, db),
    timestamp_(fetch_number("timestamp")),
    member_(fetch_number("member")),
    password_(fetch_string("password"))
{
  if (password_.empty())
    throw std::invalid_argument("Leader's password cannot be empty");
  if (password_.size() > 128)
    throw std::invalid_argument("Leader's password cannot be longer that 128 chars");
}

void init_leader::handle()
{
  auto res = db_.exec_query_variadic("SELECT make_leader($1, $2, $3)",
  {
    std::to_string(member_),
    password_,
    std::to_string(timestamp_)
  });

  nlohmann::json insertion_confirmation;
  insertion_confirmation["status"] = "OK";
  std::cout << insertion_confirmation << std::endl;
}
