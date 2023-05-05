#ifndef DB_PLUGIN_H
#define DB_PLUGIN_H

#define DB_PLUGIN_NAME         "DB"
#define DB_CURRENT_VERSION     0

#include <map>
#include <string>
#include <unordered_map>
#include "OutputExport.h"
#include "CommandArguments.h"

#include "common/db/DBConnectionPool.h"

struct db_context_v0
{
    std::unique_ptr<DBConnectionPool> connectionPool;
    std::unique_ptr<CommandArgumentsBase> property_holder;
};

#endif //DB_PLUGIN_H
