#include <spdlog/sinks/basic_file_sink.h>

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) {
        SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
        return;
    }
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::info);
}

namespace logger = SKSE::log;

void MessageListener(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kPostLoad:
            logger::info("POST LOAD");
            break;
        case SKSE::MessagingInterface::kPostPostLoad:
            logger::info("POST POST LOAD");
            break;
        case SKSE::MessagingInterface::kPreLoadGame:
            logger::info("PRE LOAD GAME");
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
            logger::info("POST LOAD GAME");
            break;
        case SKSE::MessagingInterface::kSaveGame:
            logger::info("SAVE GAME");
            break;
        case SKSE::MessagingInterface::kDeleteGame:
            logger::info("DELETE GAME");
            break;
        case SKSE::MessagingInterface::kInputLoaded:
            logger::info("INPUT LOADED");
            break;
        case SKSE::MessagingInterface::kNewGame:
            logger::info("NEW GAME");
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            logger::info("DATA LOADED");
            break;
        default:
            logger::info("Unknown system message, maybe kTotal (9)? {}", message->type);
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SetupLog();

    SKSE::GetMessagingInterface()->RegisterListener(MessageListener);

    return true;
}
