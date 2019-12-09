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

namespace mcpe_viz {
    PlayerIdToName playerIdToName;

    // note: this is only used on win32/win64 builds
    // todo - doesn't check host endianness
    int32_t local_htobe32(const int32_t src) {
        int32_t dst;
        const char* ps = (char*)&src;
        char* pd = (char*)&dst;
        pd[0] = ps[3];
        pd[1] = ps[2];
        pd[2] = ps[1];
        pd[3] = ps[0];
        return dst;
    }

    // note: this is only used on win32/win64 builds
    // todo - doesn't check host endianness
    int32_t local_be32toh(const int32_t src) {
        int32_t dst;
        const char* ps = (char*)&src;
        char* pd = (char*)&dst;
        pd[0] = ps[3];
        pd[1] = ps[2];
        pd[2] = ps[1];
        pd[3] = ps[0];
        return dst;
    }


    // these hacks work around "const char*" problems
    std::string mybasename(const std::string& fn) {
        namespace fs = std::filesystem;
        fs::path p{ fn };
        return p.filename().generic_string();
    }

    std::string mydirname(const std::string& fn) {
        namespace fs = std::filesystem;
        fs::path p{ fn };
        return p.parent_path().generic_string();
    }


    int32_t file_exists(const std::string& fn) {
        return std::filesystem::exists(fn);
    }


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


    // hacky file copying funcs
    typedef std::vector< std::pair<std::string, std::string> > StringReplacementList;
    int32_t copyFileWithStringReplacement(const std::string& fnSrc, const std::string& fnDest,
        const StringReplacementList& replaceStrings) {
        char buf[1025];

        //slogger.msg(kLogInfo1,"  copyFileWithStringReplacement src=%s dest=%s\n", fnSrc.c_str(), fnDest.c_str());

        FILE* fpsrc = fopen(fnSrc.c_str(), "r");
        if (!fpsrc) {
            slogger.msg(kLogInfo1, "ERROR: copyFileWithStringReplacement failed to open src (%s) errno=%s(%d)\n", fnSrc.c_str(), strerror(errno), errno);
            return -1;
        }
        FILE* fpdest = fopen(fnDest.c_str(), "w");
        if (!fpdest) {
            slogger.msg(kLogInfo1, "ERROR: copyFileWithStringReplacement failed to open dest (%s) errno=%s(%d)\n", fnDest.c_str(), strerror(errno), errno);
            fclose(fpsrc);
            return -1;
        }

        while (!feof(fpsrc)) {
            memset(buf, 0, 1025);
            if (fgets(buf, 1024, fpsrc)) {

                // look for replacement string
                bool doneFlag = false;
                for (const auto& it : replaceStrings) {
                    char* p = strstr(buf, it.first.c_str());
                    if (p) {
                        std::string sbefore(buf, (p - buf));
                        std::string safter(&p[it.first.size()]);
                        if (sbefore.size() > 0) {
                            fputs(sbefore.c_str(), fpdest);
                        }
                        fputs(it.second.c_str(), fpdest);
                        if (safter.size() > 0) {
                            fputs(safter.c_str(), fpdest);
                        }
                        doneFlag = true;
                        break;
                    }
                }
                if (!doneFlag) {
                    fputs(buf, fpdest);
                }
            }
        }
        fclose(fpsrc);
        fclose(fpdest);
        return 0;
    }

    // adapted from: http://stackoverflow.com/questions/2180079/how-can-i-copy-a-file-on-unix-using-c
    int copyFile(const std::string& fnSrc, const std::string& fnDest, bool checkExistingFlag) {
        namespace fs = std::filesystem;

        auto options = checkExistingFlag ? fs::copy_options::skip_existing : fs::copy_options::overwrite_existing;
        try {
            fs::copy(fnSrc, fnDest, options);
            return 0;
        }
        catch (const fs::filesystem_error & e) {
            slogger.msg(kLogInfo1, "ERROR: copyFile failed for src (%s) to dest (%s) error-code (%d)\n",
                fnSrc.c_str(), fnDest.c_str(), e.code().value());
            return 1;
        }
    }


