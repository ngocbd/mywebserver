/*
  This is stupid code. Happy hacking
 */
#include <algorithm>
#include <memory>
#include <cstdint>
#include <iostream>
#include <evhttp.h>
#include <cstdio>
#include <vector>  
#include <stdexcept>
#include <array>
#include <memory>
#include <cstring>
std::string exec(char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}
int main()
{


  if (!event_init())
  {
    std::cerr << "Failed to init libevent." << std::endl;
    return -1;
  }
  char const bindaddress[] = "127.0.0.1";
  std::uint16_t port = 8888;
  std::unique_ptr<evhttp, decltype(&evhttp_free)> Server(evhttp_start(bindaddress, port), &evhttp_free);
  if (!Server)
  {
    std::cerr << "Failed to init web server." << std::endl;
    return -1;
  }
  void (*OnReq)(evhttp_request *req, void *) = [] (evhttp_request *req, void *)
  {
    auto *OutBuf = evhttp_request_get_output_buffer(req);
    if (!OutBuf)
      return;
    char *url = (char *)evhttp_request_uri(req);
    std::cout<<url<<std::endl;
    char filename[200];
    strncpy(filename,url+1,strlen(url));
    std::cout<<"filename :"<<filename<<std::endl;
    char cmd[500];
    strcpy(cmd,"php ");
    strcat(cmd,filename);
    std::string phpOut = exec(cmd);
    evbuffer_add(OutBuf, phpOut.c_str(),phpOut.length());
    evhttp_send_reply(req, HTTP_OK, "", OutBuf);
  };
  evhttp_set_gencb(Server.get(), OnReq, nullptr);
  if (event_dispatch() == -1)
  {
    std::cerr << "Failed to run event loop." << std::endl;
    return -1;
  }
  return 0;
}
