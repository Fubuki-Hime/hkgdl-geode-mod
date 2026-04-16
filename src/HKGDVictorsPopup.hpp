#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Button.hpp>
#include "HKGDManager.hpp"

using namespace geode::prelude;

class HKGDVictorsPopup : public geode::Popup {
protected:
    int m_levelId;
    std::string m_levelName;
    int m_position;
    std::vector<HKGDRecord> m_records;
    ScrollLayer* m_scrollView = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    bool m_isOnHKGDL = false;
    
    bool init(int levelId, const std::string& levelName, int position);
    void createVictorsList();
    CCNode* createVictorCell(HKGDRecord const& record, int index);
    void onLoadingFinished(std::vector<HKGDRecord> records);
    void onSubmitRecord(CCObject* sender);
    void onVideoButton(CCObject* sender);
    
public:
    static HKGDVictorsPopup* create(int levelId, const std::string& levelName, int position, bool isOnHKGDL = true);
    void createUI();
};

class HKGDSubmitPopup : public geode::Popup {
protected:
    int m_levelId;
    std::string m_levelName;
    TextInput* m_usernameInput = nullptr;
    TextInput* m_attemptsInput = nullptr;
    TextInput* m_videoUrlInput = nullptr;
    TextInput* m_fpsInput = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    
    bool init(int levelId, const std::string& levelName);
    void onSubmit(CCObject* sender);
    void onSubmitComplete(HKGDSubmissionResult result);
    
public:
    static HKGDSubmitPopup* create(int levelId, const std::string& levelName);
};

class HKGDListPopup : public geode::Popup {
protected:
    std::vector<HKGDLevelInfo> m_levels;
    ScrollLayer* m_scrollView = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    
    bool init();
    void createLevelsList();
    CCNode* createLevelCell(HKGDLevelInfo const& level, int index);
    void onLoadingFinished(std::vector<HKGDLevelInfo> levels);
    void onLevelClicked(CCObject* sender);
    
public:
    static HKGDListPopup* create();
    void createUI();
};

class HKGDPlayerStatsPopup : public geode::Popup {
protected:
    std::string m_username;
    std::vector<HKGDRecord> m_records;
    ScrollLayer* m_scrollView = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    int m_accountId = 0;
    
    bool init(const std::string& username, int accountId);
    void createStatsList();
    CCNode* createRecordCell(HKGDRecord const& record, int index);
    void onLoadingFinished(std::vector<HKGDRecord> records);
    void onVideoButton(CCObject* sender);
    
public:
    static HKGDPlayerStatsPopup* create(const std::string& username, int accountId);
    void createUI();
};
