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
#include <Sanctuary/Gateway/GatewayServer.h>

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
        cxxopts::Options options("Sanctuary.Gateway", "Free Realms Gateway Server");

        options.add_options()
            ("c,config", "Config file path", cxxopts::value<std::string>()->default_value("gateway_config.json"))
            ("p,port", "UDP port", cxxopts::value<uint16_t>()->default_value("20060"))
            ("d,database", "Database connection string", cxxopts::value<std::string>())
            ("l,login-host", "Login server host", cxxopts::value<std::string>()->default_value("127.0.0.1"))
            ("login-port", "Login server port", cxxopts::value<uint16_t>()->default_value("20070"))
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
            "logs/gateway.log", 1024 * 1024 * 10, 3);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        auto logger = std::make_shared<spdlog::logger>("gateway", sinks.begin(), sinks.end());
        spdlog::set_default_logger(logger);

        if (result["verbose"].as<bool>()) {
            spdlog::set_level(spdlog::level::debug);
        } else {
            spdlog::set_level(spdlog::level::info);
        }

        spdlog::info("Sanctuary Gateway Server starting...");

        // Setup signal handlers
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Create ASIO io_context
        boost::asio::io_context io_context;

        // Create Gateway server
        auto gateway_port = result["port"].as<uint16_t>();
        auto login_host = result["login-host"].as<std::string>();
        auto login_port = result["login-port"].as<uint16_t>();

        spdlog::info("Initializing Gateway Server on port {}", gateway_port);
        spdlog::info("Connecting to Login Server at {}:{}", login_host, login_port);

        // TODO: Create and configure GatewayServer
        // auto gateway = std::make_unique<Gateway::GatewayServer>(io_context, gateway_port);

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

        spdlog::info("Gateway Server started successfully");

        // Main loop
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        spdlog::info("Shutting down Gateway Server...");

        // Stop io_context
        io_context.stop();

        // Join threads
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        spdlog::info("Gateway Server shutdown complete");

        return 0;

    } catch (const std::exception& e) {
        spdlog::error("Fatal error: {}", e.what());
        return 1;
    }
}
