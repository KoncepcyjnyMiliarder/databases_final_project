#include <api_actions.hpp>
#include <cassert>
#include <iostream>

api_actions::api_actions(nlohmann::json& data, database_executor& db)
  : abstract_api(data, db),
    timestamp_(fetch_number("timestamp")),
    member_(fetch_number("member")),
    password_(fetch_string("password")),
    type_(fetch_string("type", &is_type_set_)),
    project_(fetch_number("project", &is_project_set_)),
    authority_(fetch_number("authority", &is_authority_set_))
{
  if (is_type_set_ && type_ != "support" && type_ != "protest")
    throw std::invalid_argument("Type has been set, but not to protest or support");
  if (is_project_set_ && is_authority_set_)
    throw std::invalid_argument("You cannot set both project and authority at the same time");
}

void api_actions::handle()
{
  //sprawdz czy czlonek jest liderem
  auto query_res = db_.exec_query_variadic("SELECT id FROM leader WHERE id = $1", { std::to_string(member_) });

  //jesli nie, konczymy bledem
  if (query_res->tuple_count() == 0)
    throw std::runtime_error("Member is not a leader");

  //sprawdz czy haslo jest ok
  query_res = db_.exec_query_variadic("SELECT id, last_activity FROM member WHERE id = $1 AND password_hash = crypt($2, password_hash)",
  {
    std::to_string(member_),
    password_
  });
  if (query_res->tuple_count() == 0)
    throw std::runtime_error("User exists but password is wrong");

  assert(query_res->column_count() == 2);
  assert(query_res->column_name(0) == "id");
  assert(query_res->column_name(1) == "last_activity");

  //czy zamrozony
  if (timestamp_ - 31556926 > query_res->get_as_number(0, 1))
    throw std::runtime_error("User is frozen");

  //i cyk kwerenda
  std::string built_query = "SELECT action_id, is_support, project, authority, COUNT(is_upvote) FILTER (WHERE is_upvote) AS upvotes, "
                            "COUNT(is_upvote) FILTER (WHERE NOT is_upvote) AS downvotes FROM action JOIN project ON(project = project_id) "
                            "JOIN vote USING (action_id)";

  if (is_type_set_ || is_project_set_ || is_authority_set_)
    built_query += " WHERE";

  if (is_type_set_)
  {
    built_query += (type_ == "support" ? " is_support = TRUE" : " is_support = FALSE");
    if (is_project_set_ || is_authority_set_)
      built_query += " AND";
  }

  if (is_project_set_)
    built_query += " project = " + std::to_string(project_);
  else if (is_authority_set_)
    built_query += " authority = " + std::to_string(authority_);

  built_query += " GROUP BY action_id, is_support, project, authority ORDER BY action_id";

  query_res = db_.exec_query(built_query);

  assert(query_res->column_count() == 6);
  assert(query_res->column_name(0) == "action_id");
  assert(query_res->column_name(1) == "is_support");
  assert(query_res->column_name(2) == "project");
  assert(query_res->column_name(3) == "authority");
  assert(query_res->column_name(4) == "upvotes");
  assert(query_res->column_name(5) == "downvotes");

  //aktualizacja timestampa ostatniej akcji uzytkownika
  db_.exec_query_variadic("UPDATE member SET last_activity = $2 WHERE id = $1", { std::to_string(member_), std::to_string(timestamp_) });

  //wypisz dzejsona z danymi
  nlohmann::json action_confirmation;
  action_confirmation["status"] = "OK";
  for (unsigned i = 0; i < query_res->tuple_count(); ++i)
  {
    // <action> <type> <project> <authority> <upvotes> <downvotes>
    action_confirmation["data"][i] =
    {
      query_res->get_as_number(i, 0),
      query_res->get_as_boolean(i, 1) ? "support" : "protest",
      query_res->get_as_number(i, 2),
      query_res->get_as_number(i, 3),
      query_res->get_as_number(i, 4),
      query_res->get_as_number(i, 5),
    };
  }
  std::cout << action_confirmation << std::endl;
}
