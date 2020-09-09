#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cmath>
using namespace std;
inline void keep_window_open()//for windows
{
    cin.clear();
    cout << "Please enter a character to exit\n";
    char ch;
    cin >> ch;
    return;
}

inline void keep_window_open(string s)
{
    if (s=="") return;
    cin.clear();
    cin.ignore(120,'\n');
    for (;;) {
        cout << "Please enter " << s << " to exit\n";
        string ss;
        while (cin >> ss && ss!=s)
            cout << "Please enter " << s << " to exit\n";
        return;
    }
}
const char number='8';
const char let='L';
const char powd = 'p';
const char sqrtd='s';
const string sqrtkey="sqrt";
const string quitkey="quit";
const string powkey="pow";
const string declkey="let";
const char name='a';
const char quit='q';
const string prompt="> ";
const char print=';';
int fact(uint64_t x)
{
    int f=1;
    while(x>0)
    {
        f*=x;
        --x;
    }
    return f;
}
class Token{
public:
    char kind;
double value;
string name;
Token(char ch) :kind{ch} { }
// initialize kind with ch
Token(char ch, double val) :kind{ch}, value{val} { } // initialize kind
// and value
Token(char ch, string n) :kind{ch}, name{n} { }
};
class Token_stream{
    public:
    Token_stream() :full{false}, buffer{0} { }
    Token get();
    void putback(Token t);
    void ignore(char c);
    private:
    bool full{false};
    Token buffer;
};
void Token_stream::putback(Token t)
{
    buffer=t;
    full=true;
}
Token Token_stream::get()
{
    if(full)
    {
        full=false;
        return buffer;
    }
    char ch;
    cin>>ch;
    switch(ch){
        case print:
        case quit:
        case'(':
        case')':
        case'{':
        case'}':
        case'+':
        case'-':
        case'=':
        case'*':
        case'/':
        case',':
        case'%':
        case'!':
        return Token{ch};
        case'.':
        case'0':case'1':case'2':case'3':case'4':case'5':case'6':case'7':case'8':case'9':
        {
            cin.putback(ch);
            double val;
            cin>>val;
            return Token{number,val};
        }
    default:
    if(isalpha(ch)){
        string s;
        s+=ch;
        while(cin.get(ch)&&(isalpha(ch)||isdigit(ch)||ch=='_')) s+=ch;
        cin.putback(ch);
        if(s==declkey) return Token{let};
        if(s==quitkey) return Token{quit};
        if(s==sqrtkey) return Token{sqrtd};
        if(s==powkey) return Token{powd};
        return Token{name,s};
    }
    throw runtime_error("Bad Token");
    }
}
void Token_stream::ignore(char c)
{
    if(full&&c==buffer.kind){
        full =false;
        return;
    }
    full=false;
    char ch=' ';
    while (ch != c && ch != '\n')
        ch = cin.get();
    return;
}
Token_stream ts;
class Variable{
public:
    string name;
    double value;
};
vector<Variable> var_table;
double get_value(string s)
{
    for(Variable& v:var_table)
        if(v.name==s) return v.value;
    throw runtime_error("undefined name");
}
void set_value(string s, double d)
{
    for (Variable& v : var_table)
        if (v.name == s) {
            v.value = d;
            return;
        }
    throw runtime_error("set: undefined name");
}
bool is_declared(string s)
{
    for(Variable& x:var_table)
        if(x.name==s) return true;
    return false;
}
double define_name(string var,double val)
{
    if(is_declared(var)) throw runtime_error("declared twice");
    var_table.push_back(Variable{var,val});
    return val;
}
double expression(); 
double function(char c)
{
    vector<double> args;    // Vector to store (variable number) of function 
                            // arguments.
    Token t = ts.get();
    if (t.kind != '(') throw runtime_error("'(' expected after function call");

    // Handle argument list. Default: no arguments, do nothing, thus, no
    // default on switch statement.
    switch (c) {
    case sqrtd:
        args.push_back(expression());
        break;
    case powd:
        args.push_back(expression());
        t = ts.get();
        if (t.kind != ',') throw runtime_error("Bad number of function arguments");
        args.push_back(expression());
        break;
    }

    t = ts.get();
    if (t.kind != ')') throw runtime_error("Bad number of function arguments");

    // Evaluation snd restrictions implementation
    switch (c) {
    case sqrtd:
        if (args[0] < 0) throw runtime_error("sqrt() is undefined for negative numbers");
        return sqrt(args[0]);
    case powd:
        return pow(args[0], args[1]);
    default:
        // In case we have defined the name as a token for Function rule but
        // forgot to implement its evaluation
        throw runtime_error("Function not implemented");
    }
}
double primary()
{
    Token t=ts.get();
    switch(t.kind){
        case'(':
        {
        double d=expression();
        t=ts.get();
        if(t.kind!=')') throw runtime_error(") expected");
        return d;
    }
        case'{':
        {
        double d=expression();
        t=ts.get();
        if(t.kind!='}') throw runtime_error("} expected");
        return d;
    }
        case '-':
        return -primary();
        case '+':
        return primary();
        case number:
        return t.value;
        case name:
        return get_value(t.name);
        case sqrtd:
        return function(t.kind);
        case powd:
        return function(t.kind);
        default:
        throw runtime_error("primary expeected");
    }
}
double term()
{
	double left=primary();
	Token t=ts.get();
    while(true){
	switch(t.kind){
		case'*':
		left*=primary();
		t=ts.get();
		break;
		case'/':
        {
		double d=primary();
        if(d==0) throw runtime_error("cannot divide by zero");
        left/=d;
		t=ts.get();
		break;
    }
    case'%':
    {
        double d=primary();
        if(d==0) throw runtime_error("cannot divide by zero");
        left=fmod(left,d);
        t=ts.get();
        break;
    }
        case '!': 
        {
    uint64_t d = fact(left);
    return d;
        }
		default:
		ts.putback(t);
		return left;
	}
    }
}
double expression()
{
	double left=term();
    Token t=ts.get();
    while(true){
        switch (t.kind){
            case'+':
            left+=term();
            t=ts.get();
            break;
            case'-':
            left-=term();
            t=ts.get();
            break;
            default:
            ts.putback(t);
            return left;
        }
    }
}
double declaration()
{
    Token t=ts.get();
    if(t.kind!=name) throw runtime_error("name expected in declaration");
    string var_name=t.name;
    Token t2=ts.get();
    if(t2.kind!='=') throw runtime_error("expected '=' in declaration");
    double d=expression();
    define_name(var_name,d);
    return d;
}
double statement()
{
    Token t=ts.get();
    switch(t.kind){
        case let:
        return declaration();
        default:
        ts.putback(t);
        return expression();
    }
}
void clean_up_mess()
{
    ts.ignore(print);
}
void calculate()
{
    while(cin)
        try{
        cout<<prompt;
        Token t=ts.get();
        if(t.kind==print)
            t=ts.get();
        if(t.kind==quit) return;
            ts.putback(t);
        cout<<"="<<statement()<<endl;
}
catch(exception& e){
    cerr<<"Error: "<<e.what()<<endl;
    clean_up_mess();
}
}
int main(){
    define_name("pi",3.1415926535);
    calculate();
    return 0;    
}
