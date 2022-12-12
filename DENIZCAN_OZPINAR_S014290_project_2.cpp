#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;
class utilities {

public:
    struct Column{
        string head;
        vector<string> contents;
        Column(string head,vector<string> contents) : head{head},contents{contents}{}
        Column(string head) : head{head},contents{}{}
        Column(string head,string rule) : head{head},contents{}{contents.push_back(rule);}
        Column(string head,char rule) : head{head},contents{}{string tmp(1,rule);contents.push_back(tmp);}

        string get(){
            string ret;
            ret = head+" : {";
            for (unsigned long i =0; i< contents.size();i++){
                if(i != contents.size()-1) ret = ret+contents[i]+", ";
                else ret = ret+contents[i];
            }
            ret = ret+"}";
            return ret;
        }

        auto get_head(){ return head;}

        auto get_head_char(){return head[0];}

        auto operator== (const Column  &c){
            if(this->head == c.head && this->contents.size() == c.contents.size()){
                for(unsigned long var = 0; var < this->contents.size();var++){
                    if(this->contents[var] != c.contents[var] ) return false;
                }
                return true;
            }else return false;
        }

        auto get_size(){return contents.size();}

        auto add_val(string val){ if (find(contents.begin(), contents.end(), val) == contents.end()) contents.push_back(val);}

        auto get_rules(){ return contents;}

        auto update(string key,string n){ for(auto &i : contents)if(key == i) i=n;}

        auto update(string key,char n){for(auto &i : contents)if(key == i) i=n;}

        auto remove_char(char key){
            for(unsigned long var =0; var < contents.size();var++){
                if(contents[var][0] == key && contents[var].size() ==1){
                    contents.erase(contents.begin()+static_cast<int>(var));
                }
            }
        }

        auto remove_str(string key){
            for(unsigned long var =0; var < contents.size();var++){
                if(contents[var] == key){
                    contents.erase(contents.begin()+static_cast<int>(var));
                    break;
                }
            }
        }
    };

    vector<Column> cols;
    vector<char> alphabet={'A','B','C','D','E','F',
            'G','H','I','J','K','L','M','N','O','P',
            'Q','R','S','T','U','V','W','X','Y','Z'};

    template <typename T, typename V>
    auto tokenize(const T  &str, const V delim){
        stringstream ss(str);
        vector<string> out;
        string s;
        while (getline(ss, s, delim)) out.push_back(s);
        return out;
    }

    template<typename V>
    auto print(V v){for (auto i: v) cout << i << endl;}

    vector<string>term,start;
    template<typename T>
    auto create_table(const T& data){
        vector<string> non_term,rules;
        string current_state = "NON-TERMINAL";
        vector<string> headings = {"NON-TERMINAL","TERMINAL","RULES","START"};
        for (auto i: data) {
           if (current_state.compare(headings[0]) == 0){
               if (i.compare(headings[1]) == 0)
                  current_state = headings[1];
               else if (i.compare(headings[0])==0){}
               else non_term.push_back(i);

           }else if (current_state.compare(headings[1]) == 0){
               if (i.compare(headings[2]) == 0)
                  current_state = headings[2];
               else if (i.compare(headings[1])==0){}
               else term.push_back(i);

           }else if (current_state.compare(headings[2]) == 0){
               if (i.compare(headings[3]) == 0)
                  current_state = headings[3];
               else if (i.compare(headings[2])==0){}
               else rules.push_back(i);

           }else if (current_state.compare(headings[3]) == 0){
               if (i.compare(headings[3])==0){}
               else start.push_back(i);
           }
    }
        //non terminals
        for(auto &i :non_term){ cols.push_back(Column(i));}

        for(auto &i: rules){
            auto tmp = tokenize(i,':');
            for(auto &j : cols)if(j.get_head() == tmp[0]) j.add_val(tmp[1]);
        }
        return cols;
    }

    template<typename D>
    auto if_has_unit_pro(D& data){
        for(auto & item :data){
            for(auto &rule : item.get_rules()){
                for(auto &s_item : data){
                    if(s_item.get_head() == rule){
                        return true;
                    }
                }
            }
        }
        return false;
    }

