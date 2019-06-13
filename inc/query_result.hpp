#pragma once

#include <string>
#include <libpq-fe.h>

//raii
//encapsulates the result of query
class query_result
{

    PGresult* res_;

  public:

    query_result(PGresult* res);
    ~query_result();

    unsigned tuple_count();
    unsigned column_count();
    bool is_null(int row, int col);
    std::string get_as_string(int row, int col);
    bool get_as_boolean(int row, int col);
    unsigned get_as_number(int row, int col);
    std::string column_name(int col);
};