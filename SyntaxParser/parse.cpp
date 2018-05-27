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
    {'S',{"#E#"}},
    {'E', {"E+T", "T"}},
    {'T', {"T*F", "F"}},
    {'F', {"(E)", "i"}}
    /*
    {'S',{"#E#"}},
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

/*
 * Make precedence_table based on firstVT and lastVT
 */
set<char> terminals;
string vt_string{""};
void init_terminals()
{
    terminals.insert('#');
    for(auto v : grammer)
    {
        for(auto x : v.second)
        {
            //traveral every production
            for(char c : x)
            {
                if(!isupper(c))
                {
                    terminals.insert(c);
                }
            }
        }
    }
    for(auto v : terminals)
        vt_string += v;
}
size_t get_index(char ch)
{
    for(size_t i = 0; i < vt_string.size(); i++)
    {
        if(vt_string[i] == ch)
            return i;
    }
}
bool is_terminal(char ch)
{
    return terminals.find(ch) != terminals.end();
}
char **table = nullptr;
void make_table()
{
    //init table
    size_t ts = terminals.size();
    table = new char*[ts];
    for(size_t i = 0; i < ts; i++)
    {
        table[i] = new char[ts];
    }
    for(size_t i = 0; i < ts; i++)
    {
        for(size_t j = 0; j < ts; j++)
        {
            table[i][j] = '$';
        }
    }

    //make table
    for(auto g : grammer)
    {
        //for every production
        for(string p : grammer[g.first])
        {
            for(size_t i = 0; i < p.size() - 1; i++)
            {
                //X(i), X(i+1) are both VT
                if(is_terminal(p[i]) && is_terminal(p[i+1]))
                {
                    table[get_index(p[i])][get_index(p[i+1])] == '=';
                }
                //X(i) and X(i+2) are both VT, X(i+1) is VN
                if(i < p.size()-2 && is_terminal(p[i]) && is_terminal(p[i+2]) && !is_terminal(p[i+1]))
                {
                    table[get_index(p[i])][get_index(p[i+2])] = '=';
                }
                //X(i) is VT,X(i+1) is VN
                if(is_terminal(p[i]) && !is_terminal(p[i+1]))
                {
                    for(auto v : firstVT[p[i+1]])
                    {
                        table[get_index(p[i])][get_index(v)] = '<';
                    }
                }
                //X(i) is VN,X(i+1) is VT
                if(!is_terminal(p[i]) && is_terminal(p[i+1]))
                {
                    for(auto v : lastVT[p[i]])
                    {
                        table[get_index(v)][get_index(p[i+1])] = '>';
                    }
                }
            }
        }
    }
}
void print_table()
{
    cout << "   ";
    size_t ts = terminals.size();
    for(auto x : terminals)
    {
        cout << x << "  ";
    }
    cout << endl;
    for(size_t i = 0; i < ts; i++)
    {
        cout << vt_string[i] << "  ";
        for(size_t j = 0; j < ts; j++)
        {
            cout << table[i][j] << "  ";
        }
        cout << endl;
    }
}
int exceed_to(char ch1,char ch2)
{
    char flag = table[get_index(ch1)][get_index(ch2)];
    if(flag == '>')
        return 1;
    else if(flag == '<')
        return -1;
    return 0;
}

/*
 * Parse string based on operator-precedence table
 */
string production_structrue(string grammer)
{
    string ret{};
    for(auto v : grammer)
    {
        if(is_terminal(v))
            ret += 't';
        else
            ret += 'n';
    }
    return ret;
}
char reduction(string src)
{
    string src_structure = production_structrue(src);
    for(auto v : grammer)
    {
        for(auto x : v.second)
        {
            if(production_structrue(x) == src_structure)
                return v.first;
        }
    }
}
void parse(string input)
{
    char stack[10];
    stack[0] = '#';
    int top = 0;
    char nextch = input[0];
    int topVT_ptr = 0;
    char topVT{};
    while(nextch != '#' || topVT != '#')
    {
        (is_terminal(stack[top]) == true) ? (topVT_ptr = top) : (topVT_ptr = top -1);
        topVT = stack[topVT_ptr];
        if(exceed_to(topVT,nextch) <= 0)
        {
            stack[++top] = nextch;
            nextch = input[top];
        }
        else if(exceed_to(topVT,nextch) == 1)
        {
            (is_terminal(stack[topVT_ptr-1]) == true) ? (topVT_ptr--) : (topVT_ptr -= 2);
            while(exceed_to(stack[topVT_ptr],topVT) == 0)
            {
                (is_terminal(stack[topVT_ptr-1]) == true) ? (topVT_ptr--) : (topVT_ptr -= 2);
            }
            string prime_phrase {};
            for(size_t i = topVT_ptr + 1; i <= top; i++)
            {
                prime_phrase += stack[i];
            }
            top = topVT_ptr + 1;
            stack[top] = reduction(prime_phrase);
            (is_terminal(stack[top]) == true) ? (topVT_ptr = top) : (topVT_ptr = top -1);
            topVT = stack[topVT_ptr];
        }
        else 
        {
            cout << "error" << endl;
            return ;
        }
        cout << "stack value: ";
        string values {};
        for(size_t i = 0; i <= top; i++)
        {
            values += stack[i];
        }
        cout << left << setw(8) << values;
        cout << "\tnextch:" << nextch;
        cout << "\ttopVT:" << topVT;
        cout << endl;
    }
    cout << "Accept!" << endl;
}

int main(int argc, char const *argv[])
{
    //must start with init
    init_VTs();
    init_marked();
    init_terminals();

    //make operator-precedence table
    make_firstVT();
    make_lastVT();
    make_table();
    parse("i+i*i#");

    return 0;
}
