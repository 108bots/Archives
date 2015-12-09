/* Paint tool using Windows graphics Library
 * Author: Hemanth Srinivasan
 * Year: 2000
*/
#include<stdio.h>
#include<alloc.h>
#include<dos.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<graphics.h>
#include<conio.h>
#include<ctype.h>
#include<dir.h>
#include<fcntl.h>
#include<io.h>
#include<sys\stat.h>



#define LEFTBUTTON	0
#define RIGHTBUTTON	1

union REGS in,out;
struct SREGS sg,s;
struct button
	{
		int l,t,r,b;
		int fcolor,icolor;
		int type;
		char com;
		char msg[20];
		int press;
		int style;//used only in line & eraser style buttons
		int pattern;//to indicate fill pattern
	};
typedef struct button BUTTON;
struct mouse
	{
		int x,y;
		char l,r;
		int click;
	};
typedef struct mouse MOUSE;
typedef unsigned char byte;


struct
{
	int top,left,bottom,right;
}marker;

int marked=0; /* to store whether marked or not*/


int rarrow[32]={
		0xe1ff,0xe1ff,0xe1ff,0xe1ff,         //hand
		0xe1ff,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,

		0x1e00,0x1200,0x1200,0x1200,
		0x13ff,0x1249,0x1249,0x1249,
		0x9001,0x9001,0x9001,0x8001,
		0x8001,0x8001,0xffff,0x0000
	       };


int pencil1[32]={                            //pencil
		0xffff,0xffff,0xff7f,0xfe3f,
		0xfc3f,0xf81f,0xf81f,0xf81f,
		0xf81f,0xf81f,0xf81f,0xf81f,
		0xf81f,0xf81f,0xf81f,0xffff,

		0x0000,0x0000,0x0080,0x01c0,
		0x0240,0x07e0,0x0420,0x0420,
		0x0420,0x0420,0x0420,0x0420,
		0x0420,0x0420,0x07e0,0x0000
	       };

int pencil[32]={                            //pencil
		0x07ff,0x03ff,0x01ff,0x00ff,
		0x007f,0x803f,0xc01f,0xe00f,
		0xf007,0xf803,0xfc01,0xfe00,
		0xff01,0xff83,0xffc7,0xffef,

		0xf800,0x8c00,0xba00,0xb100,
		0xe880,0x4440,0x2220,0x1110,
		0x0888,0x0444,0x0222,0x0111,
		0x008a,0x0044,0x0028,0x0010
	       };

int cross[32]={                            //cross

		0x003f,0x7dbf,0x7dbf,0x7dbf,
		0x7dbf,0x7dbf,0x0000,0x7ffe,
		0x7ffe,0x0000,0xfdbf,0xfdbf,
		0xfdbf,0xfdbf,0xfdbf,0xfc3f,

		0xabc0,0x03c0,0x83c0,0x03c0,
		0x83c0,0x03c0,0xffff,0xffff,
		0xffff,0xffff,0x03c0,0x03c0,
		0x03c0,0x03c0,0x03c0,0x03c0


	       };

int text[32]={                            //hyphen
		0xf00f,0xf00f,0xffff,0xffff,
		0xffff,0xffff,0xffff,0xffff,
		0xffff,0xffff,0xffff,0xffff,
		0xffff,0xffff,0xffff,0xffff
	       };



int larrow[32]={                            // left handed arrow
		0xffff,0xffff,0xe003,0xf003,
		0xf803,0xfc03,0xfe03,0xfc03,
		0xf803,0xf043,0xe0e3,0xc1f3,
		0x83fb,0x07ff,0x8fff,0xdfff,

		0x0000,0x0000,0x1ffc,0x0804,
		0x0404,0x0204,0x0104,0x0204,
		0x0444,0x08a4,0x1114,0x220c,
		0x4404,0x8800,0x5000,0x2000

	       };

int cdefault[32]={                           //default
		0xefff,0xe7ff,0xebff,0xedff,
		0xeeff,0xef7f,0xefbf,0xefdf,
		0xefbf,0xef7f,0xef7f,0xefbf,
		0xe9df,0xe6ef,0xef77,0xff83,


		0x1000,0x1800,0x1c00,0x1e00,
		0x1f00,0x1f80,0x1fc0,0x1fe0,
		0x1fc0,0x1f80,0x1f80,0x1fc0,
		0x1fe0,0x19f0,0x10f8,0x007c

	       };


int rarrow1[32]={                            // right handed arrow
		0x003f,0x7f7f,0x7eff,0x7dff,
		0x7eff,0x7f7f,0x6fbf,0x57df,
		0x3bef,0x7df7,0xfefb,0xff7d,
		0xffbe,0xffdd,0xffeb,0xfff7,

		0xffc0,0xff80,0xff00,0xfe00,
		0xff00,0xff80,0xffc0,0xefe0,
		0xc7f0,0x83f8,0x01fc,0x00fe,
		0x007f,0x003e,0x001c,0x0008
	       };


int fill[32]={                            // paint
		0xffff,0xfeff,0xfd7f,0xfbbf,
		0xf7df,0xefef,0xc007,0x8002,
		0x8001,0x9002,0x9803,0x9c07,
		0x9e0f,0xbf1f,0xffbf,0xffff

	       };


int zoom[32]={                            // magnify
		0xffff,0xf07f,0xefbf,0xefbf,
		0xefbf,0xefbf,0xefbf,0xf03f,
		0xf07f,0xf8ff,0xf8ff,0xf8ff,
		0xf8ff,0xf8ff,0xf8ff,0xffff

	       };

int srubber[32]={                            // small rubber
		0x00ff,0x00ff,0x00ff,0x00ff,
		0x00ff,0x00ff,0x00ff,0x00ff,
		0xffff,0xffff,0xffff,0xffff,
		0xffff,0xffff,0xffff,0xffff

	       };



int brubber[32]={                            // big rubber
		0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000

	       };

void initscreen();
void drawbutton(BUTTON*,int);
void pressbutton(BUTTON*);
BUTTON *initbutton(int,int,int,int,int,int,char,char*,int);
void initmouse();
void HideorShowcursor(int);
void initmenu(BUTTON*,int,int,int,int,int,int,char,char*,int);
void displayfile(char*);
void getclick(MOUSE*,int);
void pollmouse(MOUSE*);
void spiral(int,int,int);
int incanvas(MOUSE*);
int wheninmenu(int,int,MOUSE*);
void clearmenu(int);
int checkarea(BUTTON*,MOUSE*);
MOUSE *moveinmenu(int,BUTTON[]);
void displaymenu(int);
void iconfunction_display(MOUSE *);
void drawthicknessmenu(int);
void changecursor(int*);
void freehand(MOUSE *);
void restrictmouseptr(int,int,int,int);
void drawline(MOUSE *);
void drawrectangle(MOUSE *);
void drawlineto(MOUSE *);
void erase(MOUSE *);
void plot(int,int,int);
void ellipsealgo(int,int,int,int,int);
void drawellipse(MOUSE *);
void drawspiral(MOUSE *);
void spiralalgo(int,int,int,int);
void mark(MOUSE *);
void unmark(void);
void wipe_off(void);
void paint(void);
void cut(MOUSE *);
void copy(MOUSE *);
void inclip();
void outclip();
void new_file();
void error_msg(char *ch);
char *get_filename(char *msg);
void save();
void load();
void rotate();
int get_angle();
void scale();
float get_values();
void translate();
void shear();
void magnify();
void textbox();
void about_us();









int test=0;//if 'esc`ed' from get_values
 char *sys_errlist[30];

char *ch1,*ch2;

float xx=0,yy=0;
int fd=creat("xyz.tmp",S_IREAD | S_IWRITE);
unsigned int tmp1,tmp2;
void *buffer;
BUTTON *b[100];
BUTTON *menu[5];
int buno=9,meno=0,inmeno=0,color=0,bkcolor=15,thick=1,tno=0,color1;
int patno=32,pattern=1,clipl,clipt,clipr,clipb,selected=0,clipbrd=0;
int lstyleflag=1,estyleflag=1,visible=0,evisible=0;
int mode,BORDER=BLACK,PATTERN=EMPTY_FILL;
char oldfile[15],file[15];
FILE *swap;
static int saved=0,asaved=0;


