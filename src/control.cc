#include "control.h"
#include "logger.h"
#include "global.h"

namespace mcpe_viz {
    Control control;

    void Control::setupOutput() {
        if (fnLog.compare("-") == 0) {
            fpLog = stdout;
            fpLogNeedCloseFlag = false;
        }
        else {
            if (fnLog.size() == 0) {
                fnLog = fnOutputBase + ".log";
            }
            fpLog = fopen(fnLog.c_str(), "w");
            if (fpLog) {
                fpLogNeedCloseFlag = true;
            }
            else {
                fprintf(stderr,
                    "ERROR: Failed to create output log file (%s error=%s (%d)).  Reverting to stdout...\n",
                    fnLog.c_str(), strerror(errno), errno);
                fpLog = stdout;
                fpLogNeedCloseFlag = false;
            }
        }

        // setup logger
        logger.setStdout(fpLog);
        logger.setStderr(stderr);

        if (doHtml) {
            fnGeoJSON = fnOutputBase + ".geojson";

            listGeoJSON.clear();

            fnHtml = fnOutputBase + ".html";
            fnJs = fnOutputBase + ".js";
        }
    }
}