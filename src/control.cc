#include "control.h"
#include "logger.h"
#include "global.h"

namespace mcpe_viz {
    Control control;

    void Control::setupOutput() {
        fpLog = fopen(this->fnLog().generic_string().c_str(), "w");
        if (fpLog) {
            fpLogNeedCloseFlag = true;
        }
        else {
            fprintf(stderr,
                "ERROR: Failed to create output log file (%s error=%s (%d)).  Reverting to stdout...\n",
                this->fnLog().generic_string().c_str(), strerror(errno), errno);
            fpLog = stdout;
            fpLogNeedCloseFlag = false;
        }

        // setup logger
        logger.setStdout(fpLog);
        logger.setStderr(stderr);

        if (doHtml) {
            listGeoJSON.clear();
        }
    }
}