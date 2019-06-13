#include <query_result.hpp>
#include <stdexcept>

query_result::query_result(PGresult* res)
  : res_(res)
{
  auto status = PQresultStatus(res_);
  if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
  {
    std::runtime_error to_throw(PQresultErrorMessage(res_));
    PQclear(res_);
    throw to_throw; //woohoo i've never done something like that before
  }
}

query_result::~query_result()
{
  PQclear(res_);
}

unsigned query_result::tuple_count()
{
  return PQntuples(res_);
}

unsigned query_result::column_count()
{
  return PQnfields(res_);
}

bool query_result::is_null(int row, int col)
{
  return PQgetisnull(res_, row, col);
}

std::string query_result::get_as_string(int row, int col)
{
  return PQgetvalue(res_, row, col);
}

bool query_result::get_as_boolean(int row, int col)
{
  return PQgetvalue(res_, row, col)[0] == 't';
}

unsigned query_result::get_as_number(int row, int col)
{
  return std::stoi(PQgetvalue(res_, row, col));
}

std::string query_result::column_name(int col)
{
  return PQfname(res_, col);
}
