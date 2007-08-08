#include "ArrayCIN.h"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

ArrayCIN::ArrayCIN(char* fileName, bool enable_reverse)
{
    ifstream cinfile;
    string line;
    vector< string > stringVec;

    delimiters = string("\t");

    m_reverse = enable_reverse;

    cinfile.open(fileName, ios::in);
    while (!std::getline(cinfile, line).eof())
    {
        stringVec.push_back(line);
    }
    parseCinVector(stringVec);


    if (m_reverse)
        setReverseMap();

    setMap();
}

void ArrayCIN::parseCinVector(const vector< string >& cinVec)
{
    bool is_start_reading = false;
    vector< string >::const_iterator it;

    for (it = cinVec.begin(); it != cinVec.end(); ++it)
    {
       if (it->find("#") == 0 && (!is_start_reading))
           continue;

       is_start_reading = true;

       const string& line = *it;
       string::size_type del_pos;

       if((del_pos=line.find_first_of(delimiters)) != string::npos)
       {
           string key = line.substr(0, del_pos);
           unsigned int value_pos=line.find_first_not_of(delimiters, del_pos);
           if( value_pos == string::npos )
               continue;
           string value = line.substr( value_pos, line.length() - value_pos );

           lowerStr(key);
           block_buf.push_back( make_pair( key, value ) ); 
       }
    }
}

void ArrayCIN::setMap()
{
    vector< pair<string, string> >::const_iterator it;

    stable_sort(block_buf.begin(), block_buf.end(), cmpBlockEntry());

    for(it = block_buf.begin(); it != block_buf.end(); ++it)
        if( !maps.empty() && maps.back().first == it->first )
            maps.back().second.push_back(it->second);
        else{
            vector<string> v;
            v.push_back(it->second);
            maps.push_back( make_pair( it->first, v) );
        }   
    block_buf.clear();

}

void
ArrayCIN::setReverseMap()
{
    vector< pair<string, string> >::const_iterator it;

    stable_sort(block_buf.begin(), block_buf.end(), cmpRevBlockEntry());

    for(it = block_buf.begin(); it != block_buf.end(); ++it)
        if( !reverse_map.empty() && reverse_map.back().first == it->second )
            reverse_map.back().second.push_back(it->first);
        else{
            vector<string> v;
            v.push_back(it->first);
            reverse_map.push_back( make_pair( it->second, v) );
        }   
}

int ArrayCIN::getWordsVector(const string& key, vector<string>& outVectorRef)
{
    int pos;
    if( (pos=searchCinMap( maps, key )) != -1) {
        outVectorRef = maps[pos].second;
        return outVectorRef.size();
    }
    else
        outVectorRef.clear();
    return 0;    
}

int ArrayCIN::getReverseWordsVector(const string& key, vector<string>& outVectorRef)
{
    if (!m_reverse)
        return 0;

    int pos;
    if( (pos=searchCinMap( reverse_map, key )) != -1) {
        outVectorRef = reverse_map[pos].second;
        return outVectorRef.size();
    }
    else
        outVectorRef.clear();
    return 0;    
}

int ArrayCIN::searchCinMap(const CinMap& in_map, const string& key) const
{
    int mid, low = 0, high = in_map.size() - 1;
    while(low <= high){
        mid = (low + high) / 2;
        if( key == in_map[mid].first )
            return mid;
        else if( key < in_map[mid].first )
            high = mid -1;
        else
            low = mid + 1;
    }
    return -1;
}

ArrayCIN::~ArrayCIN()
{
}

void ArrayCIN::lowerStr(string& str)
{
    for(int i=str.length()-1;i>=0;i--)
        if( !isprint(str[i]) )
            return;
    transform( str.begin(), str.end(), str.begin(),(int(*)(int)) tolower );
}

