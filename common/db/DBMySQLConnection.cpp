#include <cassert>
#include <mysql.h>
#include <errmsg.h>

#include "common/db/DBException.h"

using namespace std;


class DBMySqlReader;

class DBMySQLConnection : public DBConnection
{
public:
    DBMySQLConnection();
    DBMySQLConnection(const std::string &host, const std::string &user, const std::string &pass, const std::string &name, uint port = 0);
    DBMySQLConnection(const DBMySQLConnection &) = delete;
    DBMySQLConnection & operator=(const DBMySQLConnection &) = delete;

    virtual ~DBMySQLConnection();

    void connect(const std::string &host, const std::string &user, const std::string &pass,
                 const std::string &name, int port = 0) override;
    void disconnect();

    virtual bool connected() const {return _connected;}

    virtual bool executeQuery(std::string & query);
    virtual bool executeQuery(const char * str);
    virtual unsigned long long lastInsertedId();
    virtual unsigned int affectedRows();

    virtual std::string escapeString(const std::string & str);
    virtual std::string escapeString(const char * str);

    virtual DBReaderInterface *reader();
    virtual void clear();

    virtual std::string getError();
    virtual unsigned int getErrno();
    virtual int ping() { return mysql_ping(_connection); }


protected:

    void needToSetSQLVariables(bool flag)
    {
        _needToSetSQLVariables = flag;
    }

    bool needToSetSQLVariables()
    {
        return _needToSetSQLVariables;
    }

    void setSQLVariables();
private:
    MYSQL * _connection;
    bool    _needToSetSQLVariables;

    friend class DBMySqlReader;

    static int nb_inst;
};
int DBMySQLConnection::nb_inst = 0;


class DBMySqlReader : public DBReaderInterface
{
public:
    DBMySqlReader();
    DBMySqlReader(DBMySQLConnection * connection);
    DBMySqlReader(const DBMySqlReader & reader);
    ~DBMySqlReader();

    DBMySqlReader & operator =(const DBMySqlReader & reader);

    unsigned int columnsCount();
    unsigned int rowsCount();

    bool isValid();
    bool isValidLite();
    bool operator ()();

    std::string operator [] (unsigned int index);
    unsigned long getBytes(unsigned int index, char* buffer, unsigned long len);
    unsigned long getLength(unsigned int index);

    std::string getError();
    unsigned int getErrno();

    DBReaderInterface & operator >> (std::string & value);
    DBReaderInterface & operator >> (NullValue & value);
    DBReaderInterface & operator >> (char & value);
    DBReaderInterface & operator >> (unsigned char & value);
    DBReaderInterface & operator >> (short & value);
    DBReaderInterface & operator >> (unsigned short & value);
    DBReaderInterface & operator >> (int & value);
    DBReaderInterface & operator >> (unsigned int & value);
    DBReaderInterface & operator >> (long & value);
    DBReaderInterface & operator >> (unsigned long & value);
    DBReaderInterface & operator >> (long long & value);
    DBReaderInterface & operator >> (unsigned long long & value);
    DBReaderInterface & operator >> (bool & value);
    DBReaderInterface & operator >> (float & value);
    DBReaderInterface & operator >> (double & value);

private:
    void freeResult();
    bool toStoreResult() const { return _to_store_result;}

    DBMySQLConnection  *_connection;
    MYSQL_RES          *_result;
    int                 _cursor;
    unsigned long      *_lengths;
    MYSQL_ROW           _row;
    bool                _to_store_result;
};




std::unique_ptr<DBConnection> createDBConnection()
{
    return std::make_unique<DBMySQLConnection>();
}

std::unique_ptr<DBConnection> createAndConnectDBConnection(const std::string &host, const std::string &user,
                                                           const std::string &pass, const std::string &name, int port/* = 0*/)
{
    return std::make_unique<DBMySQLConnection>(host, user, pass, name, port);
}


DBMySQLConnection::DBMySQLConnection()
    :DBConnection(), _connection(NULL), _needToSetSQLVariables(true)
{
    nb_inst++;
}

