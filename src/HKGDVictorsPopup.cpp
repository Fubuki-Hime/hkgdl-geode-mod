#include "HKGDVictorsPopup.hpp"

bool HKGDVictorsPopup::setup(int levelId, const std::string& levelName, int position) {
    m_levelId = levelId;
    m_levelName = levelName;
    m_position = position;
    
    return true;
}

HKGDVictorsPopup* HKGDVictorsPopup::create(int levelId, const std::string& levelName, int position) {
    auto ret = new HKGDVictorsPopup();
    if (ret->init(380.f, 260.f)) {
        ret->setup(levelId, levelName, position);
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void HKGDVictorsPopup::createUI() {
    // Title
    setTitle(fmt::format("HKGD Victors - {}", m_levelName).c_str());
    
    // Position label
    auto posLabel = CCLabelBMFont::create(fmt::format("Position: #{}", m_position).c_str(), "goldFont.fnt");
    posLabel->setPosition({m_size.width / 2, m_size.height - 40.f});
    posLabel->setScale(0.5f);
    m_mainLayer->addChild(posLabel);
    
    // Loading circle
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->setPosition({m_size.width / 2, m_size.height / 2});
    m_loadingCircle->setVisible(true);
    m_mainLayer->addChild(m_loadingCircle);
    
    // Start loading victors
    HKGDManager::get()->fetchVictors(m_levelId, [this](std::vector<HKGDRecord> records) {
        this->onLoadingFinished(records);
    });
}

void HKGDVictorsPopup::onLoadingFinished(std::vector<HKGDRecord> records) {
    m_records = records;
    
    if (m_loadingCircle) {
        m_loadingCircle->setVisible(false);
        m_loadingCircle->removeFromParent();
        m_loadingCircle = nullptr;
    }
    
    if (m_records.empty()) {
        auto noRecords = CCLabelBMFont::create("No victors found", "bigFont.fnt");
        noRecords->setPosition({m_size.width / 2, m_size.height / 2});
        noRecords->setScale(0.5f);
        m_mainLayer->addChild(noRecords);
        return;
    }
    
    createVictorsList();
}

void HKGDVictorsPopup::createVictorsList() {
    float listWidth = 340.f;
    float listHeight = 180.f;
    float cellHeight = 22.f;
    
    // Calculate total content height
    float totalHeight = (m_records.size() + 1) * cellHeight; // +1 for header
    
    // Create scroll layer
    auto scrollLayer = ScrollLayer::create({listWidth, listHeight});
    scrollLayer->setPosition({(m_size.width - listWidth) / 2, 20.f});
    m_mainLayer->addChild(scrollLayer);
    m_scrollView = scrollLayer;
    
    // Content layer
    auto contentLayer = scrollLayer->m_contentLayer;
    
    // Header row
    auto header = CCLabelBMFont::create("Rank   Username            Date", "bigFont.fnt");
    header->setAnchorPoint({0, 0.5f});
    header->setPosition({10.f, totalHeight - cellHeight / 2});
    header->setScale(0.3f);
    contentLayer->addChild(header);
    
    // Add victor cells
    for (size_t i = 0; i < m_records.size(); i++) {
        auto cell = createVictorCell(m_records[i], static_cast<int>(i));
        cell->setPosition({listWidth / 2, totalHeight - (static_cast<float>(i) + 1.5f) * cellHeight});
        contentLayer->addChild(cell);
    }
    
    // Set content layer size
    contentLayer->setContentSize({listWidth, totalHeight});
    scrollLayer->scrollToTop();
}

CCNode* HKGDVictorsPopup::createVictorCell(HKGDRecord const& record, int index) {
    float cellHeight = 22.f;
    
    auto cell = CCNode::create();
    cell->setContentSize({340.f, cellHeight});
    
    // Background
    ccColor4B bgColor = index % 2 == 0 ? ccc4(50, 50, 50, 100) : ccc4(30, 30, 30, 100);
    auto bg = CCLayerColor::create(bgColor, 340.f, cellHeight);
    bg->setPosition({-170.f, -cellHeight / 2});
    cell->addChild(bg);
    
    // Rank
    auto rankLabel = CCLabelBMFont::create(fmt::format("#{}", record.rank).c_str(), "bigFont.fnt");
    rankLabel->setAnchorPoint({0, 0.5f});
    rankLabel->setPosition({-160.f, 0});
    rankLabel->setScale(0.3f);
    
    // Color based on rank
    if (record.rank == 1) {
        rankLabel->setColor({255, 215, 0}); // Gold
    } else if (record.rank == 2) {
        rankLabel->setColor({192, 192, 192}); // Silver
    } else if (record.rank == 3) {
        rankLabel->setColor({205, 127, 50}); // Bronze
    }
    cell->addChild(rankLabel);
    
    // Username
    auto usernameLabel = CCLabelBMFont::create(record.username.c_str(), "bigFont.fnt");
    usernameLabel->setAnchorPoint({0, 0.5f});
    usernameLabel->setPosition({-110.f, 0});
    usernameLabel->setScale(0.3f);
    cell->addChild(usernameLabel);
    
    // Date
    auto dateLabel = CCLabelBMFont::create(record.date.c_str(), "bigFont.fnt");
    dateLabel->setAnchorPoint({0, 0.5f});
    dateLabel->setPosition({50.f, 0});
    dateLabel->setScale(0.3f);
    cell->addChild(dateLabel);
    
    return cell;
}