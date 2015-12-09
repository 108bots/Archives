/* Text Editor using Linux nCurses Library
 * Author: Hemanth Srinivasan
 * Year: 2000
*/
#include<iostream.h>
#include<algorithm>
#include<curses.h>
#include<panel.h>
#include<string.h>
#include<menu.h>
#include<stdlib.h>
#include<unistd.h>
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
struct node
{
 char info;
 struct node *left;
 struct node *right;
 struct node *up;
 struct node *down;
} *begin,*line,*column,*screenbegin,*screenend;
char *help="F10:Menu  F1:Help  F2:Save  F3:Open  F4:GotoLine  F5:Rename  Ctrl-x:Exit";
char *fchoices[]={
                             "N->New",
                             "O->Open",
                             "S->Save",
                             "A->Save As",
                             "D->Delete",
                             "X->Exit",
                             (char *)NULL,
                           };
char *echoices[]={
	             "W->Insert Word",
		     "I->Insert Line",
		     "E->Erase Word",
    		     "S->Erase Line",
		     (char *)NULL,
                 };

char *pgchoices[]={
                    "T->Top",
		    "B->Bottom",
		    "L->Left",
		    "R->Right",
		    (char *)NULL,
		  };

char *apchoices[]={
	           "F->Foreground",
		   "K->BacKground",
		   (char *)NULL,
	      };

char *hchoices[]={
            	   "H->Help",
	           "V->About",
		   (char *)NULL,
		 };                     //sh-shadow
char *hint={"You are in menu mode press ESC for text mode..."};        		   
WINDOW *win,*wmenu,*wstatus,*whelp,*wbor,*fwin,*sh_fwin,*ewin,*sh_ewin;
WINDOW *pgwin,*sh_pgwin,*apwin,*sh_apwin,*hwin,*sh_hwin;
PANEL *pstatus,*phelp,*pmenu,*pwin,*pbor,*fpanel,*sh_fpanel,*epanel,*sh_epanel;
PANEL *pgpanel,*sh_pgpanel,*appanel,*sh_appanel,*hpanel,*sh_hpanel;
ITEM **fitems,**eitems,**pgitems,**apitems,**hitems;
MENU *fmenu,*emenu,*pgmenu,*apmenu,*hmenu;
MENU *mmenu;
WINDOW *mwin,*sh_mwin;
PANEL *mpanel,*sh_mpanel;
int fn_choices,en_choices,pgn_choices,apn_choices,hn_choices;
//no: of choices in the diff menus
int flag=1;//special flag for cursor motion in 2 item menu 
int status,mrginlt;//used to idntfy the menu i.e open
int check;//open or save
int pos=1;//to keep track of the cursor in menus
int t,forecolor,backcolor;//keep track of no: choices in the menus
 static int row_cnt=0,x,y;
int posrow;
int flagl,flagr,flagu,flagd,flagi,winflag=0;
int SAVED=1,ASAVED=0;
char *fname=new char [20];
char word[20],sent[50];
int pg_cnt=0,newfile=0;
int colflag=0;
char *yes={"Do you Want to Save this file?"};
char *sure={"Do you really want to Exit?"};
char *create={"File does not exist!!!Want to create one?"};
char *tempname=new char [20];

void createnewfile();
void xecute(int);
void helpscreen();
void we();
void openfilename(char *);
void askword();
void askline();
void save();
void PageSetup(int);
void ColorSet(char *,int);
char *colorwin();
void insertword(char *);
void insertline(char *);
void deleteword();
void deleteline();
void deletefile();
void Askfname();
void reset(struct node*);
void refreshscreen(void);
void bringtop(PANEL *p);
void initialize(void);
void topmenu(void);
void bordertype(void);
void menus(int);
void pageup();
void pagedown();  
void initialize_menus(void);
void putbottom(PANEL *q);
void activatemenus(void);
void clearscreen(void);
void menu_traversal(void);
void insert(chtype);
void backspace(void);
void status_update(void);
void takecare(char);
void swaphelp();
void swaphint();
int confirmmenu(char *);
void downtravel(int);
void gotoline();
void uptravel(int);
void renamefile();

int LEFTMARGIN=0;
int RIGHTMARGIN=0;
int TOP=0,BOTTOM=0;
int position;