    int32_t copyDirToDir(const std::string& dirSrc, const std::string& dirDest, bool checkExistingFlag) {
        namespace fs = std::filesystem;
        auto options = fs::copy_options::recursive | (checkExistingFlag ? fs::copy_options::skip_existing : fs::copy_options::overwrite_existing);
        try {
            fs::copy(dirSrc, dirDest, options);
            return 0;
        }
        catch (const fs::filesystem_error&) {
            slogger.msg(kLogInfo1, "ERROR: copyDirToDir( src=%s dest=%s ) failed to open source directory\n", dirSrc.c_str(), dirDest.c_str());
            return -1;
        }
    }


    int32_t deleteFile(const std::string& fn) {
        auto result = std::filesystem::remove(fn);
        return result ? 0 : -1;
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

#if 0    
    // from: http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
    int32_t rgb2hsv(int32_t in_r, int32_t in_g, int32_t in_b, double& out_h, double& out_s, double& out_v) {
        double min, max, delta;

        min = in_r < in_g ? in_r : in_g;
        min = min < in_b ? min : in_b;

        max = in_r > in_g ? in_r : in_g;
        max = max > in_b ? max : in_b;

        out_v = max;                                // v
        delta = max - min;
        if (delta < 0.00001)
        {
            out_s = 0;
            out_h = 0; // undefined, maybe nan?
            return 0;
        }
        if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
            out_s = (delta / max);                  // s
        }
        else {
            // if max is 0, then r = g = b = 0
            // s = 0, v is undefined
            out_s = 0.0;
            out_h = NAN;                            // its now undefined
            return 0;
        }
        if (in_r >= max) {                           // > is bogus, just keeps compilor happy
            out_h = (in_g - in_b) / delta;        // between yellow & magenta
        }
        else {
            if (in_g >= max) {
                out_h = 2.0 + (in_b - in_r) / delta;  // between cyan & yellow
            }
            else {
                out_h = 4.0 + (in_r - in_g) / delta;  // between magenta & cyan
            }
        }

        out_h *= 60.0;                              // degrees

        if (out_h < 0.0) {
            out_h += 360.0;
        }

        return 0;
    }
#endif


    // note: super super old hsl2rgb code; origin unknown
    double _hue_to_rgb(double m1, double m2, double h) {
        while (h < 1.0) { h += 1.0; }
        while (h > 1.0) { h -= 1.0; }
        if ((h * 6.0) < 1.0) {
            return m1 + (m2 - m1) * h * 6.0;
        }
        if ((h * 2.0) < 1.0) {
            return m2;
        }
        if ((h * 3.0) < 2.0) {
            return m1 + (m2 - m1) * (2.0 / 3.0 - h) * 6.0;
        }
        return m1;
    }


    int32_t _clamp(int32_t v, int32_t minv, int32_t maxv) {
        if (v < minv) return minv;
        if (v > maxv) return maxv;
        return v;
    }


    int32_t hsl2rgb(double h, double s, double l, int32_t& r, int32_t& g, int32_t& b) {
        double m2;
        if (l <= 0.5) {
            m2 = l * (s + 1.0);
        }
        else {
            m2 = l + s - l * s;
        }
        double m1 = l * 2.0 - m2;
        double tr = _hue_to_rgb(m1, m2, h + 1.0 / 3.0);
        double tg = _hue_to_rgb(m1, m2, h);
        double tb = _hue_to_rgb(m1, m2, h - 1.0 / 3.0);
        r = _clamp((int)(tr * 255.0), 0, 255);
        g = _clamp((int)(tg * 255.0), 0, 255);
        b = _clamp((int)(tb * 255.0), 0, 255);
        return 0;
    }


