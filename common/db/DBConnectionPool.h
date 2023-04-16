#ifndef DBCONNECTIONPOOL_H_
#define DBCONNECTIONPOOL_H_

#include <vector>
#include <condition_variable>
#include <list>
#include <mutex>
#include <stack>

#include <Framework/Utils/Pools/CommonObjectPool.h>

#include "common/db/DBConnection.h"
#include "common/db/DBException.h"

class DBConnectionPool
{
public:
    using Pool = ObjectPool<DBConnection, WaitLockPolicy>;
    using Connection = typename Pool::NativeObjectPoolItemWrapper;

    DBConnectionPool(const std::string &host,
                    const std::string &user,
                    const std::string &password,
                    const std::string &dbase,
                    int port,
                    size_t nbConnections = 1);

    Connection getConnection();
private:
    DBConnectionPool() = default;
    void initialize(const std::string &host,
                    const std::string &user,
                    const std::string &password,
                    const std::string &dbase,
                    int port,
                    size_t nbConnections = 1);
    std::unique_ptr<Pool> connections;
};



#endif /* DBCONNECTIONPOOL_H_ */
