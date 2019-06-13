#include <database_executor.hpp>
#include <iostream>
#include <stdexcept>
#include <algorithm>

database_executor::database_executor(const std::string& database, const std::string& login, const std::string& password)
{
  connection_ = PQsetdbLogin("localhost", //default host
                             nullptr, //default port
                             nullptr, //default optons
                             nullptr, //default debug output
                             database.c_str(),
                             login.c_str(),
                             password.c_str());

  if (PQstatus(connection_) != ConnStatusType::CONNECTION_OK)
    throw std::runtime_error(PQerrorMessage(connection_));
}

std::unique_ptr<query_result> database_executor::exec_query(const std::string& query)
{
  return std::make_unique<query_result>(PQexec(connection_, query.c_str()));
}

std::unique_ptr<query_result> database_executor::exec_query_variadic(const std::string& query, const std::vector<std::string>& args)
{
  std::vector<const char*> args_as_raw(args.size());
  std::transform(args.begin(), args.end(), args_as_raw.begin(), [](const std::string & arg)
  {
    return arg.c_str();
  });
  return std::make_unique<query_result>(PQexecParams(connection_,
                                        query.c_str(), //"If parameters are used, they are referred to in the command string as $1, $2, etc."
                                        static_cast<int>(args.size()),
                                        nullptr, //"If paramTypes is NULL [..] the server infers a data type"
                                        args_as_raw.data(),
                                        nullptr, //"It is ignored for null parameters and text-format parameters"
                                        nullptr, //"If the array pointer is null then all parameters are presumed to be text strings"
                                        0)); //"Specify zero to obtain results in text format"
}

database_executor::~database_executor()
{
  PQfinish(connection_);
}
