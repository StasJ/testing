#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <microhttpd.h>
#include "vapor/DebugConsole.h"
#include "vapor/FileUtils.h"

#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;
using namespace Wasp;

#define PORT 8888

bool debugConsoleInitialized = false;
map<string, float> vMap;
map<string, string> sMap;
map<string, vector<float>> m4x4s;

bool StringBeginsWith(string s, string test) {
    return s.compare(0, test.length(), test) == 0;
}

int NotFound404(MHD_Connection *connection)
{
    struct MHD_Response *response;
    int ret;
    
    response = MHD_create_response_from_buffer (strlen ("NOT_FOUND_ERROR"),
                                                (void *) "NOT_FOUND_ERROR",
                                                MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response (connection,
                              MHD_HTTP_NOT_FOUND,
                              response);
    MHD_destroy_response (response);
    return ret;
}

int Redirect(MHD_Connection *connection, const char *path) {
    struct MHD_Response *response;
    response = MHD_create_response_from_buffer(strlen("REDIRECT"), (void*)"REDIRECT", MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, "Location", path);
    int ret = MHD_queue_response(connection, MHD_HTTP_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}

int answer_to_connection (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls)
{
    string data;
    string URL(url);
    //printf("%s \"%s\"\n", method, URL.c_str());
    if (URL == "/") {
        return Redirect(connection, "/web/index.html");
    }
    if (URL.substr(0, 5) == "/web/") {
        //URL = URL.substr(5, string::npos);
        URL = "/Users/stas/Desktop" + URL;
        printf("\tPATH \"%s\"\n", URL.c_str());
        if (!FileUtils::FileExists(URL))
            return NotFound404(connection);
        
        data = FileUtils::ReadFileToString(URL);
    }
    else if (StringBeginsWith(URL, "/json/vars")) {
        size_t start = 0;
        while (true) {
            size_t i = URL.find_first_of("&", start);
            if (i == string::npos || i <= start)
                break;
            
            size_t ei = URL.find_first_of("=", start);
            string name = URL.substr(i+1, ei - i-1);
            if (name.length() == 0)
                break;
            string value = URL.substr(ei+1, URL.find_first_of("&", ei) - (ei+1));
            
            //printf("'%s' = '%s'\n", name.c_str(), value.c_str());
            vMap[name] = std::stof(value);
            
            start = i;
        }
        data = "{";
        for (auto it = vMap.begin(); it != vMap.end(); ++it) {
            data += "\"" + it->first + "\"" + ":" + std::to_string(it->second);
            if (it != --vMap.end())
                data += ",";
        }
        data += "}";
    }
    else if (StringBeginsWith(URL, "/json/matrices")) {
        data = "{";
        for (auto it = m4x4s.begin(); it != m4x4s.end(); ++it) {
            data += "\"" + it->first + "\":[";
            for (int i = 0; i < 16; i++) {
                data += std::to_string(it->second[i]);
                if (i != 15)
                    data += ",";
            }
            data += "]";
            if (it != --m4x4s.end())
                data += ",";
        }
        data += "}";
    }
    
    struct MHD_Response *response;
    int ret;
    
    response = MHD_create_response_from_buffer (data.length(),
                                                (void*) data.c_str(), MHD_RESPMEM_MUST_COPY);
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);
    
    return ret;
}

void InitDebugConsole()
{
    if (debugConsoleInitialized)
        return;
    debugConsoleInitialized = true;
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                               &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL)
        printf("FAILED TO INIT\n");
}

void SetFloat(std::string name, float f)
{
    vMap[name] = f;
}

float GetFloat(std::string name, float default_)
{
    auto it = vMap.find(name);
    if (it == vMap.end())
        vMap[name] = default_;
    return vMap[name];
}

void SetBool(std::string name, bool b)
{
    SetFloat(name, b ? 1.f : 0.f);
}

float GetBool(std::string name, bool default_)
{
    return GetFloat(name, default_) != 0.f;
}

void SetString(std::string name, std::string s)
{
    sMap[name] = s;
}

std::string GetString(std::string name, std::string default_)
{
    auto it = sMap.find(name);
    if (it == sMap.end())
        sMap[name] = default_;
    return sMap[name];
}

template <typename T>
void ShowMatrix(string name, T m[16])
{
    if (!debugConsoleInitialized)
        printf("Debug Console not initialized\n");
    
    vector<float> vm;
    for (int i = 0; i < 16; i++)
        vm.push_back(m[i]);
    m4x4s[name] = vm;
}
template void ShowMatrix<float>(std::string name, float m[16]);
template void ShowMatrix<double>(std::string name, double m[16]);