void main()
{
 initscr();
 noecho();
 cbreak();
 start_color();
 win=newwin(19,78,3,1);
 keypad(win,TRUE);
 begin=new struct node;
 line=begin;
 column=begin;
 screenbegin=begin;
 screenend=0;

 initialize();
 topmenu();
 bordertype();
 refreshscreen();
 int ch;

 while(1)
 { 
   ch=wgetch(win);
   keypad(win,TRUE); 
   curs_set(1);
  switch(ch)
  {
     
    case KEY_UP: int i;
                 if(line==begin)
                  beep();
                 else
                 {
                            
                     line=line->up;
                     column=line;
                     i=LEFTMARGIN;
                     getyx(win,y,x);
                     position=x;
                     if(y==TOP)
                     {
                       int count=0;
                       while(count < 2 && screenbegin->up !=0)
                       {   
                         screenbegin=screenbegin->up;
                         count++;
                       }
                       refreshscreen();
                     } 
                           
                     while(i < position && column->right->info!='\n')
                     {
                        column=column->right;
                                i++;
                     }
                     row_cnt--;   
                     refreshscreen();
                 }
                  break;

    case KEY_DOWN: if(line->down==NULL)
                              beep();
                          else
                          {
                              line=line->down;
                              row_cnt++; 
                              i=LEFTMARGIN;
                              column=line;
                              if(column->right==0)
			      {
			         refreshscreen();
			         break;
		              }
			      getyx(win,y,x);
                              position=x; 
                              if(y==18-BOTTOM)
                              {
                                  int countd=0;
                                  while(countd < 2)
                                  {
                                      screenbegin=screenbegin->down;
                                      countd++;
                                  }
                                  refreshscreen();
                              }
                              while(i < position && column->right->info !=
'\n'&& column->right->right != 0)
                              {
			          column=column->right;
                                  i++;
                              }
                              if(column==0)
			      {
			          column=line;
			          break;
			      }
			      refreshscreen();
                          }
                          break;


       case KEY_LEFT:     flagl=0;
	                  int countl;
	                  if(column==line)
                          {
                              if(line->up != NULL)
                              {
                                  if(line == screenbegin)
				  {
			              countl=0;
				      while(countl < 2 && screenbegin->up != 0)
				      {
				           screenbegin=screenbegin->up;
			                   countl++;
				      }
				  }
				  line=line->up;
                                  row_cnt--; 
                                  column=line;
                                  while(column->right->info != '\n')
                                      column=column->right;
                              }
                              else
                              {
			          flagl=1;
			          beep();
                              }
			  }
                          else
                          {
                              flagl=1;
			      column=column->left;
                              getyx(win,y,x);
			      if(x==LEFTMARGIN)
			      {
				 y--;
				 x=(77 - RIGHTMARGIN);
				 wmove(win,y,x);
                              }
			      else
			         wmove(win,y,x-1);
			  }
                          if(!flagl)
			     refreshscreen();
			  break;


       case KEY_RIGHT:     flagr=0;
	                   int countr;
	                   if(column->right != 0)
			        column=column->right;//move right in the data structure
			   if(column->right == NULL)//if column points to '\n'
                           {
                               if(line->down==NULL)
                               {
			           beep();
                               }
			       else
                               {
			           getyx(win,y,x);
				   if(y == 18 - BOTTOM)
				   {
				       //MOVE PAGE DOWN
				       countr=0;
				       while(countr < 2)
				       {
				           screenbegin=screenbegin->down;
					   countr++;
				       }
				   }
			           line=line->down;
                                   row_cnt++;
				   column=line;
			       }
			  }
                          else
                          {
                               flagr=1;
			       getyx(win,y,x);
			       if(x >= 77 - RIGHTMARGIN)
			       {
			          if(y== 18 - BOTTOM)
				  {
				     countr=0;
				     while(countr <2)
				     {
				         screenbegin=screenbegin->down;
					 countr++;
				     }
			             flagr=0;
				  }
				  else
				  {
				      y++;
				      x=LEFTMARGIN - 1; //SEE NEXT STATEMENT
			          }
			       }
                          }
                          if(!flagr)
			      refreshscreen();
			  else
			  {
			      wmove(win,y,x+1);
			      wrefresh(win);
			  }
			  break; 
            
     case KEY_BACKSPACE: backspace();
	   	  	 refreshscreen();
			 wrefresh(win);
			 break;
      

     case KEY_F(4): gotoline();
		    refreshscreen();
		    break; 
		
     case KEY_F(5): renamefile();
		    break;
     	     
     case 27:    
     case KEY_F(10):

                    activatemenus();
		    keypad(win,TRUE);
		    noecho();
                    curs_set(1);
                    break;

     case 4: deleteline();
	     refreshscreen();
             break;

     case 5: deleteword();
	     refreshscreen();
                      break;
     case 330:  struct node *waste;
		if((line->down==NULL)&&(line->right==NULL)) goto escape;
                if((line->right->info=='\n')||(line->right==NULL))
                {
		 waste=line;
		 line->down->up=waste->up;
		 waste->up->down=line->down;
                 line=line->down;
                 column=line;
                 delete waste;
                 }
               if(column->right != 0)
	        {   
	          column=column->right;   //Delete key
	          backspace();
		  refreshscreen();
		}
	        else
      escape:	 beep();
		 break;

	       case 262: column=line;     //home
	                 refreshscreen();
			 break;	
	       case 360:struct node *temp;//end
	                temp=column;
			if((line->down==NULL)&&(temp->right==NULL))break;
			while(temp->right->right != 0)
			    temp=temp->right;
			column=temp;
			refreshscreen();
			break;
    
         case KEY_F(1): status=5;
	                xecute('h');
			curs_set(1);
			break; 
			
         case KEY_F(3): status=1;
			xecute('o');
                        curs_set(1);
		        break;			

	 case KEY_F(2): status=1;
			xecute('s');
			curs_set(1);
			break;

         case KEY_NPAGE: pagedown();
                         break;
                               
         case KEY_PPAGE: pageup();
                         break;    
                               

	 case 16:   status=3;
                    xecute('t');
		    xecute('l'); 
		     xecute('r'); 
		     xecute('b'); 
                     break; 

        case 12: insertline(sent);
		 refreshscreen();
		break;

       case 23: insertword(word);
                 refreshscreen();
                break;

       case 24: status=1;
		xecute('x');
                curs_set(1);
		break;
      
     default:          curs_set(1);
                       flagi=0;           //flagi is for insert char
	               insert(ch);
                       takecare(ch);
		       SAVED = 0;
		       bringtop(pstatus);
		       bringtop(pwin);
		       wrefresh(win);
		       if(flagi)
		       {       
		           getyx(win,y,x);
			   if(x >= (77 - RIGHTMARGIN))
                           {      
                               if(y>=18)
                                screenbegin=screenbegin->down; 
			       wmove(win,y+1,LEFTMARGIN);
                                waddch(win,ch);
                               refreshscreen(); 
                            
                           } 
			   position=x;
			   posrow=y;
		           waddch(win,ch);
			   if(ch=='\n')
			   {       
			       getyx(win,y,x);
			       position=x;
			       posrow=y;
			       wmove(win,y,LEFTMARGIN);
                                  
			   }
		       }	          
		       else	   
		           refreshscreen();
                       break;
        }// end switch
        getyx(win,posrow,position);
	werase(wstatus);
	status_update();
	wrefresh(win);
	}// end while

}// end main
                

void bringtop(PANEL *p)
{
 top_panel(p);
 update_panels();
 doupdate();
}

void putbottom(PANEL *q)
{
 bottom_panel(q);
 update_panels();
 doupdate();
}
 
void takecare(char ch)
{
    if((ch=='\n' && row_cnt >= 18-BOTTOM))


    {          //if row reaches end of screen        //if row is in last line and
        flagi=0;                           //row in last col
        screenbegin=screenbegin->down;        
    }                          
}  

  void initialize()
{       
	init_pair(01,COLOR_WHITE,COLOR_BLUE);
	init_pair(05,COLOR_RED,COLOR_CYAN);
        //forecolor=7;backcolor=4;
        if(winflag)
         win=newwin(19,78,3,1);
	pwin=new_panel(win);
	wbkgd(win,COLOR_PAIR(1)); //only now the color is attached to win
 	wstatus=newwin(1,80,1,0);
	pstatus=new_panel(wstatus);
	wbkgd(wstatus,COLOR_PAIR(5));
	whelp=newwin(1,80,23,0);
	phelp=new_panel(whelp);
	wbkgd(whelp,COLOR_PAIR(5));
	mvwprintw(whelp,0,3,help);
        if(!winflag)
         strcpy(fname,"noname");
        status_update();
	bringtop(pstatus);
	bringtop(phelp);
}

 void topmenu()
{
	char *name[]={"File", "Edit", "PageSetup", "Appearance","Help" };
	int i,j=0;
	wmenu=newwin(1,80,0,0);
	pmenu=new_panel(wmenu);
	init_pair(02,7,05);
	wbkgd(wmenu,COLOR_PAIR(02)|A_BOLD);
	for(i=5;i<=4*15+5;i+=15)
	{
		mvwaddstr(wmenu,0,i,name[j]);
		mvwchgat(wmenu,0,i,1,A_STANDOUT,02,NULL);
		j++;
	}
	bringtop(pmenu);
}

 void bordertype()
{
	wbor=newwin(21,80,2,0);
	pbor=new_panel(wbor);
	wbkgd(wbor,COLOR_PAIR(1));
	box(wbor,ACS_VLINE,ACS_HLINE);
        bringtop(pbor);
}

