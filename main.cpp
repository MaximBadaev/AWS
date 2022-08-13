/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Stan
 *
 * Created on 5 мая 2022 г., 0:23
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Stan
 *
 * Created on 30 апреля 2022 г., 7:40
 */

#include <cstdlib>
#include <windows.h>
#include <iostream>
#include <clocale>
#include <string.h>
#include <vector>
#include <sstream>
#include <psapi.h>
#include <algorithm>

using namespace std;

void help(){
    cout<<"Automatic window switcher 2.0, Maxim Badaev, 2022"<<endl;
    cout<<" "<<endl;
    cout<<"Enter the comma-separated values of the window names"<<endl;
    cout<<"Example:[name1],[name2],[name3]..."<<endl;
    cout<<"For more effective search avoid spaces after commas "
    "and other excess symbols. Name values can be just single "
    "words from full window names"<<endl;
    cout<<"To stop the process of repetition of maximizing and "
    "minimizing the windows press CAPS_LOCK."<<endl;
    cout<<"To display the whole current window list enter 'list'."<<endl;
    cout<<"To display list of separate window parameters enter 'list "
    "<parameter1>,<parameter2>...'."<<endl;
    cout<<"Window parameters:"<<endl;
    cout<<"wname - window name"<<endl;
    cout<<"wid - window ID"<<endl;
    cout<<"pid - process ID"<<endl;
    cout<<"fname - file name"<<endl;
    cout<<"fpath - file path"<<endl;
    cout<<"wst - window status"<<endl;
    cout<<"To search window(s) by the window name enter 'list search <window "
    "name or a fragment of it>'."<<endl;
    cout<<"To display list of separate window parameters searched by the window"
    "name enter 'list <parameter1>,<parameter2>...search <window name or a "
    "fragment of it>'."<<endl;
    cout<<"To see this info enter 'help'."<<endl;
    cout<<" "<<endl;
}

string SplitFind(string x, char y, int z)
{
    int dcount=0;//Счётчик разделителей
    
    //In case of void words
    if (x[x.length()-1]==y) x+=' ';
    if (x[0]==y) x=' '+x;
    
    for(int i=0;i<x.length();i++)
    {
       if(i>0){//In case of void words
       if((x[i]==y)&&(x[i-1]==y)) x.insert(i," ");
       }
       if (x[i]==y) dcount++;//Когда символ i из строки х равен y
       //счётчик прибавляется
    }

    
    if (z>dcount) return "";//Выход из функции в случае слишком большого
    //номера слова

    vector <string> vx;//Объявление строкового вектора слов vx

    istringstream iss(x);//Объявление строкового потока iss с входной
    //строкой x в качестве аргумента
    while(getline(iss,x,y))//Цикл, перебирающий и делящий строку х на слова
    //с разделителем y
    {
        vx.push_back(x);//Добавление слов из строки x в вектор vx
    }
    return vx[z];//Возвращаемое функцией значение в виде слова
    //(элемента вектора vx с номером z)
 }

