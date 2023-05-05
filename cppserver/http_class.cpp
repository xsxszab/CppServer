#include "http_class.h"

#include <sstream>

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

void HttpParser::HandleRequest() {
  auto parse_result = ParseRequest();
  if (PARSE_ERROR == parse_result) {
    HandleError(400, "Bad Request");
  }

  // TODO: handle http error
  auto state = AnalysisRequest();
  if (ANALYSIS_SUCCESS == state) {
  } else {
  }
}

void HttpParser::HandleClose() {}

// TODO: check correctness
ParseResult HttpParser::ParseRequest() {
  cppserver_core::Buffer& buf = *connection_->GetReadBuffer();
  if (0 == buf.Size()) {
    error_ = true;
    return PARSE_ERROR;
  }

  if (STATE_PARSE_URI == state_) {
    URIState uri_state = ParseURI();
    if (PARSE_URI_ERROR) {
      error_ = true;
      return PARSE_ERROR;
    } else {  // parse uri success
      state_ = STATE_PARSE_HEADERS;
    }

  } else if (STATE_PARSE_HEADERS == state_) {
    HeaderState header_state = ParseHeaders();
    if (PARSE_HEADER_ERROR == header_state) {
      error_ = true;
      return PARSE_ERROR;
    }

    if (METHOD_POST == method_) {
      state_ = STATE_RECV_BODY;
    } else {
      state_ = STATE_ANALYSIS;
    }
  }

  if (STATE_RECV_BODY == state_) {
    int content_length = -1;
    if (headers_.find("Content-length") != headers_.end() &&
        stoi(headers_["Content-length"]) <=
            connection_->GetReadBuffer()->Size()) {
    } else {
      error_ = true;
      return PARSE_ERROR;
    }

    state_ = STATE_ANALYSIS;
  }

  if (STATE_ANALYSIS == state_) {
    AnalysisState analysis_state = AnalysisRequest();
    if (ANALYSIS_SUCCESS == analysis_state) {
      state_ = STATE_FINISH;
    } else {
      error_ = true;
      return PARSE_ERROR;
    }
  }

  return PARSE_SUCCESS;
}

AnalysisState HttpParser::AnalysisRequest() { return ANALYSIS_SUCCESS; }

void HttpParser::HandleError(int error_num, std::string msg) {
  std::stringstream response_header;
  std::stringstream response_body;
  msg = " " + msg;

  response_body << "<html><title>something is wrong</title>";
  response_body << "<body bgcolor=\"ffffff\">";
  response_body << error_num << msg;
  response_body << "<hr><em> CppServer</em>\n</body></html>";
  auto body = response_body.str();

  response_header << "HTTP/1.1 " << error_num << msg << "\r\n";
  response_header << "Content-Type: text/html\r\n";
  response_header << "Connection: Close\r\n";
  response_header << "Content-Length: " << body.size() << "\r\n";
  response_header << "Server: LinYa's Web Server\r\n";
  response_header << "\r\n";
  auto header = response_header.str();

  header += body;

  connection_->SetWriteBuffer(header.c_str());
  connection_->Write();
}

// void HttpManager::HandleConnection(
//     std::shared_ptr<cppserver_core::Connection> conn) {
//   if (http_connections_.find(conn) != http_connections_.end()) {
//   } else {
//     RegisterConn(conn);
//   }
// }

void HttpManager::OnConnection(
    std::shared_ptr<cppserver_core::Connection> conn) {
  RegisterConn(conn);
}

void HttpManager::OnMessage(std::shared_ptr<cppserver_core::Connection> conn) {
  if (http_connections_.find(conn) != http_connections_.end()) {
    http_connections_[conn]->HandleRequest();
  }
  std::cout << "cannot find corresponding http parser" << std::endl;
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