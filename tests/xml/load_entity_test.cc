#include "xml/load_entity.h"
#include "minecraft/v2/entity.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace mcpe_viz;

class LoadEntityTest : public ::testing::Test {
protected:
    static pugi::xml_document entList;

    void TearDown() override {
        entList.remove_children();
        Entity::clearList();
    }
};

pugi::xml_document LoadEntityTest::entList;

TEST_F(LoadEntityTest, UnitAddHostile) {
    auto ent = entList.append_child("entity");
    ent.append_attribute("id").set_value(0x20);
    ent.append_attribute("name").set_value("Zombie");
    ent.append_attribute("uname").set_value("minecraft:zombie");
    ent.append_attribute("etype").set_value(int(Entity::EType::H));

    int ret = load_entity(entList);
    
    ASSERT_EQ(ret,0);
}

TEST_F(LoadEntityTest, UnitAddPassive) {
    auto ent = entList.append_child("entity");
    ent.append_attribute("id").set_value(0x1d);
    ent.append_attribute("name").set_value("Llama");
    ent.append_attribute("uname").set_value("minecraft:llama");
    ent.append_attribute("etype").set_value(int(Entity::EType::P));
    auto variant = ent.append_child("entityvariant");
    variant.append_attribute("name").set_value("Llama, Creamy");
    variant.append_attribute("extradata").set_value(0x0);

    int ret = load_entity(entList);
    
    ASSERT_EQ(ret,0);
}

TEST_F(LoadEntityTest, UnitTryAddNoId){
    auto ent = entList.append_child("entity");
    ent.append_attribute("id");
    
    int load_item_ret = load_entity(entList);
    
    ASSERT_EQ(load_item_ret,-1);
}

TEST_F(LoadEntityTest, UnitTryAddNoName){
    auto ent = entList.append_child("entity");
    ent.append_attribute("id").set_value(0x1d);
    ent.append_attribute("name");

    int ret = load_entity(entList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadEntityTest, UnitTryAddDuplicate) {
    auto ent = entList.append_child("entity");
    ent.append_attribute("id").set_value(0x1d);
    ent.append_attribute("name").set_value("Llama");
    entList.append_copy(ent);

    int ret = load_entity(entList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadEntityTest, UnitAddTryVariantNoName){
    auto ent = entList.append_child("entity");
    ent.append_attribute("id").set_value(0x1d);
    ent.append_attribute("name").set_value("Llama");
    auto variant = ent.append_child("entityvariant");
    variant.append_attribute("name").set_value("");

    int ret = load_entity(entList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadEntityTest, UnitAddTryVariantNoExtraData){
    auto ent = entList.append_child("entity");
    ent.append_attribute("id").set_value(0x1d);
    ent.append_attribute("name").set_value("Llama");
    auto variant = ent.append_child("entityvariant");
    variant.append_attribute("name").set_value("Llama, Creamy");
    variant.append_attribute("extradata").set_value("");

    int ret = load_entity(entList);
    
    ASSERT_EQ(ret,-1);
}

TEST_F(LoadEntityTest, UnitAddTryDuplicateVariant){
    auto ent = entList.append_child("entity");
    ent.append_attribute("id").set_value(0x1d);
    ent.append_attribute("name").set_value("Llama");
    auto variant = ent.append_child("entityvariant");
    variant.append_attribute("name").set_value("Llama, Creamy");
    variant.append_attribute("extradata").set_value(0x0);
    ent.append_copy(variant);

    int ret = load_entity(entList);
    
    ASSERT_EQ(ret,-1);
}