void main()
{
   int gd = DETECT, gm, errorcode;
   int a,d,i,x,found;
   char c;
   MOUSE *mou;

   detectgraph(&gd,&gm);
   initgraph(&gd, &gm, "c:\\tc\\");
   /* read result of initialization */
   errorcode = graphresult();
   if (errorcode != grOk)
   /* an error occurred */
   {
      printf("Graphics error: %s\n", grapherrormsg(errorcode));
      printf("Press any key to halt:");
      getch();
      exit(1);
      /* terminate with an error code */
   }
   mou=(MOUSE *)malloc(sizeof(MOUSE));
   initscreen();
   pressbutton(b[9]);//free hand tool initially selected
   drawthicknessmenu(9);
   //changecursor(cursor1);
   while(1)
   {
	//setcolor(0);
   jump:       	pollmouse(mou);
	found=0;
	iconfunction_display(mou);
       //	if(kbhit())break;
   //	 saved=0;
	 if(incanvas(mou))
	 {
	  if(b[9]->press==1)
	   changecursor(pencil);
	  else
	   if(b[12]->press==1)
	    changecursor(rarrow);
	  else
	   if(b[11]->press==1)
	    changecursor(text);
	  else
	   if(b[6]->press==1)
	    changecursor(fill);
	  else
	   if(b[8]->press==1)
	    changecursor(zoom);
	  else
	   if(b[5]->press==1)
	   {
	    if(b[48]->press==1)
	     changecursor(srubber);
	    if(b[49]->press==1)
	     changecursor(brubber);
	   }
	  else
	   if((b[1]->press==1)||(b[2]->press==1)||(b[3]->press==1)||(b[4]->press==1)||(b[7]->press==1)||(b[10]->press==1))
	    changecursor(cross);
	 }
	 else
	 {
	  changecursor(rarrow);
	  pollmouse(mou);
	  iconfunction_display(mou);
	 }
	getclick(mou,LEFTBUTTON);  //lb=0,rb=1
	if(mou->click)
	{
	 found=wheninmenu(1,4,mou);
	  if(checkarea(b[13],mou))
	  {
	   if(!asaved)save();
	   closegraph();
	   restorecrtmode();
	   exit(0);
	  }

	  for(i=1;i<=12;i++)//drawingtool icons & reset button
	  {
	   if(checkarea(b[i],mou))
	   {
	    setfillstyle(1,7);
	    bar(3,223,62,393);//cover line styles
	    clearmenu(meno);
	    //pressbutton(menu[meno]);
	    meno=0;

	     pressbutton(b[buno]);//depress
	     buno=i;
	     pressbutton(b[buno]); //press
	     found=2;

	     drawthicknessmenu(buno);
	     break;
	    }
	  }
	 for(i=16;i<=31;i++)//color palette
	  {
	    if(checkarea(b[i],mou))
	    {
	      clearmenu(meno);
	      pressbutton(menu[meno]);
	      meno=0;

	      color=b[i]->fcolor;
	      color1=b[i]->fcolor;
	      b[14]->fcolor=color;
	      drawbutton(b[14],0);
	      found=3;
	      break;
	    }
	  }

	  setfillstyle(1,7);//default
	 if(visible)
	 for(i=44;i<=47;i++)//line style buttons
	 {
	  if(checkarea(b[i],mou))
	  {
	   switch(b[i]->style)
	   {
	     case 1: if(lstyleflag==1)
		     break;
		     HideorShowcursor(1);
		     pressbutton(b[i]);
		     pressbutton(b[43+lstyleflag]);
		     HideorShowcursor(2);
		     lstyleflag=1;
		     break;

	     case 2: if(lstyleflag==2)
		     break;
		     HideorShowcursor(1);
		     pressbutton(b[i]);
		     pressbutton(b[43+lstyleflag]);
		     HideorShowcursor(2);
		     lstyleflag=2;
		     break;

	    case 3: if(lstyleflag==3)
		    break;
		    HideorShowcursor(1);
		    pressbutton(b[i]);
		    pressbutton(b[43+lstyleflag]);
		    HideorShowcursor(2);
		    lstyleflag=3;
		    break;

	    case 4: if(lstyleflag==4)
		    break;
		    HideorShowcursor(1);
		    pressbutton(b[i]);
		    pressbutton(b[43+lstyleflag]);
		    HideorShowcursor(2);
		    lstyleflag=4;
		    break;

	    default:
		     break;

	     }
	   }
	 }
	 if(evisible)
	 for(i=48;i<=49;i++)//erase style buttons
	 {
	  if(checkarea(b[i],mou))
	  {
	   switch(b[i]->style)
	   {
	    case 1: if(estyleflag==1)
		    break;
		    HideorShowcursor(1);
		    pressbutton(b[i]);
		    pressbutton(b[47+estyleflag]);
		    HideorShowcursor(2);
		    estyleflag=1;
		    break;

	     case 2: if(estyleflag==2)
		     break;
		     HideorShowcursor(1);
		     pressbutton(b[i]);
		     pressbutton(b[47+estyleflag]);
		     HideorShowcursor(2);
		     estyleflag=2;
		     break;
	    default:
		     break;
	     }
	   }
	 }

	for(i=32;i<=43;i++)//pattern palette
	  {
	    if(checkarea(b[i],mou))
	    {
	      clearmenu(meno);
	      pressbutton(menu[meno]);
	      meno=0;
	      b[i]->pattern=i-32;//0 to 11
	      PATTERN=i-32;
	      //color=b[i]->fcolor;
	      color1=b[i]->fcolor;
	      b[50]->fcolor=color1;
	      drawbutton(b[50],1);
	      found=4;
	      break;
	    }
	  }

	 if(incanvas(mou))
	 {
	   //if(!saved)
	   asaved=0;
	   if(b[9]->press==1)
	    freehand(mou);
	   else
	    if(b[3]->press==1)
	    drawline(mou);
	   else
	    if(b[2]->press==1)
	    drawrectangle(mou);
	   else
	    if(b[4]->press==1)
	     drawlineto(mou);
	   else
	    if(b[5]->press==1)
	    erase(mou);
	   else
	    if(b[1]->press==1)
	     drawellipse(mou);
	   else
	    if(b[10]->press==1)
	     drawspiral(mou);
	   else
	   if(b[7]->press==1)
	   {
	     mark(mou);
	     goto jump;
	   }
	   else
	   if(b[8]->press==1)
	    magnify();
	   else
	   if(b[6]->press==1)
	     paint();
	   else
	   if(b[11]->press==1)
	    textbox();
	   else
	   goto jump;
	 }
    }
   if(!found)
    {
      getclick(mou,RIGHTBUTTON);
      if(mou->r==2)
      {
	for(i=16;i<=31;i++)
	{
	  if(checkarea(b[i],mou))
	  {
	    pressbutton(menu[meno]);
	    clearmenu(meno);
	    meno=0;
	    bkcolor=b[i]->fcolor;
	    b[15]->fcolor=bkcolor;
	    drawbutton(b[15],0);
	    drawbutton(b[14],0);
	    setfillstyle(SOLID_FILL,bkcolor);
	    bar(67,38,639,408);
	    //setviewport(2,2,638,478,1);
	    break;
	  }
	 }
      }
      setfillstyle(1,7);
      if(meno>0)
      {
	pressbutton(menu[meno]);
	clearmenu(meno);
	meno=0;
      }

    }//end of !found
 }//end of while
   getch();
   closegraph();
}

 void initscreen()
 {


   int i,j,k,q;
   BUTTON but;
   setviewport(0,0,639,479,1);

  setcolor(12);
  rectangle(0,0,639,15);
  setfillstyle(1,12);
  floodfill(1,1,12);
  setcolor(0);
  outtextxy(5,5,"FUNPAINT");

  setcolor(7);
  setfillstyle(1,7);

  rectangle(0,17,639,36);
  floodfill(20,20,7);

  rectangle(0,38,65,408);
  floodfill(20,40,7);

  rectangle(0,410,639,462);
  floodfill(20,420,7);

  rectangle(0,464,639,479);
  floodfill(20,470,7);

  setcolor(0);
  setlinestyle(0,0,1);
  line(500,464,500,479);



  /*drawing area*/
  setcolor(15);
  rectangle(67,38,639,408);
  setfillstyle(1,15);
  floodfill(70,50,15);

//  setcolor(0);
//  setlinestyle(0,0,1);
 // line(636,37,636,408);

	menu[1]=initbutton(0,17,130,36,7,0,'a'," File",1);
	drawbutton(menu[1],0);
	menu[2]=initbutton(131,17,261,36,7,0,'a'," Edit",1);
	drawbutton(menu[2],0);
	menu[3]=initbutton(262,17,392,36,7,0,'a'," Image",1);
	drawbutton(menu[3],0);
	menu[4]=initbutton(393,17,523,36,7,0,'a'," About",1);
	drawbutton(menu[4],0);



	b[1]=initbutton(2,38,32,68,7,1,'e',"",0);
	drawbutton(b[1],0);
	b[2]=initbutton(33,38,63,68,7,0,'r',"",0);
	drawbutton(b[2],0);
	b[3]=initbutton(2,69,32,99,7,0,'l',"",0);
	drawbutton(b[3],0);
	b[4]=initbutton(33,69,63,99,7,0,'c',"",0);//polygon
	drawbutton(b[4],0);
	b[5]=initbutton(2,100,32,130,7,14,'u',"",0);
	drawbutton(b[5],0);
	b[6]=initbutton(33,100,63,130,7,4,'p',"",0);
	drawbutton(b[6],0);
	b[7]=initbutton(2,131,32,161,7,0,'s',"",0);
	drawbutton(b[7],0);
	b[8]=initbutton(33,131,63,161,7,9,'z',"",0);
	drawbutton(b[8],0);
	b[9]=initbutton(2,162,32,192,7,0,'n',"",0);
	drawbutton(b[9],0);
	b[10]=initbutton(33,162,63,192,7,0,'y',"",0);
	drawbutton(b[10],0);
	b[11]=initbutton(2,193,32,223,7,0,'t',"",0);
	drawbutton(b[11],0);
	b[12]=initbutton(33,193,63,223,8,0,'d',"",0);
	drawbutton(b[12],0);

	b[13]=initbutton(622,1,638,14,7,0,'x',"",0);//close
	drawbutton(b[13],0);


	b[14]=initbutton(10,425,28,443,0,0,'F',"",0); //fore & back color
	b[15]=initbutton(23,435,41,453,15,0,'B',"",0);
	drawbutton(b[15],0);
	drawbutton(b[14],0);

	b[50]=initbutton(450,430,468,448,0,0,'P',"",0); //pattern type
	drawbutton(b[50],0);


	rectangle(66,413,234,459);//color
	line(66,436,234,436);

	rectangle(500,413,626,459);//pattern
	line(500,436,626,436);

	 j=68;
	for(i=16,k=0;i<=31;i+=2,k+=2)//color pallette
	{
		b[i]=initbutton(j,415,j+18,434,k,0,'C',"",0);
		drawbutton(b[i],0);
		b[i+1]=initbutton(j,438,j+18,457,k+1,0,'C',"",0);
		drawbutton(b[i+1],0);
		setcolor(15);
		line(j+19,413,j+19,459);
		j+=21;
	}


	q=502;
	for(i=32,k=0;i<=43;i+=2,k+=2)//pattern pallette
	{
		b[i]=initbutton(q,415,q+18,434,k,0,'C',"",0);
		drawbutton(b[i],1);
		b[i+1]=initbutton(q,438,q+18,457,k+1,0,'C',"",0);
		drawbutton(b[i+1],1);
		setcolor(15);
		line(q+19,413,q+19,459);
		q+=21;
	}



	b[44]=initbutton(9,260,56,275,7,0,'T',"",0);//line styles
	b[45]=initbutton(9,290,56,305,7,0,'T',"",0);
	b[46]=initbutton(9,320,56,335,7,0,'T',"",0);
	b[47]=initbutton(9,350,56,370,7,0,'T',"",0);


	b[48]=initbutton(9,231,27,251,7,0,'E',"",0);//erase styles
	b[49]=initbutton(30,231,56,251,7,0,'E',"",0);



	buffer=(void *)malloc(imagesize(100,50,250,150));
	if(buffer==NULL)
		printf("Could not allocate memory\n");
	initmouse();
	HideorShowcursor(2);
	strcpy(file,"noname.fp");
	displayfile(file);

	  changecursor(pencil);
}




