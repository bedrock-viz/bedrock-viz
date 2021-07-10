/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  Utility functions
*/

#include <cstdio>
#include <sstream>
#include <filesystem>

#include "util.h"
#include "utils/fs.h"

namespace mcpe_viz {
    PlayerIdToName playerIdToName;

    std::string escapeString(const std::string& s, const std::string& escapeChars) {
        if (escapeChars.size() == 0) {
            return s;
        }
        std::string ret;
        for (const auto& ch : s) {
            bool replaced = false;
            for (const auto& escape : escapeChars) {
                if (ch == escape) {
                    ret += "\\";
                    ret += escape;
                    replaced = true;
                    break;
                }
            }
            if (!replaced) {
                ret += ch;
            }
        }
        return ret;
    }
    
    // from: http://kickjava.com/src/org/eclipse/swt/graphics/RGB.java.htm
    int32_t rgb2hsb(int32_t red, int32_t green, int32_t blue, double& hue, double& saturation, double& brightness) {
        double r = (double)red / 255.0;
        double g = (double)green / 255.0;
        double b = (double)blue / 255.0;
        double vmax = std::max(std::max(r, g), b);
        double vmin = std::min(std::min(r, g), b);
        double delta = vmax - vmin;
        hue = 0.0;
        brightness = vmax;
        saturation = (vmax == 0.0) ? 0.0 : ((vmax - vmin) / vmax);

        if (delta != 0.0) {
            if (r == vmax) {
                hue = (g - b) / delta;
            }
            else {
                if (g == vmax) {
                    hue = 2 + (b - r) / delta;
                }
                else {
                    hue = 4 + (r - g) / delta;
                }
            }
            hue *= 60.0;
            if (hue < 0.0) hue += 360.0;
        }
        return 0;
    }


    int32_t oversampleImage(const std::string& fnSrc, const std::string& fnDest, int32_t oversample) {
        PngReader pngSrc;
        if (pngSrc.init(fnSrc) != 0) {
            log::error("Failed to open src png (fn={})", fnSrc);
            return -1;
        }
        pngSrc.read();

        int32_t srcW = pngSrc.getWidth();
        int32_t srcH = pngSrc.getHeight();
        int32_t colorType = pngSrc.getColorType();
        int32_t bppSrc = 3;
        if (colorType == PNG_COLOR_TYPE_RGB_ALPHA) {
            bppSrc = 4;
        }

        int32_t bppDest = bppSrc;

        int32_t destW = srcW * oversample;
        int32_t destH = srcH * oversample;
        uint8_t* buf = new uint8_t[destW * destH * bppDest];
        memset(buf, 0, destW * destH * bppDest);

        PngWriter pngOut;
        if (pngOut.init(fnDest, "Bedrock Viz Oversampled Image", destW, destH, destH, true, true) != 0) {
            log::error("Failed to create dest png (fn={})", fnDest);
            delete[] buf;
            pngSrc.close();
            return -2;
        }

        for (int32_t ty = 0; ty < destH; ty++) {
            pngOut.row_pointers[ty] = &buf[ty * destW * bppDest];
        }

        for (int32_t sy = 0; sy < srcH; sy++) {
            uint8_t* srcbuf = pngSrc.row_pointers[sy];

            for (int32_t sx = 0; sx < srcW; sx++) {

                for (int32_t oy = 0; oy < oversample; oy++) {
                    int32_t dy = sy * oversample + oy;
                    for (int32_t ox = 0; ox < oversample; ox++) {
                        int32_t dx = sx * oversample + ox;
                        // todo - optimize srcbuf val outside loops
                        memcpy(&buf[(dy * destW + dx) * bppDest], &srcbuf[sx * bppSrc], bppDest);
                    }
                }

            }
        }

        png_write_image(pngOut.png, pngOut.row_pointers);
        pngOut.close();

        delete[] buf;

        pngSrc.close();

        return 0;
    }

    std::string makeIndent(int32_t indent, const char* hdr) {
        std::string s;
        s.append(hdr);
        for (int32_t i = 0; i < indent; i++) {
            s.append("  ");
        }
        return s;
    }

    int32_t local_mkdir(const std::string& path) {
        namespace fs = std::filesystem;
        auto result = fs::create_directory(path);
        return result ? 0 : -1;
    }


    bool has_key(const PlayerIdToName& m, const std::string& k) {
        return  m.find(k) != m.end();
    }

    int32_t addPlayerIdToName(const std::string& playerId, const std::string& playerName) {
        playerIdToName[playerId] = playerName;
        return 0;
    }

    int32_t parsePlayerIdToName(const char* s) {
        std::string playerId;
        std::string playerName;

        // the format is: playerId playerName
        // parsing adapted from: http://stackoverflow.com/a/5208977
        std::string src(s);
        std::string buf;
        std::stringstream ss(src); // Insert the string into a stream

        if (ss >> playerId) {
            int32_t i = 0;
            while (ss >> buf) {
                if (i++ > 0) { playerName += " "; }
                playerName += buf;
            }

            if (playerId.size() > 0 && playerName.size() > 0) {
                return addPlayerIdToName(playerId, playerName);
            }
            log::error("Did not find valid playerId ({}) and playerName ({}) in passed string ({})", playerId, playerName, s);
            return -1;
        }
        log::error("Did not find valid playerId and playerName in passed string ({})", s);
        return -2;
    }

