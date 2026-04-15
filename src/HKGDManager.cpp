#include "HKGDManager.hpp"

HKGDManager* HKGDManager::s_instance = nullptr;

HKGDManager::HKGDManager() {
    m_apiUrl = "https://api.hkgdl.dpdns.org/api";
}

HKGDManager* HKGDManager::get() {
    if (!s_instance) {
        s_instance = new HKGDManager();
    }
    return s_instance;
}

std::string HKGDManager::getApiUrl() {
    auto mod = Mod::get();
    auto url = mod->getSettingValue<std::string>("api-url");
    if (url.empty()) {
        return m_apiUrl;
    }
    return url;
}

void HKGDManager::checkLevelOnHKGDL(int levelId, std::function<void(bool, int)> callback) {
    auto url = fmt::format("{}/levels", getApiUrl());
    
    geode::async::spawn(
        web::WebRequest().get(url),
        [callback, levelId](web::WebResponse const& response) {
            if (!response.ok()) {
                callback(false, -1);
                return;
            }
            
            auto jsonResult = response.json();
            if (jsonResult.isErr()) {
                callback(false, -1);
                return;
            }
            
            auto& json = jsonResult.unwrap();
            
            try {
                if (json.isArray()) {
                    auto arrResult = json.asArray();
                    if (arrResult.isOk()) {
                        for (auto& levelData : arrResult.unwrap()) {
                            // Check if levelId matches (as string)
                            auto levelIdResult = levelData["levelId"].asString();
                            if (levelIdResult.isOk()) {
                                std::string apiLevelId = levelIdResult.unwrap();
                                if (apiLevelId == std::to_string(levelId)) {
                                    auto rankResult = levelData["hkgdRank"].asInt();
                                    int rank = rankResult.isOk() ? static_cast<int>(rankResult.unwrap()) : -1;
                                    callback(true, rank);
                                    return;
                                }
                            }
                        }
                    }
                }
                callback(false, -1);
            } catch (...) {
                callback(false, -1);
            }
        }
    );
}

void HKGDManager::fetchVictors(int levelId, std::function<void(std::vector<HKGDRecord>)> callback) {
    auto url = fmt::format("{}/levels", getApiUrl());
    
    geode::async::spawn(
        web::WebRequest().get(url),
        [callback, levelId](web::WebResponse const& response) {
            std::vector<HKGDRecord> records;
            
            if (!response.ok()) {
                callback(records);
                return;
            }
            
            auto jsonResult = response.json();
            if (jsonResult.isErr()) {
                callback(records);
                return;
            }
            
            auto& json = jsonResult.unwrap();
            
            try {
                if (json.isArray()) {
                    auto arrResult = json.asArray();
                    if (arrResult.isOk()) {
                        for (auto& levelData : arrResult.unwrap()) {
                            // Check if levelId matches
                            auto levelIdResult = levelData["levelId"].asString();
                            if (levelIdResult.isOk()) {
                                std::string apiLevelId = levelIdResult.unwrap();
                                if (apiLevelId == std::to_string(levelId)) {
                                    // Found the level, extract records
                                    if (levelData.contains("records") && levelData["records"].isArray()) {
                                        auto recordsArrResult = levelData["records"].asArray();
                                        if (recordsArrResult.isOk()) {
                                            int rank = 1;
                                            for (auto& recordJson : recordsArrResult.unwrap()) {
                                                HKGDRecord record;
                                                record.rank = rank++;
                                                
                                                auto playerResult = recordJson["player"].asString();
                                                record.username = playerResult.isOk() ? playerResult.unwrap() : "Unknown";
                                                
                                                auto dateResult = recordJson["date"].asString();
                                                record.date = dateResult.isOk() ? dateResult.unwrap() : "";
                                                
                                                if (recordJson.contains("attempts") && !recordJson["attempts"].isNull()) {
                                                    auto attemptsResult = recordJson["attempts"].asInt();
                                                    record.attempts = attemptsResult.isOk() ? static_cast<int>(attemptsResult.unwrap()) : 0;
                                                }
                                                
                                                // Check for video URL
                                                if (recordJson.contains("videoUrl") && !recordJson["videoUrl"].isNull()) {
                                                    auto videoResult = recordJson["videoUrl"].asString();
                                                    record.videoUrl = videoResult.isOk() ? videoResult.unwrap() : "";
                                                }
                                                
                                                // Check for FPS
                                                if (recordJson.contains("fps") && !recordJson["fps"].isNull()) {
                                                    auto fpsResult = recordJson["fps"].asString();
                                                    record.fps = fpsResult.isOk() ? fpsResult.unwrap() : "";
                                                }
                                                
                                                records.push_back(record);
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            } catch (...) {
            }
            
            callback(records);
        }
    );
}

void HKGDManager::fetchLevelInfo(int levelId, std::function<void(HKGDLevelInfo)> callback) {
    fetchVictors(levelId, [callback, levelId](std::vector<HKGDRecord> records) {
        HKGDLevelInfo info;
        info.levelId = levelId;
        info.records = records;
        info.hasRecords = !records.empty();
        callback(info);
    });
}