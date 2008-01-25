#ifndef _ARRAY_CIN_H_
#define _ARRAY_CIN_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

template<class T1, class T2> 
struct CmpPair {
    typedef pair<T1, T2> T;
    bool operator() (const T& a, const T& b){ 
        return a.first < b.first;
    }
};  

template<class T1, class T2> 
struct CmpRevPair {
    typedef pair<T1, T2> T;
    bool operator() (const T& a, const T& b){ 
        return a.second < b.second;
    }
};  

typedef CmpPair<string, string> cmpBlockEntry;
typedef CmpRevPair<string, string> cmpRevBlockEntry;
typedef CmpPair<string, vector<string> > cmpMapEntry;

enum EnumSelectArrayCIN
{
    Full = 0, ShortCode = 1, Special = 2
};

class ArrayCIN
{
private:
    typedef vector< pair< string, vector< string > > > CinMap;
    string delimiters;
    vector< pair< string, string > > block_buf;
    CinMap maps;
    CinMap reverse_map;

    bool m_reverse;

public:
    ArrayCIN(char* fileName, bool enable_reverse=false);
    int getWordsVector(const string& inKey, vector<string>& outVectorRef);
    int getReverseWordsVector(const string& inKey, vector<string>& outVectorRef);
    int getWordsVectorWithWildcard(const string& inKey, vector<string>& outVectorRef);
    ~ArrayCIN();

protected:
    void parseCinVector(const vector< string >& cinVector);
    void setMap();
    void setReverseMap();
    int searchCinMap(const CinMap& in_map, const string& key) const;
    void lowerStr(string& str);
};

#endif