    // adapted from: http://ysonggit.github.io/coding/2014/12/16/split-a-string-using-c.html
    std::vector<std::string> mysplit(const std::string& s, char delim) {
        std::stringstream ss(s);
        std::string item;
        std::vector<std::string> tokens;
        while (getline(ss, item, delim)) {
            tokens.push_back(trim_copy(item));
        }
        return tokens;
    }


    // todolib - move to util?

    int32_t myParseInt32(const char* p, int32_t startByte) {
        int32_t ret;
        memcpy(&ret, &p[startByte], 4);
        return ret;
    }

    int8_t myParseInt8(const char* p, int32_t startByte) {
        return (p[startByte] & 0xff);
    }

    // todobig - move to util?
    int32_t
        printKeyValue(const char* key, int32_t key_size, const char* value, int32_t value_size, bool printKeyAsStringFlag) {
        log::trace("Unparsed Record: key_size={} key_string=[{}] key_hex=[", 
            key_size, (printKeyAsStringFlag ? key : "(SKIPPED)"));
        for (int32_t i = 0; i < key_size; i++) {
            log::trace(" {:02x}", int(key[i] & 0xff));
        }
        log::trace("] value_size={} value_hex=[", value_size);
        for (int32_t i = 0; i < value_size; i++) {
            log::trace(" {:02x}", int(value[i] & 0xff));
        }
        log::trace("]");
        return 0;
    }

    int32_t PngTiler::doTile()
    {
        // todobig - store tile filenames?

        char tmpstring[256];

        // open source file
        PngReader pngSrc;
        pngSrc.init(filename);
        pngSrc.read_info();

        int32_t srcW = pngSrc.getWidth();
        int32_t srcH = pngSrc.getHeight();
        int32_t colorType = pngSrc.getColorType();
        bool rgbaFlag = false;
        int32_t bpp = 3;
        if (colorType == PNG_COLOR_TYPE_RGB_ALPHA) {
            bpp = 4;
            rgbaFlag = true;
        }
        int32_t numPngW = (int)ceil((double)srcW / (double)tileWidth);

        uint8_t* sbuf = new uint8_t[srcW * bpp];


        PngWriter* pngOut = new PngWriter[numPngW];
        uint8_t** buf;
        buf = new uint8_t * [numPngW];
        for (int32_t i = 0; i < numPngW; i++) {
            buf[i] = new uint8_t[tileWidth * tileHeight * bpp];
        }

        bool initPngFlag = false;
        int32_t tileCounterY = 0;

        for (int32_t sy = 0; sy < srcH; sy++) {

            // initialize png helpers
            if (!initPngFlag) {
                initPngFlag = true;
                for (int32_t i = 0; i < numPngW; i++) {
                    sprintf(tmpstring, "%s/%s.%d.%d.png", dirOutput.c_str(), mybasename(filename).c_str(),
                        tileCounterY, i);
                    std::string fname = tmpstring;
                    pngOut[i].init(fname, "Bedrock Viz Image Tile", tileWidth, tileHeight, tileHeight, rgbaFlag, true);

                    // clear buffer
                    memset(&buf[i][0], 0, tileWidth * tileHeight * bpp);

                    // setup row_pointers
                    for (int32_t ty = 0; ty < tileHeight; ty++) {
                        pngOut[i].row_pointers[ty] = &buf[i][ty * tileWidth * bpp];
                    }
                }
                tileCounterY++;
            }

            png_read_row(pngSrc.png, sbuf, NULL);

            int32_t tileOffsetY = sy % tileHeight;

            // todobig - step in tileWidth and memcpy as we go - need to check the last one for out of bounds
            for (int32_t sx = 0; sx < srcW; sx++) {
                int32_t tileCounterX = sx / tileWidth;
                int32_t tileOffsetX = sx % tileWidth;
                memcpy(&buf[tileCounterX][((tileOffsetY * tileWidth) + tileOffsetX) * bpp], &sbuf[sx * bpp], bpp);
            }

            // write tile png files when they are ready
            if (((sy + 1) % tileHeight) == 0) {
                // write pngs
                for (int32_t i = 0; i < numPngW; i++) {
                    png_write_image(pngOut[i].png, pngOut[i].row_pointers);
                    pngOut[i].close();
                }
                initPngFlag = false;
            }
        }

        // close final tiles
        if (initPngFlag) {
            // write pngs
            for (int32_t i = 0; i < numPngW; i++) {
                png_write_image(pngOut[i].png, pngOut[i].row_pointers);
                pngOut[i].close();
            }
        }

        delete[] pngOut;

        for (int32_t i = 0; i < numPngW; i++) {
            delete[] buf[i];
        }
        delete[] buf;

        pngSrc.close();

        delete[] sbuf;

        return 0;
    }


} // namespace mcpe_viz