void drawbutton(BUTTON *bu,int pat_button)
{
	int x,y,i;
	HideorShowcursor(1);
	setcolor(5);
	rectangle(bu->l,bu->t,bu->r,bu->b);
	if(!pat_button)
	 setfillstyle(1,bu->fcolor);
	else
	 setfillstyle(bu->fcolor,15);//for convinience fcolor is used
					//for both pattern and color index

	 bar(bu->l,bu->t,bu->r,bu->b);
	setcolor(15);
	for(i=0;i<2;i++)
	{
		line(bu->l+i,bu->t+i,bu->r-i,bu->t+i);
		line(bu->l+i,bu->t+i,bu->l+i,bu->b-i);
	}
	setcolor(0);
	for(i=0;i<2;i++)
	{
		line(bu->r-i,bu->b-i,bu->r-i,bu->t+i);
		line(bu->r-i,bu->b-i,bu->l+i,bu->b-i);
	}
	x=bu->l+(bu->r-bu->l)/2;
	y=bu->t+(bu->b-bu->t)/2;
	setcolor(bu->icolor);
	switch(bu->com)
		{
			case 'e':
						ellipse(x,y,0,360,(bu->r-bu->l)/2-5,(bu->b-bu->t)/2-10);
						break;
			case 'r':
						rectangle(bu->l+8,bu->t+8,bu->r-8,bu->b-8);
						break;
			case 'a':
						settextstyle(2,0,5);
						outtextxy(bu->l+strlen(bu->msg),bu->t+3,bu->msg);
						break;
			case 'l':                 //setlinestyle(0,1,3);
						line(bu->l+6,bu->t+7,bu->r-7,bu->b-6);
						line(bu->l+6,bu->t+6,bu->r-6,bu->b-6);
						break;
			case 'z':
						circle(x-5,y-5,(bu->b-bu->t)/4);
						line(x,y-2,bu->r-6,bu->b-8);
						line(x-2,y,bu->r-8,bu->b-6);
						line(bu->r-6,bu->b-8,bu->r-8,bu->b-6);
						setfillstyle(1,bu->icolor);
						floodfill(bu->r-7,bu->b-8,bu->icolor);
						setcolor(15);
						arc(x-5,y-5,270,0,(bu->b-bu->t)/4-2);
						arc(x-5,y-5,270,0,(bu->b-bu->t)/4-1);
						break;
			 case 'u':
						setcolor(0);
						setfillstyle(1,7);
					       //	bar3d(bu->l+5,bu->t+(bu->b-bu->t)/2,bu->r-10,bu->t+(bu->b-bu->t)/1.5,4,5);
						setfillstyle(1,bu->icolor);
						bar3d(bu->l+5,bu->t+(bu->b-bu->t)/2,bu->r-10,bu->t+(bu->b-bu->t)/1.5,4,5);
						break;
			 case 's':
						setlinestyle(1,0,0);
						rectangle(bu->l+6,bu->t+6,bu->r-7,bu->b-7);
						setlinestyle(0,0,0);
						break;
			 case 'p':
						line(bu->l+10,bu->t+15,bu->l+(bu->r-bu->l)/2,bu->t+10);
						line(bu->l+10,bu->t+15,bu->l+(bu->r-bu->l)/2+5,bu->b-5);
						line(bu->l+10,bu->t+15,bu->r-10,bu->b-15);
						line(bu->l+(bu->r-bu->l)/2,bu->t+10,bu->r-5,bu->b-10);
						line(bu->r-5,bu->b-10,bu->l+(bu->r-bu->l)/2+5,bu->b-5);
						setfillstyle(1,1);
						floodfill(bu->l+15,bu->t+17,bu->icolor);
						setcolor(1);
						line(bu->l+10,bu->t+15,bu->l+10,bu->b-6);
						line(bu->l+11,bu->t+15,bu->l+11,bu->b-6);
						break;
			  case 'c':            //polygon
						line(40,80,50,80);
						line(40,80,40,90);
						moveto(40,90);
						lineto(60,90);
						lineto(55,85);
						lineto(45,85);
						lineto(50,80);
						break;
			  case 'n':
						rectangle(x-3,bu->t+5,x+3,y+4);
						line(x-3,y+4,x,y+10);
						line(x+3,y+4,x,y+10);
						setfillstyle(1,bu->icolor);
						floodfill(x,y+6,bu->icolor);
						break;
				case 'y':
						setcolor(bu->icolor);
						spiral(x+2,y,2);
						break;
			case 't':
						settextstyle(1,0,1);
						outtextxy(12,195,"T");
						outtextxy(12,204,"-");
						//setcolor(7);
						break;

			case 'd':   break;
			case 'x':  setlinestyle(0,1,1);
				   line(625,4,635,11);
				   line(626,4,636,11);

				   line(625,11,635,4);
				   line(626,11,636,4);

				   break;



		}
	HideorShowcursor(2);
 }




void pressbutton(BUTTON * bu)
{
	int f,g,i;
	HideorShowcursor(1);
	switch(bu->press)
	{
			case 0:    //depress->press
					bu->press=1;
					if(bu->type==1)
					{
						bu->fcolor=3; //blue
						bu->icolor=15;
						drawbutton(bu,0);
					}
					f=0;
					g=15;
					break;
			case 1:     //press->depress
					bu->press=0;
					if(bu->type==1)
					{
						bu->fcolor=7; //light gray
						bu->icolor=0;
						drawbutton(bu,0);
					}
					f=15;
					g=0;
					break;
	}
  if(bu->type==0)
  {
	setcolor(f);
	for(i=0;i<2;i++)
	{
		line(bu->l+i,bu->t+i,bu->r-i,bu->t+i);
		line(bu->l+i,bu->t+i,bu->l+i,bu->b-i);
	}
	setcolor(g);
	for(i=0;i<2;i++)
	{
		line(bu->r-i,bu->b-i,bu->r-i,bu->t+i);
		line(bu->r-i,bu->b-i,bu->l+i,bu->b-i);
	}
  }
  HideorShowcursor(2);
}

BUTTON *initbutton(int l,int t,int r,int b,int fc,int ic,char c,char *ms,int ty)
{
	BUTTON *b1;
	b1=(BUTTON *)malloc(sizeof(BUTTON));
	b1->l=l;
	b1->t=t;
	b1->r=r;
	b1->b=b;
	b1->fcolor=fc;
	b1->icolor=ic;
	b1->com=c;
	strcpy(b1->msg,ms);
	b1->press=0;
	b1->type=ty;
	return b1;
}


void initmouse()
{
	in.x.ax=0;
	int86(0x33,&in,&out);
	if(out.x.ax==0)
	{
		printf("Mouse driver not present\n");
		getch();
		exit(1);
	}
	in.x.ax=1;
	int86(0x33,&in,&out);
	HideorShowcursor(2);
}




void HideorShowcursor(int flag)
{
	if(flag==1)
		in.x.ax=02;  //set 2nd bit in ax;hide
	else
		if(flag==2)
			in.x.ax=01; //set 1st bit in ax;show
	int86(0x33,&in,&out);
}



void initmenu(BUTTON *b1,int l,int t,int r,int b,int fc,int ic,char c,char *ms,int ty)
{
	b1->l=l;
	b1->t=t;
	b1->r=r;
	b1->b=b;
	b1->fcolor=fc;
	b1->icolor=ic;
	b1->com=c;
	strcpy(b1->msg,ms);
	b1->press=0;
	b1->type=ty;
}




  void displayfile(char *st)
  {
		setfillstyle(SOLID_FILL,LIGHTRED);
		bar(200,1,400,12);
		settextstyle(0,0,0);
		setcolor(0);
		outtextxy(300,5,strlwr(st));

		st=NULL;
  }




void getclick(MOUSE *m,int but)
{
	in.x.ax=5;
	in.x.bx=but;
	int86(0x33,&in,&out);
	m->x=out.x.cx;
	m->y=out.x.dx;
	m->l=out.x.ax & 1;
	m->r=out.x.ax & 2;
	m->click=out.x.bx;
	HideorShowcursor(2);
}


void pollmouse(MOUSE *m1)
{

	char dis[10],temp[4];
	static int clear=1,xmou=0,ymou=0;
	struct viewporttype vp;
	in.x.ax=3;
	int86(0x33,&in,&out);
	m1->x=out.x.cx;
	m1->y=out.x.dx;
	m1->l=out.x.bx & 1;
	m1->r=out.x.bx & 2;
	m1->click=0;


	getviewsettings(&vp);   //store current viewport settings in vp
	if (incanvas(m1) && ((m1->x-66)!=xmou || (m1->y-37)!=ymou) && (meno==0))
	{
		clear=0;
		xmou=m1->x-66;
		ymou=m1->y-37;    //stores current cursor position on canvas
		setviewport(0,0,639,479,1);//sets as entire screen as new vp
		setfillstyle(1,7); //pattern,colour
		bar(502,464,639,479); //mouse display
		//setcolor(0);
		settextstyle(0,0,0);
		strcpy(dis,itoa((m1->x-66),temp,10));    //display mouse pos value
		strcat(dis,",");
		strcat(dis,itoa((m1->y-37),temp,10));
		outtextxy(545,468,dis);
		//setcolor(color);
		setviewport(vp.left,vp.top,vp.right,vp.bottom,vp.clip);
	}
	else if(!incanvas(m1) && !clear)
	{
		clear=1;
		HideorShowcursor(1);
		setviewport(0,0,639,479,1);
		setfillstyle(1,7);
		bar(502,464,639,479);
		setviewport(vp.left,vp.top,vp.right,vp.bottom,vp.clip);
		HideorShowcursor(2);
	}
}


//method 1, based on formula
void spiral(int xc,int yc,int rounds)
  {
   float angle=0;
   int x,y;
   while(angle< (2*M_PI*rounds))   //360 deg (i.e 2*PI) for each round
   {                                //Formula:
    x=xc+angle*sin(angle);          //x=a*sin(a) where a=angle
    y=yc+angle*cos(angle);          //y=a*cos(a)
    putpixel(x,y,color);
    angle=angle+0.01;               //increment angle, change it between 0.1 - 1
    }                               //for other effects
}


  int incanvas(MOUSE *mu)//checks if cursor is within drawing region
  {
		//if((mu->x>63)&&(mu->x<638)&&(mu->y>32)&&(mu->y<461))
		if((mu->x>65)&&(mu->x<639)&&(mu->y>36)&&(mu->y<409))
			return 1;
		else
			return 0;

  }


int wheninmenu(int start1,int end1,MOUSE *mou)      //1,4,mou
{
  int i,s,e,j,found;
  MOUSE *mo;
  for(i=start1;i<=end1;i++)
  {
   if(checkarea(menu[i],mou))
   {
     HideorShowcursor(1);
     pressbutton(menu[meno]);
     clearmenu(meno);
     pressbutton(menu[i]);
     meno=i;
     HideorShowcursor(2);
     HideorShowcursor(2);
     displaymenu(i);
     return(1);
   }
  }
 return(0);
}


void clearmenu(int menu_number)   //restores previous part of the screen
{
  int l,t;
  HideorShowcursor(1);
  switch(menu_number)
  {
    case 0:
	    HideorShowcursor(2);
	    return;
    case 1:
	    l=0;
	    t=36;
	    break;
    case 2:
	    l=130;
	    t=36;
	    break;
   case 3:
	    l=261;
	    t=36;
	    break;
   case 4:
	    l=392;
	    t=36;
	    break;

     }
  putimage(l,t,buffer,0);
  HideorShowcursor(2);
}


