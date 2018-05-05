#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <sstream>
#include <fstream>

namespace mySQL {

class Database {
private:
  std::string database_file;
public:
  Database(std::string filename) : database_file(filename) {
    // TODO
  }
};


};

#endif