DBMySQLConnection::DBMySQLConnection(const string &host, const string &user,
                                     const string &pass, const string &name,
                                     uint port)
    :DBConnection(), _connection(NULL), _needToSetSQLVariables(true)
{
    nb_inst++;
    connect(host, user, pass, name, port);
}

DBMySQLConnection::~DBMySQLConnection()
{
    disconnect();
    nb_inst--;
    if(nb_inst == 0) { mysql_library_end(); }
}


void DBMySQLConnection::connect(const string &host, const string &user, const string &pass, const string &name, int port)
{
    _connection = new MYSQL;
    assert(_connection && "Cannot allocate memory");
    mysql_init(_connection);
    mysql_options(_connection, MYSQL_READ_DEFAULT_GROUP, "nnfc");
    my_bool reconnect = 1;
    mysql_options(_connection, MYSQL_OPT_RECONNECT, &reconnect);
    //   int timeout = 4;
    //   mysql_options(_connection, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    //   mysql_options(_connection, MYSQL_OPT_READ_TIMEOUT, &timeout);
    //   mysql_options(_connection, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
    if (!mysql_real_connect(_connection, host.c_str(), user.c_str(), pass.c_str(),
        name.c_str(), port, NULL, CLIENT_MULTI_RESULTS))
    {
        _connected = false;
        delete _connection;
        throw DBException("Couldn't connect to MySQL database", mysql_error(_connection));
    }
    mysql_options(_connection, MYSQL_OPT_RECONNECT, &reconnect);

    //TRACE(DEBUG, "Connection to database established, thread id = " << mysql_thread_id(_connection));

    setSQLVariables();
    //   executeQuery("SET @@SQL_MODE='TRADITIONAL'");

    //   executeQuery("SET NAMES UTF8");

    _connected = true;
}

void DBMySQLConnection::disconnect()
{
    if(! connected())
        return;

    //TRACE(DEBUG, "Connection to database is closed, thread id = " << mysql_thread_id(_connection));
    mysql_close(_connection);
    mysql_thread_end();
    delete _connection;
    _connected = false;
}

void DBMySQLConnection::setSQLVariables()
{
    if(needToSetSQLVariables() == false)
        return;
    needToSetSQLVariables(false);
    if(mysql_query(_connection, "SET @@SQL_MODE='TRADITIONAL'"))
    {
        //LOG(ERR, "Cannot SET SQL_MODE to TRADITIONAL due to error: " <<  mysql_error(_connection));
        needToSetSQLVariables(true);
    }

    if(mysql_query(_connection, "SET NAMES UTF8"))
    {
        //LOG(ERR, "Cannot SET NAMES to UTF8 due to error: " <<  mysql_error(_connection));
        needToSetSQLVariables(true);
    }

    return;
}

bool DBMySQLConnection::executeQuery(string & query)
{
    setSQLVariables();

    unsigned long thread_id = mysql_thread_id(_connection);
    if(0  == mysql_ping(_connection) && thread_id != mysql_thread_id(_connection))
    {
        needToSetSQLVariables(true);
        setSQLVariables();
        //TRACE(DEBUG, "DBMySQLConnection::executeQuery() : reconnection to mysql server was happen");
    }

    if (mysql_query(_connection, query.c_str()))
    {
        int ret = mysql_errno(_connection);
        if ((ret == CR_SERVER_LOST) || (ret == CR_SERVER_GONE_ERROR) || (ret == CR_CONN_HOST_ERROR))
        {
            // we should to re-set SQL variables on the next query
            needToSetSQLVariables(true);

            // Make attempt to reconnect to the server and retry query
            //LOG(WARNING, "SQL query failed due to lost connection to MySQL Server, reconnecting... error: "  << mysql_error(_connection) << ". Query was: " << query);
            if ( 0  == mysql_ping(_connection) )
            {
                // set sql variables if needed
                setSQLVariables();

                // Connection is again alive => retry query
                if (mysql_query(_connection, query.c_str()))
                {
                    //TRACE(ERR, "DBMySQLConnection::executeQuery failed: error: " << mysql_error(_connection) << ". query was: " << query);
                    //LOG(ERR, "SQL query failed: error: " << mysql_error(_connection) << ". query was: " << query);
                    return false;
                }
            }
            else
            {
                //TRACE(ERR, "DBMySQLConnection::executeQuery failed to reconnect to the server, error: " << mysql_error(_connection) << ". Query was: " << query);
                //LOG(ERR, "DBMySQLConnection::executeQuery failed to reconnect to the server, error: " << mysql_error(_connection) << ". Query was: " << query);
                return false;
            }
        }
        else
        {
        //    throw  DBException("Cannot execute query", mysql_error(_connection));
            //(ERR, "DBMySQLConnection::executeQuery failed: error: " << mysql_error(_connection) << ". Query was: " << query);
            //LOG(ERR, "SQL query failed: error: " << mysql_error(_connection) << ". Query was: " << query);
        //    return RES_FAIL;
            return false;
        }
    }
    return true;
}

