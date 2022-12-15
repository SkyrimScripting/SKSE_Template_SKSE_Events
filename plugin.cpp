#include <spdlog/sinks/basic_file_sink.h>

namespace logger = SKSE::log;

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

// For every message sent to the plugin by SKSE, log the message.
void MessageListener(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        // Descriptions are taken from the original skse64 library
        // See:
        // https://github.com/ianpatt/skse64/blob/09f520a2433747f33ae7d7c15b1164ca198932c3/skse64/PluginAPI.h#L193-L212
        case SKSE::MessagingInterface::kPostLoad:
            logger::info("kPostLoad: sent to registered plugins once all plugins have been loaded");
            break;
        case SKSE::MessagingInterface::kPostPostLoad:
            logger::info(
                "kPostPostLoad: sent right after kPostLoad to facilitate the correct dispatching/registering of "
                "messages/listeners");
            break;
        case SKSE::MessagingInterface::kPreLoadGame:
            // message->dataLen: length of file path, data: char* file path of .ess savegame file
            logger::info("kPreLoadGame: sent immediately before savegame is read");
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
            // You will probably want to handle this event if your plugin uses a Preload callback
            // as there is a chance that after that callback is invoked the game will encounter an error
            // while loading the saved game (eg. corrupted save) which may require you to reset some of your
            // plugin state.
            logger::info("kPostLoadGame: sent after an attempt to load a saved game has finished");
            break;
        case SKSE::MessagingInterface::kSaveGame:
            logger::info("kSaveGame");
            break;
        case SKSE::MessagingInterface::kDeleteGame:
            // message->dataLen: length of file path, data: char* file path of .ess savegame file
            logger::info("kDeleteGame: sent right before deleting the .skse cosave and the .ess save");
            break;
        case SKSE::MessagingInterface::kInputLoaded:
            logger::info("kInputLoaded: sent right after game input is loaded, right before the main menu initializes");
            break;
        case SKSE::MessagingInterface::kNewGame:
            // message-data: CharGen TESQuest pointer (Note: I haven't confirmed the usefulness of this yet!)
            logger::info("kNewGame: sent after a new game is created, before the game has loaded");
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            logger::info("kDataLoaded: sent after the data handler has loaded all its forms");
            break;
        default:
            logger::info("Unknown system message of type: {}", message->type);
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    SetupLog();

    // This is where the magic happens!
    // See the MessageListener function above.
    SKSE::GetMessagingInterface()->RegisterListener(MessageListener);

    // Note: you could also use a lambda function
    // SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message){
    //     logger::info("System Message type {}", message->type);
    // });

    return true;
}
