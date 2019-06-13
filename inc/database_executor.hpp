#pragma once

#include <string>
#include <libpq-fe.h>
#include <query_result.hpp>
#include <memory>
#include <vector>

//serves as a wrapper around the low level C psql library
class database_executor
{

    PGconn* connection_;

  public:

    database_executor(const std::string& database, const std::string& login, const std::string& password);
    std::unique_ptr<query_result> exec_query(const std::string& query);
    std::unique_ptr<query_result> exec_query_variadic(const std::string& query, const std::vector<std::string>& args);
    ~database_executor();
};