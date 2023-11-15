/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  Requires Mojang's modified LevelDB library (see README.md for details)
  Requires libnbt++ (see README.md for details)

  To build it, use cmake


  todozooz
  * update web ui for new mobs / objects / entities
  * block or item based on ID is no longer valid?! (search on 256 in nbt code)

  maybe?
  * move from xml to json?
  * ui for mobs from xml instead of hard-coded?

  todohere

  * as of beta 1.2.x everything has gone quite mad:
  - cubic chunks now have a palette instead of direct block-id's
  - " " no longer have block light / sky light?! (therefore, can't do check spawnable anymore)



  * as of 0.17 - grass color appears to no longer be a thing that is stored in the data files?  remove related code here and in js?


  * as of 0.16 entity id's have extra data in the high bytes -- collect these and see if we can figure out what they are -- see mcpe.nbt.cc

  * update inventory images for 0.16 and 0.17


  * web - tool to extract schematics for a 3d cube of space -- produce a layer-by-layer depiction of the blocks in the cube
  -- see: https://minecraft.wiki/w/Schematic_file_format  (NBT file format for schematics)
  -- https://irath96.github.io/webNBT/ = web nbt viewer/editor

  * /u/JustinUser -- reset world file outside given area (e.g. X blocks from spawn; given rectangles; etc)

  * see data dump here; http://pastebin.com/tuMyCDyc -- any data we need? (updated: http://pastebin.com/be3dwGFA)
  also: https://www.reddit.com/r/MCPE/comments/4nip1u/updated_the_blocksitems_list_for_0150/

  * tool to show differences between worlds (v1 vs v2 etc)
  -- layer by layer?
  -- geojson?

  * add a tool to help convert from overwold to nether coordinates

  * check minor update to libnbt++ 5/2/2016

  * should error on fail copy js lib files

  * out56.log had a zombie pigman with 'nan' for position - filter this

  * xml missing item 358 (probably filled map); probably other map related things + maps in item frames etc

  * possible xml errors:
  -- another1-dropbox - top of pyramid - quartz with unknown blockdata (6 and 10)
  -- "upper slab, quartz" shows as "upper slab, nether brick" -- R.E. all variants in creative
  -- "flower pot" w/ cactus shows as flower pot with poppy -- R.E. all variants in creative
  -- block of quartz variants appear to be wrong -- R.E. in creative

  * win gui --
  -- icon
  -- use taskbar progress bar - see: http://www.codeproject.com/Articles/42345/Windows-Goodies-in-C-Taskbar-Progress-and-Status

  * could use png files to determine avg color of each block
  * add icons for enchanted items?
  * expand icons idea - use lil images of mobs etc in UI?

  * test web app on mobile -- what can be optimized? smaller tiles?

  * option to geojson items in chests etc (e.g. melon seeds; beetroot seeds)

  * NO -- rethink coordinates used in js -- just negate the y-coord? could then adjust mouse pos output accordingly

  * bug: js in tile mode, zoom to extent, does not show full extent (right side is chopped for 'another1')
  -- also see reddit bartszelag world for extreme example (very wide, not so high world)
  -- ol bug?

  * check spawnable code - it might be missing spots?

  * gui: allow users to easily add cmd-line params
  -- a btn for mcpe_viz --help output (to see usage)
  -- a text field for "Add Params"

  * minimize geojson output?
  -- remove spaces
  -- simplify the properties? e.g. GType=Foo where Foo is the type of this record

  * auto-tile (and useTilesFlag) should be per-dimension
  -- js should use per-dimension tile vars instead of global

  * optimize tiler (slow libpng is probably the limiting factor)

  * finalize new elevation stuff
  -- simplify it?  combine the shaded relief and the alpha into one web app ui control + opacity slider -- or into one layer


  todobig

  * investigate parsing mcpc worlds
  -- create an abstraction lib/base class of libminecraftworld (search on 'todolib')
  ---- move leveldb stuff to something like libminecraftworld.leveldb
  ---- create/adapt a lib for libminecraftworld.anvil (and mcregion)
  ---- read AND write
  ---- one way that ppl can convert mcpe to mcpc:
  ------ https://www.reddit.com/r/MCPE/comments/1y996x/tutorial_how_to_convert_worlds_from_pe_to_pc/
  -- note: PocketMine uses mcregion, anvil or leveldb
  ---- see CrystalKingdom.zip for an example pocketmine world w/ mcregion files (level.dat gzipped?)

  * allow user to provide APK (or file from it) and then use assets from the APK in web app?
  -- e.g. - chest contents w/ icons for items

  * add options or separate tool to do world modification
  -- remove chunks (to allow them to regenerate w/ new features from an updated mcpe)

  * add "interesting" blocks to geojson (this is something like the "layers" idea)
  -- end portal frame
  -- diamonds
  -- ores: gold, iron, coal, redstone, lapis
  -- trees: (by type -- use wood up/down?)
  -- cobwebs
  -- flowers?
  -- crops: beetroot esp + netherwart + melon stem + pumpkin stem; lily pad?
  -- ice + packed ice (how to coalesce?)

  * change git layout
  -- remove winX.zip files; make "release" for each update w/ the winX.zip files
  -- put code in subdirs?

  * android/ios app

  * write script to check for updates to js/ files?

  * check --out -- error if it is a directory

  * parse potions etc in inventories etc

  * do variants for items + entities?

  * do away with image coord space for web app? coords are crazy -- confirmed that negative-Z is north

  * do chunk grid here instead of in web ui? (same func as slime chunks)

  * use boost for filesystem stuff?

  * try with clang; possible to mingw w/ clang?

  * see reddit test1 world -- has this msg (could be result of bad mcedit or similar):
  -- WARNING: Did not find block variant for block(Wooden Double Slab) with blockdata=8 (0x8)

  * join chests for geojson? (pairchest) - would require that we wait to toGeoJSON until we parse all chunks
  -- put ALL chests in a map<x,y,z>; go through list of chests and put PairChest in matching sets and mark all as unprocessed; go thru list and do all, if PairChest mark both as done

  * see if there is interesting info re colors for overview map: https://minecraft.wiki/w/Map_item_format

  * option to name output files w/ world name

  * use "solid" info from block info to do something? could it fix the light map?

  * update block xml w/ transparency info - bool or int? or just solid/nonsolid?
  -- use this transparency (or something else? e.g. spawnable flag?) to determine which block is the top block for purposes of the light map
  -- go and look at wiki to see the type of info that is stored per block

  * convert all printf-style stuff to streams

  * find better hsl/hsv to/from rgb funcs; or really, better way to sort colors so that colortest is more useful


  user suggestions

  * tobomori: ability to hide only a block variant (e.g. Tall Grass)
  -- difficult because of fastBlockHideList etc

  todo

  ** cmdline options:
  save set of slices
  save a particular slice
  draw text on slice files (e.g. Y)
  separate logfiles for overworld + nether + unknown
  options to reduce log file detail

  ** maps/instructions to get from point A (e.g. spawn) to biome type X (in blocks but also in landmark form: over 2 seas left at the birch forest etc); same for items and entities

  todo win32/win64 build

  * immediate crash if using -O2? (and now -O1 and -O)
  * leveldb close() issue -- link fails on missing stream stuff
  * leveldb -O2 build issue -- link fails on missing stream stuff
  * leveldb fread_nolock issue -- link fails; forcing msvcrXXX.a crashes on windows
  * log file: change end line to CRLF?

  ** osx build? cross compile tools look kinda horrible; see https://www.macports.org/

  */

#include <cstdio>
#include <filesystem>
#include <string>
#include <cstdint>
#include <iostream>

#include <leveldb/db.h>
#include <leveldb/cache.h>

#include <boost/program_options.hpp>

#include "define.h"
#include "util.h"
#include "asset.h"
#include "args.h"
#include "control.h"
#include "utils/unknown_recorder.h"
#include "world/world.h"
#include "utils/fs.h"
#include "global.h"
#include "xml/loader.h"


using namespace boost::program_options;

namespace mcpe_viz {

    bool doParseConfigFile(const std::string& fn) {
        if (!file_exists(fn)) {
            return false;
        }

        // todo - this should use streams

        const char* hdr = "";
        int32_t indent = 1;

        FILE* fp = fopen(fn.c_str(), "r");
        if (!fp) {
            log::error("Failed to open file ({}) error={} ({})", fn, strerror(errno), errno);
            return false;
        }
        log::info("Loading config from ({})", fn);

        char buf[1025], * p;
        while (!feof(fp)) {
            memset(buf, 0, 1025);
            fgets(buf, 1024, fp);

            // remove comments and newlines
            if ((p = strchr(buf, '#'))) {
                *p = 0;
            }
            if ((p = strchr(buf, '\n'))) {
                *p = 0;
            }
            if ((p = strchr(buf, '\r'))) {
                *p = 0;
            }

            if ((p = strstr(buf, "hide-top:"))) {
                int32_t dimId = -1;
                int32_t blockId = -1;
                int32_t pass = false;
                if (sscanf(&p[9], "%d 0x%x", &dimId, &blockId) == 2) {
                    pass = true;
                }
                else if (sscanf(&p[9], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to hide list
                    world->dimDataList[dimId]->blockHideList.push_back(blockId);
                }
                else {
                    log::error("{}Failed to parse cfg item 'hide-top': [{}]", makeIndent(indent, hdr), buf);
                }
            }
            else if ((p = strstr(buf, "force-top:"))) {
                int32_t dimId = -1;
                int32_t blockId = -1;
                int32_t pass = false;
                if (sscanf(&p[10], "%d 0x%x", &dimId, &blockId) == 2) {
                    pass = true;
                }
                else if (sscanf(&p[10], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to hide list
                    world->dimDataList[dimId]->blockForceTopList.push_back(blockId);
                }
                else {
                    log::error("{}Failed to parse cfg item 'force-top': [{}]", makeIndent(indent, hdr), buf);
                }
            }
            else if ((p = strstr(buf, "geojson-block:"))) {
                int32_t dimId = -1;
                int32_t blockId = -1;
                int32_t pass = false;
                if (sscanf(&p[14], "%d 0x%x", &dimId, &blockId) == 2) {
                    pass = true;
                }
                else if (sscanf(&p[14], "%d %d", &dimId, &blockId) == 2) {
                    pass = true;
                }
                // check dimId
                if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
                    pass = false;
                }
                if (pass) {
                    // add to list
                    world->dimDataList[dimId]->blockToGeoJSONList.push_back(blockId);
                }
                else {
                    log::error("{}Failed to parse cfg item 'geojson-block': [{}]", makeIndent(indent, hdr), buf);
                }
            }
            else if ((p = strstr(buf, "player-id:"))) {
                if (parsePlayerIdToName(&p[strlen("player-id:")]) == 0) {
                    // all good
                }
                else {
                    log::warn("{}Unparsed config line: [{}]", makeIndent(indent, hdr), buf);
                }
            }

        }

        fclose(fp);
        return true;
    }

    bool searchForAndLoadConfigFile(const char* fileName) {
        // parse cfg files in this order, first found wins:
        // -- home dir
        // -- userprofile dir
        // -- local data dir

        // default config file from home
        if (getenv("HOME")) {
            std::string fnHome = getenv("HOME");
            fnHome += "/.";
            fnHome += fileName;
            if (doParseConfigFile(fnHome))
                return true;
        }

        // default config file from profile directory
        if (getenv("USERPROFILE")) {
            std::string fnHome = getenv("USERPROFILE");
            fnHome += "/.";
            fnHome += fileName;
            if (doParseConfigFile(fnHome))
                return true;
        }

        // local data dir
        if (doParseConfigFile(data_path(fileName).generic_string()))
            return true;

        return false;
    }

    void loadConfigFiles() {
        // as specified on cmdline
        if (!control.fnCfg.empty())
        {
            if (doParseConfigFile(control.fnCfg)) {
                return; // the file the user asked for was loaded, we're done here
            }
        }

        bool nothingLoaded = false;

        // search first for the base file
        if (!searchForAndLoadConfigFile("bedrock_viz.cfg")) {
            nothingLoaded = true;
            log::error("Could not find main config file to load");
        }

        // search second for the local override file
        if (!searchForAndLoadConfigFile("bedrock_viz.local.cfg")) {
            if (nothingLoaded) {
                log::error("Could not find any config files, visualization expected to be useless");
            }
        }
    }

    std::vector<int> parseDimIdOptArgs(const std::vector<std::string>& dimIdStrings) {
		std::vector<int> dimIds;
		for (auto dimIdStr : dimIdStrings) {
			int32_t did = std::stoi(dimIdStr);
			if (did < kDimIdOverworld || did >= kDimIdCount) {
				// Invalid ID
				log::warn("Invalid dimension-id supplied ({}), skipping", did); 
				continue;
			}
			dimIds.push_back(did);
		}
		// If no dims, render all
		if(dimIds.empty()){
			return kDimIdAll;
		}
		return dimIds;
	}

	int32_t parseLimitArgs(const std::vector<std::string>& limitStrings, std::vector<bool>& limit, std::vector<int32_t>& limitMin, std::vector<int32_t>& limitMax) {
		int32_t errct = 0;

		for (auto limitString : limitStrings) {
			int32_t did = 0, min = 0, max = 0;
			if (sscanf(limitString.c_str(), "%d,%d,%d", &did, &min, &max) == 3) {
				if (did < 0 || did >= kDimIdCount) {
					log::warn("Limit's dimension ID ({}) out of range.", did);
					log::error("Failed to parse --limit-x ({})", limitString.c_str());
					errct++;
				}
				else {
					if (max < min) {
						log::warn("X Limits appear to be reversed, swapping...");
						auto tmp = min;
						min = max;
						max = tmp;
					}
					limit[did] = true;
					limitMin[did] = min;
					limitMax[did] = max;
				}
			}
			else {
				log::error("Failed to parse --limit ({})", limitString.c_str());
				errct++;
			}
		}

		return errct;
	}


	int32_t parse_args(int argc, char** argv) {
		options_description desc{"Options"};
		desc.add_options()
			("db", value<std::string>(), "Directory which holds world files (level.dat is in this dir)")
			("outdir", value<std::string>(), "Directory for output files. Defaults to \"./output/\"")
			("xml", value<std::string>(), "XML file containing data definitions")
			("cfg", value<std::string>(), "CFG file containing parsing configuration")
			("detail", "Log extensive details about the world to the log file")
			("hide-top", "Hide a block from top block (did=dimension id, bid=block id)")
			("force-top", "Force a block to top block (did=dimension id, bid=block id)")
			("geojson-block", "Add block to GeoJSON file for use in web app (did=dimension id, bid=block id)")
			("schematic", "Create a schematic file (fnpart) from (x1,y1,z1) to (x2,y2,z2) in dimension (did)")
			("schematic-get", "Create a schematic file (fnpart) from (x1,y1,z1) to (x2,y2,z2) in dimension (did)")
			// ("render-dimension", "Render map images for specified dimensions")
			("all-image", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Create all image types")
			("biome", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Create a biome map image")
			("height-col", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Create a height column map image (red is below sea; gray is sea; green is above sea)")
			("height-col-gs", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Create a height column map image (grayscale)")
			("height-col-alpha", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Create a height column map image (alpha)")
			("shaded-relief", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Create a shaded relief image")
			("slime-chunk", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Create a slime chunk map image")
			("slices", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(),"Create slices (one image for each layer)")
			("movie", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Create movie of layers")
			("movie-dim", "Integers describing the bounds of the movie (UL X, UL Y, WIDTH, HEIGHT)")
			("grid", value<std::vector<std::string>>()->implicit_value(kDimIdAllStrings, kDimIdAllStr)
				->multitoken()->zero_tokens(), "Display chunk grid on top of images")
			("limit-x", value<std::vector<std::string>>(), "did,min,max of X chunk coordinates for dimension")
			("limit-z", value<std::vector<std::string>>(), "did,min,max of Z chunk coordinates for dimension")
			("html", "Create html and javascript files to use as a fancy viewer")
			("html-most", "Create html, javascript, and most image files to use as a fancy viewer")
			("html-all", "Create html, javascript, and *all* image files to use as a fancy viewer")
			("no-force-geojson", "Don't load geojson in html because we are going to use a web server (or Firefox)")
			("no-tile", "Generates single images instead of tiling output into smaller images. May cause loading problems if image size is > 4096px by 4096px")
			("tile-size", value<std::string>(), "Changes tile sizes to specified dimensions (Default: 2048px by 2048px)")
			("shortrun", "Debug testing parameter - process only first 1000 records")
			("leveldb-filer", "Bloom filter supposed to improve disk performance (default: 10)")
			("leveldb-block-size", "The block size of leveldb (default: 4096)")
			("leveldb-try-repair", "If the leveldb fails to open, this will attempt to repair the database. Data loss is possible, use carefully.")
			("verbose", "verbose output")
			("quiet", "supress normal output, continue to output warning and error messages")
			("help", "show basic help info")
			("help-extended", "show extended help info")
			("help-experimental", "show experimental help info");
		

		command_line_parser parser{argc,argv};
		parser.options(desc).allow_unregistered();
		parsed_options parsed_options = parser.run();
		std::vector<std::string> unrecognized_opts = collect_unrecognized(parsed_options.options,exclude_positional);
		if(unrecognized_opts.size()){
			log::error("Invalid parameters detected:");
			for(auto param : unrecognized_opts){
				log::error(param);
			}
			return -1;
		}
		variables_map vm;
		store(parsed_options, vm);
		notify(vm);

		int32_t errct = 0;

		control.init();
		
		try {
			//--outdir dir
			if (vm.count("outdir")) {
				control.outputDir = vm["outdir"].as<std::string>();
			}
			// --xml fn
			if (vm.count("xml")) {
				control.fnXml = vm["xml"].as<std::string>();
			}
			// --cfg fn
			if (vm.count("cfg")) {
				control.fnCfg = vm["cfg"].as<std::string>();
			}
			// --db dir
			if (vm.count("db")) {
				control.dirLeveldb = vm["db"].as<std::string>();
			}
			// --detail
			if (vm.count("detail")) { 
				control.doDetailParseFlag = true;
			}
			// --leveldb-filter i
			if (vm.count("leveldb-filter")) {
				control.leveldbFilter = vm["leveldb-filter"].as<int>();
				if (control.leveldbFilter < 0) {
					control.leveldbFilter = 0;
				}
			}
			// --leveldb-block-size i
			if (vm.count("leveldb-block-size")) {
				control.leveldbBlockSize = vm["leveldb-block-size"].as<int>();
				if (control.leveldbBlockSize < 0) {
					control.leveldbBlockSize = 4096;
				}
			}
			// --leveldb-try-repair
			if (vm.count("leveldb-try-repair")) {
				control.tryDbRepair = true;
			}
			// --hide-top=did,bid
			if (vm.count("hide-top")) {
				bool pass = false;
				int32_t dimId, blockId;
				std::string optarg = vm["hide-top"].as<std::string>();
				if (sscanf(optarg.c_str(), "%d,0x%x", &dimId, &blockId) == 2) {
					pass = true;
				}
				else if (sscanf(optarg.c_str(), "%d,%d", &dimId, &blockId) == 2) {
					pass = true;
				}

				if (pass) {
					// check dimId
					if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
						pass = false;
					}
					if (pass) {
						world->dimDataList[dimId]->blockHideList.push_back(blockId);
					}
				}

				if (!pass) {
					log::error("Failed to parse --hide-top {}", optarg.c_str());
					errct++;
				}
			}	
			// --force-top=did,bid
			if (vm.count("force-top")) {
				bool pass = false;
				int32_t dimId, blockId;
				std::string optarg = vm["force-top"].as<std::string>();
				if (sscanf(optarg.c_str(), "%d,0x%x", &dimId, &blockId) == 2) {
					pass = true;
				}
				else if (sscanf(optarg.c_str(), "%d,%d", &dimId, &blockId) == 2) {
					pass = true;
				}

				if (pass) {
					// check dimId
					if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
						pass = false;
					}
					if (pass) {
						world->dimDataList[dimId]->blockForceTopList.push_back(blockId);
					}
				}

				if (!pass) {
					log::error("Failed to parse --force-top {}", optarg.c_str());
					errct++;
				}
			}
			// --geojson-block=did,bid
			if (vm.count("geojson-block")) {
				bool pass = false;
				int32_t dimId, blockId;
				std::string optarg = vm["geojson-block"].as<std::string>();
				if (sscanf(optarg.c_str(), "%d,0x%x", &dimId, &blockId) == 2) {
					pass = true;
				}
				else if (sscanf(optarg.c_str(), "%d,%d", &dimId, &blockId) == 2) {
					pass = true;
				}

				if (pass) {
					// check dimId
					if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
						pass = false;
					}
					if (pass) {
						world->dimDataList[dimId]->blockToGeoJSONList.push_back(blockId);
					}
				}

				if (!pass) {
					log::error("Failed to parse --geojson-block {}", optarg.c_str());
					errct++;
				}
			}
			// --schematic did,x1,y1,z1,x2,y2,z2,fnpart
			// --schematic-get did,x1,y1,z1,x2,y2,z2,fnpart
			if (vm.count("schematic") || vm.count("schematic-get")) {
				bool pass = false;
				int32_t dimId = 0, x1 = 0, y1 = 0, z1 = 0, x2 = 0, y2 = 0, z2 = 0;
				char fnSchematic[2048];
				memset(fnSchematic, 0, 2048);
				std::string optarg;
				if (vm.count("schematic")) {
					optarg = vm["schematic"].as<std::string>();
				}
				if (vm.count("schematic-get")) {
					optarg = vm["schematic-get"].as<std::string>();
				}
				// todo - ugly sscanf for a string
				if (sscanf(optarg.c_str(), "%d,%d,%d,%d,%d,%d,%d,%s", &dimId, &x1, &y1, &z1, &x2, &y2, &z2, fnSchematic) ==
					8) {
					pass = true;
				}

				if (pass) {
					// todo - check params
					if (dimId < kDimIdOverworld || dimId >= kDimIdCount) {
						pass = false;
					}

					if (pass) {
						world->dimDataList[dimId]->addSchematic(x1, y1, z1, x2, y2, z2, fnSchematic);
					}
				}

				if (!pass) {
					log::error("Failed to parse --schematic {}", optarg.c_str());
					errct++;
				}
			}
			// --grid[=did]
			if (vm.count("grid")) {
				control.doGrid = parseDimIdOptArgs(vm["grid"].as<std::vector<std::string>>());
			}
			// --limit-x did,min,max
			if (vm.count("limit-x")) {
				auto errc = parseLimitArgs(
					vm["limit-x"].as<std::vector<std::string>>(),
					control.limitX,
					control.limitXMin,
					control.limitXMax);
				errct += errc;
			}
			// --limit-z did,min,max
			if (vm.count("limit-z")) {
				auto errc = parseLimitArgs(
					vm["limit-z"].as<std::vector<std::string>>(),
					control.limitZ,
					control.limitZMin,
					control.limitZMax);
				errct += errc;
			}
			// --html
			if (vm.count("html")) {
				control.doHtml = true;
			}
			// --tile-size[=tilew,tileh]
			if (vm.count("tile-size")) {
				std::string optarg = vm["tile-size"].as<std::string>();
				if (!optarg.empty()) {
					int32_t tw, th;
					if (sscanf(optarg.c_str(), "%d,%d", &tw, &th) == 2) {
						control.tileWidth = tw;
						control.tileHeight = th;
						log::info("Overriding tile dimensions: {} x {}", tw, th);
					}
					else {
						log::error("Failed to parse --tile-size ({})", !optarg.empty() ? optarg : "null");
						errct++;
					}
				}
			}
			// --no-tile
			if (vm.count("no-tile")) {
				control.doTiles = false;
			}
			// --html-most
			if (vm.count("html-most")) {
				control.doHtml = true;
				control.doImageBiome =
					control.doImageHeightCol =
					control.doImageHeightColGrayscale =
					control.doImageHeightColAlpha =
					control.doImageShadedRelief =
						kDimIdAll;
                control.doImageSlimeChunks = kDimJustOverworld;
			}
			// --html-all
			if (vm.count("html-all")) {
				control.doHtml = true;
				control.doImageBiome =
					control.doImageHeightCol =
					control.doImageHeightColGrayscale =
					control.doImageHeightColAlpha =
					control.doImageShadedRelief =
					control.doSlices =
                        kDimIdAll;
                control.doImageSlimeChunks = kDimJustOverworld;
			}
			// --no-force-geojson
			if (vm.count("no-force-geojson")) {
				control.noForceGeoJSONFlag = true;
			}
			// --biome[=did]
			if (vm.count("biome")) {
				control.doImageBiome = parseDimIdOptArgs(vm["biome"].as<std::vector<std::string>>());
			}
			// --height-col[=did]
			if (vm.count("height-col")) {
				control.doImageHeightCol = parseDimIdOptArgs(vm["height-col"].as<std::vector<std::string>>());
			}
			// --height-col-gs[=did]
			if (vm.count("height-col-gs")) {
				control.doImageHeightColGrayscale = parseDimIdOptArgs(vm["height-col-gs"].as<std::vector<std::string>>());
			}
			// --height-col-alpha[=did]
			if (vm.count("height-col-alpha")) {
				control.doImageHeightColAlpha = parseDimIdOptArgs(vm["height-col-alpha"].as<std::vector<std::string>>());
			}
			// --shaded-relief[=did]
			if (vm.count("shaded-relief")) {
				control.doImageShadedRelief = parseDimIdOptArgs(vm["shaded-relief"].as<std::vector<std::string>>());
			}
			// --slime-chunk[=did]
			if (vm.count("slime-chunk")) {
				control.doImageSlimeChunks = parseDimIdOptArgs(vm["slime-chunk"].as<std::vector<std::string>>());
			}
			// --all-image[=did]
			if (vm.count("all-image")) {
				control.doImageBiome =
					control.doImageHeightCol =
					control.doImageHeightColGrayscale =
					control.doImageHeightColAlpha =
					control.doImageShadedRelief =
					control.doImageSlimeChunks =
					parseDimIdOptArgs(vm["all-image"].as<std::vector<std::string>>());
			}
			// --slices[=did]
			if (vm.count("slices")) {
				control.doSlices = parseDimIdOptArgs(vm["slices"].as<std::vector<std::string>>());
			}
			// --movie[=did]
			if (vm.count("movie")) {
				control.doMovie = parseDimIdOptArgs(vm["movie"].as<std::vector<std::string>>());
			}
			// --movie-dim x,y,w,h
			if (vm.count("movie-dim")) {
				std::string optarg = vm["movie-dim"].as<std::string>();
				// movie dimensions
				if (sscanf(optarg.c_str(), "%d,%d,%d,%d", &control.movieX, &control.movieY, &control.movieW,
					&control.movieH) == 4) {
					// good
				}
				else {
					log::error("Failed to parse --movie-dim ({})", optarg.c_str());
					errct++;
				}
			}
			// --shortrun
			if (vm.count("shortrun")) {
				control.shortRunFlag = true;
			}
			// --verbose
			if (vm.count("verbose")) {
				control.verboseFlag = true;
			}
			// --quiet
			if (vm.count("quiet")) {
				control.quietFlag = true;
			}
			// --help
			if (vm.count("help")) {
				control.helpFlags = HelpFlags::Basic;
				return -1;
			}
			// --help-extended
			if (vm.count("help-extended")) {
				control.helpFlags = HelpFlags::Basic | HelpFlags:: Extended;
				return -1;
			}
			// --help-experimental
			if (vm.count("help-experimental")) {
				control.helpFlags = HelpFlags::Basic | HelpFlags::Extended | HelpFlags::Experimental;
				return -1;
			}
			// unknown option
		//            default: {
		//                log::error("Unrecognized option: '{}'", optc);
		//                return -1;
		//            }
		//            }
		}
		catch (const error &ex) {
			log::error(ex.what());
			return -1;
		}

        // todobig - be more clever about dirLeveldb -- allow it to be the dir or the level.dat file

        // verify/test args
	    if (control.dirLeveldb.length() <= 0) {
            errct++;
            log::error("Must specify --db");
        }

        if (!file_exists(control.outputDir.generic_string())) {
            local_mkdir(control.outputDir.generic_string());
        }
     
        // make sure that output directory is NOT world data directory
        std::string fnTest = (control.outputDir / "level.dat").generic_string();
        if (file_exists(fnTest)) {
            errct++;
            log::error("You cannot send mcpe_viz output to a world file data directory");
        }
        return errct;
    }

}  // namespace mcpe_viz

int main(int argc, char** argv)
{
    using namespace mcpe_viz;
    set_argv_0(argv[0]);
    setup_logger_stage_1();
    // steps:
    // 1. load args from argv
    // 2. check args, exit and print usage if any error found
    // 3. load xml and cfg
    // 4. create then world
    // 5. parse world
    // 6. generate output
    // 7. print warning of unknown blocks/items
    
    world = std::make_unique<MinecraftWorld_LevelDB>();

    if (parse_args(argc, argv) != 0) {
        mcpe_viz::print_usage(control.helpFlags);
        return -1;
    }
    if (control.doHtml) {
        listGeoJSON.clear();
    }
    auto console_log_level = control.quietFlag ? Level::Warn : (control.verboseFlag ? Level::Debug : Level::Info);
    auto file_log_level = control.verboseFlag ? Level::Trace : Level::Debug;

    setup_logger_stage_2(control.logFile(), console_log_level, file_log_level);

    {
        int ret = 0;
        if (control.fnXml.empty()) {
            ret = load_xml(data_path("bedrock_viz.xml").generic_string());
        }
        else {
            ret = load_xml(control.fnXml);
        }
        if (ret != 0) {
            log::error("Failed to load xml file");
            return -1;
        }
    }
    
    loadConfigFiles();
    
    world->init();
    world->dbOpen(std::string(mcpe_viz::control.dirLeveldb));
    // todobig - we must do this, for now - we could get clever about this later
    // todobig - we could call this deepParseDb() and only do it if the user wanted it
    if (true || mcpe_viz::control.doDetailParseFlag) {
        world->dbParse();
    }
    world->doOutput();
    world->dbClose();

    print_unknown_warnings();
    log::info("Done.");
    return 0;
}
