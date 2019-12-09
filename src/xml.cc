/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  XML support
*/
#include "xml.h"

#include <cstdio>
#include <libxml/xmlreader.h>
#include "util.h"
#include "mcpe_viz.h"
#include "minecraft/block_info.h"
#include "minecraft/item_info.h"
#include "minecraft/entity_info.h"
#include "minecraft/biome_info.h"
#include "minecraft/enchantment_info.h"
#include "minecraft/conversion.h"

#ifdef _MSC_VER 
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif


namespace mcpe_viz {
    
  int32_t xmlGetInt(xmlNodePtr cur, const xmlChar* p, bool &valid) {
    valid=false;
    int32_t ret;
    xmlChar* prop = xmlGetProp(cur,p);
    if ( prop ) {
      // see if it is hexadecimal
      if ( sscanf((char*)prop,"0x%x",&ret) == 1 ) {
        xmlFree(prop);
        valid=true;
        return ret;
      }
      // try decimal
      if ( sscanf((char*)prop,"%d",&ret) == 1 ) {
        xmlFree(prop);
        valid=true;
        return ret;
      }
      xmlFree(prop);
    }
    // todo - verbose only?
    // todo - show more context
    // fprintf(stderr,"WARNING: Failed xmlGetInt k=[%s]\n", (char*)p);
    return 0;
  }

  bool xmlGetBool(xmlNodePtr cur, const xmlChar* p, bool defaultValue, bool &valid) {
    valid = false;
    xmlChar* prop = xmlGetProp(cur,p);
    if ( prop ) {
      if ( strcasecmp((char*)prop,"true") == 0 ) {
        valid = true;
        xmlFree(prop);
        return true;
      }
      if ( strcasecmp((char*)prop,"1") == 0 ) {
        valid = true;
        xmlFree(prop);
        return true;
      }
      if ( strcasecmp((char*)prop,"false") == 0 ) {
        valid = true;
        xmlFree(prop);
        return false;
      }
      if ( strcasecmp((char*)prop,"0") == 0 ) {
        valid = true;
        xmlFree(prop);
        return false;
      }
      xmlFree(prop);
    }
    return defaultValue;
  }
    
  std::string xmlGetString(xmlNodePtr cur, const xmlChar* p, bool &valid) {
    valid = false;
    std::string s("(EMPTY)");
    xmlChar* prop = xmlGetProp(cur,p);
    if ( prop ) {
      s = (char*)prop;
      xmlFree(prop);
      valid=true;
    }
    return s;
  }

  int32_t doParseXml_Unknown(xmlNodePtr cur) {
    // some unknowns are fine (e.g. text and comment)
    if ( cur->type == XML_TEXT_NODE ) {
      // fine
    }
    else if ( cur->type == XML_COMMENT_NODE ) {
      // fine
    }
    else {
      fprintf(stderr, "WARNING: Unrecognized XML element: (parent=%s) name=(%s) type=(%d) content=(%s)\n"
              , cur->parent ? (char*)cur->parent->name : "(NONE)"
              , (char*)cur->name
              , (int)cur->type
              , cur->content ? (char*)cur->content : "(NULL)"
              );
    }
    return 0;
  }
    