void menus(int mtype)
{                        //mtype tells the type of menu,eg: e=edit,f=file etc                  
  init_pair(7,COLOR_MAGENTA,COLOR_BLACK);
  init_pair(8,COLOR_BLACK,COLOR_WHITE);
  init_pair(9,COLOR_MAGENTA,COLOR_WHITE);
  initialize_menus();
  switch(mtype)
  {         
   case 'f': t=fn_choices;//here t=7 not 6(because of NULL at the end) 
             mwin=fwin;
             sh_mwin=sh_fwin;
             mmenu=fmenu;
             mpanel=fpanel;
             sh_mpanel=sh_fpanel;
              
             mwin=newwin(8,15,2,1);
             sh_mwin=newwin(8,15,3,2);
             set_menu_win(mmenu,mwin);
	     set_menu_sub(mmenu,derwin(mwin,7,14,1,1));
             break;

   case 'e': t=en_choices;//one more
             mwin=ewin;
             sh_mwin=sh_ewin;
             mmenu=emenu;
             mpanel=epanel;
             sh_mpanel=sh_epanel; 

             mwin=newwin(6,20,2,16);
             sh_mwin=newwin(6,20,3,17);
             set_menu_win(mmenu,mwin);
             set_menu_sub(mmenu,derwin(mwin,5,19,1,1));
             break;

   case 'p': t=pgn_choices;//one more
             mwin=pgwin;
             sh_mwin=sh_pgwin;
             mmenu=pgmenu;
             mpanel=pgpanel;
             sh_mpanel=sh_pgpanel; 

             mwin=newwin(6,15,2,31);
             sh_mwin=newwin(6,15,3,32);
             set_menu_win(mmenu,mwin);
             set_menu_sub(mmenu,derwin(mwin,5,14,1,1));
             break;

   case 'a': t=apn_choices;//one more
             mwin=apwin;
             sh_mwin=sh_apwin;
             mmenu=apmenu;
             mpanel=appanel;
             sh_mpanel=sh_appanel; 

             mwin=newwin(4,18,2,46);
             sh_mwin=newwin(4,18,3,47);
             set_menu_win(mmenu,mwin);
             set_menu_sub(mmenu,derwin(mwin,3,17,1,1));
             break;

  case 'h':  t=hn_choices;//one more
             mwin=hwin;
             sh_mwin=sh_hwin;
             mmenu=hmenu;
             mpanel=hpanel;
             sh_mpanel=sh_hpanel; 

             mwin=newwin(4,15,2,61);
             sh_mwin=newwin(4,15,3,62);
             set_menu_win(mmenu,mwin);
             set_menu_sub(mmenu,derwin(mwin,3,14,1,1));
             break;
     
   default : break;
  }//end of switch for type of menu

  wbkgd(mwin,COLOR_PAIR(9));
  keypad(mwin,TRUE);
  set_menu_fore(mmenu,COLOR_PAIR(7)|A_REVERSE);
  set_menu_back(mmenu,COLOR_PAIR(8));
  set_menu_mark(mmenu," ");
  box(mwin,0,0);
  post_menu(mmenu);
  sh_mpanel=new_panel(sh_mwin);
  bringtop(sh_mpanel);
  mpanel=new_panel(mwin);
  bringtop(mpanel);

} //end of fn menus


void activatemenus()
{
 swaphint(); 
 curs_set(0);
while(1)
{
 int ch=wgetch(win);
 if(ch==27)
 {
  clearscreen();
  refreshscreen();
  return;
 }
 switch(ch)
 {
  case 'f': 
   case 6 : status=1;
          menus('f');//ctrl-f
	  menu_traversal();
          return;
	  break;

  case 5: 
  case 'e': status=2;
          menus('e');//ctrl-e
          menu_traversal();
	  return;
          break;
  
  case 16: 
  case 'p': status=3;
           menus('p');//ctrl-p
	   menu_traversal();
	   return;
           break;
  
  case 1: 
  case 'a': status=4;
          menus('a');//ctrl-a,appearance/color
	  menu_traversal();
	  return;
	  break;
  
  case 8: 
  case 'h': status=5;
          menus('h');//ctrl-h
	  menu_traversal();
	  return;
          break;

   default: putbottom(pmenu);
             werase(whelp);
            mvwprintw(whelp,0,1,help);
            bringtop(phelp);
            return; 
  }
 }
}

void initialize_menus()
{

  fn_choices=ARRAY_SIZE(fchoices);//for filemenu
  fitems=new ITEM * [fn_choices];
  for(int i=0;i<fn_choices;i++)
   fitems[i]=new ITEM [fn_choices];
  for(int i=0;i<fn_choices;i++)
   fitems[i]=new_item(fchoices[i]," ");
  fmenu=new_menu((ITEM **)fitems);  

  en_choices=ARRAY_SIZE(echoices);//for editmenu
  eitems=new ITEM * [en_choices];
  for(int i=0;i<en_choices;i++)
   eitems[i]=new ITEM [en_choices];
  for(int i=0;i<en_choices;i++)
   eitems[i]=new_item(echoices[i]," ");
  emenu=new_menu((ITEM **)eitems);

  pgn_choices=ARRAY_SIZE(pgchoices);//for pgsetupmenu
  pgitems=new ITEM * [pgn_choices];
  for(int i=0;i<pgn_choices;i++)
   pgitems[i]=new ITEM [pgn_choices];
  for(int i=0;i<pgn_choices;i++)
   pgitems[i]=new_item(pgchoices[i]," ");
  pgmenu=new_menu((ITEM **)pgitems);

  apn_choices=ARRAY_SIZE(apchoices);//for settingsmenu
  apitems=new ITEM * [apn_choices];
  for(int i=0;i<apn_choices;i++)
   apitems[i]=new ITEM [apn_choices];
  for(int i=0;i<apn_choices;i++)
   apitems[i]=new_item(apchoices[i]," ");
  apmenu=new_menu((ITEM **)apitems);

  hn_choices=ARRAY_SIZE(hchoices);//for helpmenu
  hitems=new ITEM * [hn_choices];
  for(int i=0;i<hn_choices;i++)
   hitems[i]=new ITEM [hn_choices];
  for(int i=0;i<hn_choices;i++)
   hitems[i]=new_item(hchoices[i]," ");
  hmenu=new_menu((ITEM **)hitems); 

}

void clearscreen()
{
    show_panel(pstatus);
    show_panel(pmenu);
    bringtop(pwin);
    show_panel(pbor);
    bringtop(phelp);
}

void menu_traversal()
{
   int c; 
   pos=1;  
   while(1)
  {
    c=wgetch(mwin);
   switch(c)
   {
    case KEY_DOWN: if(pos==t-1)
                   {
                    menu_driver(mmenu,REQ_FIRST_ITEM);
                    pos=1;
                    

	           }
                   else
                   {
                    menu_driver(mmenu,REQ_DOWN_ITEM);
                    pos++;
                   }
                   break;
 
    case KEY_UP:  if(pos==1)
                  {
                   menu_driver(mmenu,REQ_LAST_ITEM);
                   pos=t-1;
                  }
                  else
                  {
                   menu_driver(mmenu,REQ_UP_ITEM);
                   pos--;
                  }
                  break;

    case KEY_RIGHT: switch(status)
                   {
                     case 1: status=2;
		             clearscreen();
                             menus('e');
                             break;
                     case 2: status=3;
                             clearscreen();
                             menus('p');
                             break;
                     case 3: status=4;
                             clearscreen();
                             menus('a');
                             break;
                     case 4: status=5;
                             clearscreen();
                             menus('h');
                             break;
                     case 5: status=1;
                             clearscreen();
                             menus('f');
                             break;
  			 
                  
                   }
                   break;

   case KEY_LEFT: switch(status)
                   {
                     case 1: status=5;
                             clearscreen();
                             menus('h');
                             break;
                     case 2: status=1;
                             clearscreen();
                             menus('f');
                             break;
                     case 3: status=2;
                             clearscreen();
                             menus('e');
                             break;
                     case 4: status=3;
                             clearscreen();
                             menus('p');
                             break;
                     case 5: status=4;
                             clearscreen();
                             menus('a');
                             break;
	            
                   }
                   break; 
   
              

     case 'n': case 'o': case 's': case 'a': case 'd':case 'x': 
     case 'w': case 'i': case 'e': case 'z': case 't':case 'b':
     case 'l': case 'r': case 'f': case 'k':case 'h': case 'v': 
     case '\n': xecute(c);return;
    default: putbottom(pmenu);
             bringtop(pbor);
             bringtop(pwin);
	     swaphelp();
             return;

  }//end of switch for key up and down
  wrefresh(mwin);
 }//end of while
}//end fn menu_traversal

