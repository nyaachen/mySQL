#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <vector>

namespace mySQL {

using Data = std::string;


class Bad_parse : public std::invalid_argument {};
class Illigal_dentifier : public std::invalid_argument {};


// 单条记录
class Record : protected std::vector<Data> {
public:

};

// 一个表是多条记录的总和
class Table : protected std::vector<Record> {
private:
  Record attr_head; //表头，head名字是冲突的
public:

};

class Named_Table : public Table {
public:
  Data table_name;
  Named_Table() : Table(), table_name() {}
};

static const std::vector<std::string> KEYWORDS {"CREATE", "TABLE", "TO", "FROM",
    "DROP", "LIST", "INSERT", "INTO", "VALUES", "DELETE", "WHERE", "UPDATE",
    "SET", "SELECT", "DISTINCT", "ORDER", "BY", "ASC", "DESC"};

class Database;
class Parser {
  // this class should be binded to a database

private:
  Database &db;
  bool parse_create();
  bool parse_drop();
  bool parse_insert();
  bool parse_delete();
  bool parse_update();
  bool parse_select();
  Table Result;
public:
  bool parse();

};

class Database {
  friend class Parser;
private:
  std::string database_file;
public:
  Database(std::string filename) : database_file(filename) {
    // Extend feature here
    // TODO
  }
  // for general parse call this
  bool parse() {
    // input a string as SQL query data
    // output a
  }
  //
};


};

#endif
