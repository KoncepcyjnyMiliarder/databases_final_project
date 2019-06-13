#include <api_trolls.hpp>
#include <cassert>
#include <iostream>

api_trolls::api_trolls(nlohmann::json& data, database_executor& db)
  : abstract_api(data, db),
    timestamp_(fetch_number("timestamp"))
{
}

void api_trolls::handle()
{
  auto query_res = db_.exec_query_variadic("SELECT member_id, SUM(upvotes) AS upvote_sum, SUM(downvotes) AS downvote_sum, "
                   "is_member_active(member_id, $1) AS active FROM action "
                   "GROUP BY member_id ORDER BY (SUM(downvotes) - SUM(upvotes)) DESC, member_id", {std::to_string(timestamp_)});

  assert(query_res->column_count() == 4);
  assert(query_res->column_name(0) == "member_id");
  assert(query_res->column_name(1) == "upvote_sum");
  assert(query_res->column_name(2) == "downvote_sum");
  assert(query_res->column_name(3) == "active");

  //wypisz dzejsona z danymi
  nlohmann::json action_confirmation;
  action_confirmation["status"] = "OK";
  for (unsigned i = 0; i < query_res->tuple_count(); ++i)
  {
    // <member> <upvotes> <downvotes> <active>
    action_confirmation["data"][i] =
    {
      query_res->get_as_number(i, 0),
      query_res->get_as_number(i, 1),
      query_res->get_as_number(i, 2),
      query_res->get_as_boolean(i, 3) ? "true" : "false"
    };
  }
  std::cout << action_confirmation << std::endl;
}