void insert(chtype ch)
{
    struct node *value;
    struct node *temp1;
    if(column->right==0)
    {
        flagi=1;
        column->right=new struct node;
        temp1=column;
        column=column->right;
        column->right=0;
        column->left=temp1;
        column->up=0;
        column->down=0;
        column->info=ch;
        if(ch=='\n')                                 
        {
	    if(line->down==0)
            {
               line->down=new struct node;
               line->down->up=line;
               line->down->down=0;
               line=line->down;row_cnt++;
               line->right=NULL;
               line->left=0;
               column=line;
            }
            else
            {
               struct node *temp;
               temp=new struct node;
               reset(temp);
               temp->up=line;
               temp->down=line->down;
               line->down->up=temp;
               line->down=temp;
               line=line->down;row_cnt++;
               column=line;
            }
       }        
   }
   
    else
    {
        value=new struct node;
        reset(value);
        value->info=ch;
        value->right=column->right;
        value->left=column;
        column->right->left=value;
        column->right=value;
        column=value;
        if(ch=='\n')
        {
	    if(line->down==0)
            {
                line->down=new struct node;
                line->down->up=line;
                line=line->down;row_cnt++;
                line->down=0;
                line->right=column->right;
                column->right->left=line;
                column->right=NULL;
                column=line;
            }
            else
            {
               struct node *temp2;
               temp2=new struct node;
               reset(temp2);
               temp2->down=line->down;
               temp2->up=line;
               line->down->up=temp2;
               line->down=temp2;
               line=temp2;row_cnt++;
               line->right=column->right;
               column->right->left=line;
               column->right=0;
               column=line;
            }    
        }
    } 
} 

 void refreshscreen()
{
    werase(win);
    wmove(win,TOP,LEFTMARGIN);
    chtype ch;
    int i=0,j;
    struct node *refreshptr1, *refreshptr2;
    int x=LEFTMARGIN;
    int y=TOP;
    int row,col,CHANGEC;
    bringtop(pbor);
    mvwprintw(wstatus,0,9,"%d",posrow);
    mvwprintw(wstatus,0,17,"%",position);
    bringtop(pstatus);
    CHANGEC=0;
    
    int rowex;
    int EXLN=0;
    refreshptr1=screenbegin;
      if(x>=77-RIGHTMARGIN)
     {
         x=LEFTMARGIN;
         if(y>19)
         {screenbegin=screenbegin->down;
         //  y=y+1;
         }
     }    
    int count = 0;
    screenend=screenbegin;
    while(count < 19 && screenend != 0)
    {
        screenend=screenend->down;
	count++;
    }	
    while(refreshptr1 != screenend)
    {                                       
        refreshptr2=refreshptr1->right;
        while(refreshptr2!= NULL)
        {
             if(refreshptr2==column->right && CHANGEC != 1)
             {
                 CHANGEC=1;
                 getyx(win,row,col);
             }
             ch=refreshptr2->info;
             waddch(win,ch);
             refreshptr2=refreshptr2->right;
             getyx(win,y,x);
             if(ch=='\n')
                 wmove(win,y,LEFTMARGIN);
             /*if(x >= (77 - RIGHTMARGIN))
             {
                 x=LEFTMARGIN;
                 if(y >= 18-BOTTOM)
		     y=TOP;
		 else
		     y++;
                 wmove(win,y,x);
             }*/
         
	} 
        refreshptr1=refreshptr1->down;
   } 
   if(CHANGEC)
       wmove(win,row,col);
   wrefresh(win);
   bringtop(pwin);
   getyx(win,y,x);
   position=x;
   posrow=y;
   pg_cnt=row_cnt/19;
}

 void backspace()
{
    struct node *temp,*temp1,*headtemp;
    if(column==line)          
    {
        if(screenbegin==line)    //first line on the screen
	{
	   int count=0;
	   while(count < 2 && screenbegin->up != 0)
	   {
	       screenbegin=screenbegin->up;
	       count++;
	   }       
	}   
        temp1=line;   //current line
        line=line->up;row_cnt--;
        if(line==NULL)  // if first line
        {   
            beep();
            row_cnt=0;
            line=begin;
        
        }
        else
        {
            if(temp1->down != NULL)
            {
                line->down=temp1->down;
                temp1->down->up=line;
            }
            else
                line->down=0;
            
            temp=line;
            while(temp->right != NULL)
                temp=temp->right;
            if(temp1->right != 0)
	    {
	        temp->left->right=temp1->right;
                temp1->right->left=temp->left; //adjusting pointers
                column=temp1->right->left;
            }
	    else
	    {        
	        column=temp->left;
	        column->right=0;
            }		
	    delete temp1;
            delete temp;
      }   
   }
   else
   { 
        if(column->right==0)
        {
            column=column->left;
            column->right=0;
        }   
        else                                        
        {
            column->left->right=column->right;
            column->right->left=column->left;
            column=column->left;
        }  
  }
}