    template<typename T>
    auto rem_unit_product(T &data){
        vector<string> tmp_rule;
        while(if_has_unit_pro(data)){
            for(auto &item : data){
                for(auto &rule : item.get_rules()){
                    //single rules
                    if(rule.size() == 1){
                        //single non terms
                        if(find(term.begin(), term.end(),rule) == term.end()){
                           //single type equal to itself case
                            if(rule == item.get_head()){
                                item.remove_char(rule[0]);
                            }else{
                                //equals to different non term
                                tmp_rule.push_back(rule);
                                for(auto &f_item :data){
                                    if(f_item.get_head() == rule){
                                        for(auto &f_rule : f_item.get_rules()){
                                            item.add_val(f_rule);
                                        }
                                        item.remove_char(rule[0]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return data;
    }

    auto contains(char a , char b){ if(a == b) return true;else return false;}

    template<typename C, typename G>
    auto contains(C &c, G g){if(c.find(g) != std::string::npos) return true; else return false;}

    auto findLocation(const string &sample, char findIt){
        vector<unsigned long> characterLocations;
        for(unsigned long i = 0; i < sample.size(); i++)
            if(sample[i] == findIt) characterLocations.push_back(i);

        return characterLocations;
    }

    void removeCharsFromString(string &str, char charsToRemove ) {str.erase(remove(str.begin(), str.end(), charsToRemove), str.end());}

    template<typename S,typename V>
    auto variety (const S &str,V val){
        vector<string> tmp;
        string contain = str;
        vector<unsigned long> location = findLocation(contain,val[0]);
        auto occurance = count(str.begin(), str.end(), val[0]);
        tmp.push_back(contain);

        for (unsigned long var = 0; var < static_cast<unsigned long>(occurance); var++) {
            contain.erase(location[static_cast<unsigned long>(var)],1);
            if (find(tmp.begin(), tmp.end(), contain) == tmp.end())
                tmp.push_back(contain);
            contain = str;
            }

        contain = str;
        if(val.size() ==1){
            removeCharsFromString(contain, val[0]);
            if (find(tmp.begin(), tmp.end(), contain) == tmp.end()) {
                if(contain != string(val,1)){
                    tmp.push_back(contain);
                }
            }
        }
        return tmp;
    }

    template<typename S,typename D,typename W>
    void replace_char(S &str, D d,W &w) {replace( str.begin(), str.end(), d, w[0]);}

    template<typename D>
    auto contains_epsilon(D &data){
        for (auto &item : data){
            for (auto &rule : item.get_rules()){
                if(rule.find('e') != std::string::npos){
                    return true;
                 }
            }
        }
        return false;
    }

    template<typename D>
    auto solve_epsilon(D &data){
        vector<string> st;
        string state_with_epsilon;
        vector<Column> new_cols;
        //finds epsilon non terminal
        while(contains_epsilon(data)){
        for (auto &item : data){
            for (auto &rule : item.get_rules()){
                if(rule.find('e') != std::string::npos){
                    state_with_epsilon = item.get_head();
                    item.remove_char('e');
                    goto after_epsilon;
                 }
            }
        }
    after_epsilon:
        vector<string> new_data;
            for (auto &item : data){
                for(auto &rule : item.get_rules()){
                        if(contains(rule,state_with_epsilon) || rule == state_with_epsilon){
                            new_data = variety (rule,state_with_epsilon);
                            for (unsigned long var = 0; var < new_data.size(); var++) {
                                auto tmp = variety(new_data[var],state_with_epsilon);
                                for (unsigned long n = 0; n < tmp.size(); n++) { if (find(new_data.begin(), new_data.end(), tmp[n]) == new_data.end()){
                                        if(tmp[n] != state_with_epsilon)
                                            new_data.push_back(tmp[n]);}}
                            }
                            //adds new combinatons values
                            for(auto &new_item : new_data){
                               if(new_item.size() != 0 && new_item != state_with_epsilon){
                                item.add_val(new_item);

                               }
                            }
                            if(rule == state_with_epsilon && state_with_epsilon != item.get_head()){
                                item.add_val(string(1,'e'));
                            }
                        }

                }
            }
        }
        return data;
    }
    //prints vector column
    template<typename T>
    auto print_columns(T &data){for(auto &i: data) cout<< i.get()<<endl;}

    template<typename S>
    auto more_than_3(S &data){
        for(auto &item :data){
            for(auto &rule : item.get_rules()){if(rule.size() > 2){return true;}}
        }
        return false;
    }

    template<typename T>
    auto no_term_list(T &data){
        vector<char> non_term_tmp;
        for(auto &item : data){non_term_tmp.push_back(item.get_head()[0]);}
        return non_term_tmp;
    }

    template<typename T>
    auto elimante_long_rules(T &data){
        string name;
        vector<Column> new_items;
        vector<string> non_term_tmp;
        for(auto &item : data){
            non_term_tmp.push_back(item.get_head());
        }
        while(more_than_3(data)){
            for(auto &item : data){
                for(auto &rule : item.get_rules()){
                    string tmp = rule;
                    if(rule.size() > 2){
                        //find unused alphabet member
                        for(auto &alp : alphabet){
                            if(find(non_term_tmp.begin(), non_term_tmp.end(),string(1,alp)) == non_term_tmp.end()){
                                non_term_tmp.push_back(string(1,alp));
                                name = alp;
                                break;
                            }
                        }
                        //update rule with new symbol and add new rul
                        item.update(rule,tmp[0]+name);
                        new_items.push_back(Column(name,tmp.erase(0,1)));
                    }
                }
            }
            //add to new rules to main column data
            for(auto &item: new_items){data.push_back(item);}
            new_items = {};
        }
        return data;
    }

    template<typename D>
    auto con_term_and_non_term(D & data){
        auto list = no_term_list(data);
        for(auto &i : data){
            for(auto &j : i.get_rules()){
                if(j.size() > 1){
                    for(auto &k : j){
                        if(find(list.begin(), list.end(),k) == list.end()){
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    template<typename D>
       auto remove_terminal_and_nonterminal_together(D & data){
           auto list = no_term_list(data);
           string name;
           string tmp_rule;
           while(con_term_and_non_term(data)){
               for(auto &item : data){
                   for(auto &rule : item.get_rules()){
                       if(rule.size() > 1){
                           for(auto &rule_char : rule){
                               if(find(list.begin(), list.end(),rule_char) == list.end()){
                                   tmp_rule = rule;
                                   //unused alphabet member
                                   for(auto &alp : alphabet){
                                       if(find(list.begin(), list.end(),alp) == list.end()){
                                           list.push_back(alp);
                                           name = alp;
                                           break;
                                       }
                                   }
                                   replace_char(tmp_rule,rule_char,name);
                                   item.update(rule,tmp_rule);
                                   data.push_back(Column(name,rule_char));
                               }
                           }
                       }
                   }
               }
           }
           return data;
       }

    template<typename D>
       auto non_term_sim(D & data){
        vector<string> no_dub;
        vector<Column> cols;
        for(auto &item :data){
            for(auto &rule : item.get_rules()){
                if(item.get_rules().size() == 1 && rule.size() == 2){
                      cols.push_back(item);
                }
            }


        }

        for(unsigned int var =0; var < cols.size();var++){
            Column tmp = cols[var];
            for(auto &item : cols){
                for(auto&rule : item.get_rules()){
                    if(tmp.get_rules()[0] == rule && tmp.get_head() != item.get_head()){
                        string del = item.get_head();
                        for(auto & i: data){
                            for(auto&r : i.get_rules()){
                                //rule that needs to be updated
                                if(contains(r, del)){
                                    auto h = tmp.get_head();
                                    auto rep = r;
                                    replace_char(rep,del[0],h);
                                    i.update(r,rep);
                                    data.erase(std::remove(data.begin(), data.end(), item), data.end());
                                    cols.erase(std::remove(cols.begin(), cols.end(), item), cols.end());
                                }
                            }
                        }

                    }
                }
            }
        }
    }

    template<typename T>
    auto rec_simplify(T& data){
        //simplify multiple single terminals
        for(auto &ter :term){
            simplify(data, ter);
        }
        //simplify multiple nonterminal double rules
        non_term_sim(data);
        //simplify contents of the rules by removing dublicates
        vector<string> no_dub;
        for(auto &item :data){
            for(auto &rule : item.get_rules()){
                if(find(no_dub.begin(), no_dub.end(), rule) == no_dub.end()){
                    no_dub.push_back(rule);
                }else{
                    item.remove_str(rule);
                }
            }
            no_dub ={};
        }
        return data;
    }

    template<typename T,typename D>
    auto simplify(T &data,D &ter){
        vector<Column> col_tmp;
        for(auto &item : data){
            for(auto &rule : item.get_rules()){
                    if(item.get_size() == 1 && rule.size() == 1 && rule == ter){ col_tmp.push_back(item);}
            }
        }
        if(col_tmp.size() > 1){
            Column init = col_tmp[0];
            vector<string> rem_tmp;
            string name = init.get_head();
            for(auto &item : col_tmp){
                for(auto &rule : item.get_rules()){
                   if(rule == ter && init.get_head() != item.get_head()){
                       data.erase(std::remove(data.begin(), data.end(), item), data.end());
                   }
                }
            }

            for(auto &item : data){
                for(auto &rule : item.get_rules()){
                    for(auto &col_item : col_tmp){
                        if(contains(rule, col_item.get_head()) && init.get_head() != item.get_head()){
                            string tmp = rule;
                            replace_char(tmp,col_item.get_head_char(),name);
                            item.update(rule,tmp);
                        }
                    }
                }
            }
        }
    }

    template<typename D>
    auto print_pretty(D &data){
        cout<<"NON_TERMINAL"<<endl;
        cout<<"S0"<<endl;

        for(auto &i:data){ cout<< i.get_head()<<endl;}

        cout<<"TERMINAL"<<endl;

        for(auto &i : term) cout<<i<<endl;

        cout<<"RULES"<<endl;
            for(auto &item:data){
                for (auto rule : item.get_rules()){
                    if(item.get_head()== start[0]){
                        cout<<"S0:";
                        cout<<rule<<endl;
                    }
                }
            }

        for(auto &i: data){
            for(auto &j : i.get_rules()){ cout<<i.get_head()+":"+j<<endl;}
        }
        cout<<"START"<<endl;
        cout<<"S0"<<endl;
    }

    template<typename S>
    auto rm_nonprinting (S& str)
    {
        str.erase (remove_if (str.begin(), str.end(),
                                   [](unsigned char c){
                                       return !isprint(c);
                                   }),
                   str.end());
    }

};

int main(){
    //vector<string> lines {"NON-TERMINAL","S","A","B","TERMINAL","a","b","RULES","S:a","S:aA","S:B","A:aBB","A:e","B:Aa","B:b","START","S"};
    //vector<string> lines {"NON-TERMINAL","S","A","B","TERMINAL","0","1","RULES","S:AA","S:AB","A:0","A:e","B:1","B:e","START","S"};
    //for web online comlier set web_test true and //vector lines and un // line above
    vector<string> lines;
    string str;
    bool details = false;
    bool web_test = false;
    utilities u = utilities();
    //read file from directory
    ifstream file("G1.txt");
    if(file || web_test){
        while (getline(file, str) && file)
        {
            if(str.size() > 0){
                u.rm_nonprinting(str);
                lines.push_back(str);
            }
        }
        file.close();
        auto data = u.create_table(lines);
        if(details){ cout<<"untouched data"<<endl;u.print_columns(data);}
        data = u.solve_epsilon(data);
        if(details){ cout<<"epsilon removed"<<endl;u.print_columns(data);}
        data = u.rem_unit_product(data);
        if(details){cout<<"removed unit production"<<endl; u.print_columns(data);}
        data = u.elimante_long_rules(data);
        if(details){cout<<"elimante_long_rules completed"<<endl;u.print_columns(data);}
        data = u.remove_terminal_and_nonterminal_together(data);
        if(details){cout<<"remove_terminal_and_nonterminal_together start"<<endl;u.print_columns(data);}
        data = u.rec_simplify(data);
        if(details){cout<<"simplified"<<endl;u.print_columns(data);}

        u.print_pretty(data);
    }else{cout<<"No file was founded"<<endl;}
}
