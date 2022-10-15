#ifndef TWMAILER314_BASIC_GLOBALS_HPP
#define TWMAILER314_BASIC_GLOBALS_HPP

#include <vector>
#include <string>
#include <map>

using namespace std;

vector<string> SEND_PROMPTS = {
        "Sender: ",
        "Receiver: ",
        "Subject: ",
        "Message: (~1000 character. End with dot .)\n> "
};


map<string, string> ERROR_MES = {{"line-too-long", "Line too long. Please, re-enter or end with a dot \".\"\n"}};
map<string, string> PROMPT_MES = {
        {"empty", "> "},
        {"username", "Username: "},
        {"mes-num", "Message-Number: "},
};


#endif //TWMAILER314_BASIC_GLOBALS_HPP
