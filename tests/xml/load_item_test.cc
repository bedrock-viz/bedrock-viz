#include "xml/load_item.h"
#include "minecraft/v2/item.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace mcpe_viz;

class LoadItemTest : public ::testing::Test {
protected:
    static pugi::xml_document itemList;

    void TearDown() override {
        itemList.remove_children();
        Item::clearList();
    }
};

pugi::xml_document LoadItemTest::itemList;

TEST_F(LoadItemTest, UnitAdd) {
    auto item = itemList.append_child("item");
    item.append_attribute("id").set_value(0x106);
    item.append_attribute("name").set_value("Arrow");
    item.append_attribute("uname").set_value("minecraft:arrow");
    auto variant = item.append_child("itemvariant");
    variant.append_attribute("name").set_value("Arrow");
    variant.append_attribute("extradata").set_value(0x0);

    int ret = load_item(itemList);
    
    ASSERT_EQ(ret,0);
}

TEST_F(LoadItemTest, UnitTryAddNoId){
    auto item = itemList.append_child("item");
    item.append_attribute("id");
    
    int ret = load_item(itemList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadItemTest, UnitTryAddNoName){
    auto item = itemList.append_child("item");
    item.append_attribute("id").set_value(0x106);
    item.append_attribute("name");

    int ret = load_item(itemList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadItemTest, UnitTryAddDuplicate) {
    auto item = itemList.append_child("item");
    item.append_attribute("id").set_value(0x106);
    item.append_attribute("name").set_value("Arrow");
    itemList.append_copy(item);

    int ret = load_item(itemList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadItemTest, UnitAddTryVariantNoName){
    auto item = itemList.append_child("item");
    item.append_attribute("id").set_value(0x106);
    item.append_attribute("name").set_value("Arrow");
    auto variant = item.append_child("itemvariant");
    variant.append_attribute("name").set_value("");

    int ret = load_item(itemList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadItemTest, UnitAddTryVariantNoExtraData){
    auto item = itemList.append_child("item");
    item.append_attribute("id").set_value(0x106);
    item.append_attribute("name").set_value("Arrow");
    auto variant = item.append_child("itemvariant");
    variant.append_attribute("name").set_value("Arrow");
    variant.append_attribute("extradata").set_value("");

    int ret = load_item(itemList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadItemTest, UnitAddTryDuplicateVariant){
    auto item = itemList.append_child("item");
    item.append_attribute("id").set_value(0x106);
    item.append_attribute("name").set_value("Arrow");
    auto variant = item.append_child("itemvariant");
    variant.append_attribute("name").set_value("Arrow");
    variant.append_attribute("extradata").set_value(0x0);
    item.append_copy(variant);

    int ret = load_item(itemList);
    
    ASSERT_EQ(ret,-1);
}