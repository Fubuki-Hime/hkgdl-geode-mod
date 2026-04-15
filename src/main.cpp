#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "HKGDManager.hpp"
#include "HKGDVictorsPopup.hpp"

using namespace geode::prelude;

class $modify(HKGDLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* hkgdButton = nullptr;
        int hkgdPosition = -1;
        bool hasHKGDLRecord = false;
    };
    
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) {
            return false;
        }
        
        // Check if level is on HKGDL
        auto levelId = level->m_levelID.value();
        
        HKGDManager::get()->checkLevelOnHKGDL(levelId, [this](bool isOnHKGDL, int position) {
            if (isOnHKGDL) {
                m_fields->hkgdPosition = position;
                m_fields->hasHKGDLRecord = true;
                this->createHKGDButton();
            }
        });
        
        return true;
    }
    
    void createHKGDButton() {
        if (m_fields->hkgdButton) return;
        
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        // Create button sprite using ButtonSprite
        auto buttonSprite = ButtonSprite::create("HKGD Victors", "bigFont.fnt", "GJ_button_04.png", 0.8f);
        buttonSprite->setScale(0.55f);
        
        // Create button
        m_fields->hkgdButton = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(HKGDLevelInfoLayer::onHKGDButton)
        );
        
        // Position button under the difficulty sprite
        float buttonX, buttonY;
        
        if (m_difficultySprite) {
            // Position under the difficulty sprite
            auto diffPos = m_difficultySprite->getPosition();
            buttonX = diffPos.x;
            buttonY = diffPos.y - m_difficultySprite->getContentSize().height / 2 - 25.f;
        } else if (m_starsIcon) {
            // Fallback: position under stars icon
            auto starsPos = m_starsIcon->getPosition();
            buttonX = starsPos.x;
            buttonY = starsPos.y - 25.f;
        } else {
            // Default position
            buttonX = winSize.width / 2 - 100.f;
            buttonY = winSize.height / 2 - 60.f;
        }
        
        m_fields->hkgdButton->setPosition({buttonX, buttonY});
        
        // Add to the layer
        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        menu->addChild(m_fields->hkgdButton);
        this->addChild(menu);
        
        // Add position indicator label
        auto posLabel = CCLabelBMFont::create(fmt::format("HKGD #{}", m_fields->hkgdPosition).c_str(), "goldFont.fnt");
        posLabel->setScale(0.35f);
        posLabel->setPosition({buttonX, buttonY - 18.f});
        posLabel->setZOrder(10);
        this->addChild(posLabel);
    }
    
    void onHKGDButton(CCObject* sender) {
        if (!m_fields->hasHKGDLRecord) return;
        
        auto level = m_level;
        if (!level) return;
        
        auto popup = HKGDVictorsPopup::create(
            level->m_levelID.value(),
            level->m_levelName,
            m_fields->hkgdPosition
        );
        
        if (popup) {
            popup->createUI();
            popup->show();
        }
    }
};