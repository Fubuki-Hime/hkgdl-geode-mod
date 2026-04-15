#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
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
    
    bool setup(int levelId, const std::string& levelName, int position);
    void createVictorsList();
    CCNode* createVictorCell(HKGDRecord const& record, int index);
    void onLoadingFinished(std::vector<HKGDRecord> records);
    
public:
    static HKGDVictorsPopup* create(int levelId, const std::string& levelName, int position);
    void createUI();
};
