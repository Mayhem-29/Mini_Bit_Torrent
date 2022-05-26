#include<iostream>
#include<bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
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
#include <errno.h>
#include <string.h>
#include<fstream>
#include<future>

using namespace std;

string total;
string ip="";
string port="";
vector<thread> eachclient;

unordered_map<string,string>user_to_pass;
unordered_map<string,string>socket_to_user;
unordered_map<string,vector<string>>group;
unordered_map<string,vector<string>>requests;
unordered_map<string,unordered_map<string,vector<string>>>grouptofile;
unordered_map<string,unordered_map<string,string>>filehash;
unordered_map<string,unordered_map<string,vector<string>>>filesize;


set<string>online;

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

string createid(vector<string>&tokens)
{
    string to_send;
    if(user_to_pass.find(tokens[1])!=user_to_pass.end())
    {
        to_send="Oops! that id is already taken.";

    }
    else
    {
        user_to_pass[tokens[1]]=tokens[2];
        to_send="Id created suceesfully";

    }
    return to_send;
}



string login(vector<string>&tokens,int csock)
{
     string to_send;

     if(user_to_pass.find(tokens[1])==user_to_pass.end())
     {
         to_send="User doesnot exist";
     }
     else
     {
        if(tokens[2]!=user_to_pass[tokens[1]])
        {
            to_send="Wrong password!!";
        }
        else if(online.find(tokens[1])!=online.end())
        {
            to_send="You are already logged in";
        }
        else
        {
            online.insert(tokens[1]);
            string soc=to_string(csock);
            socket_to_user[soc]=tokens[1];
            to_send="You have successfully logged in";
        }

     }

   return to_send;
}



string logout(vector<string>&tokens,int csock)
{

    string to_send;
    string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
       
        to_send="You are already logged out";
    }
   else
   {
     online.erase(socket_to_user[soc]);
     socket_to_user.erase(soc);
     to_send="Successfully Logged out";

   }
   

   return to_send;
}



string creategroup(vector<string>&tokens,int csock)
{
    string to_send;
     string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }
    else
    {
        if(group.find(tokens[1])!=group.end())
        {
          to_send="Sorry this group already exists";
        }

        else
        {
            group.insert(pair<string,vector<string>>(tokens[1],{socket_to_user[soc]}));
            requests.insert(pair<string,vector<string>>(tokens[1],{}));
            to_send="Group successfully created";
        }
    }
    return to_send;
}



string joingroup(vector<string>&tokens,int csock)
{
     string to_send;
     string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    {
        if(group.find(tokens[1])==group.end())
        {
            to_send="Sorry!No such Group exists";
        }
        else
        {
            vector<string>::iterator it;
            it=find(group[tokens[1]].begin(),group[tokens[1]].end(),socket_to_user[soc]);
            if(it!=group[tokens[1]].end())
            to_send="You are already part of this group";
            else
            {
                requests[tokens[1]].push_back(socket_to_user[soc]);
                to_send="Your request to join group has been sent. Wait until admin approves your request";
            }
        }
    }

    return to_send;

}






string leavegroup(vector<string>&tokens,int csock)
{
     string to_send;
     string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    {
        if(group.find(tokens[1])==group.end())
        {
            to_send="Sorry!No such Group exists";
        }
        else
        {
            vector<string>::iterator it;
            it=find(group[tokens[1]].begin(),group[tokens[1]].end(),socket_to_user[soc]);
            if(it==group[tokens[1]].end())
            to_send="Sorry you are not part of this group";
            else
            {
               // group[tokens[1]].erase(std::remove(group[tokens[1]].begin(), group[tokens[1]].end(), socket_to_user[soc]), group[tokens[1]].end());

                group[tokens[1]].erase(it); 
                to_send="You have been successfully removed from the group";
            }
        }
    }

    return to_send;

}

string listrequests(vector<string>&tokens,int csock)
{
    string to_send;
     string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    {
        if(group.find(tokens[1])==group.end())
        {
            to_send="Sorry!No such Group exists";
        }

        else
        {
            if(group[tokens[1]][0]!=socket_to_user[soc])
            to_send="Sorry, you are not admin of this group";

            else
            {
                if(requests[tokens[1]].size()==0)
                to_send="No pending requests here";
                
                else
                {
                    int i;
                for(i=0;i<requests[tokens[1]].size();i++)
                {
                    to_send+=requests[tokens[1]][i];
                    to_send+='\n';
                }
                }
            }
        }
}

return to_send;

}





string acceptrequest(vector<string>&tokens,int csock)
{
    string to_send;
     string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    {
        if(group.find(tokens[1])==group.end())
        {
            to_send="Sorry!No such Group exists";
        }

        else
        {
            if(group[tokens[1]][0]!=socket_to_user[soc])
            to_send="Sorry, you are not admin of this group";

            else
            {
              vector<string>::iterator it;
            it=find(requests[tokens[1]].begin(),requests[tokens[1]].end(),tokens[2]);
            if(it==requests[tokens[1]].end())
            to_send="Sorry no such requests exists.";
            else
            {
               // group[tokens[1]].erase(std::remove(group[tokens[1]].begin(), group[tokens[1]].end(), socket_to_user[soc]), group[tokens[1]].end());

                requests[tokens[1]].erase(it);
                group[tokens[1]].push_back(tokens[2]);

                to_send="above user has successfully been added to group";
            }
            }
        }
    }

    return to_send;

}


