#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "parser.h"

using namespace std;
using namespace mySQL;



int main(int argc, char** argv) {
  string file("database.csv");
  // parse for possible command line options
  if (argc == 1) { // No argument
    // TODO open default file to read config
  }
  else { // parse all argument
    // TODO mySQL --nosync -database database.csv
  }
  Database db(file);
  auto &p = db.get_parser();
  cout << "欢迎来到mySQL" << endl << "(mySQL) >>>" << flush;
  string buffer;
  while (getline(cin, buffer)) {
    // TODO
    try {
      p.parse(buffer);
    }
    catch (const std::invalid_argument &e) {
      cerr << e.what() << endl;
    }
    cout << "Beta版本并不对数据库执行实际的写操作（但是会正常解析命令）。最终版会完成该功能。" << endl << "(mySQL) >>>" << flush;
  }
  return 0;
}
