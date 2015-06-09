#include "support.h"

template<typename T>
std::ostream &operator <<(ostream & os, const vector<string> &vec){
    os << "< ";
    for(int i = 0; i < vec.size(); ++i){
        //os << vec[i] << ", ";
    }
    os << " >" << endl;
    return os;
}


std::ostream &operator <<(ostream & os, const vector<string> &vec){
    os << "< ";
    for(int i = 0; i < vec.size(); ++i){
        os << vec[i] << ", ";
    }
    os << "(" << vec.size() << " items)";
    os << " >" << endl;
    return os;
}


vector<string> Support::splitString(string &str2split, char delimiter){
    vector<string> elements;
    size_t pos = 0;
    size_t pos_find = 0;
    do{
        pos_find = str2split.find(delimiter, pos);
        size_t npos = pos_find - pos;

        elements.push_back(str2split.substr(pos,npos));

        pos = pos_find + 1;

    }while(pos_find != string::npos);

    return elements;
}

string Support::getFilePath(string &filepath){
    return filepath.substr(0,filepath.find_last_of("/")+1);
}

string Support::getFileName(string &filepath, bool withExtension){

    vector<string> elements = splitString(filepath);

    string fname;
    if(withExtension){
        fname = elements.back();
    }
    else{
        vector<string> fname_elements = splitString(elements.back(),'.');
        fname = fname_elements.front();
    }

    return fname;
}

void Support::changeExtension(string &str, string toExt){
    if(toExt[0] != '.'){
        toExt = "." + toExt;
    }
    int find = str.find_last_of(".");
    // Only extensions with length of 4 ( 5 including ".") are supported
    if( find  >= (str.length()-5) ){
        str.erase(find,str.length());
        str.append(toExt);
    }

}

vector<string> Support::pathVector(string pathDir, string filter){

    DIR *dir; // for the directory entity
    struct dirent *ent;

    // end of the dir must be with /
    if(pathDir[pathDir.length()-1] != '/'){
        pathDir.append("/");
    }

    vector<string> files;
    if((dir = opendir(pathDir.c_str())) != NULL){
        // while there is something to read
        while((ent = readdir(dir)) != NULL){
            // don't read hidden files and parent folder
            if(ent->d_name[0] != '.'){
                myString dname(ent->d_name);
                if(dname.endsWith(filter)){
                    string name = dname.getString();
                    name = pathDir+name;
                    files.push_back(name);
                }
            }
        }
    }
    else{
        closedir(dir);
    }


    return files;
}

void Support::print(vector<string> vec){

    for(int i = 0; i < vec.size() ; ++i){
        cout << vec[i] << ", ";
    }
    cout << endl;

}

std::ostream &operator <<(ostream & os, Support::myString &s){
    os << s.getString() << endl;
    return os;
}

Support::myString::myString(string constrStr){
    this->str = constrStr;
}

string Support::myString::getString(){
    return this->str;
}

/*
  @return index where the suffix starts, how many characters are without suffix
  */
int Support::myString::endsWith(string suffix){
    int endStr = str.length() - suffix.length();

    for(int i = endStr; i < str.length(); ++i){
        if (this->str[i] != suffix[i-endStr]){
            return 0;
        }
    }

    return endStr;
}
