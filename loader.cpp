#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>
#include "database.h"

using namespace std;
using namespace mySQL;



int main(int argc, char** argv) {
  string file = "database.csv";
  Database db(file);
  cout << "欢迎来到mySQL 使用sync来同步内存与数据文件" << endl << "(mySQL) >>>" << flush;
  string buffer;
  while (getline(cin, buffer)) {
    // TODO
    try {
      if( db.parse(buffer) )
      	cout << db.parser_result << endl;
    }
    catch (const Bad_parse &e) {
      cerr << e.what() << endl;
    }
    catch (const Program_Exit &e) {break;}
    catch (const std::exception &e) {
      cerr << e.what() << endl;
      throw;
    }
    cout << "Preview Version will not perform all select sentence 。最终版会完成该功能。" << endl << "(mySQL) >>>" << flush;
  }
  return 0;
}
