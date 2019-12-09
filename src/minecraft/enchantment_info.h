#pragma once

#include <string>
#include <cstdint>
#include <map>
#include <memory>

namespace mcpe_viz {
    
  class EnchantmentInfo {
  public:
    std::string name;
    std::string officialName;

    EnchantmentInfo(const char* n) {
      setName(n);
      officialName="";
    }

    EnchantmentInfo& setName (const std::string& s) {
      name = std::string(s);
      return *this;
    }

    EnchantmentInfo& setOfficialName (const std::string& s) {
      officialName = std::string(s);
      return *this;
    }
  };

  typedef std::map<int, std::unique_ptr<EnchantmentInfo> > EnchantmentInfoList;
  extern EnchantmentInfoList enchantmentInfoList;
  bool has_key(const EnchantmentInfoList &m, int32_t k);
}