int checkarea(BUTTON *bu,MOUSE *mu)
{
	if((mu->x>bu->l)&&(mu->x<bu->r)&&(mu->y>bu->t)&&(mu->y<bu->b))
		return 1;
	else
		return 0;
}


void displaymenu(int menu_number)
{
	int i,j,flag,angle;
	BUTTON menus[8];
	HideorShowcursor(1);
	MOUSE *m0,*m1,*m2,*m3;
       //	mo=(MOUSE *)malloc(sizeof(MOUSE));
	switch(menu_number)
	{
		case 1:

		       initmenu(&menus[1],0,37,130,56,7,0,'a'," New",1);
		       initmenu(&menus[2],0,57,130,76,7,0,'a'," Open",1);
		       initmenu(&menus[3],0,77,130,96,7,0,'a'," Save",1);
		       initmenu(&menus[4],0,97,130,116,7,0,'a'," Exit",1);

		       getimage(0,36,131,117,buffer);
		       setcolor(8);
			rectangle(0,36,131,117);
			setfillstyle(1,8); //dark gray
			floodfill(119,41,8);
			for(i=1;i<=4;i++)
			 drawbutton(&menus[i],0);
			m0=moveinmenu(4,menus);
			switch(inmeno)
			{
			 case 1:
				new_file();
				break;
			 case 3: save();
				     break;
			 case 2: load();
				 break;

			  case 4: if(!asaved)save();
				  closegraph();
				  exit(0);
				  break;
			}
			inmeno=0;
			free(buffer);
			break;

		   case 2:     	initmenu(&menus[1],131,37,261,56,7,0,'a'," Delete",1);
				initmenu(&menus[2],131,57,261,76,7,0,'a'," Cut",1);
				initmenu(&menus[3],131,77,261,96,7,0,'a'," Copy",1);
				getimage(130,36,262,97,buffer);
				setcolor(8);
				rectangle(130,36,262,97);
				setfillstyle(1,8);
				floodfill(141,41,8);
				for(i=1;i<=3;i++)
				 drawbutton(&menus[i],0);
				m1=moveinmenu(3,menus);
				switch(inmeno)
				{
				   case 1:
					 wipe_off();
					  break;
				   case 2: //pressbutton(b[7]);
					    cut(m1);
					 break;
				   case 3:
					copy(m1);
					break;
				 }

					inmeno=0;
					free(buffer);
					break;


				case 3: initmenu(&menus[1],262,37,392,56,7,0,'a',"ClearScreen",1);
					initmenu(&menus[2],262,57,392,76,7,0,'a',"Translate",1);
					initmenu(&menus[3],262,77,392,96,7,0,'a'," Scale",1);
					initmenu(&menus[4],262,97,392,116,7,0,'a'," Rotate",1);
					initmenu(&menus[5],262,117,392,136,7,0,'a'," InClip",1);
					initmenu(&menus[6],262,137,392,156,7,0,'a'," OutClip",1);
					initmenu(&menus[7],262,157,392,176,7,0,'a'," Shear",1);


					getimage(261,36,393,177,buffer);
					setcolor(8);
					rectangle(261,36,393,177);
					setfillstyle(1,8);
					floodfill(279,41,8);
					for(i=1;i<=7;i++)
					drawbutton(&menus[i],0);
					m2=moveinmenu(7,menus);
					switch(inmeno)
					{
					  case 4:

						 rotate();

						 break;
					  case 1:
						 HideorShowcursor(1);
						 unmark();
						 setfillstyle(SOLID_FILL,bkcolor);
						 bar(67,38,639,408);
						 HideorShowcursor(2);
						 break;

					case 5:
						 inclip();
						 unmark();
						break;
					case 6:	outclip();
						 unmark();
						break;

					case 3:scale();
						break;

					case 2: translate();
						break;

					case 7: shear();
						break;
					}
					inmeno=0;
					free(buffer);
					break;

		       case 4: 	initmenu(&menus[1],393,37,523,56,7,0,'a'," Us",1);
				getimage(392,36,524,57,buffer);
				setcolor(8);
				rectangle(392,36,524,57);
				setfillstyle(1,8);
				floodfill(400,41,8);
				for(i=1;i<=1;i++)
				 drawbutton(&menus[i],0);
				m3=moveinmenu(1,menus);
				switch(inmeno)
				{
				case 1:about_us();
				       break;
				}
				inmeno=0;
				free(buffer);
				break;


		}
	HideorShowcursor(2);
}

MOUSE *moveinmenu(int n,BUTTON menus[])
{

  int i,flag;
  MOUSE *mo;
  mo=(MOUSE *)malloc(sizeof(MOUSE));
  flag=0;
  while(1)
  {
    HideorShowcursor(2);
    mo->click=0;
    pollmouse(mo);
    for(i=1;i<=n;i++)
    {
     if(checkarea(&menus[i],mo))
     {
       if(i!=inmeno)
       {
	 HideorShowcursor(1);
	 pressbutton(&menus[inmeno]);
	 pressbutton(&menus[i]);
	 inmeno=i;
	 flag=0;
	 HideorShowcursor(2);
       }
     }
    }
    if((!checkarea(&menus[inmeno],mo))&&(!flag))
    {
      pressbutton(&menus[inmeno]);
      inmeno=0;
      flag=1;
    }
    getclick(mo,LEFTBUTTON);
    if(mo->click)
    {
      clearmenu(meno);
      pressbutton(menu[meno]);
      menu[meno]->press=0;
      meno=0;
      break;
    }
   }
   return mo;
 }

void iconfunction_display(MOUSE *m1)
{
 int i,still=0,j;
    HideorShowcursor(2);
    //pollmouse(mse);


    for(i=1;i<=12;i++)
    {
     if(checkarea(b[i],m1))
     {
      setcolor(0);
      bar(5,467,200,478);
      switch(i)
      {
       case 1: for(j=0;j<500;j++)//to avoid flicker
		outtextxy(10,470,"Elipse Tool");
	       bar(5,467,200,478);
	       break;

       case 2: for(j=0;j<500;j++)
		outtextxy(10,470,"Rectangle Tool");
	       bar(5,467,200,478);
	       break;

       case 3: for(j=0;j<500;j++)
		outtextxy(10,470,"Line Tool");
	       bar(5,467,200,478);
	       break;

       case 4: for(j=0;j<500;j++)
		outtextxy(10,470,"Polygon Tool");
	       bar(5,467,200,478);
	       break;

       case 5: for(j=0;j<500;j++)
		outtextxy(10,470,"Erase Tool");
	       bar(5,467,200,478);
	       break;

       case 6: for(j=0;j<500;j++)
		outtextxy(10,470,"Fill Tool");
	       bar(5,467,200,478);
	       break;

       case 7: for(j=0;j<500;j++)
		outtextxy(10,470,"Select Tool");
	       bar(5,467,200,478);
	       break;

       case 8: for(j=0;j<500;j++)
		outtextxy(10,470,"Magnify Tool");
	       bar(5,467,200,478);
	       break;

       case 9: for(j=0;j<500;j++)
		outtextxy(10,470,"Pencil Tool");
	       bar(5,467,200,478);
	       break;

       case 10: for(j=0;j<500;j++)
		 outtextxy(10,470,"Spiral Tool");
		bar(5,467,200,478);
		break;

       case 11: for(j=0;j<500;j++)
		 outtextxy(10,470,"Text Tool");
		bar(5,467,200,478);
		break;

       case 12: for(j=0;j<500;j++)
		 outtextxy(10,470,"Reset Tool");
		bar(5,467,200,478);
		break;

       default: break;
      }
     }
  }

}


void drawthicknessmenu(int num)
 {
  switch(num)
  {

   case 1:
   case 2:
   case 3:
   case 4:
   case 9:
	  setcolor(0);
	  if(b[45]->press||b[46]->press||b[47]->press)
	   lstyleflag=1;  //if u exit linestyle with any button other than first pressed
	  visible=1;
	  evisible=0;
	  rectangle(5,240,60,390);//line styles rectangle
	  setcolor(15);
	  setlinestyle(0,0,1);
	  line(60,240,60,390);
	  line(5,390,60,390);


	   //bar(6,241,59,389);
	   drawbutton(b[44],0);
	   drawbutton(b[45],0);
	   drawbutton(b[46],0);
	   drawbutton(b[47],0);
	   b[44]->press=0;
	   b[45]->press=0;
	   b[46]->press=0;
	   b[47]->press=0;
	   b[44]->style=1;
	   b[45]->style=2;
	   b[46]->style=3;
	   b[47]->style=4;
	   pressbutton(b[44]);
	   setcolor(0);
	   line(14,267,50,267); //solid thin
	   setlinestyle(1,0,1);
	   line(14,297,50,297);// dotted
	   setlinestyle(3,0,1);
	   line(12,327,50,327);//dashed
	   setlinestyle(0,0,3);
	   line(14,360,50,360);//solid thick
	   setlinestyle(0,0,0);//reset default line settings
	   //thick=0;
	   //tno=0;
	   break;

   case 5:
	  setcolor(0);
	  if(b[49]->press)
	    estyleflag=1;//if u exit erase with brubber pressed
	  rectangle(5,225,60,258);//erasestyles rectangle
	  setcolor(15);
	  setlinestyle(0,0,1);
	  line(60,225,60,258);
	  line(5,258,60,258);
	  evisible=1;
	  visible=0;


	   //bar(6,241,59,389);
	   drawbutton(b[48],0);
	   drawbutton(b[49],0);
	   b[48]->press=0;
	   b[49]->press=0;
	   b[48]->style=1;
	   b[49]->style=2;
	   pressbutton(b[48]);
	   setfillstyle(1,0);
	   bar(15,237,21,244); //erase styles
	   bar(36,235,50,246);
	   //thick=0;
	   //tno=0;
	   setfillstyle(1,7);//reset default fill style i.e solid gray
	   break;
   default:
	   //thick=1;
	   evisible=visible=0;
	   break;
  }

}

void changecursor(int *shape)
{
 in.x.ax=9;
 in.x.bx=0;
 in.x.cx=0;
 in.x.dx=(unsigned)shape;
 segread(&s);
 s.es=s.ds;
 int86x(0x33,&in,&in,&s);
}