void status_update()
{
   mvwprintw(wstatus,0,4,"Row:");
   mvwprintw(wstatus,0,8,"%03d",row_cnt+1);
   mvwprintw(wstatus,0,13,"Col:");
   mvwprintw(wstatus,0,17,"%02d",position+1);
   mvwprintw(wstatus,0,35,"%s",fname);
   mvwprintw(wstatus,0,68,"Page:");               
   mvwprintw(wstatus,0,73,"%02d",pg_cnt+1);
   bringtop(pstatus);
}
void xecute(int c)
{
   if(c=='\n')
   {
     if(status==1)
     {
       if(pos==1)c='n';
       if(pos==2)c='o';
       if(pos==3)c='s';
       if(pos==4)c='a';
       if(pos==5)c='d';
       if(pos==6)c='x';
     }
     if(status==2)
     {
       if(pos==1)c='w';
       if(pos==2)c='i';
       if(pos==3)c='e';
       if(pos==4)c='z';
     }    
      if(status==3)
     {
       if(pos==1)c='t';
       if(pos==2)c='b';
       if(pos==3)c='l';
       if(pos==4)c='r';
     }    
     if(status==4)
     {
       if(pos==1)c='f';
       if(pos==2)c='k';
     }    
     if(status==5)
     {
       if(pos==1)c='h';
       if(pos==2)c='v';
     }
  }    
   switch(c)
  {  
      case 'n': if(status==1)
               {
                int b;
		check=1;
                bringtop(pbor);
                werase(win);
                delwin(win);
                winflag=1;
                initialize();
                bringtop(pwin);
                wrefresh(win);
                if(!SAVED)
                 b =confirmmenu(yes);
                 if(b==1)
                {
                  if(!ASAVED && !SAVED)
	           Askfname();save();
                } 
                createnewfile();
                initialize();
                wrefresh(win);
                return; 
               }
	       break;

    case 'o': if(status==1)
              {
               int cc;
               check=0;
               strcpy(tempname,fname);

               bringtop(pbor);
               bringtop(pwin);
               if(!SAVED && !ASAVED)
              {
                cc=confirmmenu(yes);
               if(cc)
               {
                check=1;
                Askfname();
		werase(wstatus);
		status_update();
                save();
               }
               else
               {
                werase(win);
                //winflag=0;
                //initialize();
                createnewfile();
               }
              }
              Askfname();
              openfilename(fname);
              swaphelp();
              screenbegin=begin;
              line=begin;
              column=line;
              refreshscreen();
              wmove(win,TOP,LEFTMARGIN);
              wrefresh(win);
              row_cnt=0; 
              //keypad(win,TRUE);
              return;
              }
               break;
    case 's': if(status==1)
              {
               check=1;
              //bringtop(pwin);
  	      //bringtop(pbor);
              if(ASAVED)goto leap;
              if(!ASAVED && !SAVED)
               Askfname();
        leap: bringtop(pwin);
               save();
               beep();//to indicate that file is saved..
               napms(200);
               beep();
               napms(200);
	       beep();
               swaphelp();
               return;
              }
               break;

    case 'a': if(status==1)
             {
              check=1;
              bringtop(pwin);
              bringtop(pbor);   
              Askfname();
              bringtop(pwin);
               save();
              swaphelp();
              return; 
             
             }
               break;
    case 'd': if(status==1)
              {
               check=2;
               bringtop(pwin);
               bringtop(pbor);
               Askfname();
               deletefile();
                bringtop(pbor);
                werase(win);
                delwin(win);
                winflag=1;
                initialize();
                bringtop(pwin);
                wrefresh(win);
                createnewfile();
                initialize();
                wrefresh(win);
               swaphelp();
               return; 
              }
               break;
     case 'x': if(status==1)
               {
                int d,u;
               if(!ASAVED || !SAVED)
               {
                u=confirmmenu(yes);
                if(u)
                {
		 check=1;
                 Askfname();
                 save();
                }
                else
                 goto jump;
               }
               if(SAVED)
               {
                bringtop(pbor);
		bringtop(pwin); 
        jump:        d=confirmmenu(sure);
               }
               if(d==1)
              {
               endwin();
               system("clear");
               exit(0);
               }
              else
               {
                bringtop(pbor);
                bringtop(pwin);
                swaphelp();
                refreshscreen();
               }
              }
               break;
     case 'w': if(status==2)
               {
                 bringtop(pbor);
                 bringtop(pwin);   
                 askword();
		 swaphelp();
                 insertword(word);
                 refreshscreen();
               }  
               break;
     case 'i': if(status==2)
               {
                 bringtop(pbor);
                 bringtop(pwin); 
                 askline();
	         swaphelp();
                 insertline(sent);
                 refreshscreen();
               }  
               break;
     case 'e': if(status==2)
               {
                 bringtop(pbor);
                 bringtop(pwin);
		 swaphelp();
                 deleteword();
                 refreshscreen();
               }
               break;
     case 'z': if(status==2)
               {
                 bringtop(pbor);
                 bringtop(pwin); 
		 swaphelp();
                 deleteline();
                 refreshscreen();   
               }
               break;
     case 't': if(status==3)
               {
               bringtop(pbor);
               bringtop(pwin);
               PageSetup(1);
               }
               break;
     case 'b': if(status==3)
               {
                bringtop(pbor);
                bringtop(pwin); 
                 PageSetup(2);
               }
               break;
     case 'l': if(status==3)
               {
                 bringtop(pbor);
                 bringtop(pwin);
                 PageSetup(3);
               }
               break;
     case 'r': if(status==3)
               {
                 bringtop(pbor);
                 bringtop(pwin);               
                 PageSetup(4);
               }
               break;
     case 'f': if(status==4)
               {
                 pos=1;
                 char *fc;
                 fc=new char [15];
                 bringtop(pbor);
                 bringtop(pwin);
                 
                 fc=colorwin();
                 ColorSet(fc,pos);   
                 while(colflag)
                 { 
                   fc=colorwin();
                    ColorSet(fc,pos);
                 } 
                 swaphelp();
              }  
               break;
      case 'k': if(status==4)
                { 
                  pos=2;  
                  char *bc;
		  bc=new char [15];
                  bringtop(pbor);
                  bringtop(pwin);  
                  bc=colorwin();
                  ColorSet(bc,pos);
                  while(colflag)
                  { 
                      bc=colorwin();   
                      ColorSet(bc,pos);
                  }
                  swaphelp();
                }
               break;
     case 'h': if(status==5)
               {
                pos=1;
                bringtop(pbor);
		bringtop(pwin);
                 helpscreen();
                refreshscreen();
                swaphelp();
               }
               break;
     case 'v': if(status==5)
               {
                 pos=2;
                 bringtop(pbor);
		 bringtop(pwin);
                 we();
	        refreshscreen();
		swaphelp();
               }  
               break;
    default: 
             return;
  }
  
}
  
void createnewfile()
{   
       
    delete begin;
    begin=new struct node;
    reset(begin);
    row_cnt=0;
    screenbegin=begin;
    line=begin;
    column=begin;
    screenend=0;
    SAVED=0;
    strcpy(fname,"noname");
}

void reset(struct node *p)
{
p->right=0;
p->up=0;
p->left=0;
p->down=0;
}
void Askfname()
{
	WINDOW *wfile,*sh_wfile;
	PANEL *pfile,*sh_pfile;
	keypad(wfile,FALSE);
        curs_set(1);
	init_pair(15,COLOR_BLACK,COLOR_WHITE);
	wfile=newwin(5,60,10,15);
	pfile=new_panel(wfile);
	 sh_wfile=newwin(5,60,11,16);
        sh_pfile=new_panel(sh_wfile);
         
	wbkgd(wfile,COLOR_PAIR(15));
	box(wfile,ACS_VLINE,ACS_HLINE);
         bringtop(sh_pfile);  
        bringtop(pfile);
        if(check)
         mvwaddstr(wfile,1,1,"Saving File.... ");  
        if(!check)
         mvwaddstr(wfile,1,1,"Opening File.... ");   
        if(check==2)
         mvwaddstr(wfile,1,1,"Deleting File... ");
	mvwaddstr(wfile,2,1,"Enter Filename: ");
	echo();
	mvwgetnstr(wfile,2,17,fname,25);
	noecho();
         putbottom(sh_pfile); 
	putbottom(pfile);
	bringtop(pbor);
	bringtop(pwin);
        swaphelp(); 
}    
void save()
{
    FILE *fp;
    char ch;
    mvwprintw(wstatus,0,35,fname);
    fp=fopen(fname,"w");
    struct node *saveptr1,*saveptr2;
    saveptr1=begin;

    while(saveptr1 != NULL)
    {
	 saveptr2=saveptr1->right;
	 while(saveptr2 != NULL)
	 {

	      ch=saveptr2->info;
	      putc(ch,fp);
	      saveptr2=saveptr2->right;
	 }
	 saveptr1=saveptr1->down;

    }
    fclose(fp);
    SAVED =1;
    ASAVED = 1;
}

void openfilename(char *name)
{
    FILE *fp;
    int test;
    row_cnt=0; 
          
    fp = fopen(name,"r");
   if(fp)ASAVED=1;
    //fseek(fp,0l,0);
    char ch;
    if(!fp)
    {
      test=confirmmenu(create);
      if(test)
      {
	fp = fopen(name,"w");
	werase(win);
	delwin(win);
        
	winflag = 1;
	initialize();
	delete begin;
	begin=new struct node;
	reset(begin);
	line=begin;
	column=begin;
        screenbegin=begin;
        return;
      }
     else
     {
      fname=tempname;

      return;
     }
   }
     delete begin;
     begin=new struct node;
     reset(begin);
     line=begin;
     column=begin;
     screenbegin=begin;
    while(((ch=getc(fp)) != EOF))
    {
     insert(ch);
    //refreshscreen();
   }
    //refreshscreen();
    fclose(fp); 
    return;   	
}