string listgroup(vector<string>&tokens,int csock)
{
    string to_send;
     string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    {
        if(group.size()==0)
        {
            to_send="Sorry there are no groups here";
        }
        else
        {
           // unordered_map<string,vector<string>>::iterator it;
            for(auto it=group.begin();it!=group.end();it++)
            {
                to_send+=it->first;
                to_send+='\n';
            }
        }
    }

    return to_send;
}

string upload(vector<string>&tokens,int csock)
{
    string to_send;
     string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    { 
        if(group.find(tokens[1])==group.end())
        {
            to_send="Sorry!No such Group exists";
        }
        else
        {
          /*   vector<string>::iterator it;
             it=find(group[tokens[1]].begin(),group[tokens[1]].end(),socket_to_user[soc]);
             if(it==group[tokens[1]].end())
             to_send="Sorry you are not part of this group";
            else
            {  */
               if(filehash[tokens[1]].find(tokens[2])==filehash[tokens[1]].end())
               {
                   filehash[tokens[1]][tokens[2]]=tokens[7];
                   filesize[tokens[1]][tokens[2]].push_back(tokens[5]);
                   filesize[tokens[1]][tokens[2]].push_back(tokens[6]);
                   grouptofile[tokens[1]][tokens[2]].push_back(tokens[4]);

                   to_send="File Successfully uploaded.";

               }

               else
               {
                    vector<string>::iterator it;
                    it=find(grouptofile[tokens[1]][tokens[2]].begin(),grouptofile[tokens[1]][tokens[2]].end(),tokens[4]);
                    if(it!=grouptofile[tokens[1]][tokens[2]].end())
                    {
                        to_send="File already shared";
                    }

                    else
                    {
                        grouptofile[tokens[1]][tokens[2]].push_back(tokens[4]);
                        to_send="File Successfully uploaded.";
                    }
               }


            
        }

    }

    return to_send;
}




string stop(vector<string>&tokens,int csock)
{
     string to_send;
     string soc=to_string(csock);
    // string por=to_string(cport);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    { 
        if(group.find(tokens[1])==group.end())
        {
            to_send="Sorry!No such Group exists";
        }
        else
        {
             vector<string>::iterator it;
             it=find(group[tokens[1]].begin(),group[tokens[1]].end(),socket_to_user[soc]);
             if(it==group[tokens[1]].end())
             to_send="Sorry you are not part of this group";
            else
            {

               if(filehash[tokens[1]].find(tokens[2])==filehash[tokens[1]].end())
               { 
                   to_send="No such file exists";

               }

               else
               {
                     vector<string>::iterator it;
                    // cout<<por;
                    it=find(grouptofile[tokens[1]][tokens[2]].begin(),grouptofile[tokens[1]][tokens[2]].end(),tokens[3]);
                    if(it==grouptofile[tokens[1]][tokens[2]].end())
                    {
                        to_send="You have not shared this file";


                        /*
                       to_send+=por;
                       to_send+=' ';
                       to_send+="mayur";
                       */

                    }
                    else
                    {
                        grouptofile[tokens[1]][tokens[2]].erase(it);
                        if(grouptofile[tokens[1]][tokens[2]].size()==0)
                        {
                           grouptofile[tokens[1]].erase(tokens[2]);
                           filehash.erase(tokens[2]);
                           filesize.erase(tokens[2]);
                        }

                        to_send="Deleted the shared file successfully";
                    }
               }

            }
        }
    }

   return to_send;
}



string listfiles(vector<string>&tokens,int csock)
{
     string to_send;
     string soc=to_string(csock);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    {
        if(group.find(tokens[1])==group.end())
        {
            to_send="Sorry!No such Group exists";
        }
        else
        {
            if(filehash[tokens[1]].size()==0)
            {
                to_send="No files here.";
            }

            else
            {

            for(auto it=filehash[tokens[1]].begin();it!=filehash[tokens[1]].end();it++)
            {
                to_send+=it->first;
                to_send+='\n';

            }

            }

        }
    }


   return to_send;
}



string op()
{

    string to_send="";
    for(auto it=grouptofile["ag"]["aa.mp4"].begin();it!=grouptofile["ag"]["aa.mp4"].end();it++)
    {
        to_send+=(*it);
        to_send+=" ";

    }
    to_send+='\n';

    for(auto it=grouptofile["ag"]["justincase.sql"].begin();it!=grouptofile["ag"]["justincase.sql"].end();it++)
    {
        to_send+=(*it);
        to_send+=" ";

    }
    to_send+='\n';

    return to_send;
}




