#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <vector>



namespace mySQL {

// 单词解析
bool is_sub_str(const std::string &s1, const std::string &s2) {
  return s2.find(s1) != std::string::npos;
}

static const std::string IGNORE(" "), SYMBOL("(),=");

class Word_parser {
private:
  std::string s;
  std::string::size_type pos;
public:
  Word_parser(const std::string &str): s(str), pos(0) {}
  std::string parse(){
    std::string r;
    while ( (pos < s.size()) and (s[pos] == ' ') ) ++pos;
    if (pos < s.size()) {
      if (is_sub_str(s.substr(pos,1), SYMBOL)){
        r = s[pos];
        ++pos;
      }
      else {
        auto pos2 = s.find_first_of(IGNORE+SYMBOL, pos);
        r = s.substr(pos, pos2-pos);
        pos = pos2;
      }
    }
    return r;
  }
  std::string::size_type get_pos() {return pos;}
};

// 单词解析 结束

// 标准错误类型
class Bad_parse : public std::invalid_argument {};
class Illigal_dentifier : public std::invalid_argument {};

// 前向声明
class Record;
class Table;
class Named_Table;
class Parser;
class Database;

// 数据定义
using Data = std::string;

// 单条记录是多个数据的有序排列
class Record{
  friend class Table;
  friend class Named_Table;
private:
  std::vector<Data> record;
public:
  Record(std::vector<Data> l) : record(l) {};
  Record() = default;
};

// 一个表是多条记录的总和
class Table{
  friend class Parser;
private:
  Record head; //表头
  std::vector<Record> sheet;
public:
  Table() = default;
};

class Named_Table : public Table {
  friend class Parser;
public:
  Data table_name;
  Named_Table()=default;
};

// 关键字表
static const std::vector<std::string> KEYWORDS {"CREATE", "TABLE", "TO", "FROM",
    "DROP", "LIST", "INSERT", "INTO", "VALUES", "DELETE", "WHERE", "UPDATE",
    "SET", "SELECT", "DISTINCT", "ORDER", "BY", "ASC", "DESC"};

// 命令解析执行器
class Parser {
private:
  Database &db;
  // 直接负责执行， 执行出错raise
  bool parse_create(const std::string &s);
  bool parse_drop(const std::string &s);
  bool parse_insert(const std::string &s);
  bool parse_delete(const std::string &s);
  bool parse_update(const std::string &s);
  bool parse_select(const std::string &s);
  Table Result;
public:
  Parser(Database &d): db(d) {}
  Parser &operator=(const Parser &p) = delete;
  Parser(const Parser &p) = delete;
  // 返回执行是否成功，执行出错打印错误报告不raise
  bool parse(const std::string &s) {
    auto p = Word_parser(s);
    std::string start = p.parse();
    if (start == "CREATE") {
      //parse_create(s);
      return 0;
    }
    else return -1;
  }

};

class Database {
  friend class Parser;
private:
  std::string database_file;
  Parser p;
public:
  Database(std::string filename) : database_file(filename), p(*this){
    // Extend feature here
    // TODO
  }
  Parser &get_parser() { return p;}
};


};

#endif
