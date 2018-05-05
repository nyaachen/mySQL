#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "parser.h"

using namespace std;
using namespace mySQL;

int main(int argc, char** argv) {
  string path("database/"), file("database.csv");
  // parse for possible command line options
  if (argc == 1) { // No argument
    // TODO open default file to read config
  }
  else { // parse all argument
    // TODO mySQL --nosync -database database.csv
  }
  Database db(path+file);
  cout << "欢迎来到mySQL" << endl << "(mySQL) >>>" << flush;
  string buffer;
  while (getline(cin, buffer)) {
    cout << buffer;
    // TODO

    cout << endl << "(mySQL) >>>" << flush;
  }
  return 0;
}
