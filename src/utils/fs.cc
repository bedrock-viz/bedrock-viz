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

        FILE* fpsrc = fopen(fnSrc.c_str(), "r");
        if (!fpsrc) {
            log::error("copyFileWithStringReplacement failed to open src ({}) errno={}({})", fnSrc, strerror(errno), errno);
            return -1;
        }
        FILE* fpdest = fopen(fnDest.c_str(), "w");
        if (!fpdest) {
            log::error("copyFileWithStringReplacement failed to open dest ({}) errno={}({})", fnDest, strerror(errno), errno);
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
            log::error("copyFile failed for src ({}) to dest ({}) error-code ({})", fnSrc, fnDest, e.code().value());
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
            log::error("copyDirToDir(src={} dest={}) failed to open source directory", dirSrc, dirDest);
            return -1;
        }
    }


    int32_t deleteFile(const std::string& fn) {
        auto const result = std::filesystem::remove(fn);
        return result ? 0 : -1;
    }

}