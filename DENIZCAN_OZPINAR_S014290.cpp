#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

static vector<string> states_names,tran;
static string blank, start, reject, accept, str;
static long limit = 1000000;

struct Transaction{
private:
    string dest;
    string read;
    char replace;
    char direction;
public:
    Transaction(string read,char replace,char direction, string dest) : dest{dest},read{read},replace{replace}, direction{direction}{}
    auto get_read(){return read;}
    auto get_replace(){return replace;}
    auto get_dir(){return direction;}
    auto get_dest(){return dest;}
    auto set_replace(auto r){replace = r;}

    auto print(){
        cout<<" { ";
        cout<<read<<" -> ";
        cout<<replace<< " , ";
        cout<<direction<<" to ";
        cout<<dest<<"";
        cout<<" } "<<endl;
    }
};

struct State
{
    string name;
    vector<Transaction> transactions;
    long visited_count = 0;

    State(string name, Transaction transaction): name{name}{transactions.push_back(transaction);}
    State(string name) : name{name},transactions{}{}

    auto add_tran(string read,char replace,char direction,string dest){transactions.push_back(Transaction(read,replace,direction,dest));}

    auto print(){
        if(transactions.size() > 0){
            for(auto &transcation : transactions){
                cout<<name<<"{ ";
                transcation.print();
                cout<<" }"<<endl;
            }
        }else cout<<name<<endl;
    }
    auto get_transcation(){return transactions;}

    auto visited(){
        visited_count++;
        if(visited_count == limit) return true;
        else return false;
    }
};
//init states
static vector<State> states;

template <typename T, typename V>
auto tokenize(const T  &str, const V delim){
    stringstream ss(str);
    vector<string> out;
    string s;
    while (getline(ss, s, delim)) out.push_back(s);
    return out;
}

template<typename V>
auto print(V v){for (auto &i: v) cout << i << endl;}

template<typename T>
auto create_table(const T& data){
    string current_state = "BLANK";
    vector<string> headings = {"BLANK","STATES","START_STATE","ACCEPT_STATE","REJECT_STATE","TRANSACTIONS","STRING"};
    for (auto i: data) {
         if (current_state.compare(headings[0]) == 0){
            if (i.compare(headings[1]) == 0)
                current_state = headings[1];
            else if (i.compare(headings[0])==0){}
            else blank = i;

        }else if (current_state.compare(headings[1]) == 0){
            if (i.compare(headings[2]) == 0)
                current_state = headings[2];
            else if (i.compare(headings[1])==0){}
            else states_names.push_back(i);

        }else if (current_state.compare(headings[2]) == 0){
            if (i.compare(headings[3]) == 0)
                current_state = headings[3];
            else if (i.compare(headings[2])==0){}
            else start = i;

        }else if (current_state.compare(headings[3]) == 0){
            if (i.compare(headings[4]) == 0)
                current_state = headings[4];
            else if (i.compare(headings[3])==0){}
            else accept = i;

        }else if (current_state.compare(headings[4]) == 0){
            if (i.compare(headings[5]) == 0)
                current_state = headings[5];
            else if (i.compare(headings[4])==0){}
            else reject = i;

        }else if (current_state.compare(headings[5]) == 0){
            if (i.compare(headings[6]) == 0)
                current_state = headings[6];
            else if (i.compare(headings[5])==0){}
            else tran.push_back(i);

        }else if (current_state.compare(headings[6]) == 0){
            if (i.compare(headings[6])==0){}
            else str = i;
        }
    }
    //create states
    for(auto &name : states_names){
        states.push_back(State(name));
    }
    //add tranascton to rules
    for(auto &tr : tran){
        auto tmp = tokenize(tr,' ');
        for(auto &st : states){
            if(tmp[0] == st.name){
                st.add_tran(tmp[1],tmp[2][0],tmp[3][0],tmp[4]);
                break;
            }
        }
    }
}

template<typename G>
auto print_states(){for(auto &st : states)st.print();}

template<typename T>
auto find_state(T name){
    for(auto &state : states){if (state.name == name) return &state;}

    throw;
}

auto run(){
    vector<string> visited;
    auto current_state = find_state(start);
    //insert blank character to start and to the end of str
    str.insert(0, blank);
    str.insert(str.size(),blank);
    unsigned long counter = 1;
    loop:
    visited.push_back(current_state->name);
    auto head = str[counter];
    //rises count of visited state if reach limit goes to loop state
    if(current_state->visited()) goto foundloop;
    //main logic
    for(auto &transcation : current_state->get_transcation()){
        if (transcation.get_read() == string(1,head)){
            //tran.print();
            str[counter] = transcation.get_replace();
            current_state = find_state(transcation.get_dest());
            if(transcation.get_dir() == 'R'){
                if(str.size() == counter+1)
                    str.insert(str.size(),blank);
                counter++;
            }else if(transcation.get_dir() == 'L' && counter != 0){
                counter--;
            }
            goto loop;
        }
    }
    //output to console for Rout:
    cout<<"Rout: ";
    for(auto &state : visited){cout<<state<<" ";}
    cout<<""<<endl;
    //found loop printing
    if(false){
    foundloop:
        string looped_state;
        //find looped element
        for(auto &state : states){
            if(state.visited_count == limit) looped_state = state.name;
        }
        //erease duplicates of looped
        visited.erase(std::remove(visited.begin(), visited.end(), looped_state), visited.end());

    cout<<"Rout: ";
    for(auto &state : visited){cout<<state<<" ";}
    cout<<looped_state<<" <-looped state";
    cout<<""<<endl;
    cout<<"Result: loop"<<endl;
    }

    if (current_state->name == reject) cout<<"Result: rejected"<<endl;

    else if(current_state->name == accept) cout<<"Result: accept"<<endl;
}
//removes non printing characters
template<typename S>
auto rm_nonprinting (S& str)
{
    str.erase (remove_if (str.begin(), str.end(),
                          [](unsigned char c){
                              return !isprint(c);
                          }),
               str.end());
}

int main()
{
    vector<string> lines;
    string str;
    ifstream file("Input_DENIZCAN_OZPINAR_S014290.txt");
    if(file){
        while (getline(file, str) && file)
        {
            if(str.size() > 0){
                rm_nonprinting(str);
                lines.push_back(str);
            }
        }
        file.close();
        create_table(lines);
        run();
    }else{cout<<"No file was founded"<<endl;}

    return 0;
}
