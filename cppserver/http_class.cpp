#include "http_class.h"

#include "buffer.h"
#include "connection.h"

namespace cppserver_http {

pthread_once_t MimeTypes::once_control_ = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeTypes::mime;

void MimeTypes::Init() {
  mime[".html"] = "text/html";
  mime[".avi"] = "video/x-msvideo";
  mime[".bmp"] = "image/bmp";
  mime[".c"] = "text/plain";
  mime[".doc"] = "application/msword";
  mime[".gif"] = "image/gif";
  mime[".gz"] = "application/x-gzip";
  mime[".htm"] = "text/html";
  mime[".ico"] = "image/x-icon";
  mime[".jpg"] = "image/jpeg";
  mime[".png"] = "image/png";
  mime[".txt"] = "text/plain";
  mime[".mp3"] = "audio/mp3";
  mime["default"] = "text/html";
}

std::string MimeTypes::GetMime(const std::string& suffix) {
  pthread_once(&once_control_, MimeTypes::Init);
  if (mime.find(suffix) != mime.end()) {
    return mime[suffix];
  }
  return mime["default"];
}

HttpParser::HttpParser()
    : error_(false),
      connectionState_(H_CONNECTED),
      method_(METHOD_GET),
      HTTPVersion_(HTTP_11),
      nowReadPos_(0),
      state_(STATE_PARSE_URI),
      hState_(H_START),
      keepAlive_(false) {}

HttpParser::~HttpParser() {}

void HttpParser::Reset() {
  fileName_.clear();
  path_.clear();
  nowReadPos_ = 0;
  state_ = STATE_PARSE_URI;
  hState_ = H_START;
  headers_.clear();
}

void HttpManager::HandleConnection(
    std::shared_ptr<cppserver_core::Connection> conn) {
  if (http_connections_.find(conn) != http_connections_.end()) {
  } else {
    RegisterConn(conn);
  }
}

HttpParser::ptr HttpManager::GetParser(
    std::shared_ptr<cppserver_core::Connection> conn) {
  if (http_connections_.find(conn) != http_connections_.end()) {
    return http_connections_[conn];
  }
  return HttpParser::ptr();
}

void HttpManager::RegisterConn(
    std::shared_ptr<cppserver_core::Connection> conn) {
  HttpParser::ptr parser = std::make_shared<HttpParser>();
  http_connections_[conn] = parser;
}

}  // namespace cppserver_http