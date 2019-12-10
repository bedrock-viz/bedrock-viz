#pragma once

#include <string>
#include <vector>

namespace mcpe_viz {

    int32_t local_mkdir(const std::string& path);

    // these hacks work around "const char*" problems
    std::string mybasename(const std::string& fn);
    std::string mydirname(const std::string& fn);

    int32_t file_exists(const std::string& fn);

    typedef std::vector< std::pair<std::string, std::string> > StringReplacementList;
    
    int32_t copyFileWithStringReplacement(const std::string& fnSrc, const std::string& fnDest,
        const StringReplacementList& replaceStrings);

    int32_t copyFile(const std::string& fnSrc, const std::string& fnDest, bool checkExistingFlag);

    int32_t copyDirToDir(const std::string& dirSrc, const std::string& dirDest, bool checkExistingFlag);

    int32_t deleteFile(const std::string& fn);
}