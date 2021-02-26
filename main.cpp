#include <ncurses.h>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>

const char* name = "Terminal Explorer";
std::string path = "~";
std::vector<std::string> list;
bool hidden  = false;
int start,sel;
// for opening file in default app : xdg-open <path>
int getlist(){
    list.clear();
    FILE* l;
    if(hidden)l = popen(std::string("ls -A ").append(path).c_str(),"r");
    else l = popen(std::string("ls ").append(path).c_str(),"r");
    if(l!=NULL){
        while(true){
            char* line;
            char buff[1000];
            line = fgets(buff,sizeof(buff),l);
            if(line==NULL)break;
            list.push_back(line);
            int l = list[list.size()-1].size();
            list[list.size()-1]=list[list.size()-1].substr(0,l-1);
        }
        if(list.size()==1){
            std::string s = list[0];
            if(s.find_first_of('/')>s.size()){
                pclose(l);
                return 0;
            }
            s = s.substr(s.find_first_of('/')+1);
            if(s.find_first_of('/')>s.size()){
                pclose(l);
                return 0;
            }
            s = s.substr(s.find_first_of('/')+1);
            if(s.find_first_of('/')>s.size()){
                pclose(l);
                return 0;
            }
            s = s.substr(s.find_first_of('/')+1);
            std::string temp = "",temp1 = "";
            for(int i=0;i<s.size();i++){
                if(s[i]!='\'')temp+=s[i];
            }
            for(int i=0;i<path.size();i++){
                if(path[i]!='\'')temp1+=path[i];
            }
            if(s.compare(temp1.substr(temp1.find_first_of('/')+1))==0){
                pclose(l);
                path = path.substr(0,path.find_last_of('/'));
                getlist();
                return 2;
            }
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
        mvprintw( cr++, 0, "%s", list[i].c_str());
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

void openf(std::string s){
    system(std::string("xdg-open ").append(s).c_str());
}

int main(){
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
        if(in == KEY_DOWN){
            sel = (sel + 1)%list.size();
            if(sel>getmaxy(stdscr)-5)start = sel-getmaxy(stdscr)+5;
            else start = 0;
            printmenu(sel,start);
        }
        else if(in == KEY_UP){
            sel = (sel  + list.size() - 1)%list.size();
            if(sel>getmaxy(stdscr)-5)start = sel-getmaxy(stdscr)+5;
            else start = 0;
            printmenu(sel,start);
        }
        else if(in == KEY_LEFT){
            path = path.substr(0,path.find_last_of('/'));
            getlist();
            sel = 0;
            printmenu(sel);
        }
        else if(in == KEY_RIGHT || in == '\n'){
            std::string temp = path;
            if(list[sel].find_first_of(' ')>list[sel].size() 
                && list[sel].find_first_of('(')>list[sel].size() 
                && list[sel].find_first_of(')')>list[sel].size())path = path +  "/" + list[sel];
            else path = path + "/'" + list[sel] + "'";
            int r = getlist();
            if(r==1){
                path=temp;
                getlist();
            }else if(r==2){
                if(list[sel].find_first_of(' ')>list[sel].size() 
                    && list[sel].find_first_of('(')>list[sel].size() 
                    && list[sel].find_first_of(')')>list[sel].size())openf(path +  "/" + list[sel]);
                else openf(path + "/'" + list[sel] + "'");
            }
            sel = 0;
            printmenu(sel);
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
    }
    endwin();
}
