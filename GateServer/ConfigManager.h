#pragma once
#include "const.h"

struct SectionInfo {
    SectionInfo() = default;
    ~SectionInfo();
    SectionInfo(const SectionInfo& src);
    SectionInfo& operator=(const SectionInfo& src);
    std::string operator[](const std::string& key);

    std::map<std::string, std::string> m_section_datas;
};

class ConfigManager {
public:
    ~ConfigManager();
    SectionInfo operator[](const std::string& section);
    ConfigManager& operator=(const ConfigManager& src);
    ConfigManager(const ConfigManager& src);
    ConfigManager();

private:
    // 存储section和key-value对的map
    std::map<std::string, SectionInfo> m_config_map;
};

