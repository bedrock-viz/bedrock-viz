#include "xml/load_enchantment.h"
#include "minecraft/v2/enchantment.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace mcpe_viz;

class LoadEnchantmentTest : public ::testing::Test {
protected:
    static pugi::xml_document enchantmentList;

    void TearDown() override {
        enchantmentList.remove_children();
        Enchantment::clearList();
    }
};

pugi::xml_document LoadEnchantmentTest::enchantmentList;

TEST_F(LoadEnchantmentTest, UnitAdd) {
    auto enchantment = enchantmentList.append_child("enchantment");
    enchantment.append_attribute("name").set_value("Protection");
    enchantment.append_attribute("id").set_value(0x0);
    enchantment.append_attribute("officialName").set_value("protection");

    int ret = load_enchantment(enchantmentList);
    
    ASSERT_EQ(ret,0);
}

TEST_F(LoadEnchantmentTest, UnitTryAddNoName){
    auto enchantment = enchantmentList.append_child("enchantment");
    enchantment.append_attribute("name");

    int ret = load_enchantment(enchantmentList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadEnchantmentTest, UnitTryAddNoId){
    auto enchantment = enchantmentList.append_child("enchantment");
    enchantment.append_attribute("name").set_value("Protection");
    enchantment.append_attribute("id");
    
    int ret = load_enchantment(enchantmentList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadEnchantmentTest, UnitTryAddNoOfficialName){
    auto enchantment = enchantmentList.append_child("enchantment");
    enchantment.append_attribute("name").set_value("Protection");
    enchantment.append_attribute("id").set_value(0x0);
    enchantment.append_attribute("officialName");

    int ret = load_enchantment(enchantmentList);
    
    ASSERT_EQ(ret,-1);
}