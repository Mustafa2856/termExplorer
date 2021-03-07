#include <ncurses.h>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <filesystem>

const char* name = "Terminal Explorer";
std::string path = "";
std::vector<std::pair<int,std::string>> list;
bool hidden  = false;
int start,sel;

int get_type(std::string p){
    std::filesystem::path pth(p);
    std::error_code ec;
    if(std::filesystem::is_directory(pth,ec)){
        return 1;
    }
    //if(ec){std::cout<<ec.message()<<" "<<p<<"\n";}
    return 0;
}

int getlist(){
    list.clear();
    FILE* l;
    std::string modified_path = "";
    for(int i=0;i<path.size();i++){
        if(path[i]==' ' || path[i]=='\'' || path[i]=='\"' || path[i]=='\\' || path[i]=='(' || path[i]==')')
        modified_path += '\\';
        modified_path += path[i];
    }
    if(hidden)l = popen(std::string("ls -A ").append(modified_path).c_str(),"r");
    else l = popen(std::string("ls ").append(modified_path).c_str(),"r");
    if(l!=NULL){
        while(true){
            char* line;
            char buff[1000];
            line = fgets(buff,sizeof(buff),l);
            if(line==NULL)break;
            std::string fpath = line;
            fpath = fpath.substr(0,fpath.size()-1);
            list.push_back(std::make_pair(get_type(path + "/" + fpath),fpath));
        }
    }else {
        pclose(l);
        return 1;
    }
    pclose(l);
    return 0;
}

void printmenu(int sel,int start=0){
    clear();
    attron(A_BOLD);attron(A_UNDERLINE);
    printw(name);
    attroff(A_BOLD);attroff(A_UNDERLINE);
    mvprintw(0,getmaxx(stdscr) - path.size(),path.c_str());
    int cr = 1;
    for(int i = start; i< list.size(); i++){
        if(cr == getmaxy(stdscr)-3)break;
        if(sel==i)attron(A_REVERSE);
        if(list[i].first)attron(A_BOLD);
        mvprintw( cr++, 0, "%s", list[i].second.c_str());
        if(list[i].first)attroff(A_BOLD);
        if(sel==i)attroff(A_REVERSE);
    }
    for(int i=0;i<getmaxx(stdscr);i++)
    mvprintw( getmaxy(stdscr)-3, i,"_");
    refresh();
}

bool parsecmd(std::string s){
    //TODO
    return false;
}

void openf(std::string path){
    std::string modified_path = "";
    for(int i=0;i<path.size();i++){
        if(path[i]==' ' || path[i]=='\'' || path[i]=='\"' || path[i]=='\\' || path[i]=='(' || path[i]==')')
        modified_path += '\\';
        modified_path += path[i];
    }
    system(std::string("xdg-open ").append(modified_path).append(std::string(" > /dev/null 2>&1")).c_str());
}

bool isValidChar(int c){
    if(c=='/' || c==KEY_UP || c=='\n' || c==KEY_DOWN || c==KEY_RIGHT || c==KEY_LEFT || c==27)return false;
    return true;
}

int main(){
    path = std::getenv("HOME");
    initscr();
	clear();
	noecho();
    curs_set(0);
	cbreak();
    keypad( stdscr, true);
    getlist();
    sel = 0,start = 0;
    printmenu(sel);
    int in = 0;
    while((in = getch()) != 27 && in !='q' && in!='Q'){
        keychk: 
        if(in == KEY_DOWN){
            sel = (sel + 1)%list.size();
            if((sel-start)>getmaxy(stdscr)-5)start = sel - getmaxy(stdscr) + 5;
            if(sel<start)start = sel;
            printmenu(sel,start);
        }
        else if(in == KEY_UP){
            sel = (sel  + list.size() - 1)%list.size();
            if((sel-start)>getmaxy(stdscr)-5)start = sel - getmaxy(stdscr) + 5;
            if(sel<start)start = sel;
            printmenu(sel,start);
        }
        else if(in == KEY_LEFT){
            path = path.substr(0,path.find_last_of('/'));
            if(path.size()==0)path = "/";
            getlist();
            sel = 0;start=0;
            printmenu(sel);
        }
        else if(in == KEY_RIGHT || in == '\n'){
            if(list[sel].first){
                path = path + "/" + list[sel].second;
                getlist();
                sel = 0;start=0;
                printmenu(sel);
            }else{
                openf(path + "/" + list[sel].second);
            }
        }
        else if(in == ':'){
            move( getmaxy(stdscr)-1, 0);
            curs_set(1);
            echo();
            std::string p="";
            int ch;
            while((ch=getch())!='\n'){
                p+=char(ch);
            }
            if(!parsecmd(p)){
                printmenu(sel);
            }
            noecho();
            curs_set(0);
        }
        else if(in=='h' || in=='H'){
            hidden = !hidden;
            getlist();
            printmenu(sel);
        }
        else if(in=='f'){
            openf(path);
        }
        else if(in=='t'){
            std::string modified_path = "";
            for(int i=0;i<path.size();i++){
                if(path[i]==' ' || path[i]=='\'' || path[i]=='\"' || path[i]=='\\' || path[i]=='(' || path[i]==')')
                modified_path += '\\';
                modified_path += path[i];
            }
            system(std::string("xterm -e \"cd ").append(modified_path).append(std::string(";bash\" > /dev/null 2>&1 &")).c_str());
        }
        else if(in=='s'){
            int si = 0;
            while(isValidChar(in = getch())){
                for(int i = 0;i < list.size();i++){
                    if(si>list[i].second.size())continue;
                    if(si==0 && list[i].second[si]=='.'){
                        if(list[i].second.size()>1 && list[i].second[1]<in)continue;
                        else if(list[i].second.size()<=1)continue;
                    }
                    if(list[i].second[si]>='A' && list[i].second[si]<='Z'){
                        if(in>(list[i].second[si]+32))continue;
                    }
                    else if(in>list[i].second[si])continue;
                    if(si==list[i].second[si]==in || (in==(list[i].second[si]+32) && list[i].second[si]>='A' && list[i].second[si]<='Z'))
                    si++;
                    sel = i;
                    start = i;
                    printmenu(sel,start);
                    break;
                }
            }
            goto keychk;
        }
    }
    endwin();
}