bool DBMySQLConnection::executeQuery(const char * str)
{
    string ss(str);
    return executeQuery(ss);
}

unsigned long long DBMySQLConnection::lastInsertedId()
{
    return mysql_insert_id(_connection);
}

unsigned int DBMySQLConnection::affectedRows()
{
    return mysql_affected_rows(_connection);
}

std::string DBMySQLConnection::escapeString(const std::string & str)
{
    return escapeString(str.c_str());
}

std::string DBMySQLConnection::escapeString(const char * str)
{
    unsigned long len = strlen(str);
    char *buf=new char[len*2+1];
    mysql_real_escape_string(_connection, buf, str, len);
    std::string retval = std::string(buf);
    delete[] buf;
    return retval;
}

string DBMySQLConnection::getError()
{
    return mysql_error(_connection);
}

unsigned int DBMySQLConnection::getErrno()
{
    return mysql_errno(_connection);
}

DBReaderInterface * DBMySQLConnection::reader()
{
    return new DBMySqlReader(this);
}

void DBMySQLConnection::clear()
{
    MYSQL_RES * _result = NULL;
    if((_result = mysql_use_result(_connection)) != NULL)
    {
        mysql_free_result(_result);
        while(mysql_next_result(_connection) == 0)
        {
            if((_result = mysql_use_result(_connection)) != NULL)
            {
                mysql_free_result(_result);
            }
        }
    }
}


DBMySqlReader::DBMySqlReader()
    :_connection(NULL), _result(NULL), _cursor(-1), _lengths(NULL), _row(NULL), _to_store_result(true)
{

}


DBMySqlReader::DBMySqlReader(DBMySQLConnection * connection)
    :_connection(connection), _result(NULL),  _cursor(-1), _lengths(NULL), _row(NULL), _to_store_result(true)
{

}

DBMySqlReader::DBMySqlReader(const DBMySqlReader & reader)
    :_connection(NULL), _result(NULL),  _cursor(-1), _lengths(NULL), _row(NULL), _to_store_result(true)
{
    *this = reader;
}

DBMySqlReader::~DBMySqlReader()
{
    freeResult();
}

void DBMySqlReader::freeResult()
{
    if(_result != NULL)
    {
        mysql_free_result(_result);
        _result = NULL;
    }
}


DBMySqlReader & DBMySqlReader::operator =(const DBMySqlReader & reader)
{
    _connection = reader._connection;
    _to_store_result = true;
    freeResult();
    return *this;
}

bool DBMySqlReader::isValidLite()
{
    _to_store_result = false;
    return isValid();
}

