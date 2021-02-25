#include <ncurses.h>
#include <vector>
#include <cstring>
#include <string>

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
    }else {
        pclose(l);
        return 1;
    }
    pclose(l);
    return 0;
}

void printmenu(int sel){
    clear();
    mvprintw( 0, getmaxx(stdscr)/2 - sizeof(name), name);
    int cr = 1;
    for(int i = 0; i< list.size(); i++){
        if(sel==i)attron(A_REVERSE);
        mvprintw( cr++, 0, "%s", list[i].c_str());
        if(sel==i)attroff(A_REVERSE);
    }
    refresh();
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
            //TODO: set path
        }
    }
    endwin();
}