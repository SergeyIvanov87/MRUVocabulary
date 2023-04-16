#include "common/db/DBConnectionPool.h"

DBConnectionPool::DBConnectionPool(const std::string &host,
                                   const std::string &user,
                                   const std::string &password,
                                   const std::string &dbase,
                                   int port,
                                   size_t nbConnections )
{
    initialize(host, user,password, dbase, port, nbConnections);
}

void DBConnectionPool::initialize(const std::string &host,
                                  const std::string &user,
                                  const std::string &password,
                                  const std::string &dbase,
                                  int port,
                                  size_t nbConnections/* = 1*/)
{
    try
    {
        connections = std::make_unique<Pool>([&]()
                                             {
                                                 return createAndConnectDBConnection(host, user,
                                                                                     password, dbase,
                                                                                     port).release();
                                             }, nbConnections);
    }
    catch(DBException &e)
    {
        throw;
    }
    catch(std::exception &e)
    {
        throw;
    }
}

DBConnectionPool::Connection DBConnectionPool::getConnection()
{
    if (!connections)
    {
        throw DBException("DBConnectionPool is not initialized");
    }
    return connections->getObject();
}
