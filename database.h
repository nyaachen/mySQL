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
class Program_Exit : public std::runtime_error {
public:
  Program_Exit() : std::runtime_error("Program received an exit signal") {}
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
  std::string peek(){
    std::string::size_type p = pos;
    std::string r = get_keyword();
    pos = p;
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
std::string error_count(const Word_parser &p) {
  std::string err("读取指令时发生错误\n");
  err += p.get_str() + "\n";
  err += std::string(p.get_pos(), ' ');
  err += "*\n标识符计数错误";
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
class Record : public std::vector<Data> {
  friend std::ostream &operator<<(std::ostream &os, Record &r);
public:
  Record(std::vector<Data> l) : std::vector<Data>(l) {};
  Record() = default;
  explicit Record(const std::string  &repr) {
    std::istringstream is(repr);
    std::string temp;
    while (is >> temp) this->push_back(temp);
  }
};
std::ostream &operator<<(std::ostream &os, Record &r){
  for (auto s: r) os << s << "\t";
  return os;
}

// 一个表是多条记录的总和
class Table : public std::vector<Record> {
  friend std::ostream &operator<<(std::ostream &os, Table &t);
private:
  Record head; //表头
public:
  Table() = default;
  Table(const Record &h, const std::vector<Record> &l) : std::vector<Record>(l), head(h) {}
  explicit Table(std::istream &is) : Table() {
    std::string temp;
    getline(is, temp);
    head = Record(temp);
    while(getline(is, temp)) this->push_back(Record(temp));
  }
  Record get_head() {return head;}
};
std::ostream &operator<<(std::ostream &os, Table &t){
  os << t.head << std::endl;
  for (auto i(t.begin()); i != t.end(); ++i) os << (*i) << std::endl;
  return os;
}


class Named_Table : public Table {
  std::string table_name;
public:
  Named_Table(const Table t, std::string name) : Table(t), table_name(name) {}
  Data get_table_name() const {return table_name;}
};





class Database {
private:
  // Database
  std::string database_name;
  std::vector<std::pair<std::string, std::string>> database_file;
  std::vector<Named_Table> database_list;

  // Parser
  public:
  Table parser_result;
  private:
  std::vector<std::string> parse_csv_v(Word_parser &s, bool allow_dup = false) {
    std::vector<std::string> attrib_name;
    std::set<std::string> dup_check;
    while (true) {
      std::string attr = s.parse();
      if (allow_dup or (dup_check.find(attr) == dup_check.end())) {
        dup_check.insert(attr);
        attrib_name.push_back(attr);
      }
      else throw Bad_parse(error_identifier(s));
      std::string next = s.peek();
      if (next == ",") {s.parse(); continue;}
      else break;
    }
    return attrib_name;
  }
  std::vector<std::pair<std::string, std::string>> parse_csv_p(Word_parser &s, bool allow_dup = false) {
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
      std::string next = s.peek();
      if (next == ",") {s.parse(); continue;}
      else break;
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
      // TODO 检查数据库是否已经在表中
      Named_Table t(read_file(filename), name);
      database_list.push_back(t);
      database_file.push_back(std::pair<std::string, std::string>(name, filename));
      parser_result = t;
    }
    else if (key == "(" ) {
      std::vector<std::string> attrib_name = parse_csv_v(s);
      s.assume(")");
      s.assume("TO");
      std::string filename = s.parse();
      s.assume_end();
      //TODO 检查文件是否为空
      Record r(attrib_name);
      Table t(r, std::vector<Record>());
      Named_Table nt(t, name);
      database_list.push_back(nt);
      database_file.push_back(std::pair<std::string, std::string>(name, filename));
      parser_result = t;
    }
    else throw Bad_parse(error_key(s));
  }
  bool parse_drop(Word_parser &s){
    s.assume("TABLE");
    std::string name = s.parse();
    s.assume_end();
    // TODO 检查数据库是否存在
    for(auto i(database_list.begin()); i != database_list.end(); ++i)
      if (i->get_table_name() == name) {database_list.erase(i); break;}
    for(auto i(database_file.begin()); i != database_file.end(); ++i)
      if (i->first == name) {database_file.erase(i); break;}
  }
  bool parse_insert(Word_parser &s){
    s.assume("INTO");
    std::string name = s.parse();
    std::string key = s.get_keyword();
    if (key == "VALUES") {
      s.assume("(");
      auto value = parse_csv_v(s, true);
      s.assume(")");
      s.assume_end();
      // TODO检查数据库是否存在
      for(auto i(database_list.begin()); i != database_list.end(); ++i)
        if (i->get_table_name() == name) {if ( (i->get_head()).size() != value.size()) throw Bad_parse(error_count(s)); i->push_back(value); parser_result = *i;}
    }
    else if (key == "(") {
      auto keys = parse_csv_v(s);
      s.assume(")");
      s.assume("VALUES");
      s.assume("(");
      auto value = parse_csv_v(s,true);
      if(keys.size() != value.size()) throw Bad_parse(error_identifier(s));
      s.assume(")");
      s.assume_end();
      // TODO 检查数据库 列名是否存在
      for(auto i(database_list.begin()); i != database_list.end(); ++i)
        if (i->get_table_name() == name) {
          Record head=i->get_head();
          Record temp(std::vector<Data>(head.size(), "0"));
          for (int i(0); i != keys.size(); ++i) {
            for(int j(0); j != head.size(); ++j) if (head[j] == keys[i]) {temp[j] = value[i]; break;}
          }
          i->push_back(temp);
          parser_result = *i;
          break;
        }
    }
    else throw Bad_parse(error_key(s));
  }
  bool parse_delete(Word_parser &s){
    std::string key = s.get_keyword();
    if (key == "*"){
      s.assume("FROM");
      std::string name = s.parse();
      s.assume_end();
      // TODO 检查表的存在性?
      for(auto i(database_list.begin()); i != database_list.end(); ++i)
        if (i->get_table_name() == name) {i->clear(); parser_result = *i; break;}
    }
    else if (key == "FROM") {
      std::string name = s.parse();
      s.assume("WHERE");
      key = s.parse();
      s.assume("=");
      std::string value = s.parse();
      s.assume_end();
      // TODO 检查表和列名的存在性
      for(auto i(database_list.begin()); i != database_list.end(); ++i)
        if (i->get_table_name() == name) {
          auto iter=i->begin();
          int pos(0);
          for(; pos != (i->get_head()).size(); ++pos)
          {
            if ( (i->get_head())[pos] == key)
              break;
          }
          while (iter != i->end()){
            if ( (*iter)[pos] == value ) iter = i->erase(iter);
            else ++iter;
          }
          parser_result = *i;
          break;
        }
    }
    else throw Bad_parse(error_key(s));
  }
  bool parse_update(Word_parser &s){
    std::string name = s.parse();
    s.assume("SET");
    auto pairs = parse_csv_p(s);
    std::string key = s.get_keyword();
    if (key == "WHERE") {
      key = s.parse();
      s.assume("=");
      std::string value = s.parse();
      s.assume_end();
      // TODO 检查表和列名的存在性
      for(auto i(database_list.begin()); i != database_list.end(); ++i)
        if (i->get_table_name() == name) {
          Record head = i->get_head();
          int pos(0);
          for(; pos != head.size(); ++pos)
          {
            if ( head[pos] == key)
              break;
          }
          for( auto iter=i->begin(); iter != i->end(); ++iter){
            if ( (*iter)[pos] == value ) {
              // TODO
              for (auto p: pairs){
                for( int j(0); j != head.size(); ++j)
                  if (head[j] == p.first) {(*iter)[j] = p.second; break;}
              }
            }
          }
          parser_result = *i;
          break;
        }
    }
    else if (key == "") {
      // TODO 检查表和列名的存在性
      for(auto i(database_list.begin()); i != database_list.end(); ++i)
        if (i->get_table_name() == name) {
          for(auto iter(i->begin()); iter != i->end(); ++i) {
            // TODO
            Record head = i->get_head();
            for (auto p: pairs){
              for( int j(0); j != head.size(); ++j)
                if (head[j] == p.first) {(*iter)[j] = p.second; break;}
            }
          }
        parser_result = *i;
        break;
        }
    }
    else throw Bad_parse(error_key(s));
  }
  bool parse_select(Word_parser &s){
    auto status = s.peek();
    Table (*filter)(const Table &) = nullptr;
    Table (*back_filter)(const Table &) = nullptr;
    Table *target_table = nullptr;
    if (status == "*") {
      s.parse();
      back_filter = [] (const Table &t) -> Table {return t;} ;
      s.assume("FROM");
      std::string name = s.parse();
      //TODO
      for(auto i(database_list.begin()); i != database_list.end(); ++i)
        if (i->get_table_name() == name) {target_table = &(*i); break;}
    }
    else {
      bool dist(false);
      if (status == "DISTINCT") {
        s.parse();
        dist = true;
      }
      auto column =  parse_csv_v(s);
      s.assume("FROM");
      std::string name = s.parse();
      //TODO
      for(auto i(database_list.begin()); i != database_list.end(); ++i)
        if (i->get_table_name() == name) {
          target_table = &(*i);

          break;
        }
    }
    // sub orders
    status = s.peek();// to where order_by
    if (status == "TO") {
      s.assume("TO");
      std::string filename = s.parse();
      s.assume_end();
      // TODO
    }
    else if (status == "WHERE") {
      s.assume("WHERE");
      std::string key = s.parse();
      s.assume("=");
      std::string value = s.parse();
      s.assume_end();
      // TODO
    }
    else if (status == "ORDER") {
      s.assume("ORDER");
      s.assume("BY");
      auto list = parse_csv_v(s);
      status = s.get_keyword();
      if (status == "ASC") {
        // TODO
      }
      else if (status == "DESC") {
        // TODO
      }
      else throw Bad_parse(error_key(s));
    }
    else {
      s.assume_end();
      //TODO
    }
  }

  // io sync from file
  Table read_file(std::string filename) {
    std::ifstream is(filename);
    if (is) ;
    else throw std::invalid_argument("Could not open database file: " + filename);
    Table temp(is);
    return temp;
  }
public:
  Database(const std::string &filename) : database_name(filename) {
    std::ifstream is(filename);
    if (is) {
      std::string table_name, table_file;
      while ( is >> table_name >> table_file) database_file.push_back(std::pair<std::string, std::string>(table_name, table_file));
    }
    else throw std::invalid_argument("Could not open database file: " + filename);

    for(auto p: database_file) {
      database_list.push_back(Named_Table(read_file(p.second), p.first));
    }
  }

  // io同步
  bool sync() {
    // 写回文件
    // 写回数据库
    std::ofstream os;
    os.open(database_name, std::iostream::out);
    if (os){
      for (auto w: database_file) os << w.first << " " << w.second << std::endl;
      os.close();
    }
    else {
      throw std::runtime_error("Could not open file: " + database_name);
    }
    // 写回各个子数据库
    for (auto w: database_file){
      os.open(w.second, std::iostream::out);
      if (os){
        for (auto t: database_list) {
          if (t.get_table_name() == w.first){
            os << t;
            break;
          }
        }
        os.close();
      }
      else {
        throw std::runtime_error("Could not open file: " + w.second);
      }
    }
    return true;
  }

  // 命令解析
  bool parse(const std::string &s) {
    auto p = Word_parser(s);
    std::string start = p.get_keyword();
    if (start == "CREATE") {
      parse_create(p);
      return true;
    }
    else if (start == "DROP") {
      parse_drop(p);
    }
    else if (start == "TABLE") {
      p.assume("LIST");
      for (auto p: database_file){
        std::cout << p.first << std::endl;
      }
    }
    else if (start == "INSERT") {
      parse_insert(p);
      return true;
    }
    else if (start == "DELETE") {
      parse_delete(p);
      return true;
    }
    else if (start == "UPDATE") {
      parse_update(p);
      return true;
    }
    else if (start == "SELECT") {
      parse_select(p);
      return true;
    }
    else if (start == "EXIT") {sync(); throw Program_Exit();}
    else if (start == "SYNC") {sync();}
    else throw Bad_parse(error_key(p));
    return false;
  }


};


};

#endif
