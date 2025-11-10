#include <Sanctuary/WebAPI/WebServer.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace Sanctuary::WebAPI {

WebServer::WebServer(net::io_context& ioc, uint16_t port, Database::DatabaseContext& db)
    : io_context_(ioc),
      acceptor_(ioc, {net::ip::tcp::v4(), port}),
      database_(db),
      port_(port) {

    spdlog::info("WebServer created on port {}", port);
}

WebServer::~WebServer() {
    stop();
}

void WebServer::start() {
    do_accept();
    spdlog::info("WebServer started on port {}", port_);
}

void WebServer::stop() {
    boost::system::error_code ec;
    acceptor_.close(ec);
    spdlog::info("WebServer stopped");
}

void WebServer::do_accept() {
    // TODO: Implement HTTP accept logic with Boost.Beast
    // This would set up async_accept and handle incoming HTTP requests
}

void WebServer::handle_request(
    http::request<http::string_body> req,
    std::function<void(http::response<http::string_body>)> send) {

    // Route to appropriate handler based on URL
    if (req.target() == "/api/status") {
        send(handle_status());
    } else if (req.target().starts_with("/api/users")) {
        send(handle_users());
    } else if (req.target().starts_with("/api/characters")) {
        send(handle_characters());
    } else {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, "Sanctuary WebAPI");
        res.set(http::field::content_type, "application/json");
        res.body() = R"({"error": "Not found"})";
        res.prepare_payload();
        send(std::move(res));
    }
}

http::response<http::string_body> WebServer::handle_status() {
    nlohmann::json json_response;
    json_response["status"] = "online";
    json_response["version"] = "1.0.0";

    http::response<http::string_body> res{http::status::ok, 11};
    res.set(http::field::server, "Sanctuary WebAPI");
    res.set(http::field::content_type, "application/json");
    res.body() = json_response.dump();
    res.prepare_payload();

    return res;
}

http::response<http::string_body> WebServer::handle_users() {
    nlohmann::json json_response;
    json_response["users"] = nlohmann::json::array();

    http::response<http::string_body> res{http::status::ok, 11};
    res.set(http::field::server, "Sanctuary WebAPI");
    res.set(http::field::content_type, "application/json");
    res.body() = json_response.dump();
    res.prepare_payload();

    return res;
}

http::response<http::string_body> WebServer::handle_characters() {
    nlohmann::json json_response;
    json_response["characters"] = nlohmann::json::array();

    http::response<http::string_body> res{http::status::ok, 11};
    res.set(http::field::server, "Sanctuary WebAPI");
    res.set(http::field::content_type, "application/json");
    res.body() = json_response.dump();
    res.prepare_payload();

    return res;
}

} // namespace Sanctuary::WebAPI