//------------------------------------------------------------------------------
//The structure containing window parameters
struct wps{
	HWND hwnd;//Handle to the window
	string p;//String with window name
	string wid;//Window ID
	string pid;//Process ID
        string fpath;//Path to the process file
	string wst;//Window status
        string fname;//Process file name
};
//------------------------------------------------------------------------------
//Function for forming the set of window parameters
void wpsfmg(wps *wps,HWND hwnd,string p){
    DWORD dpid;//Process ID;
    DWORD dwid=GetWindowThreadProcessId(hwnd,&dpid);//Window ID
    
    //Retrieving the filepath to the process
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dpid);
    char pfpath[256];
    GetProcessImageFileName(hProc,pfpath,256);
    wps->fpath=pfpath;
    
    //Forming the file name
    wps->fname="";
    int i=wps->fpath.length()-1;
    while(wps->fpath[i]!=92){
        wps->fname+=wps->fpath[i];
        i--;    
    }
    reverse(wps->fname.begin(),wps->fname.end());
    
    wps->p=p;//Window name string
    wps->hwnd=hwnd;//A handle to the window
    //Conversion of wid and pid via string streams
            stringstream *wconv=new stringstream;
            *wconv<<dwid;
            *wconv>>wps->wid;
            delete wconv;
            stringstream *pconv=new stringstream;
            *pconv<<dpid;
            *pconv>>wps->pid;
            delete pconv;
            //Determining the window placement status
            WINDOWPLACEMENT wpl;
            GetWindowPlacement(hwnd,&wpl);
            switch(wpl.showCmd){
                case 1: wps->wst="normal"; 
                        break;
                case 2: wps->wst="minimized";
                        break;
                case 3: wps->wst="maximized";
                        break;
            }
}
//------------------------------------------------------------------------------
//The function for displaying the list of windows
void searchlist(wps *wps, string w, stringstream *stl){
    //Vector for all parameters
    vector<vector<string>> pv;
    //ID's
    pv.push_back(vector<string>());
    pv[0].push_back("wname");pv[0].push_back("wid");
    pv[0].push_back("pid");pv[0].push_back("wst");
    pv[0].push_back("fname");pv[0].push_back("fpath");
    //Parameters
    pv.push_back(vector<string>());
    pv[1].push_back(wps->p);pv[1].push_back(wps->wid);
    pv[1].push_back(wps->pid);pv[1].push_back(wps->wst);
    pv[1].push_back(wps->fname);pv[1].push_back(wps->fpath);
    //Cleaning the parameters from the '|' symbol
    for(int i=0;i<pv[1].size();i++) 
        replace(pv[1][i].begin(),pv[1][i].end(),'|',' ');    
    
    if (w=="list"){
	char *es=new char;
        for(int i=0;i<pv[1].size();i++){
        if(i==pv[1].size()-1) *es='\n';
        else *es='|';
        *stl<<pv[1][i]<<*es;        
        }
        delete es;
    }
    else if (SplitFind(w,' ',1)=="search"){//Standard search unit
        string *sstr=new string;//A string being searched
        *sstr=SplitFind(w,' ',2);

        if (wps->p.find(*sstr,0)!=-1){
            char *es=new char;
            for(int i=0;i<pv[1].size();i++){
                if(i==pv[1].size()-1) *es='\n';
                else *es='|';
                *stl<<pv[1][i]<<*es;        
                }
            delete es;
        }    
        delete sstr;
    } 
    else{
    
        bool search=0;//Accurate search indicator
        if (SplitFind(w,' ',2)=="search"){
            search=1;
            if (wps->p.find(SplitFind(w,' ',3),0)!=-1) search=0;
        }
        
        if(search==0){
            //Selection of the parameters
            stringstream *pstr=new stringstream;
            *pstr<<SplitFind(w,' ',1);
            int *c=new int;
            *c=0;
            for(string pm;getline(*pstr,pm,',');){
                for(int i=0;i<pv[0].size();i++){
                    if (pm==pv[0][i]){
                        if (*c>0) *stl<<'|';
                        *stl<<pv[1][i];
                        (*c)++;
                    }                
                }            
            }
            *stl<<'\n';//The End Of The Line:) 
            delete c;
            delete pstr;
        }
    }
    
}
//------------------------------------------------------------------------------
//The function for displaying the list
void displist(stringstream& stl, string w){
    //2D vector for window parameters table
    vector<vector<string>> table;

    //Filling the table    
    int *i=new int;//Row counter
    *i=0;
    for(string row;getline(stl,row,'\n');){ 
        table.push_back(vector<string>());
        stringstream *tr=new stringstream;
        *tr<<row;
        for(string col;getline(*tr,col,'|');){
            table[(*i)].push_back(col);
        }
        (*i)++;
        delete tr;
        }
        delete i;
           
    //Displaying the table
    for(int i=0;i<table.size();i++){
        string *es=new string;
        if(table[i].size()>1){
            for(int j=0;j<table[i].size();j++){
                if(j<(table[i].size()-1)) *es=" | ";
                else *es="\n";
                string *item=new string;
                string *fill=new string;
                if(table[i][j].length()<17){
                *item=table[i][j];
                *fill+=string(17-item->length(),' ');
            }    
            else if(table[i][j].length()>17){
                *item=string(table[i][j],0,14);
                *fill="...";                
            }
            else *item=table[i][j];            
            
            cout<<*item<<*fill<<*es;
            
            delete item;
            delete fill;
            }
            delete es;
        }
        else cout<<table[i][0]<<endl;
    }
        
    table.clear(); 
   
    cout<<" "<<endl;    
}
//------------------------------------------------------------------------------
//The function for switching the windows
void sw(string w, wps *wps, bool *f, int t){
string *sp=new string;
                *sp=wps->p;
                if (*f==0){
                //Search by name, window ID or process ID
                    if ((sp->find(w,0)!=-1)||
                        (w==wps->wid)||
                        (w==wps->pid)){
                        *f=1;
                        cout<<"Found "<<wps->p<<" "<<wps->wid<<" "<<
                        wps->pid<<endl;
                        ShowWindow(wps->hwnd, SW_SHOWMAXIMIZED);
                        Sleep(t);
                        ShowWindow(wps->hwnd, SW_SHOWMINIMIZED);
                    }                   
                }
                delete sp;
}
//------------------------------------------------------------------------------
//General function processing the windows
void wproc(string w, stringstream *stl, int t=0){
HWND hwnd;//A handle for a window
char p[100];//A char array for the text of a window name
bool f=0;//An indicator for a window being found or not
wps wps;

hwnd = GetForegroundWindow();//Receiving the data of all foreground windows
hwnd = GetWindow(hwnd, GW_HWNDFIRST);//The data of the first window
//hwnd = GetDesktopWindow();
//hwnd = GetWindow(hwnd, GW_CHILD);
cout<<" "<<endl;

//A cycle for iterating the windows in handle hwnd
    while (hwnd !=0){
        hwnd = GetWindow(hwnd, GW_HWNDNEXT);//Receiving the next window data
        GetWindowText(hwnd,p,100);//Receiving window name
        if ((strlen(p)>0)&&(IsWindowVisible(hwnd))){/*Excluding the empty lines
        and checking if the window is visible*/
            wpsfmg(&wps,hwnd,p);//Forming the set of window parameters
            if (SplitFind(w,' ',0)=="list") searchlist(&wps,w,&*stl);//The listing unit
            else {
                sw(w,&wps,&f,t);//The switching unit
            }
        }
    }
if (SplitFind(w,' ',0)!="list")
    if (f==0) cout<<w<<" not found"<<endl;
cout<<" "<<endl;
}
//------------------------------------------------------------------------------
//Basic function
void base(){
    vector <string> wlist;
    string *w=new string;
    stringstream wstr;
    stringstream stl;
    char *e=new char;
    
    cout<<"Enter the windows being searched: ";
    getline(cin,*w);
    //The case of listing
    if (SplitFind(*w,' ',0)=="list"){
        //The top of the table
        string **pma=new string*[2];
        pma[0]=new string[6]{"WINDOW_NAME","WINDOW_ID","PROCESS_ID",
        "WINDOW_STATE","FILE_NAME","FILE_PATH"};
        pma[1]=new string[6]{"wname","wid","pid","wst","fname","fpath"};
        if (*w=="list"||SplitFind(*w,' ',1)=="search"){
            char *es=new char;
            for(int i=0;i<6;i++){
                if (i==5) *es='\n';
                else *es='|';
                stl<<pma[0][i]<<*es;
            }
            delete es;
        }
        else{
            stringstream *pstr=new stringstream;
            *pstr<<SplitFind(*w,' ',1);
            int *c=new int;
            *c=0;
            for(string pm;getline(*pstr,pm,',');){
                for(int i=0;i<6;i++){
                    if (pm==pma[1][i]){
                        if (*c>0) stl<<'|';
                        stl<<pma[0][i];
                        (*c)++;
                    }                
                }            
            }
            if (stl.str()==""){
                cout<<"No such parameter"<<endl;
                base(); 
                return;               
            }    
            stl<<'\n';//The End Of The Line:) 
            delete c;
            delete pstr;
        }
        wproc(*w,&stl); //Processing the data       
        displist(stl,*w);//Displaying the table
        stl.str("");        
        for (int i=0;i<2;i++) delete [] pma[i]; 
        delete w;
    }
    //Displaying 'help' information
    else if(*w=="help") help();
    //The case of maximizing and minimizing
    else{
        int t;
        cout<<"Enter time interval in seconds between windows appearing: ";
        cin>>t;
        t=t*1000;
        wstr<<*w;
        delete w;
        string *w2=new string;
        while(getline(wstr,*w2,',')){
            if(w2->at(0)==' ') w2->erase(0,1);//Space control
            wlist.push_back(*w2);        
        }
        delete w2;
        cout<<" "<<endl;
        
        //Checking the current status of the CapsLock key
        if(GetKeyState(VK_CAPITAL)) {
            cout<<"The CapsLock key is pressed. Sending the "
            "correcting keystroke"<<endl;
            keybd_event(VK_CAPITAL,0,0,0);
            keybd_event(VK_CAPITAL,0,KEYEVENTF_KEYUP,0);
        }               
        //Maximizing and minimizing while the CapsLock key is not pressed
        while(!GetKeyState(VK_CAPITAL)){
            for(int i=0;i<wlist.size();i++){
                string *w=new string;
                *w=wlist[i];
                wproc(*w,&stl,t);
                delete w;
            }
        }
    }
        
    cout<<"Type 'n' to start a new process or type 'e' "
    "to exit the program: ";
    cin>>*e;
    if (*e=='e') {        
    }
    else if (*e=='n') {
        //Flushing the cin buffer to avoid bugs in the next process
        cin.clear();
        fflush(stdin);
        //Next process
        cout<<" "<<endl;
        base();
    }
    
}
//------------------------------------------------------------------------------
int main()
{
    //Adaption of Russian language
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");
        
    help();
    
    base();
    
    system("pause");
    return 0;
}

