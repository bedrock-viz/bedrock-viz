# MCPE Viz
by Plethora777 -- Twitter: [@Plethora777](https://twitter.com/Plethora777) -- Reddit: [/u/Plethora777](https://www.reddit.com/user/Plethora777/)

MCPE Viz is a tool that parses Minecraft Pocket Edition (MCPE) world files.  It creates overview images from the world files, and reports *lots* of details on your world.

It can also create a powerful Google Maps-like Web App for viewing your world.

Additional Features:

* Create a powerful Google Maps-like web app viewer for your world.
  * Browse through all 256 layers of your world (Overworld and Nether) from bedrock to build limit.
  * Examine the location and details of mobs and items in your world.
  * Show blocks where mobs can spawn.
  * Visualize chunk boundaries -- consider it an extremely cumbersome F3/debug mode!
  * Find slime chunks -- well, probably... slime chunk finding code is currently a bit weird in MCPE, it appears to work, but please be sure to confirm slimes are spawning in a chunk before you do a ton of work :)
  * Explore the biomes of your world.
  * Shaded-relief elevation overlay -- get a sense of the topography of your world.
  * Measure distances in your world

* Create many types of images:
  * All layers of your world
  * Biomes in your world (very interesting!)
  * Block light in your world (kind of like an image from space at night)
  * Block height in your world (kind of like a topographic map)
  * Grass color in your world (it's neater than it sounds)

* Create movies of all layers from bedrock to build limit (*ffmpeg* required)

* Dump a *huge* amount of info about your world including:
  * Dumps detailed information about all mobs, players and items in your world
  * For each 16x16 block chunk:
    * Top blocks (block type, block data, biome)
    * Histogram of all blocks in the chunk
    * And more...
  * For each world type (e.g. overworld, nether):
    * Histogram of blocks
    * Histogram of biomes (per-block column)


## Example Output

[Here are example images](http://imgur.com/a/YiIWe)

[Here's an example of movie output](https://www.youtube.com/watch?v=kBCOrctFIGM)


## Quick Start -- How to run it on Windows with a GUI

* [Download the zip file](https://github.com/Plethora777/mcpe_viz/archive/master.zip)

* Unzip that file.

* Inside you will find "mcpe_viz.win32.zip" (windows 32-bit) and "mcpe_viz.win64.zip" (windows 64-bit).  Choose one that is appropriate for your system.  Unzip it.

* Inside you will find mcpe_viz_helper.exe

* Run mcpe_viz_helper.exe

**DO NOT RUN THIS ON YOUR ORIGINAL MCPE DATA FILES; MAKE A BACKUP COPY OF YOUR DATA AND RUN THIS AGAINST THAT COPY ONLY!**

Use the app to select the MCPE world file, the output directory, output name, and mode.  Press "Go!".  Watch the progress of mcpe_viz.  If it completes successfully, you can click the "Launch Web App" button.  If there was a problem, you should look in the "Progress" window for details that might help you resolve the issue.

See below for Web App usage notes.


## Quick Start -- How to run it on Windows from the command-line

* Same as above to get the files.

* Open a command prompt and run it something like this:

```
mcpe_viz.exe --db path-to-your-world-file-dir/ --out out1 --html-most
```

Note: Replace "path-to-your-world-file-dir/" with the actual directory which contains your world files -- it wants the directory that has level.dat in it.

**DO NOT RUN THIS ON YOUR ORIGINAL MCPE DATA FILES; MAKE A BACKUP COPY OF YOUR DATA AND RUN THIS AGAINST THAT COPY ONLY!**

This will produce a bunch of image files and "out1.html". Open "out1.html" in your browser.


## Web App

You can create a browser-based viewer for your world files.  It will allow you to pan, zoom and switch between the different images.  It's pretty spiffy!

To make a quick viewer (with all regular images, but without indvidual layers), do something like this:

```
> ./mcpe_viz --db ./mcpe/another1/ --out ./mcpe/output/out1 --html-most
```

To make a complete viewer (with all images and all layers), do something like this:

```
> ./mcpe_viz --db ./mcpe/another1/ --out ./mcpe/output/out1 --html-all
```

This will create a LOT of images -- individual images for each layer of the overworld and nether, as well as all the regular images.  It will also create "./mcpe/output/out1.html".  Load this file in your browser and you will have an Openlayers-based viewer for your world!  Openlayers (http://openlayers.org/) is a powerful javascript library for mapping.

Web App Usage Notes:
* Zoom in with double click or press the "+" button (upper left)
* Zoom out with shift-double click or press the "-" button (upper left)
* Move with click and drag
* Rotate with shift-alt click and drag
* Your mouse position is shown in the upper right in world coordinates and image coordinates
* You can go full-screen (button under mouse position window)
* Tool bar at the bottom of the screen:
  * Choose Overworld or Nether
  * Scroll through the layers from 0 (bedrock) to 127 (build limit)
  * Choose Overview, Biome etc
  * Show Mobs and Objects -- click on an entry and the map will be updated with labeled points.  You can toggle on as many different types as you like.  Click on the points or labels to get more info on the item.  Note that labels are not drawn when you are zoomed out.
  * Enable elevation overlay (shaded relief)
  * Enable chunk grid overlay

You can visualize areas that are mob spawnable using the mcpe_viz command-line switch '--check-spawn'.  For example, '--check-spawn=0,-1,-152,180' will find all spawnable blocks in the overworld centered at -1,-152 with a radius of 180.  In the web app, you can toggle the "Spawnable" blocks on using the option on the "Blocks" menu.  The icons for the spawnable area are purple dots by default.  You can click on these dots to see the details.  You can then click on the "Pos" element to go to that layer (if you ran mcpe_viz with --html-all).  When you are viewing a raw layer (e.g. layer 12) as opposed to "Overview", the icons will change into green up arrows (indicating the spwanable block is above this layer), red down arrows (indicating the spawnable block is below this layer), or white squares (indicating the spawnable block is on this layer).  Keep in mind that the spawnable block is *above* the solid block the mob could spawn on.


## Web App Notes

If you are loading the web app from a local file -- that is, not accessing it from a web server -- your browser may impose restrictions on image loading and access to image pixels which makes it very difficult for the web app to deliver the best experience.  Currently, Firefox appears to be the browser most willing to play nicely.  The web app will let you know if you are impacted by this issue.  For example, on Chrome, you will not see block information on mouse over, or be able to see the elevation overlay -- both of which are totally cool :)

If you are running Firefox or serving the files from a web server, you can make the web app load a little bit faster by adding the option "--no-force-geojson" to your command line.  This prevents mcpe_viz from using a workaround for the above issue.


## Usage

**DO NOT RUN THIS ON YOUR ORIGINAL MCPE DATA FILES**

**DO NOT RUN THIS ON YOUR ONLY BACKUP OF MCPE DATA FILES**

**MAKE A COPY OF YOUR DATA AND RUN THIS AGAINST THAT COPY ONLY!**

See "./mcpe_viz --help" for the most up-to-date usage info

Here's an example invocation:

```
> ./mcpe_viz --grid --db ./mcpe/another1/ --out ./mcpe/output/out1
```

This will read the leveldb from "./mcpe/another1" and name output files starting with "./mcpe/output/out1", and it will draw chunk boundaries on your output image.  This also dumps the *voluminous* output to "out1.log".  The log file has a *ton* of interesting information about your world.  "grep" is your friend.

Please note that --db expects the directory which contains "level.dat".


## How to backup your MCPE world files

On Android, [see this link](http://pocketbeta.minecraft.net/p/how-to-make-backup.html).

On iOS or Windows 10, [see this link](https://www.reddit.com/r/MCPE/comments/3h28ku/how_to_backuptransfer_worlds/)

Note that MCPE is sometimes weird about actually closing world files -- that is, you think that MCPE is closed and should be done with the world file, but a backup of it may not work correctly.  To be certain that you get a good backup, you should reboot and then do the backup *before* starting MCPE again.

### Automated backup to Dropbox using DropSync (Android)

On Android, you can use [DropSync](https://play.google.com/store/apps/details?id=com.ttxapps.dropsync) to automatically backup your MCPE world to your dropbox account.  You configure this tool to backup any of your world files to dropbox.  As you play on your device, DropSync will copy the updated world files to Dropbox.  Very cool!  Install Dropbox's client (there's even one for [Linux](https://www.dropbox.com/install?os=lnx)) on the machine you use for MCPE Viz.  It is recommended to copy the data from your local Dropbox folder to another folder to prevent Dropbox from syncing *from* your MCPE Viz machine.  Running MCPE Viz on the world files *does* change the world files.  This method has only been *lightly* tested, so be careful.


## Customization

You can copy mcpe_viz.cfg to ~/.mcpe_viz/mcpe_viz.cfg and then edit that file to customize the way mcpe_viz works.

You can copy mcpe_viz.xml to ~/.mcpe_viz/mcpe_viz.xml and then edit that file to set custom colors for blocks and biomes.


## Error messages

This program uses a lot of file handles and depending on your OS's defaults (OS X's are too low) you can have a failure that resembles the following:
```
  Generate full-size slices
    Writing all images in one pass
ERROR: Failed to open output file (../worlds/output/images/kYQJAGOeAQA=.mcpe_viz_slice.full.overworld.247.png) errno=Too many open files(24)
libpng error: No IDATs written into file
ERROR: PngWriter setjmp triggered -- image might be too large (2672 x 1600)
zsh: segmentation fault  ../mcpe_viz/build/mcpe_viz --db ../worlds/kYQJAGOeAQA= --out  --html-all
```

You can check your limits and temporarily set file descriptors with the following:
```
ulimit -a
ulimit -n 8192
```

## Compiling from source

If you just want to run the software on windows, see above :)  If you would like to compile it for Linux (or Windows, or OS X), read on.

### Requirements

* You know how to compile things :)
* Mojang's LevelDB from github (see below) (https://github.com/Mojang/leveldb-mcpe.git)
* libnbt++ from github (see below) (https://github.com/ljfa-ag/libnbtplusplus.git)
* [Optional] *ffmpeg* for creating movies


### How to compile

#### Mojang's LevelDB

The code uses Mojang's modified version of LevelDB.  Here's how to compile it (from the top dir of mcpe_viz):

```
> git clone https://github.com/Mojang/leveldb-mcpe.git
> chdir leveldb-mcpe
> make
```

If all goes well, there will be a "libleveldb.a" in leveldb-mcpe/

To get it to compile on Fedora, I found that I needed to do this from leveldb-mcpe/:

```
> ln -s /usr/include include/zlib
```

If you have compile errors, check the README* files from leveldb-mcpe for prerequisites

#### libnbt++

The code uses libnbt++.  Here's how to compile it (from the top dir of mcpe_viz):

```
> git clone https://github.com/ljfa-ag/libnbtplusplus.git
> chdir libnbtplusplus
> mkdir build
> chdir build
> cmake .. -DNBT_BUILD_TESTS=OFF
> make
```

If all goes well, there will be a "libnbt++.a" in libnbtplusplus/build/

If you have compile errors, check the README* files from libnbtplusplus for prerequisites

#### Compile mcpe_viz

From the top directory of mcpe_viz:

```
> make
```

If all goes well, you will have "mcpe_viz" in build/


#### Compile mcpe_viz_helper

From the top directory of mcpe_viz:

```
> cd gui
> qmake
> make
```

If all goes well, you will have "mcpe_viz_helper"

Note: On Fedora, I use "qmake-qt4" instead of "qmake".


## Icon Images

Icon images used in the web app are borrowed from the [Minecraft Wiki](http://minecraft.gamepedia.com/).  The textures themselves are copyright Mojang.


## TODO

There is lots still todo.  Search on 'todo' in the code to see what needs attention.
