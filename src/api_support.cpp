#include <api_support.hpp>
#include <cassert>
#include <iostream>

api_support::api_support(nlohmann::json& data, database_executor& db, bool is_support_or_protest)
  : abstract_api(data, db),
    timestamp_(fetch_number("timestamp")),
    member_(fetch_number("member")),
    password_(fetch_string("password")),
    action_(fetch_number("action")),
    project_(fetch_number("project")),
    authority_(fetch_number("authority", &authority_present_)),
    is_support_or_protest_(is_support_or_protest)
{
}

void api_support::handle()
{
  //sprawdz czy czlonek istnieje
  auto query_res = db_.exec_query_variadic("SELECT id FROM member WHERE id = $1", { std::to_string(member_) });

  //jesli nie, dodaj
  if(query_res->tuple_count() == 0)
    db_.exec_query_variadic("INSERT INTO member(id, password_hash, last_activity) VALUES($1, crypt($2, gen_salt('bf')), $3)",
  {
    std::to_string(member_),
    password_,
    std::to_string(timestamp_)
  });
  else
  {
    //jesli tak, sprawdz czy haslo jest ok
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

    //jesli tak, czy zamrozony
    if(timestamp_ - 31556926 > query_res->get_as_number(0, 1))
      throw std::runtime_error("User is frozen");
  }

  //czy project bylo juz dodane?
  query_res = db_.exec_query_variadic("SELECT project_id FROM project WHERE project_id = $1", { std::to_string(project_)});
  if (query_res->tuple_count() == 0)
  {
    if (authority_present_)
      db_.exec_query_variadic("INSERT INTO project(project_id, authority) VALUES($1, $2)",
    {
      std::to_string(project_),
      std::to_string(authority_)
    });
    else
      throw std::runtime_error("Project does not exist and authority was not set");
  }

  //wstawiamy nowa krotke do tablicy actions
  db_.exec_query_variadic("INSERT INTO action(action_id, member_id, project, action_time, is_support) "
                          "VALUES($1, $2, $3, $4, $5)",
  {
    std::to_string(action_),
    std::to_string(member_),
    std::to_string(project_),
    std::to_string(timestamp_),
    is_support_or_protest_ ? "TRUE" : "FALSE"
  });

  //na koniec apdejtnij mu stampa
  db_.exec_query_variadic("UPDATE member SET last_activity = $2 WHERE id = $1", { std::to_string(member_), std::to_string(timestamp_) });

  //wypisz dzejsona
  nlohmann::json action_confirmation;
  action_confirmation["status"] = "OK";
  std::cout << action_confirmation << std::endl;
}
