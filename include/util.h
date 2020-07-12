/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  utility code for mcpe_viz -- stuff that is not leveldb/mcpe specific
*/

#ifndef __MCPE_VIZ_UTIL_H__
#define __MCPE_VIZ_UTIL_H__

#include <cstdio>
#include <cstdarg>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <cstring>
#include <png.h>
#include <cmath>
#include <cerrno>

#include "config.h"
#include "logger.h"

namespace mcpe_viz {

    // note: this is only used on win32/win64 builds
    // todo - doesn't check host endianness
    inline int32_t local_htobe32(const int32_t src) {
#ifndef htobe32
        int32_t dst;
        const char* ps = (char*)&src;
        char* pd = (char*)&dst;
        pd[0] = ps[3];
        pd[1] = ps[2];
        pd[2] = ps[1];
        pd[3] = ps[0];
        return dst;
#else
        return htobe32(src);
#endif
    }

    // note: this is only used on win32/win64 builds
    // todo - doesn't check host endianness
    inline int32_t local_be32toh(const int32_t src) {
#ifndef be32toh
        int32_t dst;
        const char* ps = (char*)&src;
        char* pd = (char*)&dst;
        pd[0] = ps[3];
        pd[1] = ps[2];
        pd[2] = ps[1];
        pd[3] = ps[0];
        return dst;
#else
        return be32toh(src);
#endif
    }


    std::string escapeString(const std::string& s, const std::string& escapeChars);

    std::string makeIndent(int32_t indent, const char* hdr);

    class PngWriter {
    public:
        std::string fn;
        FILE* fp;
        png_structp png;
        png_infop info;
        png_bytep* row_pointers;
        bool openFlag;

        PngWriter() {
            fn = "";
            fp = nullptr;
            png = nullptr;
            info = nullptr;
            row_pointers = nullptr;
            openFlag = false;
        }

        ~PngWriter() {
            close();
        }

        int32_t init(const std::string& xfn, const std::string& imageDescription, int32_t w, int32_t h, int32_t numRowPointers, bool rgbaFlag, bool wholeImageFlag) {
            fn = std::string(xfn);
            return open(imageDescription, w, h, numRowPointers, rgbaFlag, wholeImageFlag);
        }

        int32_t open(const std::string& imageDescription, int32_t width, int32_t height, int32_t numRowPointers, bool rgbaFlag, bool wholeImageFlag) {
            fp = fopen(fn.c_str(), "wb");
            if (!fp) {
                log::error("Failed to open output file ({}) errno={}({})", fn, strerror(errno), errno);
                return -1;
            }

            // todo - add handlers for warn/err etc?
            /*
              png_structp   png_create_write_struct   (
              png_const_charp   user_png_ver,
              png_voidp  error_ptr,
              png_error_ptr  error_fn,
              png_error_ptr warn_fn);
            */
            png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            if (!png) {
                log::error("Failed png_create_write_struct");
                fclose(fp);
                return -2;
            }

            info = png_create_info_struct(png);
            if (!info) {
                log::error("Failed png_create_info_struct");
                fclose(fp);
                return -2;
            }

            // todo - can we do something more clever here?
            if (setjmp(png_jmpbuf(png))) {
                log::error("PngWriter setjmp triggered -- image might be too large ({} x {})", width, height);
                png_destroy_write_struct(&png, &info);
                fclose(fp);
                //return -5;
                // if we've failed here, there is not much chance that we can continue
                log::error("Cannot continue");
                exit(-1);
            }

            png_init_io(png, fp);

            // Output is 8bit depth, RGB format.
            int32_t color_type = PNG_COLOR_TYPE_RGB;
            if (rgbaFlag) {
                color_type = PNG_COLOR_TYPE_RGB_ALPHA;
            }
            png_set_IHDR(
                png,
                info,
                width, height,
                8,
                color_type,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
            );

            // note: the following two options have a *significant* impact on speed with a minimal cost in filesize
            //png_set_compression_level(png, Z_BEST_COMPRESSION);
            png_set_compression_level(png, 1); // speed!
            png_set_filter(png, 0, PNG_FILTER_NONE);

            // add text comments to png
            addText("Program", version_full);
            addText("Description", imageDescription);
            addText("URL", "https://github.com/bedrock-viz/bedrock-viz");
            // todo - other text?

            png_write_info(png, info);

            if (wholeImageFlag) {
                row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * numRowPointers);
            }

            openFlag = true;
            return 0;
        }

        int32_t close() {
            if (fp != nullptr && openFlag) {
                png_write_end(png, info);
                png_destroy_write_struct(&png, &info);
                if (row_pointers != nullptr) {
                    free(row_pointers);
                    row_pointers = nullptr;
                }
                fclose(fp);
                fp = nullptr;
            }
            openFlag = false;
            return 0;
        }

