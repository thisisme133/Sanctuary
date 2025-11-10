#pragma once

#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <Sanctuary/Database/DatabaseContext.h>

namespace Sanctuary::WebAPI {

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

class WebServer {
public:
    WebServer(net::io_context& ioc, uint16_t port, Database::DatabaseContext& db);
    ~WebServer();

    void start();
    void stop();

private:
    net::io_context& io_context_;
    net::ip::tcp::acceptor acceptor_;
    Database::DatabaseContext& database_;
    uint16_t port_;

    void do_accept();
    void handle_request(http::request<http::string_body> req,
                       std::function<void(http::response<http::string_body>)> send);

    // API endpoints
    http::response<http::string_body> handle_status();
    http::response<http::string_body> handle_users();
    http::response<http::string_body> handle_characters();
};

} // namespace Sanctuary::WebAPI
