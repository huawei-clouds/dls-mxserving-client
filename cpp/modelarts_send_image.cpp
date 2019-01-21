#include <iostream>
#include <string>
#include "curl/curl.h"
using namespace std;

// reply of the requery
size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream)
{
    string *str = (string*)stream;
    cout << *str << endl;
    (*str).append((char*)ptr, size*nmemb);
    return size * nmemb;
}

// http POST
CURLcode curl_post_req(string &url, string &imageStr, string &response, string &token)
{
    // init curl
    CURL *curl = curl_easy_init();
    // res code
    CURLcode res;
    if (curl)
    {
        // set params
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url
        struct curl_httppost *post=NULL;
        struct curl_httppost *last=NULL;
        // images
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "images", CURLFORM_FILE, imageStr.c_str(), CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
        // response
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
        // set token
        struct curl_slist *slist = 0;
        slist = curl_slist_append(slist, token.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        // start req
        res = curl_easy_perform(curl);
    }
    // release curl
    curl_easy_cleanup(curl);
    return res;
}

// send images
string send_image_post(string &imageStr, string &tokenStr, string &postUrlStr){
    curl_global_init(CURL_GLOBAL_ALL);
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
    string postUrlStr = "http://...";
    string imageStr = "xxx.jpg";
    string token = "X-Auth-Token: token";
    string ret = send_image_post(imageStr, token, postUrlStr);
    cout << ret;
    return 0;
}
