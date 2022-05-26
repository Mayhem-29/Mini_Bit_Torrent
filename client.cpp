#include<iostream>
#include<bits/stdc++.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include<math.h>
#include<cstring>
#include <unistd.h>
#include <openssl/sha.h>
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include<fstream>
#include<sys/stat.h>
#include<random>
#include<ctime>

using namespace std;

string total;
string tip="";
string tport="";
string myip="";
string myport="";
unordered_map<string,string>nametopath;
unordered_map<string,vector<int>>availablity;
vector<thread>eachclient;
bool iamonline;
vector<thread>dlod;
vector<pair<string,string>>downloadlist;
string logfile;

unordered_map<string,unordered_map<string,vector<int>>>peerdata;



void logentry(const string &text ){
    ofstream log_file(logfile, ios_base::out | ios_base::app );
    log_file << text << endl;
}




vector<string> parse(string s,char c)
{
     vector<string> res;
  int len = s.length();
  int i = 0;
  string temp = "";
  while (i != len)
  {
    if (s[i] != c)
    {
      temp.push_back(s[i]);
      i++;
    }

    else
    {
      res.push_back(temp);
      temp = "";
      i++;
    }
  }
  res.push_back(temp);

  return res;          
}



string chunksha(char* data,int datasize)
{
    //unsigned char str[] = (unsigned char*) data;
  unsigned char hash[20]; // == 20
  SHA1((unsigned char *)data, datasize, hash);

  int i = 0;
  char temp[40];
    for (i=0; i < 20; i++) 
    sprintf((char*)&(temp[i*2]), "%02x", hash[i]);

    string res="";

    for(i=0;i<20;i++)
    {
       res+=temp[i];
    }
    return res;
}







string hash_convert(string path)
{
    string ans="";
    int len=path.length()-1;
    int ii=len;
    string fname="";
    while(path[ii]!='/'&& ii>=0)
    {
        fname+=path[ii];
        ii--;
    }

    reverse(fname.begin(), fname.end());
     nametopath[fname]=path;
    // logentry("file is "+fname+" mayur");
    // logentry("path is "+path+" mayur");

   
    
    ans+=fname;
    ans+=' ';
    ans+=myip;
    ans+=' ';
    ans+=myport;
    ans+=' ';
    
    struct stat fileinfo;

    if((stat(path.c_str(), &fileinfo))==-1)
    return "ERROR";

    long int filesize=fileinfo.st_size;
    string fsize=to_string(filesize);
    ans+=fsize;
    ans+=" ";


  
    long int totalchunks=filesize/(1024*512);
    int flag=0;
     
    if(filesize%(1024*512)!=0)
    {
       totalchunks++;
       flag=1;
    }
    ans+=to_string(totalchunks);
    ans+=" ";

    
    ifstream ofile(path.c_str(),ifstream::binary);
    long int i=0;
    string chash="";
    while(i<totalchunks-1)
    {
        char* data=new char[1024*512];
        ofile.read(data,512*1024);
        int standard=524288;
        string res=chunksha(data,standard);
        chash+=res;
        //ans+="$$";
        i++;


    }

    if(flag==1)
    {
        int last=filesize%(1024*512);
        char* data=new char[last];
        ofile.read(data,last);
        string res=chunksha(data,last);
        chash+=res;
       
    }

    else
    {
        char* data=new char[1024*512];
        ofile.read(data,512*1024);
        int standard=524288;
        string res=chunksha(data,standard);
        chash+=res;
    }


    char *cstr = new char[chash.length() + 1];
    strcpy(cstr, chash.c_str());
    string fhash=chunksha(cstr,chash.length());

    ans+=fhash;


    return ans;
    

}