  int32_t doParseXML_blocklist_blockvariant(xmlNodePtr cur, BlockInfo& block) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
      if ( xmlStrcmp(cur->name, (const xmlChar *)"blockvariant") == 0 ) {

        // example:
        //   <blockvariant blockdata="0x0" name="Oak Leaves" />

        bool blockDataValid, nameValid, unameValid, colorValid, dcolorValid, spawnableFlagValid;
          
        int32_t blockdata = xmlGetInt(cur, (const xmlChar*)"blockdata", blockDataValid);
        std::string name = xmlGetString(cur, (const xmlChar*)"name", nameValid);
        std::string uname = xmlGetString(cur, (const xmlChar*)"uname", unameValid);
        int32_t color = xmlGetInt(cur, (const xmlChar*)"color", colorValid);
        int32_t dcolor = xmlGetInt(cur, (const xmlChar*)"dcolor", dcolorValid);
        bool spawnableFlag = xmlGetBool(cur, (const xmlChar*)"spawnable", true, spawnableFlagValid);
        
        // create data
        if ( blockDataValid && nameValid ) {
          BlockInfo& bv = block.addVariant(blockdata,name);
          if ( colorValid ) {
            if ( dcolorValid ) {
              color += dcolor;
            }
            bv.setColor(color);
          } else {
            // no color specified, we increment the parent block's color w/ blockdata (to keep it unique)
            color = be32toh(block.color);
            color += blockdata;
            bv.setColor(color);
          }

          if ( unameValid ) {
            bv.setUname(uname);
          }

          if ( spawnableFlagValid ) {
            bv.setSpawnableFlag(spawnableFlag);
          } else {
            bv.setSpawnableFlag(block.spawnableFlag);
          }
        } else {
          // todo error
          fprintf(stderr,"WARNING: Did not find valid blockdata and name for blockvariant of block: (%s)\n"
                  , block.name.c_str()
                  );
        }
      }
      else {
        doParseXml_Unknown(cur);
      }
        
