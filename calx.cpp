#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cmath>
using namespace std;
template<class R, class A> R narrow_cast(const A& a)
{
	R r = R(a);
	if (A(r)!=a) throw runtime_error(string("info loss"));
	return r;
}
inline void keep_window_open()
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
const char name='a';
const char constant='C';
const char help='h';
const char let='L';
const char powd = 'p';
const char sqrtd='s';
const string sqrtkey="sqrt";
const string quitkey="quit";
const string powkey="pow";
const string declkey="let";
const string constkey="const";
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
    void putback(Token t) {buffer=t;
    full=true;}
    void ignore(char c);
    private:
    bool full{false};
    Token buffer;
};
Token Token_stream::get()
{
    if(full)
    {
        full=false;
        return buffer;
    }
    char ch=' ';
    while (isspace(ch)&&ch != '\n')
        ch = cin.get(); 
    switch(ch){
        case print:
        case '\n':
         return Token{print};
        case 'H':
        case 'h':
        return Token{help};
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
        if(s==constkey) return Token{constant};
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
    bool constant;
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
            if(v.constant) throw runtime_error("cannot change the value of a constant");
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
double define_name(string var,double val,bool con)
{
    var_table.push_back(Variable{var,val,con});
    return val;
}
double expression(); 
double function(char c)
{
    vector<double> args;    
    Token t = ts.get();
    if (t.kind != '(') throw runtime_error("'(' expected after function call");
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
    switch (c) {
    case sqrtd:
        if (args[0] < 0) throw runtime_error("sqrt() is undefined for negative numbers");
        return sqrt(args[0]);
    case powd:
        return pow(args[0], args[1]);
    default:
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
    case name:
		{
			Token next = ts.get();
			if (next.kind == '=') {	// handle name = expression
				double d = expression();
				set_value(t.name,d);
				return d;
			}
			else {
				ts.putback(next);		// not an assignment: return the value
				return get_value(t.name); // return the variable's value
			}
		}
        case '-':
        return -primary();
        case '+':
        return primary();
        case number:
        return t.value;
        case sqrtd:
        case powd:
        return function(t.kind);
        default:
        throw runtime_error("primary expected");
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
    bool is_const = false;
    Token t = ts.get();
    if (t.kind == constant) is_const = true;

    // Check part by part of Declaration gramamr rule behind "let" or "const"
	Token t2 = ts.get();
	if (t2.kind != name) throw runtime_error("name expected in declaration");
	string var_name = t2.name;
	if (is_declared(var_name)) throw runtime_error("Variable declared twice");

	Token t3 = ts.get();
	if (t3.kind != '=') throw runtime_error("= missing in declaration");

	double d = expression();
	var_table.push_back(Variable{var_name,d,is_const});
	return d;
}
double statement()
{
    Token t=ts.get();
    switch(t.kind){
        case let:case constant:
        {
        ts.putback(t);
        return declaration();
        }
        default:
		ts.putback(t);
		return expression();
    }
}
void log()
{
    cout<<"basic:\n"<<"\n";
    cout<<"\t"<<"press h or Help for this page\n";
        cout<<"\t"<<"Enter ; or a newline to print output\n";
    cout<<"\t"<<"Enter q or quit to exit the program\n";
    cout<<"features:\n"<<endl;
    cout<<"functions:\n";
    cout<<"\t"<<"sqrt(expression) -for square root\n";
        cout<<"\t"<<"pow(e,i) -for e^i\n";
    cout<<endl;
    cout<<"\t enter ! after a digit for recursion\n";
    cout<<"\t"<<"you can group by using ( and }\n";
    cout<<"\t"<<"you can +,-,/,*,% with floating point numbers\n";
    cout<<endl;
    cout<<"Variables:\n";
    cout<<"\t"<<"You can declare variables by:\n";
    cout<<"\t"<<"let var =expression\n";
    cout<<"\t"<<"assigining variables\n";
    cout<<"\t"<<"var=expression\n";
    cout<<"\t"<<"declaring constant:\n";
    cout<<"\tconst var=expression\n";
    cout<<"Predefined names:\n";
    cout<<"\t"<<"pi=3.1415926535\n";
    cout<<"\t"<<"e=2.71828182846\n"; 
}
void clean_up_mess()
{
    ts.ignore(print);
}
void calculate()
{
    cout<<"enter H or h for help\n";
    while(cin)
        try{
        cout<<prompt;
        Token t=ts.get();
        while(t.kind==print)
            t=ts.get();
        if(t.kind==quit) return;
            ts.putback(t);
        if(t.kind==help){
            system("clear");
            log();
            ts.ignore(print);
        }
        else{
            ts.putback(t);
        cout<<"="<<statement()<<"\n";   
        } 
    }
catch(exception& e){
    cerr<<"Error: "<<e.what()<<endl;
    clean_up_mess();
}
}
int main(){
    define_name("pi",3.1415926535,true);
    define_name("e", 2.71828182846, true);
    calculate();
    return 0;    
}
