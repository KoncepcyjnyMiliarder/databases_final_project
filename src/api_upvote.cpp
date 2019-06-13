#include <api_upvote.hpp>
#include <cassert>
#include <iostream>

api_upvote::api_upvote(nlohmann::json& data, database_executor& db, bool is_upvote_or_downvote)
  : abstract_api(data, db),
    timestamp_(fetch_number("timestamp")),
    member_(fetch_number("member")),
    password_(fetch_string("password")),
    action_(fetch_number("action")),
    is_upvote_or_downvote_(is_upvote_or_downvote)
{
}

void api_upvote::handle()
{
  //sprawdz czy czlonek istnieje
  auto query_res = db_.exec_query_variadic("SELECT id FROM member WHERE id = $1", { std::to_string(member_) });

  //jesli nie, dodaj
  if (query_res->tuple_count() == 0)
    db_.exec_query_variadic("INSERT INTO member(id, password_hash, last_activity) VALUES($1, crypt($2, gen_salt('bf')), $3)",
  {
    std::to_string(member_),
    password_,
    std::to_string(timestamp_)
  });
  else
  {
    //jesli tak, sprawdz czy haslo jest ok
    query_res = db_.exec_query_variadic("SELECT id, last_activity FROM member WHERE id = $1 AND password_hash = crypt($2, password_hash)", { std::to_string(member_), password_ });
    if (query_res->tuple_count() == 0)
      throw std::runtime_error("User exists but password is wrong");

    assert(query_res->column_count() == 2);
    assert(query_res->column_name(0) == "id");
    assert(query_res->column_name(1) == "last_activity");

    //jesli tak, czy zamrozony
    if (timestamp_ - 31556926 > query_res->get_as_number(0, 1))
      throw std::runtime_error("User is frozen");
  }
  //czy akcja istnieje?
  query_res = db_.exec_query_variadic("SELECT action_id FROM action WHERE action_id = $1", { std::to_string(action_) });
  if (query_res->tuple_count() == 0)
    throw std::runtime_error("Action does not exist");

  //czy ten czlonek juz glosowal?
  query_res = db_.exec_query_variadic("SELECT voter_id FROM vote WHERE voter_id = $1 AND action_id = $2",
  {
    std::to_string(member_),
    std::to_string(action_)
  });
  if (query_res->tuple_count() != 0)
  {
    assert(query_res->column_count() == 1);
    throw std::runtime_error("Already voted before");
  }

  //wstawiamy nowa krotke do tablicy vote
  db_.exec_query_variadic("INSERT INTO vote(voter_id, action_id, voting_time, is_upvote) "
                          "VALUES($1, $2, $3, $4)",
  {
    std::to_string(member_),
    std::to_string(action_),
    std::to_string(timestamp_),
    is_upvote_or_downvote_ ? "TRUE" : "FALSE"
  });
  //aktualizujemy liczniki glosow w tabeli akcji
  if(is_upvote_or_downvote_)
    db_.exec_query_variadic("UPDATE action SET upvotes = upvotes + 1 "
                            "WHERE action_id = $1", { std::to_string(action_) });
  else
    db_.exec_query_variadic("UPDATE action SET downvotes = downvotes + 1 "
                            "WHERE action_id = $1", { std::to_string(action_) });

  //aktualizacja timestampa ostatniej akcji uzytkownika
  db_.exec_query_variadic("UPDATE member SET last_activity = $2 WHERE id = $1", { std::to_string(member_), std::to_string(timestamp_) });

  //wypisz dzejsona
  nlohmann::json action_confirmation;
  action_confirmation["status"] = "OK";
  std::cout << action_confirmation << std::endl;
}
