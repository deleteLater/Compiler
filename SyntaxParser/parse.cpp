#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

/*
 * Make some convention:
 *   $ means epsilon
 *   Only upperCase letter can be a non-terminal symbol
 *   (That means all symbols are terminal except upperCase letter)
 * 
 * Our operator-precedence Grammer:
 *     E -> E+T | T
 *     T -> T*F | F
 *     F -> (E) | i
 */
map<char, vector<string>> grammer
{
    {'E', {"E+T", "T"}},
    {'T', {"T*F", "F"}},
    {'F', {"(E)", "i"}}
};

/*
 *  firstVT and lastVT
 */
map<char, vector<char>> firstVT
{
    {'E', {}},
    {'T', {}},
    {'F', {}}
};
map<char, vector<char>> lastVT
{
    {'E', {}},
    {'T', {}},
    {'F', {}}
};

void make_firstVT()
{
    //add direct terminal and 
    //find those non-terminals which has inclusions between them

    //if _include[P] = X, that means firstVT[P] should include firstVT[X]
    map<char,char> inclusions;

    //that's rule one:
    //     if P->a... or P->Qa... ,then 'a' should included in firstVT[P]
    for (auto v : grammer)
    {
        for (string production : grammer[v.first])
        {
            //parse production
            for (size_t i = 0; i < production.size(); i++)
            {
                if (!isupper(production[i]))
                {
                    firstVT[v.first].push_back(production[i]);
                    break;
                }
                else
                {
                    //make a mark: firstVT[v.first] should include firstVT[production[i]]
                    inclusions[v.first] = production[i];
                }
            }
        }
    }

    //deal with those non-terminals which has inclusions between them
    //that's rule two:
    //      if P->Q... ,then firstVT[Q] should be included in firstVT[P]

    //在这里有一个问题，就是当 firstVT(E) 包含 firstVT(T),firstVT[T] 包含 firstVT[F]时
    //包含是有先后顺序的
    for(auto v : inclusions)
    {
        //append elements in firstVT[v.first] to firstVT[v.second]
        firstVT[v.first].insert
        (
            firstVT[v.first].end(),
            firstVT[v.second].begin(),
            firstVT[v.second].end()
        );
    }
}

int main(int argc, char const *argv[])
{
    make_firstVT();
    return 0;
}