void freehand(MOUSE *mm)
{
  int x1,y1,x2,y2;
  x1=mm->x;
  y1=mm->y;
  while(1)
  {
  // HideorShowcursor(2);
    if(mm->l==1)
    {
      setcolor(color);
      if(b[44]->press==1)
       setlinestyle(0,0,1);
      else
      if(b[45]->press==1)
       setlinestyle(1,0,1);
      else
       setlinestyle(3,0,1);


    //  HideorShowcursor(1);
      restrictmouseptr(66,37,638,408);
      if(b[47]->press==1)
      {
       setlinestyle(0,0,3);
       restrictmouseptr(68,39,638,407);
      }

      while(mm->l==1)
      {
       pollmouse(mm);
       x2=mm->x;
       y2=mm->y;
	HideorShowcursor(1);
       line(x1,y1,x2,y2);
       x1=x2;
       y1=y2;
      HideorShowcursor(2);
      }
     }
     else
      {
       restrictmouseptr(1,1,639,479);
       setlinestyle(0,0,1);
       return;
      }
    }
  }


void restrictmouseptr(int x1,int y1,int x2,int y2)
{
	in.x.ax=7;
	in.x.cx=x1;
	in.x.dx=x2;
	int86(0x33,&in,&out);
	in.x.ax=8;
	in.x.cx=y1;
	in.x.dx=y2;
	int86(0x33,&in,&out);
}



void drawline(MOUSE *mm)
{
  int x1,y1,x2,y2;
  x1=mm->x;
  y1=mm->y;

  while(1)
  {
   setcolor(15-color);
   HideorShowcursor(2);
    if(mm->l==1)
    {

      if(b[44]->press==1)
       setlinestyle(0,0,1);
      if(b[45]->press==1)
       setlinestyle(1,0,1);
      if(b[46]->press==1)
       setlinestyle(3,0,1);


      HideorShowcursor(1);
      restrictmouseptr(66,37,638,408);
      if(b[47]->press==1)
      {
       setlinestyle(0,0,3);
       restrictmouseptr(68,39,638,407);
      }
      setwritemode(AND_PUT);
      while(mm->l==1)
      {
       pollmouse(mm);
       //showmousepos(mm);
       x2=mm->x;
       y2=mm->y;
       line(x1,y1,x2,y2);
       line(x1,y1,x2,y2);
      }
      setcolor(color);
	setwritemode(OR_PUT);
      line(x1,y1,x2,y2);
       restrictmouseptr(1,1,639,479);
      HideorShowcursor(2);
       setlinestyle(0,0,1);
     }
     else
       return;
    }
  }


void drawrectangle(MOUSE *mm)
{
  int x1,y1,x2,y2;
  x1=mm->x;
  y1=mm->y;

  while(1)
  {
  setcolor(15-color);
   HideorShowcursor(2);
    if(mm->l==1)
    {

      if(b[44]->press==1)
       setlinestyle(0,0,1);
      if(b[45]->press==1)
       setlinestyle(1,0,1);
      if(b[46]->press==1)
       setlinestyle(3,0,1);


      HideorShowcursor(1);
      restrictmouseptr(66,37,638,408);
      if(b[47]->press==1)
      {
       setlinestyle(0,0,3);
       restrictmouseptr(68,39,638,407);
      }
      setwritemode(AND_PUT);
      while(mm->l==1)
      {
       pollmouse(mm);
       //showmousepos(mm);
       x2=mm->x;
       y2=mm->y;
       rectangle(x1,y1,x2,y2);
       rectangle(x1,y1,x2,y2);
      }
      setcolor(color);
	setwritemode(OR_PUT);
      rectangle(x1,y1,x2,y2);
       restrictmouseptr(1,1,639,479);
      HideorShowcursor(2);
       setlinestyle(0,0,1);
     }
     else
       return;
    }
  }


void drawlineto(MOUSE *mm)
{
  int x1,y1,x2,y2;
  x1=mm->x;
  y1=mm->y;

 while(1)
 {
  setcolor(15-color);
   HideorShowcursor(2);
    if(mm->l==1)
    {

      if(b[44]->press==1)
       setlinestyle(0,0,1);
      if(b[45]->press==1)
       setlinestyle(1,0,1);
      if(b[46]->press==1)
       setlinestyle(3,0,1);

      HideorShowcursor(1);
      restrictmouseptr(66,37,638,408);
      if(b[47]->press==1)
      {
       setlinestyle(0,0,3);
       restrictmouseptr(68,39,638,407);
      }
      setwritemode(AND_PUT);
      while(mm->l==1)
      {
       pollmouse(mm);
       x2=mm->x;
       y2=mm->y;
       line(x1,y1,x2,y2);
       line(x1,y1,x2,y2);
      }
      setcolor(color);
      setwritemode(OR_PUT);
      line(x1,y1,x2,y2);
      x1=x2;
      y1=y2;
     }
     else
     {
       restrictmouseptr(1,1,639,479);
       HideorShowcursor(2);
       setlinestyle(0,0,1);
      }
       getclick(mm,LEFTBUTTON);
       x2=mm->x;
       y2=mm->y;
       restrictmouseptr(68,39,638,407);
       getclick(mm,RIGHTBUTTON); //to come of polygon mode
       if(mm->r==2){restrictmouseptr(1,1,639,479); return;}
    }

}


void erase(MOUSE *mm)
{
  int x,y,size;
  x=mm->x;
  y=mm->y;
  while(1)
  {
   HideorShowcursor(2);
    if(mm->l==1)
    {
      if(b[48]->press==1)
       size=8;
      if(b[49]->press==1)
      size=16;

      HideorShowcursor(1);
      restrictmouseptr(66,37,638,408);

	setcolor(BLACK);
	setlinestyle(3,0,1); //rectangle
      while(mm->l==1)
      {
       pollmouse(mm);
       x=mm->x;
       y=mm->y;
       rectangle(x,y,x+size,y+size);
       setfillstyle(SOLID_FILL,bkcolor);
       bar(x,y,x+size,y+size);
      }
     }
     else
      {

       restrictmouseptr(1,1,639,479);
       setlinestyle(0,0,1);
       return;
      }
    }
  }


void plot(int x,int y,int colour)
{
	if((x>=66)&&(y>=37)&&(x<=638)&&(y<=408))
	{
		if(mode==0) putpixel(x,y,colour | getpixel(x,y)); else
		if(mode==1) putpixel(x,y,colour & getpixel(x,y)); else
		if(mode==2) putpixel(x,y,colour ^ getpixel(x,y));
		else putpixel(x,y,colour);
	}
}


void ellipsealgo(int l,int t,int r,int b,int col)
	{
		int c1,c2,r1,r2;
		{
			HideorShowcursor(1);
			setcolor(col);
			if((r>=l)&&(b>=t))
			{
				r1=(r-l)/2;
				r2=(b-t)/2;
				c1=l+r1;
				c2=t+r2;
			}
			else
				if((r>=l)&&(t>b))
				{
					r1=(r-l)/2;
					r2=(t-b)/2;
					c1=l+r1;
					c2=b+r2;
				}
			else
				if((l>r)&&(t<=b))
				{
					r1=(l-r)/2;
					r2=(b-t)/2;
					c1=r+r1;
					c2=t+r2;
				}
			else
				if((l>r)&&(t>b))
				{
					r1=(l-r)/2;
					r2=(t-b)/2;
					c1=r+r1;
					c2=b+r2;
				}
			ellipse(c1,c2,0,360,r1,r2);
		}
		HideorShowcursor(2);
	}


void drawellipse(MOUSE *mm)
{
  int x1,y1,x2,y2;
  x1=mm->x;
  y1=mm->y;

  while(1)
  {
  setcolor(color);
   HideorShowcursor(2);
    if(mm->l==1)
    {

      if(b[44]->press==1)
       setlinestyle(0,0,1);
      if(b[45]->press==1)
       setlinestyle(1,0,1);
      if(b[46]->press==1)
       setlinestyle(3,0,1);


      HideorShowcursor(1);
      restrictmouseptr(66,37,638,408);
      if(b[47]->press==1)
      {
       setlinestyle(0,0,3);
       restrictmouseptr(68,39,638,407);
      }
      //setwritemode(AND_PUT);
      while(mm->l==1)
      {
       pollmouse(mm);
       //showmousepos(mm);
       x2=mm->x;
       y2=mm->y;
       //ellipsealgo(x1,y1,x2,y2,color);
       //ellipsealgo(x1,y1,x2,y2,bkcolor);
      }
      setcolor(color);
       setwritemode(OR_PUT);
       ellipsealgo(x1,y1,x2,y2,color);
       restrictmouseptr(1,1,639,479);
      HideorShowcursor(2);
       setlinestyle(0,0,1);
     }
     else
       return;
    }
  }



void drawspiral(MOUSE *mm)
{
  int x1,y1,x2,y2,rounds;
  x1=mm->x;
  y1=mm->y;

  while(1)
  {
  setcolor(15-color);
   HideorShowcursor(2);
    if(mm->l==1)
    {

      HideorShowcursor(1);
      restrictmouseptr(66,37,638,408);
      mode=2;
      while(mm->l==1)
      {
       pollmouse(mm);
       //showmousepos(mm);
       x2=mm->x;
       y2=mm->y;
       rounds=abs(x2-x1)/10;
       spiralalgo(x1,y1,rounds,15-color);
       spiralalgo(x1,y1,rounds,15-color);
      }
      mode=1;
      setcolor(color);
      spiralalgo(x1,y1,rounds,color);
       restrictmouseptr(1,1,639,479);
      HideorShowcursor(2);
     }
     else
       return;
    }
  }

void spiralalgo(int xc,int yc,int rounds,int colour)
{
	float angle=0;
	int x,y;
	while(angle<(2*M_PI*rounds))
	{
		x=xc+angle*sin(angle);
		y=yc+angle*cos(angle);
		plot(x,y,colour);
		angle=angle+0.01;
	}
}



void mark(MOUSE *mm)
{
	int colors=15-BLACK;
	int x1,y1,x2,y2;
	//byte *picture;


	if(marked)
	 unmark();
	marker.left=marker.right=0;
	marker.top=marker.bottom=0;

	setcolor(colors);
	setwritemode(XOR_PUT);

	x1=x2=mm->x;
	y1=y2=mm->y;
	restrictmouseptr(66,37,638,408);
	setlinestyle(DOTTED_LINE,1,1);
	HideorShowcursor(1);
	while(mm->l)
	{
		rectangle(x1,y1,x2,y2);
		pollmouse(mm);
		x2=mm->x;
		y2=mm->y;
		rectangle(x1,y1,x2,y2);
	}
	setlinestyle(SOLID_LINE,1,1);
	HideorShowcursor(2);
	if(x1<x2)
	{
		marker.left=x1;
		marker.right=x2;
		if(y1<y2)
		{
			marker.top=y1;
			marker.bottom=y2;
		}
		else
		{
			marker.top=y2;
			marker.bottom=y1;
		}
	}
	else
	{
		marker.left=x2;
		marker.right=x1;
		if(y1<y2)
		{
			marker.top=y1;
			marker.bottom=y2;
		}
		else
		{
			marker.top=y2;
			marker.bottom=y1;
		}
	}
	marked=1;
	//captureimage(marker.left,marker.top,marker.right,marker.bottom,picture);

	setwritemode(COPY_PUT);
	//pressbutton(b[12]);
	restrictmouseptr(1,1,639,479);
}

