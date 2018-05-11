#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "database.h"

using namespace std;
using namespace mySQL;



int main(int argc, char** argv) {
  string file("database.csv");
  Database db();
  cout << "欢迎来到mySQL" << endl << "(mySQL) >>>" << flush;
  string buffer;
  while (getline(cin, buffer)) {
    // TODO
    try {
      db.parse(buffer);
    }
    catch (const Bad_parse &e) {
      cerr << e.what() << endl;
    }
    cout << "Beta版本并不对数据库执行实际的写操作（但是会正常解析命令）。最终版会完成该功能。" << endl << "(mySQL) >>>" << flush;
  }
  return 0;
}
