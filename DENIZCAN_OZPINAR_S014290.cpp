#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>
#include <set>
#include <filesystem>

using namespace std;
//for web compiler test
//vector<string> arr ={"ALPHABET","0","1","STATES","A","B","C","START","A","FINAL","C","TRANSITIONS","A 0 A","A 1 A","A 1 B","B 1 C","END"};

class nfa {
private:
    set<string> alphabet, states, final;
    string initial;
    map< pair<string, string>, set<string> > transitions;
    map< set<string>, set<string> > cont;

public:
    template<typename T,typename A,typename G>
    auto set_states(const T &states, const A &initial, const G &final) {
        this->states = states;
        this->final = final;
        this->initial = *initial.begin();
    }

    template<typename T>
    auto set_alphabet(const T &alphabet) {
       this->alphabet = alphabet;
    }

    template<typename T,typename A,typename G>
    auto add_transition(const T& current_state, const A& alphabet, const G& result) {
        transitions[make_pair(current_state, alphabet)] = result;
    }

    template<typename T,typename A>
    auto get_state(const T& state, const A& alphabet) {
        auto key = make_pair(state, alphabet);
        return transitions[key];
    }

    template<typename T,typename A>
    auto get_states(const T &states, const A &alphabet) {
        set<string> selected_states;
        for(auto i : states) {
            auto out_state = get_state(i, alphabet);
            selected_states.insert(out_state.begin(), out_state.end());
        }
        return selected_states;
    }

    template<typename V>
    auto print(V v){
        for (auto i: v)
            cout << i << " <- ";
        cout<< "end"<<endl;
    }

    template<typename T>
    auto do_cont(const T &state) {
        set<string> key;
        key.insert(state);
        if(cont.count(key) == 1) return cont[key];
        set<string> tmp_states;
        vector<string> remaining;
        remaining.push_back(state);
        while (!remaining.empty()) {
            string current = remaining.front();
            tmp_states.insert(current);
            remaining.erase(remaining.begin());
        }
        cont[key] = tmp_states;
        return tmp_states;
    }

    auto do_cont(const set<string> &states) {
        if (cont.count(states) == 1) return cont[states];
        set<string> tmp_states;
        for(auto i: states) {
            set<string> current_states = do_cont(i);
            tmp_states.insert(current_states.begin(), current_states.end());
        }
        cont[states] = tmp_states;
        return tmp_states;
    }

    template <typename T, typename V>
    auto contains(T cont, V object){return find(cont.begin(), cont.end(), object) != cont.end();}

    template<typename T>
    auto is_final(const T &states) {
        for(auto i: states) { if(contains(final, i)) return true;}
        return false;
    }

    template <typename T, typename V>
    auto tokenize(const T  &str, const V delim){
        stringstream ss(str);
        vector<string> out;
        string s;
        while (getline(ss, s, delim)) out.push_back(s);
        return out;
    }

    template <typename T,typename C>
    auto to_string(T str, C delim) {
        string out;
        string sep = "";
        string sep2; sep2 += delim; sep2 += ' ';
        for (auto i: str) {
            out += sep + i;
            sep = sep2;
        }
        return out;
    }

    template <typename T>
    auto to_string(T str) {return to_string(str, ' ');}

    template<typename T, typename S,typename D>
    auto print_dfa(const T &tran,const S &states,const D &dfa_states){
        cout<< "ALPHABET"<<endl;
        for(auto i: alphabet)
            cout<<i<<endl;
        cout<<"STATES"<<endl;
        for(auto i: states)
            cout<<i<<endl;
        cout<< "START"<<endl;
        cout << to_string(dfa_states[0])<<endl;
        cout<<"FINAL"<< endl;
        for (auto current: dfa_states) {
           if(is_final(current))
              cout<< to_string(current)<<endl;
        }
        cout<<"TRANSITIONS"<<endl;
        for(auto i: tran){
            auto tmp = tokenize(i,' ');
            if(tmp[0] == "()")
                cout<<"(deadloop) "+tmp[1]+" (deadloop)"<<endl;
            else if (tmp[2] == "()")
                cout<<tmp[0]+" "+tmp[1]+" (deadloop)"<<endl;
            else
                cout<< i << endl;
        }
        cout<<"END"<< endl;

    }

