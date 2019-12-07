#include "logger.h"

#include <cstdarg>
#include <cstdlib>

namespace mcpe_viz {
    Logger::Logger() {
        this->logLevelMask = kLogDefault;
        this->fpStderr = nullptr;
        this->fpStdout = nullptr;
        this->doFlushFlag = false;
    }

    void Logger::setFlush(bool f) { doFlushFlag = f; }

    void Logger::setLogLevelMask(uint32_t m) { logLevelMask = m; }

    void Logger::setStdout(FILE * fp) { fpStdout = fp; }

    void Logger::setStderr(FILE * fp) { fpStderr = fp; }

    int32_t Logger::msg(uint32_t levelMask, const char * fmt, ...) {
        // check if we care about this message
        if ((levelMask & logLevelMask) || (levelMask & kLogFatalError)) {
            // we care
        } else {
            // we don't care
            return -1;
        }

        // todobig - be more clever about logging - if very important or interesting, we might want to print to log AND to stderr
        FILE *fp = fpStdout;

        if (fp == nullptr) {
            // todobig - output to screen?
            return -1;
        }

        if (levelMask & kLogFatalError) { fprintf(fp, "** FATAL ERROR: "); }
        else if (levelMask & kLogError) { fprintf(fp, "ERROR: "); }
        else if (levelMask & kLogWarning) { fprintf(fp, "WARNING: "); }
//        else if (levelMask & kLogInfo) {} // fprintf(fp,"INFO: "); }
//        else {} // fprintf(fp,"UNKNOWN: "); }

        va_list argptr;
        va_start(argptr, fmt);
        vfprintf(fp, fmt, argptr);
        va_end(argptr);

        if (levelMask & kLogFatalError) {
            fprintf(fp, "** Exiting on FATAL ERROR\n");
            fflush(fp);
            exit(-1);
        }

        if (doFlushFlag) {
            fflush(fp);
        }

        return 0;
    }
}
