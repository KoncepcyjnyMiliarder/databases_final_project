#include <api_votes.hpp>
#include <cassert>
#include <iostream>

api_votes::api_votes(nlohmann::json& data, database_executor& db)
  : abstract_api(data, db),
    timestamp_(fetch_number("timestamp")),
    member_(fetch_number("member")),
    password_(fetch_string("password")),
    project_(fetch_number("project", &is_project_set_)),
    action_(fetch_number("action", &is_action_set_))
{
  if (is_project_set_ && is_action_set_)
    throw std::invalid_argument("You cannot set both project and action at the same time");
}

void api_votes::handle()
{
  //sprawdz czy czlonek jest liderem
  auto query_res = db_.exec_query_variadic("SELECT id FROM leader WHERE id = $1", { std::to_string(member_) });

  //jesli nie, konczymy bledem
  if (query_res->tuple_count() == 0)
    throw std::runtime_error("Member is not a leader");

  //sprawdz czy haslo jest ok
  query_res = db_.exec_query_variadic("SELECT id, last_activity FROM member WHERE id = $1 AND password_hash = crypt($2, password_hash)", { std::to_string(member_), password_ });
  if (query_res->tuple_count() == 0)
    throw std::runtime_error("User exists but password is wrong");

  assert(query_res->column_count() == 2);
  assert(query_res->column_name(0) == "id");
  assert(query_res->column_name(1) == "last_activity");

  //czy zamrozony
  if (timestamp_ - 31556926 > query_res->get_as_number(0, 1))
    throw std::runtime_error("User is frozen");

  //i cyk kwerenda
  if (is_action_set_)
    query_res = db_.exec_query_variadic("SELECT member.id, COUNT(is_upvote) FILTER (WHERE is_upvote AND action_id = $1) AS upvotes, "
                                        "COUNT(is_upvote) FILTER (WHERE NOT is_upvote AND action_id = $1) AS downvotes FROM member "
                                        "LEFT JOIN vote ON(voter_id = member.id) "
                                        "GROUP BY member.id ORDER BY member.id", { std::to_string(action_) });
  else if (is_project_set_)
    query_res = db_.exec_query_variadic("SELECT member.id, COUNT(is_upvote) FILTER (WHERE is_upvote AND project = $1) AS upvotes, "
                                        "COUNT(is_upvote) FILTER (WHERE NOT is_upvote AND project = $1) AS downvotes FROM member "
                                        "LEFT JOIN vote ON(voter_id = member.id) LEFT JOIN action USING(action_id) "
                                        "GROUP BY member.id ORDER BY member.id", { std::to_string(project_) });
  else
    query_res = db_.exec_query("SELECT member.id, COUNT(is_upvote) FILTER (WHERE is_upvote) AS upvotes, "
                               "COUNT(is_upvote) FILTER (WHERE NOT is_upvote) AS downvotes FROM member "
                               "LEFT JOIN vote ON(voter_id = member.id) GROUP BY member.id ORDER BY member.id");

  assert(query_res->column_count() == 3);
  assert(query_res->column_name(0) == "id");
  assert(query_res->column_name(1) == "upvotes");
  assert(query_res->column_name(2) == "downvotes");

  //aktualizacja timestampa ostatniej akcji uzytkownika
  db_.exec_query_variadic("UPDATE member SET last_activity = $2 WHERE id = $1", { std::to_string(member_), std::to_string(timestamp_) });

  //wypisz dzejsona z danymi
  nlohmann::json action_confirmation;
  action_confirmation["status"] = "OK";
  for (unsigned i = 0; i < query_res->tuple_count(); ++i)
  {
    // <member> <upvotes> <downvotes>
    action_confirmation["data"][i] =
    {
      query_res->get_as_number(i, 0),
      query_res->get_as_number(i, 1),
      query_res->get_as_number(i, 2)
    };
  }
  std::cout << action_confirmation << std::endl;
}
