#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "ArrayCIN.h"

using namespace std;


int main()
{
    char* file = "array30.cin";

    ArrayCIN cin(file, false);

    vector<string> vec;
    cin.getWordsVector(string("qiue"), vec);

    for (int i = 0; i < vec.size(); i++)
    {
        cout << vec[i] << "\n";
    }

    return 0;
}