string download(vector<string>&tokens,int csock)
{
     string to_send;
     string soc=to_string(csock);
    // string por=to_string(cport);
    if(socket_to_user.find(soc)==socket_to_user.end())
    {
        to_send="Please Login first";
    }

    else
    { 
        if(group.find(tokens[1])==group.end())
        {
            to_send="Sorry!No such Group exists";
        }
        else
        {
             vector<string>::iterator it;
             it=find(group[tokens[1]].begin(),group[tokens[1]].end(),socket_to_user[soc]);
             if(it==group[tokens[1]].end())
             to_send="Sorry you are not part of this group";
            else
            {

               if(filehash[tokens[1]].find(tokens[2])==filehash[tokens[1]].end())
               { 
                   to_send="No such file exists";

               }

               else
               {
                   to_send+=tokens[2];
                   to_send+=" ";
                   to_send+=filesize[tokens[1]][tokens[2]][0];
                   to_send+=" ";
                   to_send+=filesize[tokens[1]][tokens[2]][1];
                   to_send+=" ";
                   to_send+=filehash[tokens[1]][tokens[2]];
                   to_send+=" ";
                   for(auto it=grouptofile[tokens[1]][tokens[2]].begin();it!=grouptofile[tokens[1]][tokens[2]].end();it++)
                    {
                        to_send+=(*it);
                        to_send+=" ";

                    }

                   grouptofile[tokens[1]][tokens[2]].push_back(tokens[4]);
                    



               }
            
            }

        }

    }

    return to_send;

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
    vector<string>tokens=parse(m,' ');
    if(tokens[0]=="create_user")
    {
       string to_send= createid(tokens);
       write(csock,to_send.c_str(),strlen(to_send.c_str()));
    }

    else if(tokens[0]=="login")
    {
        string to_send=login(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="logout")
    {
        string to_send= logout(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));
        

    }

    else if(tokens[0]=="create_group")
    {
        string to_send=creategroup(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="join_group")
    {
        string to_send=joingroup(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="leave_group")
    {
        string to_send=leavegroup(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="list_requests")
    {
        string to_send=listrequests(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="accept_request")
    {
        string to_send=acceptrequest(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="list_groups")
    {
        string to_send=listgroup(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="upload")
    {
        string to_send=upload(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="stop_share")
    {
         string to_send=stop(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    else if(tokens[0]=="list_files")
    {
         string to_send=listfiles(tokens,csock);
        write(csock,to_send.c_str(),strlen(to_send.c_str()));
    }

    else if(tokens[0]=="download_file")
    {
        string to_send=download(tokens,csock);
         write(csock,to_send.c_str(),strlen(to_send.c_str()));
        
    }





    /*
    else if(tokens[0]=="list")
    {
         string to_send=op();
        write(csock,to_send.c_str(),strlen(to_send.c_str()));
    
    } */

    

    else 
    {
        string to_send="Wrong Command";
        write(csock,to_send.c_str(),strlen(to_send.c_str()));

    }

    


    }
    close(csock);
    return;
}



void quit()
{
    while(1)
    {
        string inp;
        getline(cin,inp);
        {
            if(inp=="quit")
            {
                exit(0);
            }
        }
    }
}







int main(int argc,char** argv)
{
   string filename=argv[1];
   string tp;
  // cout<<filename;
   fstream newfile;
   newfile.open(filename,ios::in);
   if(newfile.is_open())
   {
       //cout<<"open";
       getline(newfile,tp);
       newfile.close();
   }

   int i=0;

   while(tp[i]!=':' && i<tp.length())
   {
       
      ip+=tp[i];
      i++;
   }
   i++;
   while(i<tp.length())
   {
      port.push_back(tp[i]);
      i++;
   }

    int trackersocket;
    int clsocket;

    struct sockaddr_in my;
    struct sockaddr_in client;
    int o=1;

     thread hear(quit);
      hear.detach();
  /*
    promise<int>p;
    auto f=p.get_future();
    thread hear (&func,move(p));
    hear.join();
    if(f.get()==1)
    {
        exit(0);
    }  */


    if((trackersocket=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("can't obtain socket for tracker");
        exit(EXIT_FAILURE);
    }

    my.sin_addr.s_addr = inet_addr(ip.c_str());
    my.sin_port=htons(stoi(port));
    my.sin_family=AF_INET;
    bzero(&(my.sin_zero),8);


    /* if (setsockopt(trackersocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &o, sizeof(o))) { 
        perror("reusalbity error"); 
        exit(EXIT_FAILURE); 
    }  */
 
    if(bind(trackersocket,(struct sockaddr*)&my,sizeof(struct sockaddr))==-1)

    {
        perror("could not bind");
       exit(EXIT_FAILURE);
    };

    if(listen(trackersocket,5)<0)
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
        message = "Hello Client , I have received your connection.";


        /*
        message+= cip;
        message+='\n';
        message+=to_string(cport);
         message+='\n';

        message+=to_string(clsocket);
         message+='\n';
         */


		write(clsocket , message.c_str() , strlen(message.c_str()));

        //create new thread for each client

       // thread temp(handler,clsocket,cip,cport);
        eachclient.push_back(thread(handler,clsocket,cip,cport));

    }
    i=0;
    for(i=0;i<eachclient.size();i++)
    {
        if(eachclient[i].joinable())
        eachclient[i].join();
    }
   return 0;
}