#ifndef HTTP_CLASS_H
#define HTTP_CLASS_H

#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

namespace cppserver_core {

class Buffer;

}

namespace cppserver_http {

enum LineStatus { LINE_OK = 1, LINE_ERROR = 2, LINE_OPEN = 3 };
enum ParserState { PARSE_REQUESTLINE = 1, PARSE_HEADER = 2, PARSE_CONTENT = 3 };
enum HttpCode {
  NO_REQUEST = 1,
  GET_REQUEST,
  BAD_REQUEST,
  FORBIDDEN_REQUEST,
  INTERNAL_ERROR,
  CLOSED_CONNECTION
};

// class HttpData {
//  public:
//   using ptr = std::shared_ptr<HttpData>;
//  private:
// };

// class HttpParser {
//  public:
//   HttpParser() = default;
// ~HttpParser() = default;
// private:
// ParserState parser_state_;
// };

class HttpParser {
 public:
  HttpParser(std::shared_ptr<cppserver_core::Buffer> buf);
  ~HttpParser() = default;

  HttpCode Parse();

  void SetBuf(std::shared_ptr<cppserver_core::Buffer> buf);

 private:
  LineStatus ParseLine();
  HttpCode ParseRequestLine();
  HttpCode ParseHeader();

  std::shared_ptr<cppserver_core::Buffer> buf_;
  int index_{0};
  ParserState parser_state_{PARSE_REQUESTLINE};
};

}  // namespace cppserver_http

#endif