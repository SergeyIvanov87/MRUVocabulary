#ifndef DBEXCEPTION_H_
#define DBEXCEPTION_H_

#include <stdexcept>
#include "common/db/DBConnection.h"

class DBException : public std::runtime_error
{
public:
    DBException(std::string message, const char *reason = nullptr, DBConnection* con = nullptr)
      : std::runtime_error(message + (reason ? (std::string(". Reason: ") + reason) : "")),  connection(con)
    {}

    DBException(const char *message, const char *reason = nullptr, DBConnection* con = nullptr)
      : std::runtime_error(std::string(message) + (reason ? (std::string(". Reason: ") + reason) : "")),  connection(con)
    {}

    DBException & operator=(const DBException &) = delete;
    ~DBException() noexcept
    {
    }

    DBConnection* getConnection()
    {
        return connection;
    }
protected:
    DBConnection* connection;
};


#endif /* DBEXCEPTION_H_ */