bool DBMySqlReader::isValid()
{
    if(_connection == NULL)
        return false;

    if(_result != NULL)
        return true;
    if(mysql_errno(_connection->_connection))
    {
        return false;
    }

    if(toStoreResult())
    {
        _result = mysql_store_result(_connection->_connection);
    }
    else
    {
        _result = mysql_use_result(_connection->_connection);
    }

    if(_result == NULL)
    {
        int ret = _connection->getErrno();
        if ((ret == CR_SERVER_LOST) || (ret == CR_SERVER_GONE_ERROR) || (ret == CR_CONN_HOST_ERROR))
        {
            // we should to re-set SQL variables on the next query
            _connection->needToSetSQLVariables(true);

            // Make attempt to reconnect to the server and retry query
            //LOG(WARNING, "mysql_store_result failed due to lost connection to MySQL Server, reconnecting... Error: " << _connection->getError());

            if (0 ==  mysql_ping(_connection->_connection))
            {
                // Connection is again alive => retry query
                if(toStoreResult())
                {
                    _result = mysql_store_result(_connection->_connection);
                }
                else
                {
                    _result = mysql_use_result(_connection->_connection);
                }

                if (_result == NULL)
                {
                    //TRACE(ERR, "DBMySqlReader::isValid mysql_store_result failed Error: "  << _connection->getError());
                    //LOG(ERR, "mysql_store_result failed Error: "  << _connection->getError());
                    return false;
                }
            }
            else
            {
                //TRACE(ERR, "DBMySqlReader::isValid failed to reconnect to the server Error: "  << _connection->getError());
                //LOG(ERR, "DBMySqlReader::isValid failed to reconnect to the server Error: " << _connection->getError());
                return false;
            }
        }
        else
        {
            //TRACE(ERR, "DBMySqlReader::isValid mysql_store_result failed: " << _connection->getError());
            //LOG(ERR, "DBMySqlReader::isValid mysql_store_result failed: " << _connection->getError());
            return false;
        }
    }
    return true;
}

unsigned int DBMySqlReader::columnsCount()
{
    return mysql_field_count(_connection->_connection);
}

unsigned int DBMySqlReader::rowsCount()
{
    if(!isValid() /*|| !toStoreResult() */) return 0;
    return mysql_num_rows(_result);
}

string DBMySqlReader::getError()
{
    return _connection->getError();
}

unsigned int DBMySqlReader::getErrno()
{
    return _connection->getErrno();
}

// takes pointer to the buffer and buffer size
// returns real copied bytes,
// assumption: called after reader()
unsigned long DBMySqlReader::getBytes(unsigned int index, char* buffer, unsigned long len)
{
    if (_lengths[index] ==0) return 0;
    if ((len < _lengths[index]) || (buffer == NULL)) return 0;
    memcpy(buffer, _row[index], _lengths[index]);
    return _lengths[index];
}

// assumption: called after reader()
unsigned long DBMySqlReader::getLength(unsigned int index)
{
    return _lengths[index];
}

bool DBMySqlReader::operator ()()
{
    _cursor = -1;
    if(!isValid()) return false;
    _row = mysql_fetch_row(_result);
    if(_row == NULL)
    {
        int ret = _connection->getErrno();
        if ((ret == CR_SERVER_LOST))
        {
            // we should to re-set SQL variables on the next query
            _connection->needToSetSQLVariables(true);

            // Make attempt to reconnect to the server and retry query
            //LOG(WARNING, "DBMySqlReader::operator ()() : mysql_fetch_row failed due to lost connection to MySQL Server, reconnecting... Error: " << _connection->getError());

            if (0 ==  mysql_ping(_connection->_connection))
            {
                // Connection is again alive => retry query
                _row = mysql_fetch_row(_result);

                if (_row == NULL)
                {
                    //TRACE(ERR, "DBMySqlReader::operator ()() : mysql_fetch_row failed Error: "  << _connection->getError());
                    //LOG(ERR, "DBMySqlReader::operator ()() : mysql_fetch_row failed Error: "  << _connection->getError());
                    return false;
                }
            }
            else
            {
                //TRACE(ERR, "DBMySqlReader::operator ()() : failed to reconnect to the server Error: "  << _connection->getError());
                //LOG(ERR, "DBMySqlReader::operator ()() : failed to reconnect to the server Error: " << _connection->getError());
                return false;
            }
            return false;
        }
        else
        {
            return false;
        }
    }
    _lengths = mysql_fetch_lengths(_result);
    _cursor = 0;
    return true;
}