    auto to_dfa() {
        set<string> initial = do_cont(this->initial);
        vector< set<string> > remain;
        remain.push_back(initial);
        vector< set<string> > dfa_states;
        vector<string> dfa_con;
        vector<string> new_states;
        while(!remain.empty()) {
            set<string> current = remain.front();
            remain.erase(remain.begin());
            if(contains(dfa_states, current))
                continue;
            dfa_states.push_back(current);
            string tmp;
            auto current_set = to_string(current, ',');
            if(current_set.size() == 0) new_states.push_back("deadloop");
            else new_states.push_back(current_set);
            for (auto i: alphabet) {
                set<string> t_states = do_cont(get_states(current, i));
                string t_set = to_string(t_states, ',');
                tmp = "("+current_set+") "+i+" ("+t_set+")";
                dfa_con.push_back(tmp);
                if (!contains(dfa_states, t_states)) remain.push_back(t_states);
            }

        }
        print_dfa(dfa_con,new_states,dfa_states);
    }

    template<typename T>
    auto get_transition_string(const T& m) {
        string ret;
        for(auto i: m)
            ret += "(" + i->first.first + ", " + i->first.second + ") -> { " + to_string(i->second, ',') + " }\n";
        return ret;
    }
};

class utilities {
private:
    map<string, string> s_map;
    vector<string> alpha,state_names,tran;
    string start_state,final_state;
public:
    template<typename T>
    auto get_items(const T& key) {
        auto items = split(get_item(s_map, key), ',');
        set<string> item_set(items.begin(), items.end());
        s_map.erase(key);
        return item_set;
    }

    struct Column{
        string head;
        vector<string> contents;
        Column(string head,vector<string> contents) : head{head},contents{contents}{}
        string get(){
            string ret;
            ret = head+" : {";
            for (int i =0; i<contents.size();i++){
                if(i != contents.size()-1) ret = ret+contents[i]+", ";
                else ret = ret+contents[i];
            }
            ret = ret+"}";
            return ret;
        }
        auto get_start_stat(){
            string ret;
            ret = ret+head+" : ";
            for(auto i: contents)
                ret = ret+i;
            return ret;
        }
        auto get_head(){ return head;}

        auto add_val(auto val){contents.push_back(val);}
    };

    vector<Column> cols;
    auto get_start_state(){return start_state;}

    template<typename T,typename S>
    auto get_item(const T& map, const S& key) {return map.at(key);}

    template<typename T,typename C, typename R>
    auto split(const T &s, const C delim, R result) {
        stringstream ss;
        ss.str(s);
        string item;
        while (getline(ss, item, delim)) result++ = item;
    }

    template<typename T,typename C>
    auto split(const T &s, const C delim) {
        vector<string> elems;
        split(s, delim, back_inserter(elems));
        return elems;
    }

    template<typename V>
    auto print(V v){
        for (auto i: v) cout << i << endl;
    }