void swaphelp()
{
 werase(whelp);
 mvwaddstr(whelp,0,1,help);
 bringtop(phelp);
}

void swaphint()
{
 werase(whelp);
 mvwaddstr(whelp,0,1,hint);
 bringtop(phelp);
}

void deletefile()
{
     unlink(fname);    
     return;
}

void askword()
{
        WINDOW *wfile,*sh_wfile;
        PANEL *pfile,*sh_pfile;
        curs_set(1);
	char ch;
	int i=0;
	init_pair(50,COLOR_BLACK,COLOR_WHITE);
	wfile=newwin(5,40,10,25);
	pfile=new_panel(wfile);
          sh_wfile=newwin(5,40,11,26);
        sh_pfile=new_panel(sh_wfile);

	wbkgd(wfile,COLOR_PAIR(50));
	box(wfile,ACS_VLINE,ACS_HLINE);
          bringtop(sh_pfile);
        bringtop(pfile);
	mvwaddstr(wfile,2,1,"Enter Word: ");
	echo();
	mvwscanw(wfile,2,17,"%s",word);
	noecho();
	  putbottom(sh_pfile); 
	putbottom(pfile);
	bringtop(pbor);
	bringtop(pwin);
        swaphelp();
}

void insertword(char *word)
{
    int i=0;
    struct node *temp;
    if(column->right==0) // first column
    {
       while(word[i] != '\0')
       { 
           column->right=new struct node;
           column->right->info=word[i];
           column->right->left=column;
           column->right->right=0;
           column=column->right;
           i++;
       }
       column->right=new struct node;
       column->right->info=' ';
       column->right->left=column;
       column=column->right;
       column->right=0;
    }
    else       
        if(column->info==' ')
        {
            temp=column->right;
            i=0;
            while(word[i] != '\0')
            {
                column->right=new struct node;
                column->right->info=word[i];
                column->right->left=column;
                column->right->right=0;
                column=column->right;
                i++;
            }                                                  
            column->right=new struct node;
            column->right->info=' ';
            column->right->left=column;
	    column=column->right;
            column->right=temp;
            temp->left=column;
        }
      else  if(column->right->info==' ')
        {
          temp=column->right;
          i=0;
          column->right=new struct node; 
          column->right->info=' ';
          column->right->left=column;
          column=column->right;  
          while(word[i]!='\0')
          {
           column->right=new struct node;
           column->right->info=word[i];
           column->right->left=column;
           column->right->right=0;
           column=column->right;
           i++;
          }
          column->right=temp;
          temp->left=column;
        }              
        else
        {
            // search for space on the left side of the word
	    temp=column;
	    while(temp->info != ' ' && temp!=line)
	        temp=temp->left;                                              
            column=temp;		 
            temp=temp->right;
            i=0;
            while(word[i] != '\0')
            {
                column->right=new struct node;
                column->right->info=word[i];
                column->right->left=column;
                column->right->right=0;
                column=column->right;
                i++;
            }                                                  
            column->right=new struct node; //inserting right space
	    column->right->info=' ';
	    column->right->left=column;
	    column=column->right;
	    column->right=temp;
            temp->left=column;
        }
            
 }
 
void insertline(char *sentence)
{
     int i=0;
     struct node *temp,*dummy;
     temp=new struct node;
     dummy=temp;
        
     reset(temp);
     while(sentence[i] != '\0')
     {
          temp->right=new struct node;
          temp->right->info=sentence[i];
          temp->right->right=0;
          temp->right->left=temp;
          temp=temp->right;
          i++;
     }
     if(line == begin)    // if first line
     {
          
          line->up=dummy;
          dummy->down=line;
          line=dummy;
          column=line;
	  begin=dummy;
          screenbegin=begin;
     }
     else                     // if inbetween line
     {
         dummy->down=line;
         dummy->up=line->up;
         line->up->down=dummy;
         line->up=dummy;
         line=line->up;           //point to the inserted line;
         column=line;
     }
} 

void askline()
{
	WINDOW *wfile,*sh_wfile;
	PANEL *pfile,*sh_pfile;
	int i=0;
	char *p;
        curs_set(1);
	init_pair(15,COLOR_BLACK,COLOR_WHITE);
	wfile=newwin(5,60,10,15);
	pfile=new_panel(wfile);
	  sh_wfile=newwin(5,60,11,16);
        sh_pfile=new_panel(sh_wfile);

	wbkgd(wfile,COLOR_PAIR(15));
	box(wfile,ACS_VLINE,ACS_HLINE);
	  bringtop(sh_pfile);
        bringtop(pfile);
	mvwaddstr(wfile,2,1,"Enter Line: ");
	echo();
	mvwgetnstr(wfile,3,1,sent,50);
	noecho();
	for(i=0;sent[i] != '\0';i++)
	     ;
	sent[i++]='\n';
	sent[i]='\0';     
	 putbottom(sh_pfile);  
	putbottom(pfile);
	bringtop(pwin);
	bringtop(pbor);
}
void deleteline()
{
       struct node *temp;
       if(line==screenbegin)
       {
            if(screenbegin->up != 0)
             screenbegin=screenbegin->up;
       }     
       if(line==begin)
       {
           if(line->right == 0)
               return;
           else
           {
              column=line;
              column->right=0;
              if(line->down != 0)
              {
                  begin=line->down;
                  screenbegin=begin;
                  line=begin;
                  column=line;
              }
            }
       }
       else
           if(line->down==0)
           {
              temp=line;
              line=line->up;
              line->down=0;
              delete temp;
              column=line;
           }
           else
           {
              temp=line;
              line->down->up=line->up;
              line->up->down=line->down;
              line=line->down;
              column=line;
              delete temp;
           }
}
void deleteword()
{
	struct node *temp1,*temp2,*temp,*temp3;
        if(column->right==0||column->right->info=='\n')
	     beep();
	else
        {
               temp1=column->right;        
               if(temp1->info==' ')
                    beep();            //create exception window
               else
               {  
                  temp3=column;
                  while(temp3->info!=' ' && temp3->right!=0)
                      temp3=temp3->right;
                  if(temp3->right==0)
                  {
                    temp2=column;
                    while(temp2->info!=' ' && temp2!=line)
                    {
                         temp2=temp2->left;
                    }
                   temp3=line->down;
                   temp2=temp3;
                   line->down->up=line->up; 
                   column=temp2; 
                   line=temp3 ;          
                   
                 }
                else
                {      
                  temp2=column->right;
                  while(temp2->info != ' ' && temp2->info != '\n' && temp2 != 0)
                      temp2=temp2->right;
		  while(temp1->info != ' ' && temp1 != line)
		      temp1=temp1->left;      
	          if(temp2->info == ' ')
		      temp2=temp2->right;
		  temp1->right=temp2;
		  if(temp2 != 0)      
		      temp2->left=temp1;
		  column=temp1;
                }
	       }
         }

}