string hashcheck(string path)
{
   
    
    struct stat fileinfo;

    if((stat(path.c_str(), &fileinfo))==-1)
    return "ERROR";
    long int filesize=fileinfo.st_size;
    long int totalchunks=filesize/(1024*512);
    int flag=0;
     
    if(filesize%(1024*512)!=0)
    {
       totalchunks++;
       flag=1;
    }
    

    
    ifstream ofile(path.c_str(),ifstream::binary);
    long int i=0;
    string chash="";
    while(i<totalchunks-1)
    {
        char* data=new char[1024*512];
        ofile.read(data,512*1024);
        int standard=524288;
        string res=chunksha(data,standard);
        chash+=res;
        //ans+="$$";
        i++;


    }

    if(flag==1)
    {
        int last=filesize%(1024*512);
        char* data=new char[last];
        ofile.read(data,last);
        string res=chunksha(data,last);
        chash+=res;
       
    }

    else
    {
        char* data=new char[1024*512];
        ofile.read(data,512*1024);
        int standard=524288;
        string res=chunksha(data,standard);
        chash+=res;
    }


    char *cstr = new char[chash.length() + 1];
    strcpy(cstr, chash.c_str());
    string fhash=chunksha(cstr,chash.length());

    return fhash;
    

}




void handler(int csock,string cip,int cport)
{
    while(1)
    {
    char msg[20000];
    memset(msg, 0, sizeof(msg));
    if(read(csock,msg,20000)<=0)
    {
        break;
    }

   // read(csock,msg,20000);
    string m=string(msg);
    logentry("received request in form: "+m);
    vector<string>tokens=parse(m,' ');

    if(tokens[0]=="show")
    {
        string to_send="";
       // string name=nametopath[tokens[1]];
        int i;
        if(iamonline==true && availablity.find(tokens[1])!=availablity.end())
        {
        for(i=0;i<availablity[tokens[1]].size();i++)
        {
            char ci='0'+availablity[tokens[1]][i];
            to_send+=ci;
        }

         write(csock,to_send.c_str(),strlen(to_send.c_str()));

         logentry("sent to the info vector for file "+tokens[1]+" : "+to_send);
        }
        else
        {
            to_send="NA";
            write(csock,to_send.c_str(),strlen(to_send.c_str()));
            logentry("sent NA because either i am offline or file is not there.");

        }


    }

    if(tokens[0]=="give")
    {
        char data[524288];
        memset(data, 0, sizeof(data));
        string s=nametopath[tokens[1]];
        ifstream readstream(s.c_str(),ios::in|ios::binary);
        long long start=stoll(tokens[2])* 524288;
        //logentry("file opened at ocation"+s+" at position"+to_string(start));
        readstream.seekg(start, readstream.beg); 
        readstream.read(data, sizeof(data));
        //int count=readstream.gcount();
        //string last=to_string(count);
        // write(csock,last.c_str(),strlen(last.c_str()));

         int rc= write(csock, data, readstream.gcount());

        //logentry("piece sent "+tokens[2]+" size is "+to_string(readstream.gcount())+" "+to_string(rc));

        readstream.close();
         logentry("For file "+tokens[1]+" and piece number: "+tokens[2]+"data sent successfully");



        

    }
    

  


////////////////////////////if else thing goes here///////////////////////////////////////////




     }
    close(csock);
    return;
}