        int32_t addText(const std::string& key, const std::string& val) {
            png_text text[1];
            char* skey = new char[key.size() + 1];
            strcpy(skey, key.c_str());
            char* sval = new char[val.size() + 1];
            strcpy(sval, val.c_str());

            text[0].compression = PNG_TEXT_COMPRESSION_NONE;
            text[0].key = skey;
            text[0].text = sval;
            png_set_text(png, info, text, 1);

            delete[] skey;
            delete[] sval;
            return 0;
        }
    };



    class PngReader {
    public:
        std::string fn;
        FILE* fp;
        png_structp png;
        png_infop info;
        png_infop end_info;
        png_bytepp row_pointers;
        bool openFlag;

        PngReader() {
            fn = "";
            fp = nullptr;
            png = nullptr;
            info = nullptr;
            row_pointers = nullptr;
            openFlag = false;
        }

        ~PngReader() {
            close();
        }

        int32_t init(const std::string& xfn) {
            fn = std::string(xfn);
            return open();
        }

        int32_t open() {
            if (fn.length() <= 0) {
                log::error("Empty input filename (fn={})", fn);
                return -1;
            }

            fp = fopen(fn.c_str(), "rb");
            if (!fp) {
                log::error("Failed to open input file ({}) errno={}({})", fn, strerror(errno), errno);
                return -1;
            }

            // todo - add handlers for warn/err etc?
            /*
              png_structp   png_create_write_struct   (
              png_const_charp   user_png_ver,
              png_voidp  error_ptr,
              png_error_ptr  error_fn,
              png_error_ptr warn_fn);
            */
            png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            if (!png) {
                log::error("Failed png_create_write_struct");
                fclose(fp);
                return -2;
            }

            info = png_create_info_struct(png);
            if (!info) {
                log::error("Failed png_create_info_struct (info)");
                fclose(fp);
                png_destroy_read_struct(&png,
                    (png_infopp)NULL, (png_infopp)NULL);
                return -3;
            }

            end_info = png_create_info_struct(png);
            if (!end_info) {
                log::error("Failed png_create_info_struct (end_info)");
                fclose(fp);
                png_destroy_read_struct(&png, &info, (png_infopp)NULL);
                return -4;
            }

            // todo - can we do something more clever here?
            if (setjmp(png_jmpbuf(png))) {
                log::error("ERROR: PngReader setjmp triggered (fn={})", fn);
                //png_destroy_read_struct(&png, &info, &end_info);
                //fclose(fp);
                return -5;
            }

            png_init_io(png, fp);

            openFlag = true;
            return 0;
        }

        int32_t getWidth() {
            return png_get_image_width(png, info);
        }
        int32_t getHeight() {
            return png_get_image_height(png, info);
        }
        int32_t getColorType() {
            return png_get_color_type(png, info);
        }

        int32_t read() {
            png_read_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
            row_pointers = png_get_rows(png, info);
            return 0;
        }

        // use this before reading row-by-row
        int32_t read_info() {
            png_read_info(png, info);
            return 0;
        }

        int32_t close() {
            if (fp != nullptr && openFlag) {
                // png_read_end(png, end_info);
                png_destroy_read_struct(&png, &info, &end_info);
                fclose(fp);
                fp = nullptr;
            }
            openFlag = false;
            return 0;
        }
    };



    class PngTiler {
    public:
        std::string filename;
        int32_t tileWidth;
        int32_t tileHeight;
        std::string dirOutput;

        PngTiler(const std::string& fn, int32_t tileW, int32_t tileH, const std::string& dirOut) {
            filename = fn;
            tileWidth = tileW;
            tileHeight = tileH;
            dirOutput = dirOut;
        }

        int32_t doTile();

    };


    int32_t oversampleImage(const std::string& fnSrc, const std::string& fnDest, int32_t oversample);

    // quick-n-dirty emulation of java random number generator
    // details from: https://docs.oracle.com/javase/8/docs/api/java/util/Random.html
    class JavaRandom {
    public:
        int64_t seed;

        void setSeed(int64_t newseed) {
            seed = (newseed ^ (int64_t)0x5DEECE66D) & (((int64_t)1 << 48) - 1);
        }

        int32_t next(int64_t bits) {
            // seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1)
            seed = (seed * (int64_t)0x5DEECE66D + (int64_t)0xB) & (((int64_t)1 << 48) - 1);

            // return (int)(seed >>> (48 - bits)).
            // todo - is this good enough for an unsigned shift right?
            int64_t ret = (uint64_t)seed >> (48 - bits);

            return (int32_t)ret;
        }

        int32_t nextInt(int64_t bound) {
            if ((bound & -bound) == bound)  // i.e., bound is a power of 2
                return int32_t((bound * this->next(31)) >> 31);

            int32_t bits, val;
            do {
                bits = this->next(31);
                val = bits % bound;
            } while (bits - val + (bound - 1) < 0);

            return val;
        }
    };


    typedef std::map<std::string, std::string> PlayerIdToName;
    bool has_key(const PlayerIdToName& m, const std::string& k);

    extern PlayerIdToName playerIdToName;

    int32_t parsePlayerIdToName(const char* s);

    std::vector<std::string> mysplit(const std::string& s, char delim);


    // borrowed from: https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    // trim from start (in place)
    static inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
            }));
    }

    // trim from end (in place)
    static inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    // trim from both ends (copying)
    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }

    int32_t myParseInt32(const char* p, int32_t startByte);

    int8_t myParseInt8(const char* p, int32_t startByte);

    int32_t
        printKeyValue(const char* key, int32_t key_size, const char* value, int32_t value_size, bool printKeyAsStringFlag);
} // namespace mcpe_viz

#endif // __MCPE_VIZ_UTIL_H__