void PageSetup(int n)
{
	PANEL *ppage,*sh_ppage;
	WINDOW *wpage,*sh_wpage;
        curs_set(1);
	char *names[]={"Top:  ","Bottom:  ","Left:  ","Right:   "};
	int ch,i,j,k,Sum;
	chtype cht,*a;
	wpage=newwin(4,15,8,25);
	ppage=new_panel(wpage);
	  sh_wpage=newwin(4,15,9,26);
        sh_ppage=new_panel(sh_wpage);
        init_pair(15,COLOR_BLACK,COLOR_WHITE);
	wbkgd(wpage,COLOR_PAIR(15));
	box(wpage,ACS_VLINE,ACS_HLINE);
	    bringtop(sh_ppage);
        bringtop(ppage);
	mvwprintw(wpage,1,3,"%s",names[n-1]);
	echo();
	mvwscanw(wpage,1,11,"%d",&mrginlt);
	noecho();
         bringtop(sh_ppage);   
	bringtop(ppage);
	bringtop(pwin);
	if(n == 1)
	      TOP = mrginlt;
	 else if(n == 3)
	     LEFTMARGIN  = mrginlt;
	 else if(n == 4)
	     RIGHTMARGIN = mrginlt;
	 else
		BOTTOM = mrginlt;
	refreshscreen();
        swaphelp();
}


void ColorSet(char *colour,int k)
{
  int z;
  
   colflag=0;
  
    if(!strcmp(colour,"black")) {z=1;colflag=0;}
   else
    if(!strcmp(colour,"red")){ z=2;colflag=0;}
  else  if(!strcmp(colour,"green")) { z=3;colflag=0;}
  else  if(!strcmp(colour,"yellow")) {z=4;colflag=0;}
  else  if(!strcmp(colour,"blue")) {z=5;colflag=0;}
  else  if(!strcmp(colour,"magenta")) {z=6;colflag=0;}
   else if(!strcmp(colour,"cyan")) {z=7;colflag=0;}
  else  if(!strcmp(colour,"white")) {z=8;colflag=0;}
   else {
          colflag=1;
          return;
        }   
  
          
 if(k==1)forecolor=z-1;
 else
  backcolor=z-1;
 init_pair(11,forecolor,backcolor);
 wbkgd(win,COLOR_PAIR(11));
 wbkgd(wbor,COLOR_PAIR(11));
 bringtop(pbor);
 bringtop(pwin);
}

 char* colorwin()
{
 
 WINDOW *colwin,*sh_colwin;
 PANEL *colpanel,*sh_colpanel;
 curs_set(1);
 char *cname=new char [15];
 colwin=newwin(5,40,9,12);
 colpanel=new_panel(colwin);
  sh_colwin=newwin(5,40,10,13);
 sh_colpanel=new_panel(sh_colwin);
 init_pair(15,COLOR_BLACK,COLOR_WHITE);
 wbkgd(colwin,COLOR_PAIR(15));
 box(colwin,0,0);
  bringtop(sh_colpanel);
 bringtop(colpanel);
 if(colflag)
 {
  mvwaddstr(colwin,1,10,"Sorry!!! Not Available");    
 echo();
}
 mvwaddstr(colwin,2,7,"color:");
 echo();
 mvwgetnstr(colwin,2,13,cname,10);
  putbottom(sh_colpanel); 
 putbottom(colpanel);
 bringtop(pwin);
 bringtop(pbor);
 return cname;
}

int confirmmenu(char *confirm)
{
 WINDOW *conwin,*sh_conwin; 
 PANEL *conpanel,*sh_conpanel;
 init_pair(12,COLOR_BLACK,COLOR_WHITE);
 init_pair(11,COLOR_WHITE,COLOR_GREEN);
 init_pair(10,COLOR_WHITE,COLOR_RED);      
 curs_set(0); 
 conwin=newwin(5,60,10,10);
  sh_conwin=newwin(5,60,11,11);   
 conpanel=new_panel(conwin);
 sh_conpanel=new_panel(sh_conwin); 
 box(conwin,0,0);
 wbkgd(conwin,COLOR_PAIR(12));
 mvwprintw(conwin,1,15,"%s",confirm);
 mvwprintw(conwin,2,20,"Press y/Y for yes");
 mvwprintw(conwin,3,20,"Press n/N for no");
 mvwchgat(conwin,2,26,3,A_STANDOUT,11,NULL);
 mvwchgat(conwin,3,26,3,A_STANDOUT,10,NULL);    
 bringtop(sh_conpanel);
 bringtop(conpanel);
 char w=wgetch(conwin);
 if(w=='y'||w=='Y')
 {
 putbottom(conpanel);
  putbottom(sh_conpanel); 
 return 1;
 }
 if(w=='n'||w=='N')
 {
  putbottom(conpanel); 
 putbottom(sh_conpanel);
 return 0;
 }
 int g=confirmmenu(confirm);
}
 
 
void helpscreen()
{
 char *key[] = {        
			"F1",
			"F2",
                        "F3",
			"F4",
			"F5",
                        "F10",
                        "In menus mode",
                        "f",
                        "e",
                        "p",
                        "a",
			"h",
			"ESC",
			" ",
			"ALT-F",
			"ALT-E",
			"ALT-P",
			"ALT-A",
			"ALT-H",
			" ",
			"CTRL-D",
			"CTRL-E",
			"CTRL-L",
			"CTRL-W",
			"CTRL-X",
                        (char *)NULL,
                  };

char *funt[]={
 "HELP",
 "SAVE",
 "OPEN",
 "GOTO LINE",
 "RENAME FILE",
 "ACTIVATE MENUS MODE",
 " ",
 "FILE",
 "EDIT",
 "PAGESETUP",
 "APPEARANCE",
 "HELP",
 "RETURNS TO TEXT MODE",
 " ",
 "OPEN FILE MENU",
 "OPEN EDIT MENU",
 "OPEN PAGESETUP MENU",
 "OPEN APPEARANCE MENU",
 "OPEN HELP MENU",
 " ",
 "DELETE LINE",
 "DELETE WORD",
 "INSERT LINE",
 "INSERT WORD",
 "EXIT",
 (char *)NULL,
 };


       ITEM **my_items;
        int c;                          
        MENU *my_menu;
        WINDOW *my_menu_win;
        PANEL *phelpscr;
        int n_choices, i;
        curs_set(1); 
        
        //keypad(stdscr, TRUE);
        init_pair(17, COLOR_GREEN, COLOR_RED);
        init_pair(18, COLOR_RED, COLOR_GREEN);
        init_pair(19,COLOR_BLUE,COLOR_YELLOW);
        init_pair(16,COLOR_BLUE,COLOR_GREEN);
        n_choices = ARRAY_SIZE(key);
        my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i)
                my_items[i] = new_item(key[i], funt[i]);

        my_menu = new_menu((ITEM **)my_items);
        my_menu_win = newwin(16, 60,4,9 );
        phelpscr=new_panel(my_menu_win);
        wbkgd(my_menu_win,COLOR_PAIR(17)|A_STANDOUT);
        keypad(my_menu_win, TRUE);
     
        set_menu_win(my_menu, my_menu_win);
        set_menu_sub(my_menu, derwin(my_menu_win, 10,45, 6, 11));
        set_menu_format(my_menu,7, 1);
                        
        set_menu_mark(my_menu, "  ");
	set_menu_fore(my_menu, COLOR_PAIR(17) | A_REVERSE);
        set_menu_back(my_menu, COLOR_PAIR(18));
       
        box(my_menu_win, 0, 0);
        mvwprintw(my_menu_win,1,12, "Press  F1 to exit from help mode");
        mvwprintw(my_menu_win,3,10,"Use ARROW or PgUP|PgDOWN to Scroll");
        mvwprintw(my_menu_win,4,13,"KEYS          FUNCTIONS");
        mvwchgat(my_menu_win,1,12,32,A_BLINK,19,NULL);
        mvwhline(my_menu_win, 5,1, ACS_HLINE, 58);
        mvwchgat(my_menu_win,4,13,4,A_BOLD,16,NULL);
        mvwchgat(my_menu_win,4,27,9,A_BOLD,16,NULL);
        
        post_menu(my_menu);
        wrefresh(my_menu_win);


        while((c = wgetch(my_menu_win)) != KEY_F(1))
        {       switch(c)
                {       case KEY_DOWN:
                                menu_driver(my_menu, REQ_DOWN_ITEM);
                                break;
                        case KEY_UP:
                                menu_driver(my_menu, REQ_UP_ITEM);
                                break;
                        case KEY_NPAGE:  //Page Down 
                                menu_driver(my_menu, REQ_SCR_DPAGE);
                                break;
                        case KEY_PPAGE:  //Page Up  
                                menu_driver(my_menu, REQ_SCR_UPAGE);
                                break;
                }
                wrefresh(my_menu_win);
        }
    hide_panel(phelpscr);
    bringtop(pmenu);
    bringtop(pstatus);
    bringtop(pbor);
    bringtop(pwin);
 return;
}



