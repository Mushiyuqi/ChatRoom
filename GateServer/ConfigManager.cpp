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

ConfigManager& ConfigManager::operator=(const ConfigManager& src) {
    if (&src == this) {
        return *this;
    }

    this->m_config_map = src.m_config_map;
    return *this;
}

ConfigManager::ConfigManager(const ConfigManager& src) {
    this->m_config_map = src.m_config_map;
}

ConfigManager::ConfigManager() {
    boost::filesystem::path currentPath = boost::filesystem::current_path();
}

