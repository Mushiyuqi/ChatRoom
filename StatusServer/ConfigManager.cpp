#include "ConfigManager.h"
SectionInfo::~SectionInfo() {
    m_section_datas.clear();
}

SectionInfo::SectionInfo(const SectionInfo& src) {
    m_section_datas = src.m_section_datas;
}

SectionInfo& SectionInfo::operator=(const SectionInfo& src) {
    if (&src == this) {
        return *this;
    }

    this->m_section_datas = src.m_section_datas;
    return *this;
}

std::string SectionInfo::operator[](const std::string& key) {
    if (m_section_datas.find(key) == m_section_datas.end()) {
        return "";
    }
    // 这里可以添加一些边界检查
    return m_section_datas[key];
}

ConfigManager::~ConfigManager() {
    m_config_map.clear();
}

SectionInfo ConfigManager::operator[](const std::string& section) {
    if (!m_config_map.contains(section)) {
        return {};
    }
    return m_config_map[section];
}

ConfigManager& ConfigManager::GetInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() {
    const boost::filesystem::path currentPath = boost::filesystem::current_path();
    const boost::filesystem::path configPath = currentPath / "config.ini";
    std::cout << "ConfigManager::ConfigManager configPath is " << configPath << std::endl;
    // 处理ini文件
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(configPath.string(), pt);
    // 提取配置
    for (const auto& [fst, snd] : pt) {
        // 获取主树中的配置
        const ::std::string& sectionName = fst;
        const boost::property_tree::ptree& sectionTree = snd;
        // 获取子树中的配置
        std::map<std::string, std::string> sectionConfig;
        for (const auto& [fstSub, sndSub] : sectionTree) {
            const std::string& key = fstSub;
            const std::string& value = sndSub.get_value<std::string>();
            sectionConfig[key] = value;
        }
        // 添加到配置管理器中
        SectionInfo sectionInfo;
        sectionInfo.m_section_datas = sectionConfig;
        m_config_map[sectionName] = sectionInfo;
    }

    // 输出所有的section和key-value对
    for (const auto& [fst, snd] : m_config_map) {
        // 主树
        const std::string& section_name = fst;
        SectionInfo section_config = snd;
        // 子树
        std::cout << "[" << section_name << "]" << std::endl;
        for (const auto& [fstSub, sndSub] : section_config.m_section_datas) {
            std::cout << fstSub << "=" << sndSub << std::endl;
        }
    }
}

