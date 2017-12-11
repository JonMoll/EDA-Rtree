#ifndef CCSV_H
#define CCSV_H

#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

class CCSV
{
    private:
        vector<string> m_lines;

    public:
        CCSV(string file_name);

        int NumberLines();
        string GetWord(int row, int column);
};

#endif
