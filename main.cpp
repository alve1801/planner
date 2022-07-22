// compile this w/ ```g++ -o a main.cpp -g -lncurses```
#include<string.h> // strlen
#include<stdlib.h> // malloc
#include<ncurses.h> // duh
#include<vector> // for the quests
#define ttlen 24 // how wide to display quest titles

std::vector<char*>quests;

void load(){
	FILE*f=fopen("quests","r");
	for(int i=getc(f);i;i--){
		int dlen=getc(f)*256+getc(f);
		char*q=(char*)malloc(dlen);
		for(int i=0;i<dlen;i++)q[i]=getc(f);
		quests.push_back(q);
		getc(f);
	}
	fclose(f);
}

void save(){
	FILE*f=fopen("quests","w");
	putc(quests.size(),f);
	for(char*q:quests){
		putc(strlen(q)/256,f);
		putc(strlen(q)%256,f);
		fprintf(f,q);
		putc(10,f);
	}
	fclose(f);
}

void pt(char*s,int x,int y,bool bold=0){
	// only prints the first line as the title
	// XXX make it do periods if longer?
	if(bold){attron(COLOR_PAIR(2));attron(A_BOLD);}
	for(int i=0;s[i]&&s[i]!=10&&i<ttlen;i++)
		mvaddch(x,y++,s[i]);
	attron(COLOR_PAIR(1));attroff(A_BOLD);
}

void ps(char*s,int x,int y){
	// because ncurses treats newlines in a very stupid way
	int sx=x,sy=y;
	for(int i=0;s[i];i++)
		if(s[i]==10)sx++,sy=y;
		else mvaddch(sx,sy++,s[i]);
}

int s,c;bool t;
// s tells us which quest is currently selected, c tells us current char in edit mode, t is edit mode

int main(){
	load();

	initscr();start_color();
	noecho();
	curs_set(FALSE);
	cbreak();
	//timeout(1); // ?

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	//init_pair(2, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);

	for(;;){
		clear();
		if(t)ps("Edit mode",0,1);
		ps("Quests:",1,1);
		for(int i=0;i<quests.size();i++)
			pt(quests[i],i+2,3,i==s);

		pt("New Quest",quests.size()+2,3,s==quests.size());

		if(s<quests.size())
			ps(quests[s],2,ttlen+3);
		else
			ps("Create a new quest",1,ttlen+3);

		if(t){
			char c=getch();
			if(c==27){save();t=0;}
		}else{
			switch(getch()){
				case'q':save();endwin();return 0; // do we need the save here?
				case'w':s--;break;
				case's':s++;break;
				case 10:case 27:t=1;if(s==quests.size())quests.push_back((char*)malloc(1000));break; // XXX create new quest if on last option
			}

			s%=(quests.size()+1);
		}

		refresh();
	}
	endwin();
}

//mvaddch(x,y,c)
//mvprintw(y,x,str)

/*
format of savefile (called "quests"):
- 1b telling us how many quests are in the file
- each quest has:
 - 2b telling us length of data
 - data - first line is treated as the title
*/