    int32_t makeHslRamp(int32_t* pal, int32_t start, int32_t stop, double h1, double h2, double s1, double s2, double l1, double l2) {
        double steps = stop - start + 1;
        double dh = (h2 - h1) / steps;
        double ds = (s2 - s1) / steps;
        double dl = (l2 - l1) / steps;
        double h = h1, s = s1, l = l1;
        int32_t r, g, b;
        for (int32_t i = start; i <= stop; i++) {
            hsl2rgb(h, s, l, r, g, b);
            int32_t c = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
            pal[i] = c;
            h += dh;
            s += ds;
            l += dl;
        }
        return 0;
    }


    bool compareColorInfo(std::unique_ptr<ColorInfo> const& a, std::unique_ptr<ColorInfo> const& b) {
        double dh = a->h - b->h;
        if (dh < 0.0) { return true; }
        if (dh > 0.0) { return false; }
        double ds = a->s - b->s;
        if (ds < 0.0) { return true; }
        if (ds > 0.0) { return false; }
        double dl = a->l - b->l;
        if (dl < 0.0) { return true; }
        if (dl > 0.0) { return false; }
        return false;
    }


    int32_t oversampleImage(const std::string& fnSrc, const std::string& fnDest, int32_t oversample) {
        PngReader pngSrc;
        if (pngSrc.init(fnSrc) != 0) {
            slogger.msg(kLogInfo1, "ERROR: Failed to open src png");
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
        if (pngOut.init(fnDest, "MCPE Viz Oversampled Image", destW, destH, destH, true, true) != 0) {
            slogger.msg(kLogInfo1, "ERROR: Failed to create dest png");
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


    bool vectorContains(const std::vector<int>& v, int32_t i) {
        for (const auto& iter : v) {
            if (iter == i) {
                return true;
            }
        }
        return false;
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
                // slogger.msg(kLogInfo1, "INFO: parsePlayerIdToName -- found valid playerId (%s) and playerName (%s) in passed string (%s)\n", playerId.c_str(), playerName.c_str(), s);
                return addPlayerIdToName(playerId, playerName);
            }
            slogger.msg(kLogInfo1, "ERROR: Failed parsePlayerIdToName -- did not find valid playerId (%s) and playerName (%s) in passed string (%s)\n", playerId.c_str(), playerName.c_str(), s);
            return -1;
        }

        slogger.msg(kLogInfo1, "ERROR: Failed parsePlayerIdToName -- did not find valid playerId and playerName in passed string (%s)\n", s);
        return -2;
    }

    inline uint8_t _getBitFromByte(const char* cdata, int32_t bitnum) {
        int byteStart = bitnum / 8;
        int byteOffset = bitnum % 8;
        return cdata[byteStart] & (1 << byteOffset);
    }

    // todo - this is probably quite slow
    int32_t getBitsFromBytes(const char* cdata, int32_t bitstart, int32_t bitlen) {
        int32_t ret = 0;
        for (int b = 0; b < bitlen; b++) {
            uint8_t bit = _getBitFromByte(cdata, bitstart + b);
            if (bit) {
                ret |= 1 << b;
                // ret |= 1 << (bitlen-1-b);
            }
        }
        return ret;
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


    void dumpBuffer(const char* header, const char* buf, size_t bufLen) {
        fprintf(stderr, "%s  (hex):\n", header);
        for (size_t i = 0; i < bufLen; i++) {
            fprintf(stderr, "%02x ", (uint8_t)buf[i] & 0xff);
        }
        fprintf(stderr, "\n");

        fprintf(stderr, "%s (char):\n", header);
        for (size_t i = 0; i < bufLen; i++) {
            if (isprint(buf[i])) {
                fputc(buf[i], stderr);
            }
            else {
                fputc('.', stderr);
            }
        }
        fprintf(stderr, "\n");
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


} // namespace mcpe_viz


