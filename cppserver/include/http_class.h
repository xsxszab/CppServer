#ifndef HTTP_CLASS_H
#define HTTP_CLASS_H

#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "http_enums.h"

namespace cppserver_http {

struct CaseInsensitiveCmp {
  bool operator()(const std::string& a, const std::string& b);
};

class HttpRequest {
 public:
  using ptr = std::shared_ptr<HttpRequest>;
  using map_type =
      std::unordered_map<std::string, std::string, CaseInsensitiveCmp>;

  HttpRequest(uint8_t verion = 0x11, bool persist = false);

  HttpMethod GetMethod() const { return method_; }
  uint8_t GetVersion() const { return version_; }
  HttpStatus GetStatus() const { return status_; }
  const std::string& GetPath() const { return path_; }
  const std::string& GetQuery() const { return query_; }
  const std::string& GetFragment() const { return fragment_; }
  const std::string& GetBody() const { return body_; }
  const map_type& GetHeaders() const { return headers_; }
  const map_type& GetParams() const { return params_; }
  const map_type& GetCookies() const { return cookies_; }

  void SetMethod(HttpMethod method) { method_ = method; }
  void SetVersion(uint8_t version) { version_ = version; }
  void SetStatus(HttpStatus status) { status_ = status; }
  void SetPath(const std::string& path) { path_ = path; }
  void SetQuery(const std::string& query) { query_ = query; }
  void SetFragment(const std::string& fragment) { fragment_ = fragment; }
  void SetBody(const std::string& body) { body_ = body; }
  void SetHeaders(const map_type& headers) { headers_ = headers; }
  void SetParams(const map_type& params) { params_ = params; }
  void SetCookies(const map_type& cookies) { cookies_ = cookies; }

  std::string& GetHeader(const std::string& key,
                         const std::string& default_str = "") const;
  std::string& GetParam(const std::string& key,
                        const std::string& default_str = "") const;
  std::string& GetCookie(const std::string& key,
                         const std::string& default_str = "") const;

  void SetHeader(const std::string& key, const std::string& value);
  void SetParam(const std::string& key, const std::string& value);
  void SetCookie(const std::string& key, const std::string& value);

  void DelHeader(const std::string& key);
  void DelParam(const std::string& key);
  void DelCookie(const std::string& key);

  bool HasHeader(const std::string& key);
  bool HasParam(const std::string& key);
  bool HasCookie(const std::string& key);

 private:
  HttpMethod method_;
  uint8_t version_;
  HttpStatus status_;

  std::string path_;
  std::string query_;
  std::string fragment_;
  std::string body_;

  map_type headers_;
  map_type params_;
  map_type cookies_;
  bool is_persistent_;  // for persistent connection
};

class HttpParser {};

}  // namespace cppserver_http

#endif