#ifndef SUPPORT_H
#define SUPPORT_H

#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <string>

#include <ostream>
#include <iostream>

using namespace std;



std::ostream &operator <<(ostream & os, const vector<class T> &vec);
std::ostream &operator <<(ostream & os, const vector<string> &vec);

namespace Support{

void print(vector<string> vec);

/*
  Changes extension (last characters after .dot) into another
  example
  changeExtension("test.jpg", "png") -> test.png
  */
void changeExtension(string &str, string toExt);


/*
  Gets only the filename from the full path
  example:
  /home/dir/file.ext
  out:
  file
  @param filepath fullpath with filename and extensipn
  @param withExtension if true, returns filename with extension

  */
string getFileName(string &filepath, bool withExtension = false);

vector<string> splitString(string &str2split, char delimiter = '/');


string getFilePath(string &filepath);
/*
  Create a vector of all files in given directory. With full path.
  simmilar to `ls`
  This function is good to use when you need a list of strings with paths to files
  in given directory.
  If the vector returned form the function is empty the directory either doesn't exist or contain no files
  @param pathDir a relative or an absolute path to the given folder from where you want enlist files
  @param filter allows filtering files e.g ".jpg", "img_"
  @return vector of strings with path to required file
  */
vector<string> pathVector(string pathDir, string filter = "");

/*
  Checks whether the files are of same name (without .extension)
  @return true on the same filenames (without extension)
  @return false when filenames vary in the name
  @param name1 filename with extension of the first file
  @param name2 filename with extension of the second file
*/
bool ofSameName(string name1, string name2);



class myString{
public:
    //ctor
    myString(string constrStr);
    int endsWith(string suffix);
    string getString();
private:
    string str;
};
}



#endif