void unmark()
{
	int color2=15-BLACK;

	setcolor(color2);
	setwritemode(XOR_PUT);
	setlinestyle(DOTTED_LINE,1,1);
	rectangle(marker.left,marker.top,marker.right,marker.bottom);
	setlinestyle(SOLID_LINE,1,1);
	setwritemode(COPY_PUT);
	marker.left=marker.right=0;
	marker.top=marker.bottom=0;
	//free(picture);
	marked=0;

}


void wipe_off()
{
	if(marked)
	{

		setfillstyle(SOLID_FILL,bkcolor);
		bar(marker.left,marker.top,marker.right,marker.bottom);
		marker.left=marker.right=0;
		marker.top=marker.bottom=0;
		unmark();
	}
}

void paint()
{
  int BORDER1,BORDER2,BORDER3,BORDER4;
  MOUSE *mm;
  mm=(MOUSE *)malloc(sizeof(MOUSE));
  int x,y,i;
  getclick(mm,LEFTBUTTON);
  x=mm->x;
  y=mm->y;
  //travel in 4 directions to get the bordercolor
  for(i=x;i<=638;i++)
  {
    if(getpixel(i,y)!=bkcolor)
    {
     BORDER1=getpixel(i,y);
     break;
    }
  }

  for(i=x;i>=66;i--)
  {
    if(getpixel(i,y)!=bkcolor)
    {
     BORDER2=getpixel(i,y);
     break;
    }
  }

  for(i=y;i<=407;i++)
  {
    if(getpixel(x,i)!=bkcolor)
    {
     BORDER3=getpixel(x,i);
     break;
    }
  }

  for(i=x;i>=38;i--)
  {
    if(getpixel(x,i)!=bkcolor)
    {
     BORDER4=getpixel(x,i);
     break;
    }
  }
  //DON'T FILL IF BORDERS ARE DIFFERENT
 if(BORDER1!=BORDER2||BORDER3!=BORDER4||BORDER2!=BORDER3||BORDER1!=BORDER4||BORDER1!=BORDER3||BORDER2!=BORDER4)
     return;
  BORDER=BORDER1;
  HideorShowcursor(1);
  setfillstyle(PATTERN,color);
  floodfill(x,y,BORDER);
  HideorShowcursor(2);
 }

void cut(MOUSE *mm)
{
	int left,right,bottom,top,prevx,prevy;
	int diffx,diffy;
	int color=15-BLACK;
	void far *image;
	unsigned int   size,moved=0;
	int x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,xmin,ymin,xmax,ymax,xadd,yadd,oldx,oldy;
	pollmouse(mm);
	while(1)
	{
	getclick(mm,RIGHTBUTTON);
	 if(mm->r==2)break;
	}


	if(mm->r==2)
	{
	x5=x1=mm->x;
	y5=y1=mm->y;
	if(!marked) return;
       }
	do
	{
			HideorShowcursor(1);
			setwritemode(XOR_PUT);
			left=x3=marker.left,right=x4=marker.right,top=y3=marker.top,bottom=y4=marker.bottom;
			size=imagesize(x3,y3,x4,y4);

			 if((image=farmalloc(size))==NULL)
			   {
			     closegraph();
			       printf("Error: not enough heap space in save_screen().\n");
			       exit(1);
			    }
			getimage(x3+1,y3+1,x4-1,y4-1,image);
			oldx=x3,oldy=y3;
			xmin=x3+1,ymin=y3+1,xmax=x4-1,ymax=y4-1;
			setwritemode(XOR_PUT);
			restrictmouseptr(1,1,638,478);
			HideorShowcursor(2);
		if((mm->r==2))
		{
			if((x5>=xmin)&&(y5>=ymin)&&(x5<=xmax)&&(y5<=ymax))
			{
				HideorShowcursor(1);
				setfillstyle(SOLID_FILL,bkcolor);
				 bar(oldx-1,oldy-1,x4+1,y4+1);
				while(mm->r==2)
				{
				      pollmouse(mm);
				       x2=mm->x;
				       y2=mm->y;

					xadd=x1-x2;
					yadd=y1-y2;
					x1=x2;
					y1=y2;

					left=left-xadd; //prevent pasting out of screen
					top= top-yadd;
					right=right-xadd;
					bottom=bottom-yadd;
					if((left<66+2)||(right>638-2)||(top<36+2)||(bottom>408-2))
					{
					left=left+xadd;
					top= top+yadd;
					right=right+xadd;
					bottom=bottom+yadd;
					}
						setwritemode(AND_PUT);
						setcolor(15-BLACK);
						rectangle(left,top,right,bottom);
						rectangle(left,top,right,bottom);
				}
				marker.left=marker.right=0;
				marker.top=marker.bottom=0;
				putimage(left+1,top+1,image,COPY_PUT);
			       farfree(image);
				HideorShowcursor(2);
				setwritemode(OR_PUT);
				moved=1;
			}
			else
			{
				setcolor(bkcolor);
				rectangle(xmin-1,ymin-1,xmax+1,ymax+1);
				moved=1;
			       HideorShowcursor(2);
			}
		}


	}while(moved!=1);
	restrictmouseptr(1,1,638,478);
	HideorShowcursor(2);
	unmark();
	setwritemode(OR_PUT);
}


void copy(MOUSE *mm) //very similar to cut ** denotes changes
{
	int left,right,bottom,top,prevx,prevy;
	int color=15-BLACK;
	void far *image;
	unsigned int   size,moved=0;
	int x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,xmin,ymin,xmax,ymax,xadd,yadd,oldx,oldy;
	pollmouse(mm);
	while(1)
	{
	getclick(mm,RIGHTBUTTON);
	 if(mm->r==2)break;
	}


	if(mm->r==2)
	{
	x5=x1=mm->x;
	y5=y1=mm->y;
	if(!marked) return;
       }
	do
	{
			HideorShowcursor(1);
			setwritemode(XOR_PUT);
			left=x3=marker.left,right=x4=marker.right,top=y3=marker.top,bottom=y4=marker.bottom;
			size=imagesize(x3,y3,x4,y4);

			 if((image=farmalloc(size))==NULL)
			   {
			     closegraph();
			       printf("Error: not enough heap space in save_screen().\n");
			       exit(1);
			    }
			getimage(x3+1,y3+1,x4-1,y4-1,image);
			oldx=x3,oldy=y3;
			xmin=x3+1,ymin=y3+1,xmax=x4-1,ymax=y4-1;
			setwritemode(XOR_PUT);
			restrictmouseptr(1,1,638,478);
			HideorShowcursor(2);
		if((mm->r==2))
		{
			if((x5>=xmin)&&(y5>=ymin)&&(x5<=xmax)&&(y5<=ymax))
			{
				HideorShowcursor(1);
				setfillstyle(SOLID_FILL,bkcolor);
				setwritemode(AND_PUT);//**
				unmark();
				setwritemode(OR_PUT);//** copy-paste
				while(mm->r==2)
				{
				      pollmouse(mm);
				       x2=mm->x;
				       y2=mm->y;

					xadd=x1-x2;
					yadd=y1-y2;
					x1=x2;
					y1=y2;

					left=left-xadd; //prevent pasting out of screen
					top= top-yadd;
					right=right-xadd;
					bottom=bottom-yadd;
					if((left<66+2)||(right>638-2)||(top<36+2)||(bottom>408-2))
					{
					left=left+xadd;
					top= top+yadd;
					right=right+xadd;
					bottom=bottom+yadd;
					}
						setwritemode(AND_PUT);
						setcolor(15-BLACK);
						rectangle(left,top,right,bottom);
						rectangle(left,top,right,bottom);
				}
				marker.left=marker.right=0;
				marker.top=marker.bottom=0;
				putimage(left+1,top+1,image,COPY_PUT);
			       farfree(image);
				HideorShowcursor(2);
				setwritemode(OR_PUT);
				moved=1;
			}
			else
			{
				setcolor(bkcolor);
				rectangle(xmin-1,ymin-1,xmax+1,ymax+1);
				moved=1;
			       HideorShowcursor(2);
			}
		}


	}while(moved!=1);
	restrictmouseptr(1,1,638,478);
	HideorShowcursor(2);
	unmark();
	setwritemode(OR_PUT);
}



void inclip()
{

	if(!marked) return;

	 setfillstyle(SOLID_FILL,bkcolor);
	  HideorShowcursor(1);
	bar(marker.left,marker.top,marker.right,marker.bottom);
	marker.left=marker.right=0;
	marker.top=marker.bottom=0;
	 HideorShowcursor(2);
}

void outclip()
{
  int x1,y1,x2,y2;
  if(!marked) return;

	 setfillstyle(SOLID_FILL,bkcolor);
   x1=marker.left ;
     y1=marker.top;
      x2=marker.right;
      y2=marker.bottom;
      HideorShowcursor(1);
  if((x2>x1)&&(y2>y1))
  {
    bar(67,38,638,y1);
    bar(67,y2,638,407);
    bar(67,y1,x1,y2);
    bar(x2,y1,638,y2);
  }
  if((x1>=x2)&&(y1>=y2))
  {
    bar(67,38,638,y2);
    bar(67,y1,638,407);
    bar(67,y2,x2,y1);
    bar(x1,y2,638,y1);
  }
  if((x2>x1)&&(y1>y2))
  {
    bar(67,38,638,y2);
    bar(67,y1,638,407);
    bar(67,y2,x1,y1);
    bar(x2,y2,638,y1);
  }
  if((x1>=x2)&&(y2>=y1))
  {
    bar(67,38,638,y1);
    bar(67,y2,638,407);
    bar(67,y1,x2,y2);
    bar(x1,y1,638,y2);
   }
    HideorShowcursor(2);
   marker.left=marker.right=0;
   marker.top=marker.bottom=0;
}


void new_file()
{
//  int xx;
 HideorShowcursor(1);

 if(!asaved)save();
 setfillstyle(SOLID_FILL,bkcolor);
 bar(67,38,639,408);
 HideorShowcursor(2);
 strcpy(file,"noname.fp");
 displayfile(file);

}

