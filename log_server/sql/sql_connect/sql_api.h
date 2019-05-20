#ifndef _SQL_CONNECT_
#define _SQL_CONNECT_

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
#include <mysql.h>
using namespace std;

class sqlApi{
public:
    sqlApi(const string &h, const int port, const string &u, const string &p, const string &db);
    //name sex age hobby school
    int insert(const string& name, const string& key); 
    int select(const std::string& name, const std::string& key);
    int connect();
    ~sqlApi();
private:
    MYSQL *m_conn;
    MYSQL_RES *m_res;
    string m_host;
    string m_user;
    string m_passwd;
    string m_db;
    int m_port;
};

#endif
