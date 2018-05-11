#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <vector>
#include <utility>
#include <cctype>
#include <map>
#include <set>

#include <iostream>

namespace mySQL {

// 异常处理
// 单词解析
bool is_sub_str(const std::string &s1, const std::string &s2) {
  return s2.find(s1) != std::string::npos;
}

static const std::string IGNORE(" "), SYMBOL("(),=");

class Bad_parse : public std::invalid_argument {
  public:
    Bad_parse(const std::string &p) : std::invalid_argument(p) {}
};

class Word_parser;
std::string error_identifier(const Word_parser &p);
std::string error_key(const Word_parser &p);

class Word_parser {
private:
  std::string s;
  std::string::size_type pos;
public:
  Word_parser(const std::string &str): s(str), pos(0) {}
  bool assume(const std::string &keyword) {
    std::string word = parse();
    for (auto &w: word) w = std::toupper(w);
    if (word == keyword) return true;
    else throw Bad_parse(error_key(*this));
  }
  bool assume_end(){
    return assume("");
  }
  std::string get_keyword() {
    std::string word = parse();
    for (auto &w: word) w = std::toupper(w);
    return word;
  }
  std::string parse(){
    std::string r;
    pos = s.find_first_not_of(IGNORE, pos);
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
  std::string::size_type get_pos() const {
    if (pos == std::string::npos) return s.size();
    return pos;
  }
  std::string get_str() const {return s;}
};

std::string error_key(const Word_parser &p) {
  std::string err("读取指令时发生错误\n");
  err += p.get_str() + "\n";
  err += std::string(p.get_pos(), ' ');
  err += "*\n不能识别的关键字";
  return err;
}
std::string error_identifier(const Word_parser &p) {
  std::string err("读取指令时发生错误\n");
  err += p.get_str() + "\n";
  err += std::string(p.get_pos(), ' ');
  err += "*\n非法的值";
  return err;
}

// 单词解析 结束



// 前向声明
class Record;
class Table;
class Named_Table;
class Database;

// 数据定义
using Data = std::string;

// 单条记录是多个数据的有序排列
class Record{
  friend class Table;
  friend class Named_Table;
  friend std::ostream &operator<<(std::ostream &os, Record &r);
private:
  std::vector<Data> record;
public:
  Record(std::vector<Data> l) : record(l) {};
  Record() = default;
};
std::ostream &operator<<(std::ostream &os, Record &r){

}

// 一个表是多条记录的总和
class Table{
  friend class Parser;
  friend std::ostream &operator<<(std::ostream &os, Table &t);
private:
  Record head; //表头
  std::vector<Record> sheet;
public:
  Table() = default;
};
std::ostream &operator<<(std::ostream &os, Table &t){
  os << t.head << std::endl;

}


class Named_Table : public Table {
  friend class Parser;
  Data table_name;
public:
  Named_Table()=default;
};

// 关键字表
static const std::vector<std::string> KEYWORDS {"CREATE", "TABLE", "TO", "FROM",
    "DROP", "LIST", "INSERT", "INTO", "VALUES", "DELETE", "WHERE", "UPDATE",
    "SET", "SELECT", "DISTINCT", "ORDER", "BY", "ASC", "DESC"};




class Database {
private:
  // Database
  std::string database_name;
  std::vector<std::pair<std::string, std::string>> database_file;
  std::vector<Named_Table> database_list;
  // Parser
  Table parser_result;
  std::vector<std::string> parse_csv_pe_v(Word_parser &s, bool allow_dup = false) {
    std::vector<std::string> attrib_name;
    std::set<std::string> dup_check;
    while (true) {
      std::string attr = s.parse();
      if (allow_dup or (dup_check.find(attr) == dup_check.end())) {
        dup_check.insert(attr);
        attrib_name.push_back(attr);
      }
      else throw Bad_parse(error_identifier(s));
      std::string next = s.get_keyword();
      if (next == ",") continue;
      else if (next == ")") break;
      else throw Bad_parse(error_key(s));
    }
    return attrib_name;
  }
  std::vector<std::pair<std::string, std::string>> parse_csv_pe_p(Word_parser &s, bool allow_dup = false) {
    std::vector<std::pair<std::string, std::string>> attrib;
    std::set<std::string> dup_check;
    while (true) {
      std::string attr = s.parse();
      if (allow_dup or (dup_check.find(attr) == dup_check.end()))  {
        s.assume("=");
        std::string value = s.parse();
        dup_check.insert(attr);
        attrib.push_back(std::pair<std::string, std::string>(attr, value));
      }
      else throw Bad_parse(error_identifier(s));
      std::string next = s.get_keyword();
      if (next == ",") continue;
      else if (next == ")") break;
      else throw Bad_parse(error_key(s));
    }
    return attrib;
  }
  bool parse_create(Word_parser &s){
    s.assume("TABLE");
    std::string name = s.parse();
    std::string key= s.get_keyword();
    if( key == "FROM"){
      std::string filename = s.parse();
      s.assume_end();
      // TODO
    }
    else if (key == "(" ) {
      std::vector<std::string> attrib_name = parse_csv_pe_v(s);
      s.assume("TO");
      std::string filename = s.parse();
      s.assume_end();
      //TODO
    }
    else throw Bad_parse(error_key(s));
  }
  bool parse_drop(Word_parser &s){
    s.assume("TABLE");
    std::string name = s.parse();
    s.assume_end();
    // TODO
  }
  bool parse_insert(Word_parser &s){
    s.assume("INTO");
    std::string name = s.parse();
    std::string key = s.get_keyword();
    if (key == "VALUES") {
      s.assume("(");
      auto value = parse_csv_pe_v(s, true);
      s.assume_end();
      // TODO
    }
    else if (key == "(") {
      auto keys = parse_csv_pe_v(s);
      s.assume("VALUES");
      s.assume("(");
      auto value = parse_csv_pe_v(s,true);
      s.assume_end();
      // TODO
    }
    else throw Bad_parse(error_key(s));
  }
  bool parse_delete(Word_parser &s){
    std::string key = s.get_keyword();
    if (key == "*"){
      s.assume("FROM");
      std::string name = s.parse();
      s.assume_end();
      // TODO
    }
    else if (key == "FROM") {
      std::string name = s.parse();
      s.assume("WHERE");
      std::string key = s.parse();
      s.assume("=");
      std::string value = s.parse();
      s.assume_end();
      // TODO
    }
  }
  bool parse_update(Word_parser &s){

  }
  bool parse_select(Word_parser &s){

  }

public:
  Database() = default;

  // io同步
  bool sync();

  // 命令解析
  bool parse(const std::string &s) {
    auto p = Word_parser(s);
    std::string start = p.get_keyword();
    if (start == "CREATE") {
      parse_create(p);
    }
    else if (start == "DROP") {
      parse_drop(p);
    }
    else if (start == "TABLE") {
      p.assume("LIST");
      // TODO
    }
    else if (start == "INSERT") {
      parse_insert(p);
    }
    else if (start == "DELETE") {
      parse_delete(p);
    }
    else if (start == "UPDATE") {
      parse_update(p);
    }
    else if (start == "SELECT") {
      parse_select(p);
    }
    else if (start == "EXIT") {throw std::runtime_error("EXIT");}
    else throw Bad_parse(error_key(p));
    return true;
  }


};


};

#endif