string DBMySqlReader::operator [] (unsigned int index)
{
    string value;
    if(!isValid())
        return value;
    if(index >= columnsCount() || index < 0)
        return value;
    if(_lengths[index] != 0)
    {
        value = _row[index];
    }
    return value;
}




DBReaderInterface & DBMySqlReader::operator >> (string & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    if(_lengths[_cursor] != 0)
    {
        value = _row[(unsigned int)_cursor];
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (NullValue & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    value = false;
    if(_lengths[(unsigned int)_cursor] == 0)
    {
        value = true;
    }
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (long & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG  ||
        field->type == MYSQL_TYPE_NEWDECIMAL )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[12];
            memset(val, 0, 12);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //value = atol(val);
            value = std::stol(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (unsigned long & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG  ||
        field->type == MYSQL_TYPE_NEWDECIMAL )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[11];
            memset(val, 0, 11);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //      value = atol(val);
            value = std::stoul(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (long long & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG  ||
        field->type == MYSQL_TYPE_NEWDECIMAL )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[21];
            memset(val, 0, 21);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //      value = atoll(val);
            value = std::stoll(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (unsigned long long & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG  ||
        field->type == MYSQL_TYPE_NEWDECIMAL )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[21];
            memset(val, 0, 21);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //      value = atoll(val);
            value = std::stoull(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (char & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[5];
            memset(val, 0, 5);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //      value = atoi(val);
            value = std::stoi(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (unsigned char & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[4];
            memset(val, 0, 4);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //      value = atoi(val);
            value = std::stoi(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (short & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG ||
        field->type == MYSQL_TYPE_NEWDECIMAL )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[7];
            memset(val, 0, 7);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //      value = atoi(val);
            value = std::stoi(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (unsigned short & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG ||
        field->type == MYSQL_TYPE_NEWDECIMAL )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[6];
            memset(val, 0, 6);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //      value = atoli(val);
            value = std::stoi(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (int & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG ||
        field->type == MYSQL_TYPE_NEWDECIMAL )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[12];
            memset(val, 0, 12);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            //      value = atoi(val);
            value = std::stoi(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (unsigned int & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_TINY ||
        field->type == MYSQL_TYPE_SHORT ||
        field->type == MYSQL_TYPE_DECIMAL ||
        field->type == MYSQL_TYPE_LONG ||
        field->type == MYSQL_TYPE_INT24 ||
        field->type == MYSQL_TYPE_LONGLONG ||
        field->type == MYSQL_TYPE_NEWDECIMAL )
    {
        if(_lengths[_cursor] != 0)
        {
            char val[11];
            memset(val, 0, 11);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
        //      value = atoli(val);
        value = std::stoul(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (bool & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    //MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if(_lengths[_cursor] != 0)
    {
        char val;
        memset(&val, 0, 1);
        memcpy(&val, _row[(unsigned int)_cursor], 1);
        if(val == '0') value = false;
        else value = true;
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (double & value)
{
    if((unsigned int)_cursor >= columnsCount() || _cursor < 0)
        return *this;
    MYSQL_FIELD * field = mysql_fetch_field_direct(_result, _cursor);
    if( field->type == MYSQL_TYPE_FLOAT ||
        field->type == MYSQL_TYPE_DOUBLE)
    {
        if(_lengths[_cursor] != 0)
        {
            char val[22];
            memset(val, 0, 22);
            memcpy(val, _row[(unsigned int)_cursor], _lengths[_cursor]);
            value = atof(val);
        }
    }
    _cursor++;
    return *this;
}

DBReaderInterface & DBMySqlReader::operator >> (float & value)
{
    double v;
    this->operator>>(v);
    value = (float)v;
    return *this;
}
