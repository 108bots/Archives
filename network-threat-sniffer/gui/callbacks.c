/*Threat Sniffer GUI using Glade Library - Callbacks file
 * Author: Hemanth Srinivasan
 * Year: 2002
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>

int rowcount=-1;
//int rowcount1=-1;


void
on_clear_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
	
         GtkWidget *dialog5;
	GtkWidget *clist1,*clist2;
	char buf[50],buf1[50],buf2[50],buf3[50];
	FILE *fp;
        //int row;
       GtkWidget *entry0,*entry1,*entry2,*entry3,*entry4,*entry5;
       // printf("*****************\n");
	entry0=lookup_widget(GTK_BUTTON(button),"src_ip");
       //  printf("nmnmmnmnmnmnnmmn");
	entry1=lookup_widget(GTK_BUTTON(button),"src_nic");
	entry2=lookup_widget(GTK_BUTTON(button),"dst_ip");
	entry3=lookup_widget(GTK_BUTTON(button),"dst_nic");
	entry4=lookup_widget(GTK_BUTTON(button),"login");	
	entry5=lookup_widget(GTK_BUTTON(button),"time");

	clist2=lookup_widget(GTK_BUTTON(button),"clist2");
	clist1=lookup_widget(GTK_BUTTON(button),"clist1");
	rowcount=(gint)g_list_nth_data(GTK_CLIST(clist1)->selection,0);
        
        gtk_clist_clear(GTK_CLIST(clist2));

       gtk_entry_set_text(GTK_ENTRY(entry0),"");
	 gtk_entry_set_text(GTK_ENTRY(entry1),"");
	 gtk_entry_set_text(GTK_ENTRY(entry2),"");
	 gtk_entry_set_text(GTK_ENTRY(entry3),"");
	 gtk_entry_set_text(GTK_ENTRY(entry4),"");
	 gtk_entry_set_text(GTK_ENTRY(entry5),"");

	dialog5 = create_dialog5();
  	gtk_widget_show(dialog5);

    

}


void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *clist2,*clist1;
	char *buf,buf1[50],buf2[10],buf3[50],command[100],command1[100];
        char command2[100];
	int row,row1;

	clist1=lookup_widget(GTK_BUTTON(button),"clist1");
	row1=(gint)g_list_nth_data(GTK_CLIST(clist1)->selection,0);

	clist2=lookup_widget(GTK_BUTTON(button),"clist2");
	row=(gint)g_list_nth_data(GTK_CLIST(clist2)->selection,0);
	gtk_clist_get_text(GTK_CLIST(clist2),row,0,&buf);
	printf(" Buf : %s \n",buf);
	strcpy(buf1,"rm -f /sniff/");
	strcat(buf1,buf);
	system(buf1);
	gtk_clist_remove(GTK_CLIST(clist2),row);
        snprintf(buf2,9,"%d",row1+1);
	strcpy(buf3,"/sniff/sniffiles");
	strcat(buf3,buf2);
        printf("buf3 %s",buf3); 

       snprintf(command,99,"head -%d %s>temp1",row1,buf3);
       printf("\ncommand %s",command);
	system(command);

	 snprintf(command1,99,"tail +%d %s>>temp1",row1+2,buf3);
	 printf("\ncommand1 %s",command1);
	system(command1);
        
	 snprintf(command2,99,"mv temp1 %s",buf3);
	 printf("\ncommand2 %s",command2);
	system(command2);
}


void
on_help_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{

   FILE *fp;
   GtkWidget *mytext;
   char buf[1000],c;
   int x=0;
   
	GtkWidget *dialog2;
	dialog2 = create_dialog2();
  	gtk_widget_show(dialog2);

   mytext=lookup_widget(dialog2,"mytext");
   fp=fopen("/sniff/help","r");
   while((c=getc(fp))!=EOF)
   {

	buf[x]=c;
	x++;
   } 
   buf[x]='\0';
  //printf("text: %s",buf);
		
   gtk_text_insert(GTK_TEXT(mytext),NULL,NULL,NULL,buf,strlen(buf));
   fclose(fp);	
}


void
on_about_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *dialog1;
	dialog1 = create_dialog1();
  	gtk_widget_show(dialog1);
}


void
on_button1_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *dialog1;
	dialog1=lookup_widget(button,"dialog1");
	gtk_signal_connect_object(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(dialog1));
}




void
on_button3_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *window,*entry1,*entry2,*clist,*window2;
	char *text[2];
	int n,flag1,flag2;
        char user_name[50],password[50];
	GtkWidget *dialog6;
	text[0]=malloc(50);
	text[1]=malloc(50);
	n=strlen(text[1]);
	text[1][n-1]='\0';
        window2=lookup_widget(GTK_BUTTON(button),"window2");
	entry1=lookup_widget(GTK_BUTTON(button),"entry7");
	strcpy(user_name,gtk_entry_get_text(GTK_ENTRY(entry1)));
        //printf("User na %s \n",user_name);
        flag1=strcmp(user_name,"root");
        entry2=lookup_widget(GTK_BUTTON(button),"entry8");
	strcpy(password,gtk_entry_get_text(GTK_ENTRY(entry2)));
        flag2=strcmp(password,"server");
      if(flag1==0 && flag2==0)
       {
	window=create_window1();
	gtk_widget_show(window);
	clist=lookup_widget(GTK_BUTTON(window),"clist1");
	gtk_clist_clear(GTK_CLIST(clist));
	
	strcpy(text[0],"Computer1");
	gtk_clist_insert(GTK_CLIST(clist),-1,text);

	strcpy(text[0],"Computer2");
	gtk_clist_insert(GTK_CLIST(clist),-1,text);
        
       gtk_signal_connect_object(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(window2));
       
       }
      else
       {
	gtk_entry_set_text(GTK_ENTRY(entry1),"");
	gtk_entry_set_text(GTK_ENTRY(entry2),"");
	dialog6 = create_dialog6();
  	gtk_widget_show(dialog6);
       } 

}


void
on_loadfile_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	gint row=-1;
	GtkWidget *clist1,*clist2;
	FILE *fp;
	int n;
	char *text[2];
	
	GtkWidget *dialog3;
	
	text[0]=malloc(50);
	text[1]=malloc(50);
	n=strlen(text[1]);
	text[1][n-1]='\0';
	dialog3 = create_dialog3();
	clist1=lookup_widget(GTK_BUTTON(button),"clist1");
	clist2=lookup_widget(GTK_BUTTON(button),"clist2");
	row=(gint)g_list_nth_data(GTK_CLIST(clist1)->selection,0);
	if(row==0)
	{
		gtk_clist_clear(GTK_CLIST(clist2));
		fp=fopen("/sniff/sniffiles1","r");
		  if(fp==NULL)
			{
			gtk_widget_show(dialog3);
			return;
					
			}

		while((fgets(text[0],49,fp)!=NULL))
		{
			n=strlen(text[0]);
			text[0][n-1]='\0';
			printf("content %s \n",text[0]);
			gtk_clist_insert(GTK_CLIST(clist2),-1,text);
		}
		fclose(fp);
	}     

	if(row==1)
	{
		gtk_clist_clear(GTK_CLIST(clist2));
		fp=fopen("/sniff/sniffiles2","r");
		 if(fp==NULL)
			{
			gtk_widget_show(dialog3);
			return;
					
			}
		while((fgets(text[0],49,fp)!=NULL))
		{
			n=strlen(text[0]);
			text[0][n-1]='\0';
			gtk_clist_insert(GTK_CLIST(clist2),-1,text);
		}
		fclose(fp);
	}


}


void
on_view_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *clist2;
	
	int row;
	char *buf,buf1[50];	
	clist2=lookup_widget(GTK_BUTTON(button),"clist2");
	row=(gint)g_list_nth_data(GTK_CLIST(clist2)->selection,0);
	printf(" Row %d \n",row);
	gtk_clist_get_text(GTK_CLIST(clist2),row,0,&buf);
        //printf("content %s \n",buf);
	strcpy(buf1,"gedit /sniff/");
	strcat(buf1,buf);
	system(buf1);


}


void
on_detail_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

        GtkWidget *clist2;
	char *buf,buf1[50],buf3[50],command[300],c;
	int row,x;
	FILE *fp;
	GtkWidget *dialog4;
	GtkWidget *entry0,*entry1,*entry2,*entry3,*entry4,*entry5;
       // printf("*****************\n");
	entry0=lookup_widget(GTK_BUTTON(button),"src_ip");
       //  printf("nmnmmnmnmnmnnmmn");
	entry1=lookup_widget(GTK_BUTTON(button),"src_nic");
	entry2=lookup_widget(GTK_BUTTON(button),"dst_ip");
	entry3=lookup_widget(GTK_BUTTON(button),"dst_nic");
	entry4=lookup_widget(GTK_BUTTON(button),"login");	
	entry5=lookup_widget(GTK_BUTTON(button),"time");
	
	dialog4 = create_dialog4();
	clist2=lookup_widget(GTK_BUTTON(button),"clist2");
	row=(gint)g_list_nth_data(GTK_CLIST(clist2)->selection,0);
	gtk_clist_get_text(GTK_CLIST(clist2),row,0,&buf);
        
	//first entry
  	strcpy(buf1,"/sniff/");
        strcat(buf1,buf);
       //  printf("buf:%s",buf1);     
  	snprintf(command,299,"grep 'Source IP address=' %s",buf1);
	strcat(command," | cut -d = -f 2 >/sniff/otherlist");
        
    	system(command);
               
	fp=fopen("/sniff/otherlist","r");
        if(fp==NULL)
         {
         
	   gtk_widget_show(dialog4);
	   return;
					
			
         }
        x=0;
	while((c=getc(fp))!='\n')
	{
 	  buf3[x]=c;
	  x++;
	}
	buf3[x]='\0';
	gtk_entry_set_text(GTK_ENTRY(entry0),buf3);	
        fclose(fp);
	remove("/sniff/otherlist");
        

	//second entry
	strcpy(buf1,"/sniff/");
        strcat(buf1,buf);
              
  	snprintf(command,299,"grep 'Source ETHERNET(NIC) address=' %s",buf1);
	strcat(command," | cut -d = -f 2 >/sniff/otherlist");
        
    	system(command);
               
	fp=fopen("/sniff/otherlist","r");
        x=0;
	while((c=getc(fp))!='\n')
	{
 	  buf3[x]=c;
	  x++;
	}
	buf3[x]='\0';
	gtk_entry_set_text(GTK_ENTRY(entry1),buf3);	
        fclose(fp);
	remove("/sniff/otherlist");
        
	
	//third entry
	strcpy(buf1,"/sniff/");
        strcat(buf1,buf);
              
  	snprintf(command,299,"grep 'Destination IP address=' %s",buf1);
	strcat(command," | cut -d = -f 2 >/sniff/otherlist");
        
    	system(command);
               
	fp=fopen("/sniff/otherlist","r");
        x=0;
	while((c=getc(fp))!='\n')
	{
 	  buf3[x]=c;
	  x++;
	}
	buf3[x]='\0';
	gtk_entry_set_text(GTK_ENTRY(entry2),buf3);	
        fclose(fp);
	remove("/sniff/otherlist");

	//fourth entry
	strcpy(buf1,"/sniff/");
        strcat(buf1,buf);
              
  	snprintf(command,299,"grep 'Destination ETHERNET(NIC) address=' %s",buf1);
	strcat(command," | cut -d = -f 2 >/sniff/otherlist");
        
    	system(command);
               
	fp=fopen("/sniff/otherlist","r");
        x=0;
	while((c=getc(fp))!='\n')
	{
 	  buf3[x]=c;
	  x++;
	}
	buf3[x]='\0';
	gtk_entry_set_text(GTK_ENTRY(entry3),buf3);	
        fclose(fp);
	remove("/sniff/otherlist");
        
        
	//fifth entry
	strcpy(buf1,"/sniff/");
        strcat(buf1,buf);
              
  	snprintf(command,299,"grep 'The person logged in as=' %s",buf1);
	strcat(command," | cut -d = -f 2 >/sniff/otherlist");
        
    	system(command);
               
	fp=fopen("/sniff/otherlist","r");
        x=0;
	while((c=getc(fp))!='\n')
	{
 	  buf3[x]=c;
	  x++;
	}
	buf3[x]='\0';
	gtk_entry_set_text(GTK_ENTRY(entry4),buf3);	
        fclose(fp);
	remove("/sniff/otherlist");
        

	//sixth entry
	strcpy(buf1,"/sniff/");
        strcat(buf1,buf);
              
  	snprintf(command,299,"grep 'Time of Transfer=' %s",buf1);
	strcat(command," | cut -d = -f 2 >/sniff/otherlist");
        
    	system(command);
               
	fp=fopen("/sniff/otherlist","r");
        x=0;
	while((c=getc(fp))!='\n')
	{
 	  buf3[x]=c;
	  x++;
	}
	buf3[x]='\0';
	gtk_entry_set_text(GTK_ENTRY(entry5),buf3);	
        fclose(fp);
	remove("/sniff/otherlist");
        
	
	

}

 


void
on_button5_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

 	GtkWidget *dialog3;
        dialog3=lookup_widget(button,"dialog3");
	gtk_signal_connect_object(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(dialog3));
}


void
on_button2_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

  GtkWidget *dialog2;
	dialog2=lookup_widget(button,"dialog2");
	gtk_signal_connect_object(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(dialog2));

}


void
on_button6_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

 
  GtkWidget *dialog4;
	dialog4=lookup_widget(button,"dialog4");
	gtk_signal_connect_object(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(dialog4));

}


void
on_button7_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

 GtkWidget *dialog5;

  
	
	
         dialog5=lookup_widget(button,"dialog5");
	 
 gtk_signal_connect_object(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(dialog5)); 
		

	if(rowcount==0)
	{
		system("rm -f /sniff/comp1*");
		system("rm -f /sniff/sniffiles1");
	}


	if(rowcount==1)
	{
		system("rm -f /sniff/comp2*");
		system("rm -f /sniff/sniffiles2");
	}

 rowcount=-1;
}


void
on_button8_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

  GtkWidget *dialog5;
	dialog5=lookup_widget(button,"dialog5");
	gtk_signal_connect_object(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(dialog5)); 
	
}


void
on_button9_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

 GtkWidget *dialog6;
	dialog6=lookup_widget(button,"dialog6");
	gtk_signal_connect_object(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(dialog6)); 
	 

}

