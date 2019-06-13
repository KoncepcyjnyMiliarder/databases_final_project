#include <api_projects.hpp>
#include <cassert>
#include <iostream>

api_projects::api_projects(nlohmann::json& data, database_executor& db)
  : abstract_api(data, db),
    timestamp_(fetch_number("timestamp")),
    member_(fetch_number("member")),
    password_(fetch_string("password")),
    authority_(fetch_number("authority", &is_authority_set_))
{
}

void api_projects::handle()
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
  std::string built_query = "SELECT * FROM project";

  if (is_authority_set_)
    built_query += " WHERE authority = " + std::to_string(authority_);

  query_res = db_.exec_query(built_query);

  assert(query_res->column_count() == 2);
  assert(query_res->column_name(0) == "project_id");
  assert(query_res->column_name(1) == "authority");

  //aktualizacja timestampa ostatniej akcji uzytkownika
  db_.exec_query_variadic("UPDATE member SET last_activity = $2 WHERE id = $1", { std::to_string(member_), std::to_string(timestamp_) });

  //wypisz dzejsona z danymi
  nlohmann::json action_confirmation;
  action_confirmation["status"] = "OK";
  for (unsigned i = 0; i < query_res->tuple_count(); ++i)
  {
    // <project> <authority>
    action_confirmation["data"][i] =
    {
      query_res->get_as_number(i, 0),
      query_res->get_as_number(i, 1)
    };
  }
  std::cout << action_confirmation << std::endl;
}
