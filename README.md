# Bedrock Viz

[![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/bedrock-viz/bedrock-viz?include_prereleases&logo=github&logoColor=white&style=plastic)](https://github.com/bedrock-viz/bedrock-viz/releases)
[![GitHub last commit](https://img.shields.io/github/last-commit/bedrock-viz/bedrock-viz?logo=github&logoColor=white&style=plastic)](https://github.com/bedrock-viz/bedrock-viz/commits/master)
[![GitHub Workflow Status](https://img.shields.io/github/workflow/status/bedrock-viz/bedrock-viz/Pipeline?logo=github&logoColor=white&style=plastic)](https://github.com/bedrock-viz/bedrock-viz/actions/workflows/pipeline.yml)
[![GitHub issues](https://img.shields.io/github/issues-raw/bedrock-viz/bedrock-viz?logo=github&logoColor=white&style=plastic)](https://github.com/bedrock-viz/bedrock-viz/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/bedrock-viz/bedrock-viz?logo=github&logoColor=white&style=plastic)](https://github.com/bedrock-viz/bedrock-viz/pulls)

[![ReportCI](https://report.ci/badge/gh/bedrock-viz/bedrock-viz?branch=master)](https://report.ci/reports/gh/bedrock-viz/bedrock-viz)
[![Codecov](https://img.shields.io/codecov/c/github/bedrock-viz/bedrock-viz?label=codecov&logo=codecov&logoColor=white&style=plastic)](https://codecov.io/gh/bedrock-viz/bedrock-viz)

[![Discord](https://img.shields.io/discord/726889889806942310?logo=discord&logoColor=white&style=plastic)](https://discord.gg/dWhQNvbPw6)


A fork of [mcpe_viz](https://github.com/Plethora777/mcpe_viz.git), now maintained by [Jasper Wan](https://github.com/jasper-wan).

Bedrock Viz is a tool that parses Minecraft Bedrock Edition world files.  It creates overview images from the world files, and reports *lots* of details on your world.

It can also create a powerful Google Maps-like Web App for viewing your world.

Additional Features:

* Create a powerful Google Maps-like web app viewer for your world.
  * Browse through all 256 layers of your world (Overworld and Nether) from bedrock to build limit.
  * Examine the location and details of mobs and items in your world.
  * Visualize chunk boundaries -- consider it an extremely cumbersome F3/debug mode!
  * Find slime chunks -- well, probably... slime chunk finding code is currently a bit weird in MCPE, it appears to work, but please be sure to confirm slimes are spawning in a chunk before you do a ton of work :)
  * Explore the biomes of your world.
  * Shaded-relief elevation overlay -- get a sense of the topography of your world.
  * Measure distances in your world

* Create many types of images:
  * All layers of your world
  * Biomes in your world (very interesting!)
  * Block height in your world (kind of like a topographic map)

* Create movies of all layers from bedrock to build limit (*ffmpeg* required)

## Quick Start

For Windows 10 users:

* [Download the zip file](https://github.com/bedrock-viz/bedrock-viz/releases/download/v0.1.7/bedrock-viz_v0.1.7_win10-64.zip) and unzip that file
* Inside you will find `bedrock-viz.exe`
* Run `bedrock-viz.exe` in your favorite terminal program (such as `Command Prompt` or `Power Shell`)

For macOS users(requires macOS 10.15 and above):
* Install via homeberw:
  ```
  brew tap bedrock-viz/homebrew-tap && brew install bedrock-viz
  ```

For Linux users:
* [Download the source](https://github.com/bedrock-viz/bedrock-viz/releases/download/v0.1.7/bedrock-viz_v0.1.7_linux.tar.gz) and compiling from source

**DO NOT RUN THIS ON YOUR ORIGINAL WORLD SAVES**

**MAKE A BACKUP COPY OF YOUR DATA AND RUN THIS AGAINST THAT COPY ONLY**

Use the app to select the MCPE world file, the output directory, output name, and mode.  Press "Go!".  Watch the progress of bedrock-viz.  If it completes successfully, you can click the "Launch Web App" button.  If there was a problem, you should look in the "Progress" window for details that might help you resolve the issue.

See below for Web App usage notes.


## Quick Start -- How to run it on Windows from the command-line

* Same as above to get the files.
* Open a command prompt and run it something like this:

```
bedrock-viz.exe --db path-to-your-world-file-dir/ --out out1 --html-most
```

Note: Replace "path-to-your-world-file-dir/" with the actual directory which contains your world files -- it wants the directory that has level.dat in it.

**DO NOT RUN THIS ON YOUR ORIGINAL MCPE DATA FILES; MAKE A BACKUP COPY OF YOUR DATA AND RUN THIS AGAINST THAT COPY ONLY!**

This will produce a bunch of image files and "index.html" in the directory `out1`. Open "index.html" in your browser.


## Web App

You can create a browser-based viewer for your world files.  It will allow you to pan, zoom and switch between the different images.  It's pretty spiffy!

To make a quick viewer (with all regular images, but without indvidual layers), do something like this:

```
> ./bedrock-viz --db ./mcpe/another1/ --out ./mcpe/output/out1 --html-most
```

To make a complete viewer (with all images and all layers), do something like this:

```
> ./bedrock-viz --db ./mcpe/another1/ --out ./mcpe/output/out1 --html-all
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
  * Scroll through the layers from 0 (bedrock) to 255 (build limit)
  * Choose Overview, Biome etc
  * Show Mobs and Objects -- click on an entry and the map will be updated with labeled points.  You can toggle on as many different types as you like.  Click on the points or labels to get more info on the item.  Note that labels are not drawn when you are zoomed out.
  * Enable elevation overlay (shaded relief)
  * Enable chunk grid overlay


## Web App Notes

If you are loading the web app from a local file -- that is, not accessing it from a web server -- your browser may impose restrictions on image loading and access to image pixels which makes it very difficult for the web app to deliver the best experience.  Currently, Firefox appears to be the browser most willing to play nicely.  The web app will let you know if you are impacted by this issue.  For example, on Chrome, you will not see block information on mouse over, or be able to see the elevation overlay -- both of which are totally cool :)

If you are running Firefox or serving the files from a web server, you can make the web app load a little bit faster by adding the option "--no-force-geojson" to your command line.  This prevents bedrock-viz from using a workaround for the above issue.

## Using an NGINX Webserver in Docker to display your world
If you want to display your map locally through a webserver in Docker, these commands will set up an NGINX webserver with your data copied into a volume mounted to the /usr/share/nginx/html directory. The webserver is exposed on port 8080 and the web app can be viewed at http://localhost:8080.

**Be aware of what folder will be copied to avoid copying sensitive files/folders. It is recommended to always ouput to a new directory.**
```
docker volume create bedrock-viz-http
docker create --name bedrock-viz -p 8080:80 -v bedrock-viz-http:/usr/share/nginx/html -d nginx
# This cmd will copy the entire /path/to/output/folder recursively. Be careful!
docker cp /path/to/output/folder:/usr/share/nginx/html bedrock-viz
docker run bedrock-viz
```
To clean up your webserver and data when you are done:
```
docker rm bedrock-viz
docker volume rm bedrock-viz-http
```

## Usage

**MAKE A COPY OF YOUR DATA AND RUN THIS AGAINST THAT COPY ONLY!**

Here's an example invocation:

```
> ./bedrock-viz --grid --db ./mcpe/another1/ --out ./mcpe/output/out1
```

This will read the leveldb from the "./mcpe/another1" folder and name output files starting with "./mcpe/output/out1", and it will draw chunk boundaries on your output image.  This also dumps the *voluminous* output to "out1.log".  The log file has a *ton* of interesting information about your world.  "grep" is your friend.

Please note that --db expects the directory which contains "level.dat".

To view all command line options, run
```
"./bedrock-viz --help"
```

For extended usage instructions, see [USAGE.md](docs/USAGE.md)

## How to backup your MCPE world files

On Android, [see this link](http://pocketbeta.minecraft.net/p/how-to-make-backup.html).

On iOS or Windows 10, [see this link](https://www.reddit.com/r/MCPE/comments/3h28ku/how_to_backuptransfer_worlds/)

Note that MCPE is sometimes weird about actually closing world files -- that is, you think that MCPE is closed and should be done with the world file, but a backup of it may not work correctly.  To be certain that you get a good backup, you should reboot and then do the backup *before* starting MCPE again.

### Automated backup to Dropbox using DropSync (Android)

On Android, you can use [DropSync](https://play.google.com/store/apps/details?id=com.ttxapps.dropsync) to automatically backup your MCPE world to your dropbox account.  You configure this tool to backup any of your world files to dropbox.  As you play on your device, DropSync will copy the updated world files to Dropbox.  Very cool!  Install Dropbox's client (there's even one for [Linux](https://www.dropbox.com/install?os=lnx)) on the machine you use for MCPE Viz.  It is recommended to copy the data from your local Dropbox folder to another folder to prevent Dropbox from syncing *from* your MCPE Viz machine.  Running MCPE Viz on the world files *does* change the world files.  This method has only been *lightly* tested, so be careful.

## Error messages

This program uses a lot of file handles and depending on your OS's defaults (OS X's are too low) you can have a failure that resembles the following:
```
  Generate full-size slices
    Writing all images in one pass
  ERROR: Failed to open output file (../worlds/output/images/kYQJAGOeAQA=.mcpe_viz_slice.full.overworld.247.png) errno=Too many open files(24)
  libpng error: No IDATs written into file
  ERROR: PngWriter setjmp triggered -- image might be too large (2672 x 1600)
  zsh: segmentation fault  ../bedrock-viz/build/bedrock-viz --db ../worlds/kYQJAGOeAQA= --out  --html-all
```

You can check your limits and temporarily set file descriptors with the following:
```
ulimit -a
ulimit -n 8192
```

### Compiling from source

see [BUILD.md](docs/BUILD.md)
