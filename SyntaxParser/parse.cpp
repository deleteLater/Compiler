#include <iostream>
#include <string>
#include <map>
#include <set>
#include <iomanip>

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
 * 
 * Another test grammer:
 *      E -> E+T | T
 *      T -> T*F | F
 *      F -> P^F | P
 *      P -> (E) | i
 */
map<char, set<string>> grammer
{
    {'E', {"E+T", "T"}},
    {'T', {"T*F", "F"}},
    {'F', {"(E)", "i"}}
   /*
    {'E',{"E+T","T"}},
    {'T',{"T*F","F"}},
    {'F',{"P^F","P"}},
    {'P',{"(E)","i"}}
    */
};

/*
 *  firstVT and lastVT
 */
map<char, set<char>> firstVT;
map<char, set<char>> lastVT;
void init_VTs()
{
    for(auto x : grammer)
    {
        firstVT.insert(pair<char,set<char>>(x.first,{}));
        lastVT.insert(pair<char,set<char>>(x.first,{}));
    }
}
void print_firstVT()
{
    for (auto v : firstVT)
    {
        cout << "firstVT[" << v.first << "]: ";
        for (auto x : v.second)
        {
            cout << x << " ";
        }
        cout << endl;
    }
}
void print_lastVT()
{
    for (auto v : lastVT)
    {
        cout << "lastVT[" << v.first << "] : ";
        for (auto x : v.second)
        {
            cout << x << " ";
        }
        cout << endl;
    }
}

//if inclusions[P] = X, that means firstVT[P]/lastVT[P] should include firstVT[X]/lastVT[X]
map<char, char> inclusions;
void print_inclusions()
{
    for(auto v : inclusions)
    {
        cout << v.first << " include " << v.second << endl;
    }
}

//if marked['E'] = true, that means E's inclusion have been handled
map<char, bool> marked;
void init_marked()
{
    for(auto v : grammer)
    {
        marked.insert(pair<char,bool>(v.first,false));
    }
}

/*
 *  Make firstVT for each non-terminal
 */
void firstVT_inclusions(char non_terminal)
{
    //WHY RECURSIVE?
    //suppose we have two inclusion need handling now
    //      firstVT['E'] include firstVT['T'](one)
    //      firstVT['T'] include firstVT['F'](two)
    //we need handle the two first,because firstVT['F'] have no inclusion with other
    //that means firstVT['F'] is complete,it can to be included by other.
    //we cannot handle one first because firstVT['E'] depends on firstVT['T']
    //while first['T'] is not complete until two finished

    if (inclusions.find(non_terminal) == inclusions.end())
    {
        return;
    }
    //when we make one's firsVT,we may also make other's firstVT,see example above
    //when we make E's firstVT,we actually also make T's firstVT in this process
    //so we need mark it have been handled to avoid handle it again
    marked[non_terminal] = true;
    firstVT_inclusions(inclusions[non_terminal]);
    //append elements
    firstVT[non_terminal].insert(
        firstVT[inclusions[non_terminal]].begin(),
        firstVT[inclusions[non_terminal]].end());
}
void make_firstVT()
{
    //init common variable
    inclusions.clear();
    for (auto v : marked)
    {
        marked[v.first] = false;
    }
    //add direct terminal and
    //find those non-terminals which has inclusions between them

    //that's rule one:
    //     if P->a... or P->Qa... ,then firstVT[P] include 'a'
    for (auto v : grammer)
    {
        for (string production : grammer[v.first])
        {
            //parse production
            //actually because of the operator-precedence grammer
            //we only need to analyze at most two character of the production
            for (size_t i = 0; i < production.size(); i++)
            {
                if (!isupper(production[i]))
                {
                    firstVT[v.first].insert(production[i]);
                    break;
                }
                else if (v.first != production[i])
                {
                    //this means firstVT[v.first] include firstVT[production[i]]
                    inclusions[v.first] = production[i];
                }
            }
        }
    }
    //deal with those non-terminals which has inclusions between them
    //that's rule two:
    //      if P->Q... ,then firstVT[Q] should be included in firstVT[P]
    for (auto v : inclusions)
    {
        //if v.first's inclusion haven't been handled yet
        if (!marked[v.first])
            firstVT_inclusions(v.first);
    }
}

/*
 *  Make lastVT for each non-terminal
 *  just like how we deal with firstVT
 */
void lastVT_inclusions(char non_terminal)
{
    //see firstVT_inclusions then you can understander
    if (inclusions.find(non_terminal) == inclusions.end())
    {
        return;
    }
    marked[non_terminal] = true;
    lastVT_inclusions(inclusions[non_terminal]);
    lastVT[non_terminal].insert(
        lastVT[inclusions[non_terminal]].begin(),
        lastVT[inclusions[non_terminal]].end());
}
void make_lastVT()
{
    //init common variable
    inclusions.clear();
    for (auto v : marked)
    {
        marked[v.first] = false;
    }
    //add direct terminal and
    //find those non-terminals which has inclusions between them

    //that's rule one:
    //     if P->...a or P->...aQ ,then 'a' should included in lastVT[P]
    for (auto v : grammer)
    {
        for (string production : grammer[v.first])
        {
            //parse production
            for (int i = production.size() - 1; i >= 0; i--)
            {
                if (!isupper(production[i]))
                {
                    lastVT[v.first].insert(production[i]);
                    break;
                }
                else if (v.first != production[i])
                {
                    //this means lastVT[v.first] should include lastVT[production[i]]
                    inclusions[v.first] = production[i];
                }
            }
        }
    }
    //that's rule two:
    //      if P->...Q ,then lastVT[P] include lastVT[Q]
    for (auto v : inclusions)
    {
        //if v.first's inclusion haven't been handled yet
        if (!marked[v.first])
            lastVT_inclusions(v.first);
    }
}

int main(int argc, char const *argv[])
{
    //must start with init
    init_VTs();
    init_marked();

    make_firstVT();
    make_lastVT();
    print_firstVT();
    print_lastVT();
    return 0;
}
