#include <iostream>
#include <string>
#include "parser.h"

using namespace std;
using namespace mySQL;

int main(int argc, char** argv) {
  // parse for possible command line options
  if (argc == 1) { // No argument
    cout << "以默认无参数运行。如需要参数帮助请查阅说明文件。" << endl;
  }
  else { // parse all argument
    // TODO
    for(int i(0); i != argc; ++i) cout << argc[i] << " ";
    cout << endl;
  }
  cout << "欢迎来到mySQL" << endl << "(mySQL) >>>" << flush;
  string buffer
  while (getline(cin, buffer)) {
    cout << buffer;
  }
  return 0;
}
