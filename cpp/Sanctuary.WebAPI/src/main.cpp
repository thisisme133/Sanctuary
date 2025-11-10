#include <iostream>
#include <memory>
#include <thread>
#include <csignal>
#include <atomic>
#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <boost/asio.hpp>
#include <Sanctuary/Database/DatabaseFactory.h>
#include <Sanctuary/WebAPI/WebServer.h>

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
        cxxopts::Options options("Sanctuary.WebAPI", "Free Realms Web API Server");

        options.add_options()
            ("c,config", "Config file path", cxxopts::value<std::string>()->default_value("webapi_config.json"))
            ("p,port", "HTTP port", cxxopts::value<uint16_t>()->default_value("8080"))
            ("d,database", "Database connection string", cxxopts::value<std::string>()->default_value("sanctuary.db"))
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
            "logs/webapi.log", 1024 * 1024 * 10, 3);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        auto logger = std::make_shared<spdlog::logger>("webapi", sinks.begin(), sinks.end());
        spdlog::set_default_logger(logger);

        if (result["verbose"].as<bool>()) {
            spdlog::set_level(spdlog::level::debug);
        } else {
            spdlog::set_level(spdlog::level::info);
        }

        spdlog::info("Sanctuary Web API Server starting...");

        // Setup signal handlers
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Initialize database
        auto db_conn = result["database"].as<std::string>();
        auto db_type = result["db-type"].as<std::string>();

        spdlog::info("Connecting to database: {}", db_conn);

        auto database = db_type == "mysql"
            ? Database::DatabaseFactory::create(Database::DatabaseType::MySQL, db_conn)
            : Database::DatabaseFactory::create_sqlite(db_conn);

        // Create ASIO io_context
        boost::asio::io_context io_context;

        // Create Web server
        auto port = result["port"].as<uint16_t>();
        spdlog::info("Starting Web API Server on port {}", port);

        WebAPI::WebServer web_server(io_context, port, *database);
        web_server.start();

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

        spdlog::info("Web API Server started successfully");

        // Main loop
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        spdlog::info("Shutting down Web API Server...");

        // Stop io_context
        io_context.stop();

        // Join threads
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        spdlog::info("Web API Server shutdown complete");

        return 0;

    } catch (const std::exception& e) {
        spdlog::error("Fatal error: {}", e.what());
        return 1;
    }
}