    template<typename T,typename C>
    auto tokenize(const T &str, const C delim)
    {
        stringstream ss(str);
        vector<string> out;
        string s;
        while (getline(ss, s, delim)) out.push_back(s);
        return out;
    }
    //creates table for table for dfa creation
    template<typename T>
    auto create_table(const T& data){
        string current_state = "ALPHABET";
        vector<string> headings = {"ALPHABET","STATES","START","FINAL","TRANSITIONS","END"};
        for (auto i: data) {
           if (current_state.compare(headings[0]) == 0){
               if (i.compare(headings[1]) == 0)
                  current_state = headings[1];
               else if (i.compare(headings[0])==0){}
               else alpha.push_back(i);

           }else if (current_state.compare(headings[1]) == 0){
               if (i.compare(headings[2]) == 0)
                  current_state = headings[2];
               else if (i.compare(headings[1])==0){}
               else state_names.push_back(i);

           }else if (current_state.compare(headings[2]) == 0){
               if (i.compare(headings[3]) == 0)
                  current_state = headings[3];
               else if (i.compare(headings[2])==0){}
               else start_state = i;

           }else if (current_state.compare(headings[3]) == 0){
               if (i.compare(headings[4]) == 0)
                  current_state = headings[4];
               else if (i.compare(headings[3])==0){}
               else final_state = i;

           }else if (current_state.compare(headings[4]) == 0){
               if (i.compare(headings[5]) == 0)
                  current_state = headings[5];
               else if (i.compare(headings[4])==0){}
               else tran.push_back(i);

           }else if(current_state.compare(headings[5]) == 0) break;

    }
        vector<string> tmp_states;
        for (auto i:state_names)
            tmp_states.push_back(i);

        Column S("Q",tmp_states);
        cols.push_back(S);

        vector<string> tmp_alp;
        for (auto i: alpha) tmp_alp.push_back(i);

        cols.push_back(Column("A",tmp_alp));

        for (auto i: tran){
            auto t = tokenize(i,' ');
            auto size = cols.size();
            auto if_added = true;
            auto tmp= "T("+t[0]+", "+t[1]+")";
            for (auto i =0;i<cols.size();i++){
                if(cols[i].get_head() == tmp){
                   cols[i].add_val(t[2]);
                   if_added = false;
                   break;
                }
            }
            if(if_added) cols.push_back(Column(tmp,vector<string>{t[2]}));
        }
            for (auto i: state_names){
                for( auto j: alpha){
                    auto if_added = true;
                    auto check_string = "T("+i+", "+j+")";
                    for (auto k: cols) {
                        if (check_string == k.get_head()){
                            if_added = false;
                            break;
                        }
            }
                    if(if_added) cols.push_back(Column(check_string,vector<string>{" "}));
            }

        }
        cols.push_back(Column("ST",vector<string>{start_state}));
        cols.push_back(Column("F",vector<string>{final_state}));

    vector<string> ret_vector;
    for (auto i: cols){
        if(i.head != "ST")ret_vector.push_back(i.get());
        else ret_vector.push_back(i.get_start_stat());
    }
    //print(ret_vector);
    return ret_vector;
    }

    template<typename T,typename C>
    auto strip(T& str, const C c) {
        for (int i = 0; i < str.length(); ++i) {
            char current = str[i];
            if(current == c) {
                str.erase(str.begin() + i);
                i--;
            }
        }
        return str;
    }

    template<typename T>
    auto strip(T& str) {
        return strip(str, ' ');
    }

    template<typename T,typename C,typename E>
    auto get_contained(const T& str, const C start, const E end) {
        auto first = str.find_first_of(start);
        auto last = str.find_last_of(end);
        return str.substr(first+1, last-first-1);
    }

    template<typename T>
    utilities(const T& data) {
        auto arr = create_table(data);
        string line;
        for (auto line : arr){
            if(line.length() <= 0) continue;

            auto splitted = split(line, ':');
            auto cont = splitted[1];
            auto key = strip(splitted[0]);
            //if not start state
            if(key != "ST") cont = get_contained(cont, '{', '}');

            s_map[key] = strip(cont);
        }
    }

    nfa create_nfa() {
        auto states = get_items("Q");
        auto alphabet = get_items("A");
        auto initial = get_items("ST");
        auto final = get_items("F");
        nfa nfa;
        nfa.set_states(states, initial, final);
        nfa.set_alphabet(alphabet);

        for (auto i: states) {
            for (auto j: alphabet) {
                auto key = "T(" + i + "," + j + ")";
                    nfa.add_transition(i, j, get_items(key));
            }
        }
        return nfa;
    }

};

template<typename V>
auto print(V v){for (auto i: v) cout << i << endl;}

//remove non printed str
template<typename S>
auto rm_nonprinting (S& str)
{
    str.erase (remove_if (str.begin(), str.end(),
                               [](unsigned char c){
                                   return !isprint(c);
                               }),
               str.end());
}

template<typename T>
auto create_dfa(const T& data) {
        utilities setup(data);
        nfa nfa = setup.create_nfa();
        nfa.to_dfa();
}

int main() {
    vector<string> data;
    string str;
    //read file from directory
    ifstream file("NFA1.txt");
    while (getline(file, str))
    {
        if(str.size() > 0){
            rm_nonprinting(str);
            //cout<<str<<endl;
            data.push_back(str);
        }
    }
    file.close();
    create_dfa(data);
    return 0;
}
