#include <iostream>
#include <memory>
#include <thread>
#include <csignal>
#include <atomic>
#ifndef NO_CXXOPTS_LIB
#include <cxxopts.hpp>
#endif
#ifndef NO_SPDLOG_LIB
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#endif
#ifndef NO_JSON_LIB
#include <nlohmann/json.hpp>
#endif
#include <boost/asio.hpp>
#include <Sanctuary/Login/LoginServer.h>

using namespace Sanctuary;

std::atomic<bool> g_running{true};

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        spdlog::info("Received shutdown signal");
        g_running = false;
    }
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line options
        cxxopts::Options options("Sanctuary.Login", "Free Realms Login Server");

        options.add_options()
            ("c,config", "Config file path", cxxopts::value<std::string>()->default_value("login_config.json"))
            ("p,port", "UDP port for clients", cxxopts::value<uint16_t>()->default_value("20070"))
            ("g,gateway-port", "UDP port for gateway servers", cxxopts::value<uint16_t>()->default_value("20071"))
            ("d,database", "Database connection string", cxxopts::value<std::string>())
            ("db-type", "Database type (sqlite/mysql)", cxxopts::value<std::string>()->default_value("sqlite"))
            ("v,verbose", "Verbose logging", cxxopts::value<bool>()->default_value("false"))
            ("h,help", "Print help");

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        // Setup logging
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/login.log", 1024 * 1024 * 10, 3);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        auto logger = std::make_shared<spdlog::logger>("login", sinks.begin(), sinks.end());
        spdlog::set_default_logger(logger);

        if (result["verbose"].as<bool>()) {
            spdlog::set_level(spdlog::level::debug);
        } else {
            spdlog::set_level(spdlog::level::info);
        }

        spdlog::info("Sanctuary Login Server starting...");

        // Setup signal handlers
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Create ASIO io_context
        boost::asio::io_context io_context;

        // Create Login server
        auto client_port = result["port"].as<uint16_t>();
        auto gateway_port = result["gateway-port"].as<uint16_t>();

        spdlog::info("Initializing Login Server");
        spdlog::info("Client port: {}", client_port);
        spdlog::info("Gateway port: {}", gateway_port);

        // Initialize database
        std::string db_conn = result.count("database")
            ? result["database"].as<std::string>()
            : "sanctuary.db";

        spdlog::info("Connecting to database: {}", db_conn);

        // Create and configure LoginServer
        auto login = std::make_unique<Login::LoginServer>(io_context, client_port, gateway_port, db_conn);
        login->start();

        // Setup UDP give_time timer (needed for connection state management)
        boost::asio::steady_timer udp_timer(io_context);
        std::function<void(const boost::system::error_code&)> udp_tick;
        udp_tick = [&login, &udp_timer, &udp_tick](const boost::system::error_code& ec) {
            if (!ec && login) {
                login->give_udp_time();
                udp_timer.expires_after(std::chrono::milliseconds(50));
                udp_timer.async_wait(udp_tick);
            }
        };
        udp_timer.expires_after(std::chrono::milliseconds(50));
        udp_timer.async_wait(udp_tick);

        // Run io_context in worker threads
        auto thread_count = std::max(2u, std::thread::hardware_concurrency());
        spdlog::info("Starting {} worker threads", thread_count);

        std::vector<std::thread> threads;
        threads.reserve(thread_count);

        for (unsigned i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        spdlog::info("Login Server started successfully");

        // Main loop
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        spdlog::info("Shutting down Login Server...");

        // Stop io_context
        io_context.stop();

        // Join threads
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        spdlog::info("Login Server shutdown complete");

        return 0;

    } catch (const std::exception& e) {
        spdlog::error("Fatal error: {}", e.what());
        return 1;
    }
}