void error_msg(char *cha)   //prints out the error message when called
{

	void far *pic;
	unsigned int size1;

	size1=imagesize(240,180,400,300);
	pic=farmalloc(size1);
	getimage(240,180,400,300,pic);
	HideorShowcursor(1);
	rectangle(240,180,400,300);
		settextjustify(CENTER_TEXT,CENTER_TEXT);
	setfillstyle(SOLID_FILL,RED);
	bar(247,205,393,293);
	outtextxy(320,193,"ERROR");
	outtextxy(320,230,cha);
	sleep(3);
	putimage(240,180,pic,COPY_PUT);
	farfree(pic);

	cha=NULL;
	settextjustify(LEFT_TEXT,TOP_TEXT);
	HideorShowcursor(2);
}

char *get_filename(char *mesg)
{
	int i = 0;
	char text_data[15],bufp[1],text_data1[15];
	char ext[3];

	HideorShowcursor(1);
	setcolor(BLUE);
	setfillstyle(SOLID_FILL,LIGHTRED);
	bar(240,180,400,300);

	setfillstyle(SOLID_FILL,CYAN);
	bar(247,205,393,293);
	settextstyle(0,HORIZ_DIR,1);
	setcolor(BLUE);
	settextjustify(CENTER_TEXT,CENTER_TEXT);
	outtextxy(320,193,mesg);

	outtextxy(320,223,"FILE NAME:");
	moveto(280,260);
	while(1) {
		 fflush(stdin);
		 outtext("_");
		text_data[i] = getch();

		if( text_data[i] == '\n' || text_data[i] == '\r')
		{
			text_data[i] = '\0';
			break;
		}
		if( text_data[i] == 27 )
		{ /* ESC */
			break;
		}
		if(text_data[i]=='\b')
		{/*Backspace*/
			i--;
			moverel(-8,0);
		}
		if( isalnum(text_data[i]) || text_data[i] == '.' )
		{
			sprintf(bufp,"%c",text_data[i]);
			outtext(bufp);

		}
		else

			continue;
		i++;
		moverel(8,0);
		if(i==14)      /*MAX NO OF CHARS*/
			break;
	}

	if(text_data[i] == 27 )
		text_data1[0] = '\0';
	else
	{
		fnsplit(text_data,"","",text_data1,ext);
		strcat(text_data1,".fp");
	}
	HideorShowcursor(2);
	settextjustify(LEFT_TEXT,TOP_TEXT);
	return (text_data1);
}

void save()
{
	char desc[] = "SAVE FILE",fname[15];
	int ch,count,i,j,last_pt;

	void far *image1;
	unsigned int size;
	HideorShowcursor(1);

	/*setfillstyle(SOLID_FILL,BLUE);
	bar(300,425,420,450);
	setcolor(LIGHTGREEN);
	outtextxy(305,435,"SAVING........");*/


	if(!asaved && !saved)
	{
	   size=imagesize(240,180,400,300);
	   image1=farmalloc(size);
	   getimage(240,180,400,300,image1);

	   strcpy(fname,get_filename(desc));
	   strcpy(file,fname);




	   putimage(240,180,image1,COPY_PUT);
	   farfree(image1);
	   if( fname[0] == '\0' )
	   {
		return;
	   }

	  int fd1 = open(file,O_CREAT | O_WRONLY);
	  if( fd1 == -1 )
	   {
		//error_msg("Cannot Save File!");
		perror(sys_errlist[errno]);
		return;
	   }


	   setfillstyle(SOLID_FILL,BLUE);
	   bar(300,425,420,450);
	   setcolor(LIGHTGREEN);
	   outtextxy(305,435,"SAVING........");



	  for(i=37;i<=408;i++)
	   for(j=66;j<=638;j++)
	    {
		ch = getpixel(j,i);
		write(fd1,&ch,1);
	    }
	  close(fd1);
       }
      else
      {

	int fd1 = open(file,O_WRONLY);
	  if( fd1 == -1 )
	   {
		error_msg("Cannot Save File!");
		//perror(sys_errlist[errno]);
		return;
	   }

	   setfillstyle(SOLID_FILL,BLUE);
	   bar(300,425,420,450);
	   setcolor(LIGHTGREEN);
	   outtextxy(305,435,"SAVING........");

	  for(i=37;i<=408;i++)
	   for(j=66;j<=638;j++)
	    {
		ch = getpixel(j,i);
		write(fd1,&ch,1);
	    }

	  close(fd1);

      }
	saved=1;
	asaved=1;

	setfillstyle(SOLID_FILL,LIGHTGRAY);
	bar(300,425,420,450);
	setcolor(color);

	displayfile(file);
	HideorShowcursor(2);
}

void load()
{
	char desg[] = "OPEN FILE",f_name[15];
	void far *image1;
	unsigned int size;
	FILE *fp;
	int i,j,count,ch;
	unsigned char col;
	HideorShowcursor(1);
	//if(!saved)save();
	size=imagesize(240,180,400,300);
	image1=farmalloc(size);
	getimage(240,180,400,300,image1);
	strcpy(f_name,get_filename(desg));


	putimage(240,180,image1,COPY_PUT);
	farfree(image1);

	if( f_name[0] == '\0')
	{
		return;
	}


	  int fd2 = open(f_name,O_RDONLY);
	  if( fd2 == -1 )
	   {
		error_msg("Cannot Open File!");
		  //perror(sys_errlist[errno]);
		return;
	   }
	for(i=37;i<=408;i++)
		for(j=66;j<=638;j++)
		{
			read(fd2,&ch,1);
			putpixel(j,i,ch);
		}

	close(fd2);
	saved=1;
	strcpy(file,f_name);
	displayfile(f_name);
	HideorShowcursor(2);
}



void rotate()
{
 int x1,y1,x2,y2,x3,y3,x,y,a,b,x5,x6,y5,y6;
 float x4,y4;
 void far *image1;
 unsigned int size;
 int ch;
 float th;
 if(!marked)return;
   HideorShowcursor(1);
 x1=marker.left;
 y1=marker.top;
 x2=marker.right;
 y2=marker.bottom;

 a=x3=x1;//basex//point of rotation
 b=y3=y2;
 int i,j;

 size=imagesize(240,180,400,300);
 image1=farmalloc(size);
 getimage(240,180,400,300,image1);
 int angle=get_angle();
 while(angle>360)
 {
     angle=get_angle();

     if(angle<=360)break;

  }
 th=angle*M_PI/180.0;
 putimage(240,180,image1,COPY_PUT);
 farfree(image1);


 unmark();

  fd = open("xyz.tmp",O_WRONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
	       //	perror(sys_errlist[errno]);
		return;
	}
	for(i=y1;i<=y2;i++)
	  for(j=x1;j<=x2;j++)
	  {
		ch = getpixel(j,i);
		write(fd,&ch,1);

	  }



	close(fd);
      setfillstyle(SOLID_FILL,bkcolor);
       bar(x1,y1,x2,y2);

	 fd = open("xyz.tmp",O_RDONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
	       //	perror(sys_errlist[errno]);
		return;
	}

	for(y=y1;y<=y2;y++)
		for(x=x1;x<=x2;x++)
		{
			read(fd,&ch,1);
			x4=x*cos(th)-y*sin(th)-(a*cos(th)-b*sin(th))+a;
			y4=x*sin(th)+y*cos(th)-(a*sin(th)+b*cos(th))+b;
			if(x4>=68 && x4<=638 && y4>=38 && y4<=407)
			{
			  putpixel(floor(x4),floor(y4),ch);

			    putpixel((int)x4,(int)y4,ch);

			  putpixel(ceil(x4),ceil(y4),ch);
			 }
		}
       close(fd);
    HideorShowcursor(2);

}


int get_angle()
{
	int i = 0;
	char text_data[15],bufp[1],text_data1[15];
	char ext[3];

	HideorShowcursor(1);
	setcolor(BLUE);
	setfillstyle(SOLID_FILL,LIGHTRED);
	bar(240,180,400,300);

	setfillstyle(SOLID_FILL,CYAN);
	bar(247,205,393,293);
	settextstyle(0,HORIZ_DIR,1);
	setcolor(BLUE);
	settextjustify(CENTER_TEXT,CENTER_TEXT);
	outtextxy(320,193,"ENTER ANGLE");

	outtextxy(320,223,"VALUE IN DEGREES:");
	moveto(280,260);
	while(1) {
		 fflush(stdin);
		 outtext("_");
		text_data[i] = getch();

		if( text_data[i] == '\n' || text_data[i] == '\r')
		{
			text_data[i] = '\0';
			break;
		}
		if( text_data[i] == 27 )
		{ /* ESC */
			break;
		}
		if(text_data[i]=='\b')
		{/*Backspace*/
			i--;
			moverel(-8,0);
		}
		if(isalnum(text_data[i]))
		{
			sprintf(bufp,"%c",text_data[i]);
			outtext(bufp);

		}
		else

			continue;
		i++;
		moverel(8,0);
		if(i==3)      /*MAX NO OF CHARS*/
			break;
	}


	if(text_data[i] == 27 )
		{text_data[0] = '0';text_data[1]='\0';}
	int angle=atoi(text_data);
	settextjustify(LEFT_TEXT,TOP_TEXT);
	HideorShowcursor(2);
	return (angle);
}

float get_values()
{
	int i = 0;
	float temp;
	char text_data[5],bufp[1];

	HideorShowcursor(1);
	setcolor(BLUE);
	setfillstyle(SOLID_FILL,LIGHTRED);
	bar(240,180,400,300);

	setfillstyle(SOLID_FILL,CYAN);
	bar(247,205,393,293);
	settextstyle(0,HORIZ_DIR,1);
	setcolor(BLUE);
	settextjustify(CENTER_TEXT,CENTER_TEXT);
	outtextxy(320,193,ch1);

	outtextxy(320,223,ch2);
	moveto(280,260);
	while(1) {
		 fflush(stdin);
		 outtext("_");
		text_data[i] = getch();

		if( text_data[i] == '\n' || text_data[i] == '\r')
		{
			text_data[i] = '\0';
			break;
		}
		if( text_data[i] == 27 )
		{
			break;
		}
		if(text_data[i]=='\b')
		{
			i--;
			moverel(-8,0);
		}
		if(isdigit(text_data[i]) || text_data[i]=='.'||text_data[i]=='-')
		{
			sprintf(bufp,"%c",text_data[i]);
			outtext(bufp);

		}
		else

			continue;
		i++;
		moverel(8,0);
		if(i==4)     //MAX NO OF CHARS
			break;
	}


	if(text_data[i] == 27 )
		{text_data[0] = '0';text_data[1]='\0';
		test=1;
		}
	temp=atof(text_data);
	settextjustify(LEFT_TEXT,TOP_TEXT);
	HideorShowcursor(2);
	ch1=NULL;ch2=NULL;
	return temp;
}



