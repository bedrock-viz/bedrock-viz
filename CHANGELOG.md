# Changelog

## Repo Info
- Repository      : https://github.com/bedrock-viz/bedrock-viz/
- Maintained by   : [jasper-wan](https://github.com/jasper-wan), [et al](https://github.com/orgs/bedrock-viz/people)
- Contributions   : [Contributors](./CONTRIBUTORS.md)
- Latest highlight: Caves & Cliffs; Blocks & Items, and tooling to make that easier
- Latest release  : 0.1.4

## bedrock_viz changelog

v0.1.4 - 2021.07.04
- Parser changes
  - new utility to parse the xml data, check for conflicting colors and write out a legend file
- Fixes in this release from:
  - [cabbey](https://github.com/cabbey)

v0.1.3 - 2021.01.01

- Viewer changes
  - Extraneous comments from template removed from generated HTML
  - Signs that don't use all 4 lines of text no longer break object display
  - Embedded images for chests fixed
  - Villagers and Pillagers should now both show in the mob menu correctly
  - Mob menu should fit on screen better
- Parser changes
  - Command line options which take a Dimension ID now accept a list of them
  - Tiled output is now the default, an option for non-tiled is available
  - Extensive updates to known blocks: consistent names, missing ids, unique colors
  - More Nether updates: Netherite Items are now recognized, added missing Nether Block variants
- Housekeeping
  - Improvements to the release processing scripts and source control
  - A lot of re-structuring of the codebase has happened to make it more developer friendly
- Fixes in this release from:
  - [jasper-wan](https://github.com/jasper-wan)
  - [paulgrahek](https://github.com/paulgrahek)
  - [cabbey](https://github.com/cabbey) (first time contributor, new dev team member)
  - [hoersten](https://github.com/hoersten) (first time contributor)

v0.1.2 - 2020.06.28

- Viewer changes
  - bedrock-viz rebranding
  - new version checking routine
  - updated default map position
  - added marked.js to version view for changelog display
- Parser changes
  - Code cleanup / bedrock-viz rebranding
  - Block-id changes (special thanks to [picotazo](https://github.com/picotazo) for starting this)
  - Nether update (1.16) support
  - Removed some nbt processing that was a massive performance hit on parsing.
  - Memory leak fix
  - --help changes to make for a better user experience
  - data/static paths are used properly now
  - more logging cleanup
  - leveldb repair option
- Documentation updates
  - Build updated
  - Changelog updated
  - Contributors updated

v0.1.1 - 2019.12.11

- Many fixes and cleanup of code

v0.1-alpha - 2019.12.07

- Initial release from fork of https://github.com/Plethora777/mcpe_viz


## mcpe_viz changelog

v0.0.48 - 2018.12.30  
More fixes for crashing on --html-all  

v0.0.47 - 2018.12.29  
Forgot to build win32 + win64 for v46 :)  

v0.0.46 - 2018.12.28  
Fix for repeated "NBT exception" on worlds with mixed old and new chunk format  
Moved "ERROR: Invalid blockid=" from screen log to log file -- underlying issue still needs to be fixed  
Fix Turtle id in web ui  

v0.0.45 - 2018.12.28  
Report unmapped remote player id's to the user (make it easier for people to add new playerid mappings)  
Moved water creatures to a separate list on passive mobs; Added Cod, Dolphin and Turtle  

v0.0.44 - 2018.12.27  
Another fix for crash on --html-all.  It's still a bit broken in some cases but hopefully will not crash.  

v0.0.43 - 2018.12.26  
Fix for crash on --html-all  

v0.0.42 - 2018.12.25  
More fixes for new name-based items and blocks  
Detect and avoid problems with block palette in new chunk format  
XML - More fixes for unames and variants; items and entites can have multiple unames  

v0.0.41 - 2018.12.25  
More fixes for new name-based items and blocks  
Better detection of block vs item in inventories etc  
XML - Fixed a few typos; added scaffolding (with fake id)  

v0.0.40 - 2018.12.24  
More fixes for new name-based items and blocks  
XML - Fixed a few typos  

v0.0.39 - 2018.12.23  
Fixes for new name-based items and blocks  
XML - Added uname's for items  
XML - Changed entity ids to uname  
Special thanks to @flagmaggot for pushing in the right direction!  

v0.0.38 - 2018.5.23  
Fixes for new new chunk version (v8)  
Does not address separate storage for water overlay  

v0.0.37 - 2018.4.14  
Fixed issue with parsing new paletted chunk format (bedrock appearing on y=15 etc)  
Added ability to have multiple uname's for a block (use a semicolon to separate)  
XML - Added some uname's that were missing (there are likely TONS more)  

v0.0.36 - 2018.4.9  
Added parsing code for (crazy) new paletted chunk format  
Deprecated --spawnable because the new chunk format does not store block light info  
Note that you will see many warnings about missing variants etc -- working on it :)  
Added "uname=" to XML for blocks to support new paletted chunk format  

v0.0.35 - 2017.11.29  
Fix for weird dimension id from converted world (xbox to bedrock version)  
Added check to ensure users do not put output in world file directory  
Added entity lookup by string id (new to v1.2.x?)  
Added Shulker Box object  
Tweaked XML file for Glazed Terracotta  
Added a few missing items in XML file  
Added idstring for entities in XML file  
Added even more missing stuff from XML file (thanks to zetoken!)  
Updated OpenLayers to v4.5.0  

v0.0.34 - 2017.08.15  
Fix for MCPE v1.2.0 (updated leveldb library)  

v0.0.33 - 2017.08.14  
Fix for MCPE v1.2.0 (sign text issue)  

v0.0.33 - 2017.06.03  
Fix for MCPE v1.1.0  

v0.0.31 - 2017.01.13  
Added support for Ender Chest contents (viewable in 'Player' entities)  
Big XML cleanup (thanks to @Shkyrockett)  
Tabs be gone! (thanks to @shellbj)  
Minor README updates - notably, @shellbj reports that mcpe_viz will compile & work on Mac OS X  
Added parsing for a few new chunk types (AutonomousEntities, dimension)  

v0.0.30 - 2016.12.28  
Fixed a few entity id's off-by-one (Thanks to Shkyrockett & Jocopa3)  

v0.0.29 - 2016.12.26  
Fixed check spawnable for cubic chunks (again)  

v0.0.28 - 2016.12.25  
Fixed check spawnable for cubic chunks  

v0.0.27 - 2016.12.24  
Quick fix for The End -- fix crash  
Quick fix for Biomes -- apparently grass color is no longer stored in the world files, only height + biome  

v0.0.26 - 2016.12.23  
Quick fix for force-top in cubic chunks  

v0.0.25 - 2016.12.23  
Added support for MCPE v1.0 -- cubic chunks, 256 build height, etc  
Note: --check-spawnable is not yet supported for cubic chunks  
Fixed issue with escaping special characters in sign text  
XML - Fixed issue with stained clay being identified as stained glass  

v0.0.24 - 2016.11.14  
Added new blocks, items and mobs for MCPE v0.16 and v0.17 beta -- special thanks to @Jocopa3 for his block/item/entity list  
Work around new oddness with entity id's (appear to be flags in the upper bytes of entity id's)  
XML - Partially sync'ed our list with Jocopa3's  
XML - Cleaned up a bit  
Web - Fixed "off-by-one" in chunk grid display on the z-axis  
Web - Updated to OpenLayers 3.19.1  
Web - Updated to Bootstrap 3.3.7  

v0.0.23 - 2016.06.23  
Added proper slime chunk finding (algorithm credit: MCPE slime-chunk checker; reverse engineered by @protolambda and @jocopa3)  
Web - Added colored labels for passive / hostile mobs  
Added very preliminary support for extracting schematics from world files (see --schematic-get)  
Pass more block/item information along to javascript  
XML - added entity type attribute (etype)  

v0.0.22 - 2016.06.03  
Added new blocks, items and mobs for MCPE v0.15 beta  
Web - Added option to hide mobs more than 128 blocks from player  
Web - Fixed inconsistency in chunk grid display near the zero crossings for X and Z  
Fixed issue with outputing entities with invalid positions (NaN)  
XML - Fixed issue with Quartz block variants  

v0.0.21 - 2016.05.15  
Parse villages (mVillages) NBT data and put them in geojson  
Web - Added display of detailed information about villages (doors, radius, etc)  
Web - Added display of objects (Tile Entities): Hopper, Dropper, Dispenser  
Web - Added option to show Nether-to-Overworld (and vice versa) coordinate conversion  
Web - Added option to display chunk coordinates  
Web - Added hotkeys for chunk grid ('G'), nether coordinate conversion ('N')  
Web - Fixed bug setting extent which caused newer versions of OpenLayers to stretch the image oddly  
Web - Updated to OpenLayers 3.15.1  
XML - Added bucket item variants ('empty', 'water', 'lava')  

v0.0.20 - 2016.02.25  
Updated code to support truly huge world files (e.g. 1gb 33000 x 26000!)  
Web - Added bounding circle for spawnable blocks areas  
XML - Added a few missing rabbit items  

v0.0.19 - 2016.02.06  
Web - Added block and item images for item / inventory display;  
Web - Added option to view icon images or text for items/inventories;  
Web - Added previous / next buttons to feature popover when clicking a point that has multiple features;  
Web - Added passive mob Rabbit;  
Web - Added hostile mob Witch;  
Web - Added objects: Boat, Minecart, Minecart with Chest, Minecart with Hopper, Minecart with TNT;  
Web - Added Options: 'About MCPE Viz';  
Web - Added warning message about slime chunks calculation;  
Web - Simplified code for 'Add All' / 'Hide All';  
Web - Attempt to center geojson items in image pixels (still not quite right);  
Web - Improved geojson output for block and item variants;  
Web - Added a 'VillagerType' summary field for Villagers;  
XML - Added item variants (e.g. potions);  
XML - Added method to map from MCPC block/item id's to MCPE block/item id's;  
Added code to help convert wiki block/item renders to icon images;  
Added optimized file copy function for all platforms  

v0.0.18 - 2016.01.31  
Added support for new 0.14 blocks, items and mobs;  
Web - Removed unneeded spaces from GeoJSON files to reduce file size;  
Readme - Added links for info on how to do world backups and info on using dropsync;  
Code cleanup heading towards making a mcpe-leveldb library;  
Changed many 'int' to 'int32_t';  
Simplified win32 and win64 build (CMake and build script) - results in smaller .exe files;  
Re-enabled optimization on win32 and win64 builds (leveldb still can't have anything but -O0);  
Simplified win32/win64 builds for libnbt++ and leveldb;  
Added leveldb bloom filter with default of 10 bits (appears to be a bit faster when doing --html-all);  
Added (hidden) command-line options for leveldb block_size and bloom filter bits;  
Found and fixed super obscure issue in leveldb code (added lpContext=NULL) to fix crashes in win32 and win64  

v0.0.17 - 2015.12.26  
Web - added better display of remote players;  
CFG - added config file option 'player-id' (see mcpe_viz.cfg for details);  
CFG - added special mcpe_viz.local.cfg file option - you can use this for player-id mappings;  
Parser - added leveldb record type 'idcounts' content still unknown;  
Code cleanup of std::string passing  

v0.0.16 - 2015.12.14  
Web - Added support for finding mob spawnable blocks.  See the '--check-spawn' option;  
Web - For features, "Pos" element is now clickable to go to the indicated layer;  
Web - Moved output images to subdirectory 'images/';  
Web - Enforce layer z-index ordering to improve interaction of the many layers we now have;  
XML - Added flags for blocks: opaque, liquid, spawnable;  
XML - Added stair blockvariants;  
XML - Fixed parsing of boolean values;  
Added global histograms for biomes and blocks to the log file-- histograms are sorted and show percentages;  
Web - Updated OpenLayers from v3.11.1 to v3.11.2;  

v0.0.15 - 2015.11.19  
Web - Quick fixes for new modal dialogs;  
GUI - Quick fix for quoting executable in command-line;  
Version bump  

v0.0.14 - 2015.11.19  
Web/GUI - Quick fixes for changelog parsing;  
Version bump  

v0.0.13 - 2015.11.19  
Web - Added support for tiled images - this allows us to handle very large MCPE worlds;  
GUI - Added 'auto-tile' to mcpe_viz run so that images are automatically tiled if they are big;  
Web - Added pre-generated shaded relief layer because dynamic shaded relief does not (currently) work in tiled image mode;  
Web - Added height shading layer;  
Web - Fixed handling of player position with respect to the Nether;  
Web - Tweaked layout of controls for 'Options';  
Web - Fixed updating slider labels in UI;  
Web - Changed all alert() to bootstrap modal dialogs;  
GUI - Added command-line text field that users can copy to modify the way mcpe_viz runs;  
GUI/Web - Added new version 'highlight' to update checker;  
Added ChangeLog;  
Version bump  

v0.0.12 - 2015.11.16  
Web - Added 'Blocks' menu where you can show blocks in the web app - controlled via geojson-block command-line switch and/or config file entries;  
Version bump  

v0.0.11 - 2015.11.15  
GUI - Added button to check for updates on GitHub;  
Web - Added option to check for updates on GitHub;  
Web - Keep drop-up menu visible after clicking a selection (suggestion from TheThoughtNinja);  
Web - Name mob spawners with name of mob spawned (suggestion from TheThoughtNinja);  
Web - Added 'Sign (Non-blank)' and 'Sign (Blank)';  
Web - Show item name for 'Dropped item'
Version bump  

v0.0.10 - 2015.11.13  
Web - Added Help -- a guided tour of the web app UI;  
Web - Added bundled javascript libraries -- web app will now work offline;  
Updated README.md;  
Version bump  

v0.0.9 - 2015.11.9  
GUI - Wrap mcpe_viz arguments in quotes to avoid issues with path names on Windows;  
GUI - Add version number in UI;  
Share mcpe_viz.version.h with main code and gui code;  
Version bump  

(Older changes are in "git log" :)  

	
	

