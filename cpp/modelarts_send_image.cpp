#include <iostream>
#include <string>
#include <vector>
#include "curl/curl.h"
using namespace std;

// reply of the requery
size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream)
{
    string *str = (string*)stream;
    //cout << *str << endl;
    (*str).append((char*)ptr, size*nmemb);
    return size * nmemb;
}

// http POST
CURLcode curl_post_req(string &url, string &params, string &response, string &token)
{
    curl_global_init(CURL_GLOBAL_ALL);
    // init curl
    CURL *curl = curl_easy_init();
    // res code
    CURLcode res;
    string data;
    string header;
    if (curl)
    {
        // set params
        if (token.size() > 10){
            struct curl_httppost *post=NULL;
            struct curl_httppost *last=NULL;
            curl_formadd(&post, &last, CURLFORM_COPYNAME, "images", CURLFORM_FILE, params.c_str(), CURLFORM_END);
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
            struct curl_slist *slist = 0;
            slist = curl_slist_append(slist, token.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        }else{
            struct curl_slist *plist = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, params.size());
        }
        // start req
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, req_reply);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&header);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
        res = curl_easy_perform(curl);
        if (token.size() > 10)
            response = data;
        else
            response = header;
    }
    // release curl
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return res;
}

string SplitString(const string& s, vector<string>& v, const string& c, int loi)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
    return v[loi];
}

string get_token(string &end_point, string &username, string &passward, string &domainname){
    string token_url = "https://" + end_point + "/v3/auth/tokens";
    string jsonStr = "{\
    \"auth\": {\
        \"identity\": {\
        \"methods\": [\
            \"password\"\
        ],\
        \"password\": {\
            \"user\": {\
            \"name\": \""+username+"\",\
            \"password\": \""+passward+"\",\
            \"domain\": {\
                \"name\": \""+domainname+"\"\
            }\
            }\
        }\
        },\
        \"scope\": {\
        \"project\": {\
            \"name\": \"cn-north-1\"\
        }\
        }\
    }\
    }";
    string postResponseStr;
    string token;
    CURLcode res = curl_post_req(token_url, jsonStr, postResponseStr, token);
    if (res != CURLE_OK){
        cerr << "curl_easy_perform() failed: " + string(curl_easy_strerror(res)) << endl;
        return "error";
    } else{
        if (postResponseStr.find("X-Subject-Token") == std::string::npos){
            return "error";
        }
        vector<string> v1;
        string t = SplitString(postResponseStr, v1, "X-Subject-Token: ", 1);
        vector<string> v2;
        token = SplitString(t, v2, "\nX-XSS-Protection", 0);
        token = "x-auth-token: " + token;
        return token;
    }
    return "error";
}

string send_image_post(string &imageStr, string &tokenStr, string &postUrlStr){
    string postResponseStr;
    CURLcode res = curl_post_req(postUrlStr, imageStr, postResponseStr, tokenStr);
    if (res != CURLE_OK){
        cerr << "curl_easy_perform() failed: " + string(curl_easy_strerror(res)) << endl;
        return "error";
    } else
        return postResponseStr;
    return "error";
}

int main(){
    string end_point = "iam.cn-north-1.myhuaweicloud.com";
    string username = "username";
    string passward = "passward";
    string domainname = "domainname";
    string token = get_token(end_point, username, passward, domainname);
    string postUrlStr = "https://...";
    string imageStr = "xxx.jpg";
    string ret = send_image_post(imageStr, token, postUrlStr);
    cout << ret;
    return 0;
}