      cur = cur->next;
    }
    return 0;
  }
    
  int32_t doParseXML_blocklist(xmlNodePtr cur) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
      if ( xmlStrcmp(cur->name, (const xmlChar *)"block") == 0 ) {
          
        bool idValid, nameValid, unameValid, colorValid, solidFlagValid, opaqueFlagValid, liquidFlagValid, spawnableFlagValid;
          
        int32_t id = xmlGetInt(cur, (const xmlChar*)"id", idValid);
        std::string name = xmlGetString(cur, (const xmlChar*)"name", nameValid);
        std::string uname = xmlGetString(cur, (const xmlChar*)"uname", unameValid);
        int32_t color = xmlGetInt(cur, (const xmlChar*)"color", colorValid);
        bool solidFlag = xmlGetBool(cur, (const xmlChar*)"solid", true, solidFlagValid);
        bool opaqueFlag = xmlGetBool(cur, (const xmlChar*)"opaque", true, opaqueFlagValid);
        bool liquidFlag = xmlGetBool(cur, (const xmlChar*)"liquid", false, liquidFlagValid);
        bool spawnableFlag = xmlGetBool(cur, (const xmlChar*)"spawnable", true, spawnableFlagValid);

        // create data
        if ( idValid && nameValid ) {
          BlockInfo& b = blockInfoList[id].setName(name);
          b.setId(id);
          if ( colorValid ) {
            b.setColor(color);
          }
          if ( unameValid ) {
            b.setUname(uname);
          }
          
          b.setSolidFlag(solidFlag);
          b.setOpaqueFlag(opaqueFlag);
          b.setLiquidFlag(liquidFlag);
          b.setSpawnableFlag(spawnableFlag);

          // debug
          //fprintf(stderr, "DEBUG: block: %s\n", b.toString().c_str());
          
          doParseXML_blocklist_blockvariant(cur, b);
        } else {
          // todo error
          fprintf(stderr,"WARNING: Did not find valid id and name for block: (0x%x) (%s) (0x%x)\n"
                  , id
                  , name.c_str()
                  , color
                  );
        }
      }
      else {
        doParseXml_Unknown(cur);
      }
      cur = cur->next;
    }
    return 0;
  }

  int32_t doParseXML_itemlist_itemvariant(xmlNodePtr cur, int32_t item_id) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
      if ( xmlStrcmp(cur->name, (const xmlChar *)"itemvariant") == 0 ) {

        // example:
        //   <itemvariant extradata="0x0" name="Ink Sac" />
        
        bool extradataValid, nameValid, unameValid;
          
        int32_t extradata = xmlGetInt(cur, (const xmlChar*)"extradata", extradataValid);
        std::string name = xmlGetString(cur, (const xmlChar*)"name", nameValid);
        std::string uname = xmlGetString(cur, (const xmlChar*)"uname", unameValid);
        
        // create data
        if ( extradataValid && nameValid && unameValid ) {
          itemInfoList[item_id]->addVariant(extradata,name,uname);
        } else {
          // todo error
          fprintf(stderr,"WARNING: Did not find valid extradata and name for blockvariant of block: (%s)\n"
                  , itemInfoList[item_id]->name.c_str()
                  );
        }
      }
      else {
        doParseXml_Unknown(cur);
      }
        
      cur = cur->next;
    }
    return 0;
  }

  int32_t doParseXML_itemlist(xmlNodePtr cur) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
      if ( xmlStrcmp(cur->name, (const xmlChar *)"item") == 0 ) {

        bool idValid, nameValid, unameValid;
          
        int32_t id = xmlGetInt(cur, (const xmlChar*)"id", idValid);
        std::string name = xmlGetString(cur, (const xmlChar*)"name", nameValid);
        std::string uname = xmlGetString(cur, (const xmlChar*)"uname", unameValid);

        // create data
        if ( idValid && nameValid && unameValid ) {
          itemInfoList.insert( std::make_pair(id, std::unique_ptr<ItemInfo>(new ItemInfo(name.c_str(), uname.c_str()))) );
          
          doParseXML_itemlist_itemvariant(cur, id);
        } else {
          // todo error
          fprintf(stderr,"WARNING: Did not find valid id and name for item: (0x%x) (%s)\n"
                  , id
                  , name.c_str()
                  );
        }
      }
      else {
        doParseXml_Unknown(cur);
      }
      cur = cur->next;
    }
    return 0;
  }

  int32_t doParseXML_entitylist(xmlNodePtr cur) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
      if ( xmlStrcmp(cur->name, (const xmlChar *)"entity") == 0 ) {

        bool idValid, unameValid, nameValid, etypeValid;
          
        int32_t id = xmlGetInt(cur, (const xmlChar*)"id", idValid);
        std::string uname = xmlGetString(cur, (const xmlChar*)"uname", unameValid);
        std::string name = xmlGetString(cur, (const xmlChar*)"name", nameValid);
        std::string etype = xmlGetString(cur, (const xmlChar*)"etype", etypeValid);
        if ( ! etypeValid ) {
          etype = "";
        }

        // create data
        if ( idValid && nameValid ) {
          entityInfoList.insert( std::make_pair(id, std::unique_ptr<EntityInfo>(new EntityInfo(name, uname, etype))) );
        } else {
          // todo error
          fprintf(stderr,"WARNING: Did not find valid id and name for entity: (0x%x) (%s)\n"
                  , id
                  , name.c_str()
                  );
        }
      }
      else {
        doParseXml_Unknown(cur);
      }
      cur = cur->next;
    }
    return 0;
  }

  int32_t doParseXML_enchantmentlist(xmlNodePtr cur) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
      if ( xmlStrcmp(cur->name, (const xmlChar *)"enchantment") == 0 ) {

        bool idValid, nameValid, officialNameValid;
          
        int32_t id = xmlGetInt(cur, (const xmlChar*)"id", idValid);
        std::string name = xmlGetString(cur, (const xmlChar*)"name", nameValid);
        std::string officialName = xmlGetString(cur, (const xmlChar*)"officialName", officialNameValid);
          
        // create data
        if ( idValid && nameValid ) {
          std::unique_ptr<EnchantmentInfo> b(new EnchantmentInfo(name.c_str()));
          if ( officialNameValid ) {
            b->setOfficialName(officialName);
          }
          enchantmentInfoList.insert( std::make_pair(id, std::move(b)) );
        } else {
          // todo error
          fprintf(stderr,"WARNING: Did not find valid id and name for enchantment: (0x%x) (%s)\n"
                  , id
                  , name.c_str()
                  );
        }
      }
      else {
        doParseXml_Unknown(cur);
      }
      cur = cur->next;
    }
    return 0;
  }

  int32_t doParseXML_biomelist(xmlNodePtr cur) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
      if ( xmlStrcmp(cur->name, (const xmlChar *)"biome") == 0 ) {

        bool idValid, nameValid, colorValid;
          
        int32_t id = xmlGetInt(cur, (const xmlChar*)"id", idValid);
        std::string name = xmlGetString(cur, (const xmlChar*)"name", nameValid);
        int32_t color = xmlGetInt(cur, (const xmlChar*)"color", colorValid);

        // create data
        if ( idValid && nameValid ) {
          std::unique_ptr<BiomeInfo> b(new BiomeInfo(name.c_str()));
          if ( colorValid ) {
            b->setColor(color);
          }
          biomeInfoList.insert( std::make_pair(id, std::move(b)) );
        } else {
          // todo error
          fprintf(stderr,"WARNING: Did not find valid id and name for biome: (0x%x) (%s)\n"
                  , id
                  , name.c_str()
                  );
        }
      }
      else {
        doParseXml_Unknown(cur);
      }
      cur = cur->next;
    }
    return 0;
  }

  int32_t doParseXML_mcpcToMcpeList(xmlNodePtr cur) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
      int32_t mode = -1;

      if ( xmlStrcmp(cur->name, (const xmlChar *)"block") == 0 ) {
        mode = 1;
      }
      else if ( xmlStrcmp(cur->name, (const xmlChar *)"item") == 0 ) {
        mode = 2;
      }

      if ( mode > 0 ) {

        bool mcpcIdValid, mcpeIdValid;
          
        int32_t mcpcId = xmlGetInt(cur, (const xmlChar*)"mcpcId", mcpcIdValid);
        int32_t mcpeId = xmlGetInt(cur, (const xmlChar*)"mcpeId", mcpeIdValid);

        // create data
        if ( mcpcIdValid && mcpeIdValid ) {

          if ( mode == 1) {
            mcpcToMcpeBlock.insert( std::make_pair( mcpcId, mcpeId ) );
            mcpeToMcpcBlock.insert( std::make_pair( mcpeId, mcpcId ) );
          } else {
            mcpcToMcpeItem.insert( std::make_pair( mcpcId, mcpeId ) );
            mcpeToMcpcItem.insert( std::make_pair( mcpeId, mcpcId ) );
          }
        } else {
          // todo error - better detail
          fprintf(stderr,"WARNING: Did not find valid mcpc_id and mcpe_id for item!\n"
                  );
        }
      }
      else {
        doParseXml_Unknown(cur);
      }
      cur = cur->next;
    }
    return 0;
  }
  
  int32_t doParseXML_xml(xmlNodePtr cur) {
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {

      // todo - should count warning/errors and return this info

      if ( false ) {
      }
      else if ( xmlStrcmp(cur->name, (const xmlChar *)"blocklist") == 0 ) {
        doParseXML_blocklist(cur);
      }
      else if ( xmlStrcmp(cur->name, (const xmlChar *)"itemlist") == 0 ) {
        doParseXML_itemlist(cur);
      }
      else if ( xmlStrcmp(cur->name, (const xmlChar *)"entitylist") == 0 ) {
        doParseXML_entitylist(cur);
      }
      else if ( xmlStrcmp(cur->name, (const xmlChar *)"biomelist") == 0 ) {
        doParseXML_biomelist(cur);
      }
      else if ( xmlStrcmp(cur->name, (const xmlChar *)"enchantmentlist") == 0 ) {
        doParseXML_enchantmentlist(cur);
      }
      else if ( xmlStrcmp(cur->name, (const xmlChar *)"mcpcToMcpeList") == 0 ) {
        doParseXML_mcpcToMcpeList(cur);
      }
      else {
        doParseXml_Unknown(cur);
      }
        
      cur = cur->next;
    }
    return 0;
  }
    
  int32_t doParseXml( const std::string& fn ) {
    xmlDocPtr doc;
    xmlNodePtr cur;

    if ( ! file_exists(fn.c_str()) ) {
      return -1;
    }
      
    doc = xmlParseFile(fn.c_str());
    if (doc == NULL ) {
      // fprintf(stderr,"ERROR: XML Document not parsed successfully.\n");
      return 1;
    }

    // todo - use verboseflag to show all items as they are processed
    fprintf(stderr,"Reading XML from %s\n", fn.c_str());
      
    int32_t ret = 2;
    cur = xmlDocGetRootElement(doc);
    while (cur != NULL) {
      if ( xmlStrcmp(cur->name, (const xmlChar *)"xml") == 0 ) {
        ret = doParseXML_xml(cur);
      }
      else {
        doParseXml_Unknown(cur);
      }
      cur = cur->next;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
      
    return ret;
  }

} // namespace mcpe_viz
