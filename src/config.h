#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

std::ifstream f("settings.config.json");
json data = json::parse(f);

extern int MAX_FRAMES_IN_FLIGHT = data["maxFramesInFlight"];

extern uint32_t WIDTH = data["width"];
extern uint32_t HEIGHT = data["height"];
