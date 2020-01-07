//
// Created by tamirsku@wincs.cs.bgu.ac.il on 07/01/2020.
//
#include <stdlib.h>
#include <iostream>
#include "../include/connectionHandler.h"
#include "../include/SplitThings.h"
#include "../include/KeyBoardTask.h"
#include "../include/ReadFromSocketTask.h"
#include <thread>
#include <mutex>
using namespace std;
int main (int argc, char *argv[]) {
    bool loggedIN = false;
    while(!loggedIN){
        string action,subAction;
        std::getline(std::cin, action);
        vector<string> words;
        SplitThings::splitWords(action,words);
        string currWord=words[0];
        if(subAction=="login"){
            loggedIN = true;
            int seperate=words[1].find(':');
            string host=words[1].substr(0,seperate);
            string port=words[1].substr(seperate+1,words[1].length()-1);
            string username=words[2];
            string password=words[3];
            string stomp="CONNECT"+string("\n")+
                  "accept-version:1.2"+string("\n")+
                  "host:stomp.cs.bgu.ac.il"+string("\n")+
                  "login:"+username+string("\n")+
                  "passcode:"+password+string("\n")+"\0";
            ConnectionHandler connectionHandler(host, boost::lexical_cast<short>(port));
            if (!connectionHandler.connect()) {
                std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
                return 1;
            }
            connectionHandler.sendFrameAscii(stomp,'\0');
            mutex sharedMutex;
            User userLogged(username,password);
            StompMsgEncoderDecoder msgEncoderDecoder(userLogged);
            KeyBoardTask keyboardTask(sharedMutex,connectionHandler,msgEncoderDecoder);
            ReadFromSocketTask readFromSocketTask(sharedMutex,connectionHandler);
            thread t1(&KeyBoardTask::run,&keyboardTask);
            thread t2(&ReadFromSocketTask::run,&readFromSocketTask);
            t1.join();
            t2.join();
        }
    }

    return 0;
}
