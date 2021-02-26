#include <ncurses.h>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>

const char* name = "Terminal Explorer";
std::string path = "~";
std::vector<std::string> list;

int getlist(){
    list.clear();
    FILE* l = popen(std::string("ls ").append(path).c_str(),"r");
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
        if(list.size()==1 && list[0].size()>=6 && list[0][5]=='/'){
            std::string s = list[0].substr(6);
            if(s.find_first_of('/')>s.size()){
                pclose(l);
                return 0;
            }
            s = s.substr(s.find_first_of('/')+1);
            if(s.compare(path.substr(2))==0){
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

void printmenu(int sel){
    clear();
    printw(name);
    mvprintw(0,getmaxx(stdscr) - path.size(),path.c_str());
    int cr = 1;
    for(int i = 0; i< list.size(); i++){
        if(sel==i)attron(A_REVERSE);
        mvprintw( cr++, 0, "%s", list[i].c_str());
        if(sel==i)attroff(A_REVERSE);
    }
    refresh();
}

bool isvalidpath(std::string s){
    //TODO
    return false;
}

int main(){
    initscr();
	clear();
	noecho();
    curs_set(0);
	cbreak();
    keypad( stdscr, true);
    getlist();
    int sel = 0;
    printmenu(sel);
    int in = 0;
    while((in = getch()) != 27){
        if(in == KEY_DOWN){
            sel = (sel + 1)%list.size();
            printmenu(sel);
        }else if(in == KEY_UP){
            sel = (sel  + list.size() - 1)%list.size();
            printmenu(sel);
        }else if(in == KEY_LEFT){
            path = path.substr(0,path.find_last_of('/'));
            getlist();
            sel = 0;
            printmenu(sel);
        }else if(in == KEY_RIGHT || in == '\n'){
            std::string temp = path;
            path = path +  "/" + list[sel];
            int r = getlist();
            if(r==1){
                path=temp;
                getlist();
            }
            sel = 0;
            printmenu(sel);
        }else if(in == ':'){
            move( getmaxy(stdscr)-3, 0);printw("");curs_set(1);
            echo();
            std::string p;
            int ch;
            while((ch=getch())!='\n'){
                p+=char(ch);
            }
            if(isvalidpath(p)){
                path = p;
                getlist();
                sel = 0;
                printmenu(sel);
            }
            noecho();
            curs_set(0);
        }
    }
    endwin();
}