void we()
{
    WINDOW *wfile;
    PANEL *pfile;
    curs_set(0);
    init_pair(6,COLOR_MAGENTA,COLOR_WHITE);
    init_pair(03,COLOR_BLUE,COLOR_CYAN);
    wfile=newwin(15,70,5,4);
    pfile=new_panel(wfile);
    keypad(wfile,TRUE);
    wbkgd(wfile,COLOR_PAIR(03));
    box(wfile,ACS_VLINE,ACS_HLINE);
    mvwaddstr(wfile,2,1,"This Product Was Developed at BIT,Bangalore,India");
    mvwaddstr(wfile,3,1,"by Shreyas shetty.k & S.hemanth balaji");  

    mvwaddstr(wfile,8,1,"EZedit is a user friendly text editor written for the Linux.");
    mvwchgat(wfile,3,4,35,A_REVERSE|A_STANDOUT,6,NULL);
    mvwchgat(wfile,8,1,6,A_STANDOUT,6,NULL);
    mvwaddstr(wfile,10,31,"[OK]");
    mvwchgat(wfile,10,31,4,A_BLINK|A_STANDOUT,6,NULL);
    noecho();
    wgetch(wfile);
    noecho();
    putbottom(pfile);
    bringtop(pwin);
    bringtop(pbor);

}

void downtravel(int dist)
{
 struct node *temp;
 int y=row_cnt;
 int x=1;
 for(temp=line;temp->down!=0&&x<=dist;temp=temp->down,x++)
 {
  y++;
 if(y>19)
  screenbegin=screenbegin->down;
 }
 line=temp;
 row_cnt=y;
 column=temp;
 screenend=line;
 werase(wstatus);
 status_update();
}

void gotoline()
{
  PANEL *ppage,*sh_ppage;
  WINDOW *wpage,*sh_wpage;
  int num,length;
  curs_set(1);
   wpage=newwin(5,15,8,25);  
  ppage=new_panel(wpage);
  sh_wpage=newwin(5,15,9,26);
  sh_ppage=new_panel(sh_wpage);
  init_pair(22,COLOR_BLACK,COLOR_WHITE);
  wbkgd(wpage,COLOR_PAIR(22));
  box(wpage,ACS_VLINE,ACS_HLINE);
  bringtop(sh_ppage);
  bringtop(ppage); 
  mvwprintw(wpage,2,1,"Line no:");
  echo();  
  mvwscanw(wpage,2,9,"%d",&num);
  putbottom(sh_ppage);
  putbottom(ppage);
  bringtop(pbor);
  bringtop(pwin);
  if(row_cnt<num)
  {
   length=num-row_cnt-1;
   downtravel(length);
   return;
  }
  else
   if(row_cnt>num)
   {
    length=row_cnt-num+1;
    uptravel(length);
    return;
   }
   else
     return;
 }
 
  
void uptravel(int dist)
{
 struct node *temp;
 int y=row_cnt;
 int x=1;
 for(temp=line;temp->up!=0&&x<=dist;temp=temp->up,x++)
 {
  y--;
 if(y>19)
  screenbegin=screenbegin->up;
 if(y<=18)screenbegin=begin;
 }
 line=temp;
 row_cnt=y;
 column=line;
 screenend=line;
 werase(wstatus);
 status_update();
} 

void renamefile()
{
 WINDOW *wrename,*sh_wrename;
 PANEL *prename,*sh_prename;
 curs_set(1);
 char *opath,*npath;
 opath=new char [120];
 npath=new char [120];
 init_pair(20,COLOR_BLACK,COLOR_WHITE);
 wrename=newwin(5,60,10,10);
 prename=new_panel(wrename);
  sh_wrename=newwin(5,60,11,11);
 sh_prename=new_panel(sh_wrename); 
 wbkgd(wrename,COLOR_PAIR(20));
 box(wrename,0,0);
 bringtop(sh_prename);
 bringtop(prename);
 mvwprintw(wrename,2,2,"Oldpath:");
 echo();
  mvwscanw(wrename,2,10,"%s",opath);
  werase(wrename);
   box(wrename,0,0);   
  mvwprintw(wrename,2,2,"Newpath:");
 echo();
  mvwscanw(wrename,2,10,"%s",npath);
    werase(wrename);
//to get the new file name
 char *nname=new char [15];
 int x=0,i;
 int y=strlen(npath);
 while(npath[y-1]!='/')
 {
  nname[x]=npath[y-1];
  x++;y--;
 }
 reverse(nname,nname+x);


  int bad=rename(opath,npath);
  if(!bad)
  {
    strcpy(fname,nname);
    werase(wstatus);
    status_update();
    werase(wrename);
     box(wrename,0,0);   
    mvwprintw(wrename,2,9,"File Renamed");
      mvwprintw(wrename,3,9,"Press ENTER to quit");   
    wgetch(wrename);
    putbottom(sh_prename);
    putbottom(sh_prename);
    bringtop(pbor);
    bringtop(pwin);
    return;
  }
 if(bad==-1)
 {
  werase(wrename);
   box(wrename,0,0);   
   mvwprintw(wrename,2,9,"File not Renamed,Error in Paths@$%..#&.. ");  
      mvwprintw(wrename,3,9,"Press ENTER to quit");
    wgetch(wrename);
    putbottom(sh_prename);
    putbottom(sh_prename);
    bringtop(pbor);
    bringtop(pwin);
    return;
  }
 } 
 
  void pagedown()
{ 
  struct node *temp5;
  temp5=line;
  int ww,z=1,xx;
  xx=row_cnt;
  
  while(z<=19)
  {
       
   temp5=temp5->down;
   row_cnt++;
   if(temp5==NULL)
   {
    row_cnt=xx;    
    beep();
    return;
   }
  z++;
 }
 screenbegin=temp5;
 line=temp5;
 column=temp5;
 refreshscreen();
 return;

}

 void pageup()
{
  struct node *temp5;
  temp5=line;
  if(temp5->up==0)
  {
   beep();
   return;
  }
  int ww,z=1;
  ww=row_cnt;
  
  while(z<=19)
  {
  
   temp5=temp5->up;
    row_cnt--;
   if(temp5->up==NULL)
   {
    row_cnt=ww;
    beep();
    return;
   }
   z++;
 }
 screenbegin=temp5;
 line=temp5;
 column=temp5;
 refreshscreen();
 return;
}
 
