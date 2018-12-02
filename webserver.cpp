/*
  A trivial http webserver using Libevent's evhttp.
  This is not the best code in the world, and it does some fairly stupid stuff
  that you would never want to do in a production webserver. Caveat hackor!
 */

#include <memory>
#include <cstdint>
#include <iostream>
#include <evhttp.h>
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
    evbuffer_add_printf(OutBuf, "<html><body><center><h1>Hello World!</h1></center></body></html>");
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