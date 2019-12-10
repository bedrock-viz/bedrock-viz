#include "fs.h"
#include <filesystem>

#include "../logger.h"

namespace mcpe_viz {
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
        auto const result = std::filesystem::remove(fn);
        return result ? 0 : -1;
    }

}