void iamserver()
{

int trackersocket;
    int clsocket;

    struct sockaddr_in my;
    struct sockaddr_in client;
    int o=1;

    if((trackersocket=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("can't obtain socket for tracker");
        exit(EXIT_FAILURE);
    }
/*
    if (setsockopt(trackersocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &o, sizeof(o))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    }   */

    my.sin_addr.s_addr = inet_addr(myip.c_str());
    my.sin_port=htons(stoi(myport));
    my.sin_family=AF_INET;
    bzero(&(my.sin_zero),8);

    if(bind(trackersocket,(struct sockaddr*)&my,sizeof(struct sockaddr))==-1)

    {
        perror("could not bind");
       exit(EXIT_FAILURE);
    };

    if(listen(trackersocket,30000)<0)
    {
        perror("could not listen");
       exit(EXIT_FAILURE);
    }
    int c=sizeof(struct sockaddr_in);
    while((clsocket=accept(trackersocket,(struct sockaddr *)&client,(socklen_t *)&c))!=-1) //clsocket is socket of client
    {
        string cip=inet_ntoa(client.sin_addr);   //cip is ip of client
        int cport=(ntohs(client.sin_port));      //cport is port of client
        string message;
        message = "Hello Client , I have received your connection \n";


        /*
        message+= cip;
        message+='\n';
        message+=to_string(cport);
         message+='\n';

        message+=to_string(clsocket);
         message+='\n';
         */


	//	write(clsocket , message.c_str() , strlen(message.c_str()));

        //create new thread for each client

       // thread temp(handler,clsocket,cip,cport);
        eachclient.push_back(thread(handler,clsocket,cip,cport));


}
int i=0;
    for(i=0;i<eachclient.size();i++)
    {
        if(eachclient[i].joinable())
        eachclient[i].join();
    }
   return;

}

void contactseeder(string s)
{
   vector<string>tokens=parse(s,' ');
   if(tokens[0]=="show")
   {
       int peersock;
       struct sockaddr_in peeradd;

       if((peersock=socket(AF_INET,SOCK_STREAM,0))<0)
      {
       perror("unable to createsocket");
       exit(EXIT_FAILURE);
      }
    
       logentry("socket acquired for"+tokens[2]);

    string ipaddress="127.0.0.1";

    peeradd.sin_addr.s_addr = inet_addr(ipaddress.c_str());
    peeradd.sin_port=htons(stoi(tokens[2]));
    peeradd.sin_family=AF_INET;
    bzero(&(peeradd.sin_zero),8);

    if(connect(peersock,(struct sockaddr*)&peeradd,sizeof(struct sockaddr))==-1)
    {
       perror("unable to conttect to tracker");
       exit(EXIT_FAILURE);
    }

    logentry("connected to"+tokens[2]);

    string toserver="show";
    toserver+=" ";
    toserver+=tokens[1];

    logentry("sent info to"+tokens[2]+" : "+toserver);

        send(peersock,toserver.c_str(),strlen(toserver.c_str()),MSG_NOSIGNAL);
        char reply[20000];
        memset(reply, 0, sizeof(reply));
        read(peersock ,reply, 20000); 
        string r=string(reply);
        int i;
        if(r!="NA")
        {
        
        for(i=0;i<r.length();i++)
        {
            peerdata[tokens[1]][tokens[2]].push_back((r[i]-'0'));
        }
        }
        

        logentry(tokens[2]+" replied with: "+r);

     logentry("Closed connection with "+tokens[2]);
     close(peersock);

   }


   else
   {
       int peersock;
       struct sockaddr_in peeradd;

       if((peersock=socket(AF_INET,SOCK_STREAM,0))<0)
      {
       perror("unable to createsocket");
       exit(EXIT_FAILURE);
      }
    
       //logentry("socket acquired for"+tokens[3]);

    string ipaddress="127.0.0.1";

    peeradd.sin_addr.s_addr = inet_addr(ipaddress.c_str());
    peeradd.sin_port=htons(stoi(tokens[3]));
    peeradd.sin_family=AF_INET;
    bzero(&(peeradd.sin_zero),8);

    if(connect(peersock,(struct sockaddr*)&peeradd,sizeof(struct sockaddr))==-1)
    {
       perror("unable to connect to tracker");
       exit(EXIT_FAILURE);
    }

   // logentry("connected to"+tokens[3]);

    string toserver="give";
    toserver+=" ";
    toserver+=tokens[1];
    toserver+=" ";
    toserver+=tokens[2];

    logentry("sent to info to "+tokens[3]+" : "+toserver);

        send(peersock,toserver.c_str(),strlen(toserver.c_str()),MSG_NOSIGNAL);
        logentry("sent to "+tokens[3]+" "+toserver);
        char reply[20000];
        memset(reply, 0, sizeof(reply));
        char data[524288];
      memset(data, 0, sizeof(data));


       

       // read(peersock ,reply, 20000); 
       // string r=string(reply);
       // int rsize=stoi(r);

       string filesz=tokens[5];
       string totalpieces=tokens[6];
       //logentry("total peces and size "+filesz+" "+totalpieces);

       int lastmod=stoll(filesz)%524288;

      int decider=524288;
      if(stoi(tokens[2])==(stoi(totalpieces)-1))
      decider=lastmod;
       int readbytes=0;
       int done=0;
       while (done < decider) {
         memset(data, 0, sizeof(data));
        readbytes=read(peersock, data, 524287);
       // logentry("recieeved from "+tokens[3]+" "+to_string(readbytes)+" bytes"+"for pieces "+tokens[2]);
        if (readbytes<= 0)break;
       // data[readbytes] = 0;
        fstream outfile(tokens[4], fstream::in | fstream::out | fstream::binary);
        int position=done+(stoi(tokens[2])*524288);
       // done=done+readbytes;
        outfile.seekp(position, ios::beg);
        data[readbytes] = 0;
        outfile.write(data, readbytes);
        outfile.close();
        done=done+readbytes;
       // logentry("acquired piece no. "+tokens[2]+" done "+ to_string(done));
        availablity[tokens[1]][stoi(tokens[2])]=1;
       
        
    }
    
        


       

    // logentry("closed connection with"+tokens[2]);
     close(peersock);
      logentry("acquired piece no. "+tokens[2]+" from "+tokens[3]+" successfully and closed the connection.");
   }

}


bool cmp (pair<int,vector<string> >a,pair<int,vector<string> >b)
{
   return a.second.size()<b.second.size();
}


void download(string info)
{
     string to_send="";
     logentry("Info recieved from tracker to download file: "+info);
    vector<string>tokens=parse(info,' ');
    if(tokens.size()==5)
    {
        to_send="No seeders available";
        cout<<to_send<<endl<<endl;
        return;
       // return to_send;
    }

    int i=4;
    vector<string>seeders;
   // availablity.insert({tokens[0],vector<int>(stoi(tokens[2]),0)});
    for(i=4;i<tokens.size()-1;i++)
    {
        if(tokens[i]!=myport)
        seeders.push_back(tokens[i]);
    }

    vector<thread>allseeders;

    for(i=0;i<seeders.size();i++)
    {
        string forward="show";
        forward+=" ";
        forward+=tokens[0];
        forward+=" ";
        forward+=seeders[i];

        logentry("Thread for getting info of "+seeders[i]+" initiated");

        allseeders.push_back(thread(contactseeder,forward));
    }

    for(i=0;i<allseeders.size();i++)
    {
        if(allseeders[i].joinable())
        allseeders[i].join();
    }

    logentry("Successfully recieved info from all seeders.");

    map<int,vector<string>>rarestfirst;

  
   if(peerdata[tokens[0]].size()==0)
   {
      // logentry("not enough peers available");
       cout<<"sorry,enough peers are not available"<<endl<<endl;
       return;
   }

    for(auto it=peerdata[tokens[0]].begin();it!=peerdata[tokens[0]].end();it++)
    {
        for(int j=0;j<it->second.size();j++)
        {
            if(it->second[j]==1)
            {
                rarestfirst[j].push_back(it->first);
            }

        }

    }

   vector<pair<int,vector<string>>>rarest;

    for(auto it=rarestfirst.begin();it!=rarestfirst.end();it++)
    {
      rarest.push_back({it->first,it->second});
    }

    sort(rarest.begin(),rarest.end(),cmp);

    

    for(auto i=rarest.begin();i!=rarest.end();i++)
    {
       if(i->second.size()==0)
       {
           to_send="Sorry whole file is not downloadable";
           cout<<to_send<<endl<<endl;
           return;
           //return to_send;
       }
    }

    


    string outfile=tokens[tokens.size()-1]+"/"+tokens[0];
    string totalfile=tokens[tokens.size()-1]+"/"+tokens[0];

    nametopath[tokens[0]]=totalfile;

    FILE* fp = fopen(outfile.c_str(), "r+");
    if(fp!=0)
    {
       // logentry("alreadyfile");
        cout<<"Above file already exists"<<endl<<endl;
       // logentry("alreadyfile");

        return;
   
    }
	long long fs=stoll(tokens[1]);
    string ss(fs, '\0');
    fstream in(outfile.c_str(),ios::out|ios::binary);
    in.write(ss.c_str(),strlen(ss.c_str()));  
    in.close();

    logentry("Created a dummy file "+totalfile);

    
     availablity.insert({tokens[0],vector<int>(stoi(tokens[2]),0)});


    srand(time(nullptr));
    vector<thread>go_get_pieces;


    for(auto i=rarest.begin();i!=rarest.end();i++)
    {
        int k=i->second.size();
        
       

      int  portnumber=rand()%k;

      string forward="give";
      forward+=" ";
      forward+=tokens[0];
      forward+=" ";
      forward+=to_string(i->first);
      forward+=" ";
      forward+=i->second[portnumber];
      forward+=" ";
      forward+=totalfile;
      /////////////
      
       forward+=" ";
        forward+=tokens[1]; 

        forward+=" ";
         forward+=tokens[2];





      //////////////

      go_get_pieces.push_back(thread(contactseeder,forward));
     // string km=i->first+'0';

      logentry("thread for getting piece "+to_string(i->first)+ "sent to "+i->second[portnumber]+" initiated");
     // logentry("sent atring to contavtseeder"+forward);
      


    }

     for(i=0;i<go_get_pieces.size();i++)
    {
        if(go_get_pieces[i].joinable())
        go_get_pieces[i].join();
    }


    string newhash=hashcheck(totalfile);
    logentry("The hash of original file was: "+tokens[3]);
    logentry("The hash of downloaded file is: "+newhash);
    to_send="file downloaded";
    cout<<to_send<<endl<<endl;
    return;









   




     
    




   // return to_send;
}





int main(int argc,char** argv)
{
     string total=argv[1];
     string filename=argv[2];

     int i=0;
     while(total[i]!=':'&&i<total.length())
     {
         myip+=total[i];
         i++;
     }
     i++;
     while(i<total.length())
     {
         myport+=total[i];
         i++;
     }

     string tp;
  
   fstream newfile;
   newfile.open(filename,ios::in);
   if(newfile.is_open())
   {
       //cout<<"open";
       getline(newfile,tp);
       newfile.close();
   }

    i=0;

   while(tp[i]!=':' && i<tp.length())
   {
       
      tip+=tp[i];
      i++;
   }
   i++;
   while(i<tp.length())
   {
      tport.push_back(tp[i]);
      i++;
   }

   int mysock;
   struct sockaddr_in track;



   logfile = myport + "log.txt";
    ofstream forlog;
    forlog.open(logfile);
    forlog.clear();
    forlog.close();



   //--------------------------
    /*
   struct sockaddr_in mine;
   mine.sin_addr.s_addr=inet_addr(myip.c_str());
   uint16_t por=stoi(myport);
   mine.sin_family=AF_INET;
   mine.sin_port=htons(por);

   */
   //-----------------------------

  thread hear(iamserver);
  hear.detach();


   if((mysock=socket(AF_INET,SOCK_STREAM,0))<0)
   {
       perror("unable to createsocket");
       exit(EXIT_FAILURE);
   }


  /*
  if(bind(mysock,(struct  sockaddr*) &mine,sizeof(struct sockaddr_in))==1)
   {
        perror("could not bind");
       exit(EXIT_FAILURE);
    };

  */


    track.sin_addr.s_addr = inet_addr(tip.c_str());
    track.sin_port=htons(stoi(tport));
    track.sin_family=AF_INET;
    bzero(&(track.sin_zero),8);

    if(connect(mysock,(struct sockaddr*)&track,sizeof(struct sockaddr))==-1)
    {
       perror("unable to connect to tracker");
       exit(EXIT_FAILURE);
    };

         char ack[20000];
         memset(ack, 0, sizeof(ack));
         read(mysock ,ack, 20000); 
         

    cout<<"connected and recieved ack: "+string(ack)<<endl<<endl;
   






   while(1)
   {
      string command;
      getline(cin,command);
      if(command.substr(0,11)=="upload_file")
      {
         vector<string>tokens=parse(command,' ');
         string newcom=hash_convert(tokens[1]);
         if(newcom=="ERROR")
         {
             cout<<"No such file exists"<<endl<<endl;
             continue;
         }
          string toserver="upload ";
          toserver+=tokens[2];
          toserver+=" ";
          toserver+=newcom;

          vector<string>sendtoken=parse(toserver,' ');

          send(mysock,toserver.c_str(),strlen(toserver.c_str()),MSG_NOSIGNAL);
          logentry("Following data sent to tracker for upload: "+toserver );
         char reply[20000];
         memset(reply, 0, sizeof(reply));
        read(mysock ,reply, 20000); 
        string r=string(reply);
        cout<<r<<endl<<endl;

        if(r=="File Successfully uploaded.")
        {
           // vector<int>parts(sendtoken[6],1);
           availablity.insert({sendtoken[2],vector<int>(stoi(sendtoken[6]),1)});
        }


      }

      else if(command.substr(0,13)=="download_file")
      {
         vector<string>tokens=parse(command,' ');
         struct stat fileinfo;

         if((stat(tokens[3].c_str(), &fileinfo))==-1)
         {
           cout<<"The given destination path doesn't exist"<<endl<<endl;
         }

         else
         {
             command+=" ";
             command+=myport;
            send(mysock,command.c_str(),strlen(command.c_str()),MSG_NOSIGNAL);
            char reply[20000];
            memset(reply, 0, sizeof(reply));
            read(mysock ,reply, 20000); 
            string r=string(reply);

            //r+=" ";
            r+=tokens[3];
           // availablity.insert({sendtoken[2],vector<int>(stoi(sendtoken[6]),1)});

          //  cout<<download(r)<<endl;
            dlod.push_back(thread(download,r));

            downloadlist.push_back({tokens[1],tokens[2]});

          

             



            
         }
      }

      else if(command.substr(0,10)=="stop_share")
      {
        command+=" ";
        command+=myport;

            send(mysock,command.c_str(),strlen(command.c_str()),MSG_NOSIGNAL);
            char reply[20000];
            memset(reply, 0, sizeof(reply));
            read(mysock ,reply, 20000); 
            string r=string(reply);
            cout<<r<<endl<<endl;


      }

      else if(command.substr(0,5)=="login")
      {
        send(mysock,command.c_str(),strlen(command.c_str()),MSG_NOSIGNAL);
      char reply[20000];
      memset(reply, 0, sizeof(reply));
      read(mysock ,reply, 20000); 
      string r=string(reply);
      
      cout<<r<<endl<<endl;
       if(r=="You have successfully logged in")
       {
           iamonline=true;
       }


      }



      else if(command.substr(0,6)=="logout")
      {
        send(mysock,command.c_str(),strlen(command.c_str()),MSG_NOSIGNAL);
      char reply[20000];
      memset(reply, 0, sizeof(reply));
      read(mysock ,reply, 20000); 
      string r=string(reply);
      
      cout<<r<<endl<<endl;
       if(r=="Successfully Logged out")
       {
           iamonline=false;
       }


      }

      else if(command.substr(0,14)=="show_downloads")
      { 
          string to_screen="";
        
          for(auto it=downloadlist.begin();it!=downloadlist.end();it++)
          {
              if(availablity.find(it->second)!=availablity.end())
              {
                 int flag=0;
                 for(int p=0;p<availablity[it->second].size();p++)
                 {
                     if(availablity[it->second][p]==0)
                     {
                         to_screen+="[D] ["+it->first+"] "+it->second+"\n";
                         flag=1;
                     }
                 }
                 if(flag==0)
                 {
                     to_screen+="[C] ["+it->first+"] "+it->second+"\n";
                 }
              }
          }

          cout<<to_screen<<endl;
      }




      else
      {
      send(mysock,command.c_str(),strlen(command.c_str()),MSG_NOSIGNAL);
      char reply[20000];
      memset(reply, 0, sizeof(reply));
      read(mysock ,reply, 20000); 
      string r=string(reply);
      cout<<r<<endl<<endl;
      }
     // cout<<"done"<<endl;
   }


   for(i=0;i<dlod.size();i++)
    {
        if(dlod[i].joinable())
        dlod[i].join();
    }

   return 0;
}