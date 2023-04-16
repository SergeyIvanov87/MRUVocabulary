#ifndef DBCONNECTION_H_
#define DBCONNECTION_H_

#include <memory>
#include <string.h>
#include <iostream>

class DBReaderInterface;

class DBConnection
{
public:
    DBConnection() : _connected(false) {};
    virtual ~DBConnection() {};

    DBConnection(const DBConnection &) = delete;
    DBConnection &operator=(const DBConnection &) = delete;

    virtual void connect(const std::string &host, const std::string &user,
                         const std::string &pass, const std::string &name, int port = 0) = 0;
    virtual void disconnect() = 0;

    virtual bool connected() const {return _connected;}

    virtual bool executeQuery(std::string &query) = 0;
    virtual bool executeQuery(const char *str) = 0;
    virtual unsigned long long lastInsertedId() = 0;
    virtual unsigned int affectedRows() = 0;

    virtual std::string escapeString(const std::string &str) = 0;
    virtual std::string escapeString(const char *str) = 0;

    virtual DBReaderInterface *reader() = 0;

    // clear all result data of connection
    virtual void clear() = 0;

    virtual std::string getError() = 0;
    virtual unsigned int getErrno() = 0;
    virtual int ping() = 0;
protected:

    bool _connected;
};

std::unique_ptr<DBConnection> createDBConnection();
std::unique_ptr<DBConnection> createAndConnectDBConnection(const std::string &host, const std::string &user,
                                                           const std::string &pass, const std::string &name, int port = 0);


class NullValue
{
public:
    NullValue()
        :_null(false) {}
    NullValue & operator = (const bool value)
        {_null = value; return *this;}
    bool operator == (const bool value)
        {return(_null == value);}
private:
    bool _null;
};


class DBReaderInterface
{
public:
    DBReaderInterface() {}
    virtual ~DBReaderInterface() {}

    DBReaderInterface(const DBReaderInterface &) = delete;
    DBReaderInterface &operator=(const DBReaderInterface &) = delete;

    virtual unsigned int columnsCount() = 0;
    virtual unsigned int rowsCount() = 0;

        // stores all results in memory
    virtual bool isValid() = 0;
        // each row will be fetched from server in operator()
        // should be used for sql requests which need more memeory
    virtual bool isValidLite() = 0;
    virtual bool operator ()() = 0;

    virtual std::string operator [] (unsigned int index) = 0;
    virtual unsigned long getBytes(unsigned int index, char* buffer, unsigned long len) = 0;
    virtual unsigned long getLength(unsigned int index) = 0;

    virtual std::string getError() = 0;
    virtual unsigned int getErrno() = 0;

    virtual DBReaderInterface &operator >> (std::string &value) = 0;
    virtual DBReaderInterface &operator >> (NullValue &value) = 0;
    virtual DBReaderInterface &operator >> (short &value) = 0;
    virtual DBReaderInterface &operator >> (unsigned short &value) = 0;
    virtual DBReaderInterface &operator >> (long &value) = 0;
    virtual DBReaderInterface &operator >> (unsigned long &value) = 0;
    virtual DBReaderInterface &operator >> (long long &value) = 0;
    virtual DBReaderInterface &operator >> (unsigned long long &value) = 0;
    virtual DBReaderInterface &operator >> (char &value) = 0;
    virtual DBReaderInterface &operator >> (unsigned char &value) = 0;
    virtual DBReaderInterface &operator >> (int &value) = 0;
    virtual DBReaderInterface &operator >> (unsigned int &value) = 0;
    virtual DBReaderInterface &operator >> (bool &value) = 0;
    virtual DBReaderInterface &operator >> (float &value) = 0;
    virtual DBReaderInterface &operator >> (double &value) = 0;
};

class DBReader : public DBReaderInterface
{
public:
    DBReader(DBReaderInterface *r = NULL);
    ~DBReader();

    DBReader & operator = (DBReaderInterface *r) {if(_reader != NULL) delete _reader; _reader = r; return *this;}
    DBReaderInterface * operator ->() {return _reader;}

    virtual unsigned int columnsCount() {return _reader != NULL && _reader->columnsCount();}
    virtual unsigned int rowsCount() {return _reader != NULL && _reader->rowsCount();}

    virtual bool isValid() {return _reader != NULL && _reader->isValid();}
    virtual bool isValidLite() { return _reader != NULL && _reader->isValidLite();}
    virtual bool operator ()() {return _reader != NULL && _reader->operator ()();}

    virtual std::string operator [] (unsigned int index) {return _reader->operator [] (index);};
    virtual unsigned long getBytes(unsigned int index, char* buffer, unsigned long len) {return _reader->getBytes(index, buffer, len);}
    virtual unsigned long getLength(unsigned int index) {return _reader->getLength(index);}

    virtual std::string getError() {return _reader->getError();}
    virtual unsigned int getErrno() {return _reader->getErrno();}

    virtual DBReaderInterface & operator >> (std::string & value){ return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (NullValue & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (short & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (unsigned short & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (long & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (unsigned long & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (long long & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (unsigned long long & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (char & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (unsigned char & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (int & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (unsigned int & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (bool & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (float & value) { return _reader->operator>>(value);}
    virtual DBReaderInterface & operator >> (double & value) { return _reader->operator>>(value);}

private:
    DBReaderInterface *_reader;

    DBReader(const DBReader &);
    DBReader(DBReader &);
    DBReader & operator = (const DBReader &);
};

inline DBReader::DBReader(DBReaderInterface * r)
    : _reader(r)
{
}

inline DBReader::~DBReader()
{
    if(_reader != NULL) delete _reader;
}




#endif /* DBCONNECTION_H_ */
