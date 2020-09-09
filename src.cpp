#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>
using namespace std;
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
const string prompt="> ";
const char print=';';
const char quit='q';
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
};
class Token_stream{
    public:
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
        case'*':
        case'/':
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
    while(cin>>ch)
        if(ch==c)return;
}
Token_stream ts;
double expression();
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
        cout<<"="<<expression()<<endl;
}
catch(exception& e){
    cerr<<"Error: "<<e.what()<<endl;
    clean_up_mess();
}
}
int main(){
    calculate();
    return 0;    
}