void scale()
{
 int x1,y1,x2,y2,x,y,dx,dy;
 void far *image1;
 unsigned int size;
 int ch;
 float sx,sy;
 long x4,y4;
 if(!marked)return;
   HideorShowcursor(1);

 x1=marker.left;
 y1=marker.top;
 x2=marker.right;
 y2=marker.bottom;

 dx=x1;//basex//point of rotation
 dy=y2;
 int i,j;

 size=imagesize(240,180,400,300);
 image1=farmalloc(size);
 getimage(240,180,400,300,image1);
 ch1="SCALE FACTOR";
 ch2="X VALUE";
 sx=get_values();
 //printf("%f",sx);
 if(test)
 {
  putimage(240,180,image1,COPY_PUT);
 farfree(image1);
 unmark();
  return;
 }
 test=0;
 ch1="SCALE FACTOR";
 ch2="Y VALUE";
 sy=get_values();
if(test)
 {
  putimage(240,180,image1,COPY_PUT);
 farfree(image1);
 unmark();
  return;
 }
// printf("%f %f",sx,sy);
 putimage(240,180,image1,COPY_PUT);
 farfree(image1);


 unmark();


    fd = open("xyz.tmp",O_WRONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	}
	for(i=y1;i<=y2;i++)
	  for(j=x1;j<=x2;j++)
	  {
		ch = getpixel(j,i);
		write(fd,&ch,1);
	  }



	close(fd);
      setfillstyle(SOLID_FILL,bkcolor);
       bar(x1,y1,x2,y2);


	 fd = open("xyz.tmp",O_RDONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	}

	for(y=y1;y<=y2;y++)
		for(x=x1;x<=x2;x++)
		{
			read(fd,&ch,1);
		       x4=x*sx+dx*(1-sx);
		       y4=y*sy+dy*(1-sy);
			if(x4>=68 && x4<=638 && y4>=38 && y4<=407)
			{
			  putpixel(floor(x4),floor(y4),ch);

			   putpixel((int)x4,(int)y4,ch);

			  putpixel(ceil(x4),ceil(y4),ch);
			 }
		}
       close(fd);
    HideorShowcursor(2);

}


void translate()
{
 int x1,y1,x2,y2,x,y,dx,dy;
 void far *image1;
 unsigned int size;
 int ch;
 int x4,y4;
 if(!marked)return;
   HideorShowcursor(1);

 x1=marker.left;
 y1=marker.top;
 x2=marker.right;
 y2=marker.bottom;

 int i,j;

 size=imagesize(240,180,400,300);
 image1=farmalloc(size);
 getimage(240,180,400,300,image1);
 ch1="TRANSLATE FACTOR";
 ch2="X VALUE";
 dx=(int)get_values();
 //printf("%f",sx);
 if(test)
 {
  putimage(240,180,image1,COPY_PUT);
 farfree(image1);
 unmark();
  return;
 }
 test=0;
 ch1="TRANSLATE FACTOR";
 ch2="Y VALUE";
 dy=(int)get_values();
if(test)
 {
  putimage(240,180,image1,COPY_PUT);
 farfree(image1);
 unmark();
  return;
 }
// printf("%f %f",sx,sy);
 putimage(240,180,image1,COPY_PUT);
 farfree(image1);


 unmark();


    fd = open("xyz.tmp",O_WRONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	}
	for(i=y1;i<=y2;i++)
	  for(j=x1;j<=x2;j++)
	  {
		ch = getpixel(j,i);
		write(fd,&ch,1);
	  }



	close(fd);
      setfillstyle(SOLID_FILL,bkcolor);
       bar(x1,y1,x2,y2);


	 fd = open("xyz.tmp",O_RDONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	}

	for(y=y1;y<=y2;y++)
		for(x=x1;x<=x2;x++)
		{
			read(fd,&ch,1);
		       x4=x+dx;
		       y4=y+dy;
			if(x4>=68 && x4<=638 && y4>=38 && y4<=407)
			{
			  putpixel(floor(x4),floor(y4),ch);

			   putpixel((int)x4,(int)y4,ch);

			  putpixel(ceil(x4),ceil(y4),ch);
			 }
		}
       close(fd);
    HideorShowcursor(2);

}


void shear()
{
 int x1,y1,x2,y2,x,y;
 float shx,shy;
 void far *image1;
 unsigned int size;
 int ch;
 int x4,y4;
 if(!marked)return;
   HideorShowcursor(1);

 x1=marker.left;
 y1=marker.top;
 x2=marker.right;
 y2=marker.bottom;

 int i,j;

 size=imagesize(240,180,400,300);
 image1=farmalloc(size);
 getimage(240,180,400,300,image1);
 ch1="SHEAR FACTOR";
 ch2="X VALUE";
 shx=get_values();
 //printf("%f",shx);
 if(test)
 {
  putimage(240,180,image1,COPY_PUT);
 farfree(image1);
 unmark();
  return;
 }
 test=0;
 ch1="SHEAR FACTOR";
 ch2="Y VALUE";
 shy=get_values();
// printf("%f",shy);
if(test)
 {
  putimage(240,180,image1,COPY_PUT);
 farfree(image1);
 unmark();
  return;
 }
// printf("%f %f",sx,sy);
 putimage(240,180,image1,COPY_PUT);
 farfree(image1);


 unmark();


    fd = open("xyz.tmp",O_WRONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	}
	for(i=y1;i<=y2;i++)
	  for(j=x1;j<=x2;j++)
	  {
		ch = getpixel(j,i);
		write(fd,&ch,1);
	  }



	close(fd);
      setfillstyle(SOLID_FILL,bkcolor);
       bar(x1,y1,x2,y2);


	 fd = open("xyz.tmp",O_RDONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	}

	for(y=y1;y<=y2;y++)
		for(x=x1;x<=x2;x++)
		{
			read(fd,&ch,1);
		       //x4=x+y*shx;
		      // y4=y+x*shy;
		      x4=x-y*shx+y2*shx;
		       y4=-x*shy+y+x1*shy;
			if(x4>=68 && x4<=638 && y4>=38 && y4<=407)
			{
			  putpixel(floor(x4),floor(y4),ch);

			   putpixel((int)x4,(int)y4,ch);

			  putpixel(ceil(x4),ceil(y4),ch);
			 }
		}
       close(fd);
    HideorShowcursor(2);

}

void magnify()
{
 int x1,y1,x2,y2,x,y;
 int ch,n=2;
 int x4,y4,xx,yy,a,b;

 if(!marked)return;

 MOUSE *mm;
 mm=(MOUSE *)malloc(sizeof(MOUSE));
 mm->click=0;
 pollmouse(mm);


 x1=marker.left;
 y1=marker.top;
 x2=marker.right;
 y2=marker.bottom;

 int i,j;

 getclick(mm,LEFTBUTTON);
  if(incanvas(mm))
     {
      xx=mm->x;
      yy=mm->y;
     }

 unmark();
  HideorShowcursor(1);

    fd = open("xyz.tmp",O_WRONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	}
	for(i=y1;i<=y2;i++)
	  for(j=x1;j<=x2;j++)
	  {
		ch = getpixel(j,i);
		write(fd,&ch,1);
	  }

	close(fd);
      setfillstyle(SOLID_FILL,bkcolor);
       bar(x1,y1,x2,y2);


	 fd = open("xyz.tmp",O_RDONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	}
	i=xx;
	j=yy;
	for(y=y1;y<=y2;y++)
	{
		for(x=x1;x<=x2;x++)
		{
			read(fd,&ch,1);
			if(i>=68 && i<=638 && j>=38 && j<=406)
			{
			  setfillstyle(1,ch);
			  bar(i,j,i+2,j+2);
			   i+=2;
			 }
		}

		j+=2;
		i=xx;
	    }
	    a=xx+2*(x2-x1);//width of zoomed image
	    b=j;
	    if(b>408)b=408;
       close(fd);

       int esc;
       esc=getch();

      if(esc==27)
      {
	setfillstyle(SOLID_FILL,bkcolor);
	bar(xx,yy,a,b);



	fd = open("xyz.tmp",O_RDONLY);
	if( fd == -1 ) {
		error_msg("Cannot Save File!");
		//	perror(sys_errlist[errno]);
		return;
	  }

		for(y=y1;y<=y2;y++)
		for(x=x1;x<=x2;x++)
		{
			read(fd,&ch,1);
			putpixel(x,y,ch);
		}
	     close(fd);
	     HideorShowcursor(2);
	}
       else
	HideorShowcursor(2);
}


void textbox()
{
       int x,y;
       char FileName[72];//max chars
       MOUSE *mq;
       mq=(MOUSE *)malloc(sizeof(MOUSE));
       mq->click=0;
       pollmouse(mq);
       getclick(mq,LEFTBUTTON);
       if(incanvas(mq))
       {
	x=mq->x;
	y=mq->y;
       }
       HideorShowcursor(1);
       setcolor(color);
       outtextxy(x,y,"_");
       setfillstyle(SOLID_FILL,bkcolor);


	for(int j=0;j<72;j++)
		FileName[j]='\0';

	int i=0;
	while(FileName[i-1]!=13&&i<73)
	{
		FileName[i++]=getche();
		if((FileName[i-1]=='\b')&&(i==1))
			FileName[--i]='\0';
		else
		if(FileName[i-1]=='\b')
		{
			FileName[i-1]='\0';
			FileName[i-2]='\0';
			i-=2;
		}
		bar(x-3,y-5,x+textwidth(FileName)+16,y+8);
		outtextxy(x,y,FileName);
		outtextxy(x+textwidth(FileName),y,"_");
	}
	FileName[i-1]='\0';
	bar(x-3,y-5,x+textwidth(FileName)+16,y+8);
	outtextxy(x,y,FileName);

	setfillstyle(0,7);
	setlinestyle(0,0,1);
	HideorShowcursor(2);
}

void about_us()
{

	void far *pict;
	unsigned int size2;

	size2=imagesize(150,180,450,300);
	pict=farmalloc(size2);
	getimage(150,180,450,300,pict);
	HideorShowcursor(1);
	settextjustify(CENTER_TEXT,CENTER_TEXT);
	setfillstyle(SOLID_FILL,BLUE);
	bar(150,180,450,300);
	setcolor(YELLOW);
	outtextxy(300,190,"DEVELOPED BY:");
	outtextxy(300,205,"S.HEMANTH BALAJI");
	outtextxy(300,220,"&");
	outtextxy(300,235,"SHREYAS SHETTY.K");
	outtextxy(300,250,"@");
	outtextxy(300,265,"Dept Of CSE");
	outtextxy(300,280,"Bangalore Institute Of Technology");
	sleep(5);
	putimage(150,180,pict,COPY_PUT);
	farfree(pict);

	//cha=NULL;
	settextjustify(LEFT_TEXT,TOP_TEXT);
	setcolor(color);
	HideorShowcursor(2);

}