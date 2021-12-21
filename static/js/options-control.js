/**
 * Options Control
 * An interface to interact with map options. Initially displays a button, which will show a modal with options on click.
 *
 * Does not implement any options.
 *
 * References the global scope to change:
 *  - creationBedrockVizVersion
 *  - showChunkCoordinatesFlag
 *  - showNetherCoordinatesFlag
 *  - doCheckPlayerDistanceFlag
 *  - doChunkGrid() function
 *  - doSlimeChunks() function
 *  - doElevationAlpha() function
 *  - doElevationStatic() function
 *  - doShadedRelief() function
 *  - doModal() function
 *
 * @todo big - Global scope reference
 * @todo Move the option HTML off the template and into here, create it with JS like other Controls. May have issues with the sliders.
 * @todo make the update checks come from https://api.github.com/repos/bedrock-viz/bedrock-viz/releases/latest for the latest version info.
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
const OptionsControl = function(opt_options) {
    const options = opt_options || {};

    // currently open options modal
    let optionsModal;

    // keeps track of which options are turned off/on with a map - key: boolean
    const optionStates = {};

    // create settings button and icon, bind click event
    const $settingsButton = $(document.createElement('button'))
        .addClass('mytooltip').attr('title', 'Map Options')
        .on('click touchstart', showOptions)
        .append(
            $(document.createElement('img'))
                .addClass('interface-icon')
                .attr('src', 'images/map-control-assets/settings.png')
        );

    // create the main map control element
    const element = document.createElement('div')

    // bind various events to the modal content
    setUpModalContent();

    $(element)
        .addClass('settings ol-unselectable ol-control')
        .append($settingsButton);

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

    // private methods

    /**
     * Bind to change events on the sliders, cllick events on the option toggles
     */
    function setUpModalContent() {
        const $modalBody = $('#options-modal-content').find('.modal-body').first();

        // set initial value and bind to slider events
        $modalBody.find('.elevationAlphaOpacityOut').html(elevationAlphaOpacity);
        $modalBody.find('.elevationAlphaOpacity')
            .val(elevationAlphaOpacity)
            .change(() => {
                elevationAlphaOpacity = $modalBody.find('.elevationAlphaOpacity').val();
                $modalBody.find('.elevationAlphaOpacityOut').html(elevationAlphaOpacity);
                updateElevationAlphaOpacity();
            });

        $modalBody.find('.elevationStaticOpacityOut').html(elevationStaticOpacity);
        $modalBody.find('.elevationStaticOpacity')
            .val(elevationStaticOpacity)
            .change(() => {
                elevationStaticOpacity = $modalBody.find('.elevationStaticOpacity').val();
                $modalBody.find('.elevationStaticOpacityOut').html(elevationStaticOpacity);
                updateElevationStaticOpacity();
            });

        $modalBody.find('.shadeOpacityOut').html(shadeOpacity);
        $modalBody.find('.shadeOpacity')
            .val(shadeOpacity)
            .change(() => {
                shadeOpacity = $modalBody.find('.shadeOpacity').val();
                $modalBody.find('.shadeOpacityOut').html(shadeOpacity);
                if (layerElevation !== null) {
                    layerElevation.setOpacity(shadeOpacity / 100.0 );
                }
            });

        // if there's been a CORS warning, don't bother showing the dynamic shader relief options, it ain't gonna work
        if (globalCORSWarningFlag) {
            $modalBody.find('.dynamicShadedRelief').hide();
        }

        // click handler, when an options is clicked, toggle the option state and take appropriate action
        $modalBody.delegate('.option-toggle', 'click', (e) => {
            const selectedOption = $(e.currentTarget).data('option');
            const currentOptionState = optionStates[selectedOption];
            const targetOptionState = !currentOptionState;

            // depending on which option was clicked (based on data-option), do what needs to be done
            switch (selectedOption) {
                case 'chunkGrid':
                    doChunkGrid(targetOptionState)
                    break;
                case 'chunkDisplayToggle':
                    showChunkCoordinatesFlag = !showChunkCoordinatesFlag;
                    break;
                case 'showNetherCoordinatesToggle':
                    showNetherCoordinatesFlag = !showNetherCoordinatesFlag;
                    break;
                case 'slimeChunksToggle':
                    doSlimeChunks(targetOptionState);
                    break;
                case 'checkPlayerDistanceToggle':
                    doCheckPlayerDistanceFlag = !doCheckPlayerDistanceFlag;
                    if (vectorPoints) {
                        vectorPoints.changed();
                    }
                    break;
                case 'elevationAlphaToggle':
                    doElevationAlpha(targetOptionState);
                    break;
                case 'elevationStaticToggle':
                    doElevationStatic(targetOptionState);
                    break;
                case 'checkForUpdates':
                    checkForUpdate();
                    break;
                case 'about':
                    console.log(optionsModal);
                    if (optionsModal) {
                        optionsModal.modal('hide');
                        optionsModal = null;
                    }
                    doModal(
                        'About Bedrock Viz',
                        globalAboutMessage
                    );
                    break;
                case 'dynamicShadedRelief':
                    doShadedRelief(targetOptionState);
                    break;
            }

            optionStates[selectedOption] = targetOptionState;

            applyOptionStatesToInterface();
        });
    }

    /**
     * After checking for a new version, show information about the result of the check in a modal.
     * @param newVersion the new version number to compare against the current
     * @param newVersionHighlight a brief blurb about the new version, to highlight in the main modal content area
     * @param changeLog a detailed changelog
     */
    function showUpdateInfo(newVersion, newVersionHighlight, changeLog) {
        const isCurrent = String(newVersion).trim().toLowerCase() === String(creationBedrockVizVersion).trim().toLowerCase();
        let title;
        let msg;
        if (!isCurrent) {
            title = 'New Update Available!';
            msg = 'You are running <b>v' + creationBedrockVizVersion + '</b> and <b>v' + newVersion + '</b> is available on GitHub.<br/><br/>' +
                'New Version Highlight:<br/><b>' + newVersionHighlight + '</b><br/><br/>';
        }
        else
        {
            title = 'You are running the latest version';
            msg = 'You are running the latest version: <b>v' + creationBedrockVizVersion + '</b><br/><br/>';
        }

        msg += '<div class="panel panel-default">' +
            '<div class="panel-heading" role="tab" id="headingOne">' +
            '<h4 class="panel-title">' +
            '<a role="button" data-toggle="collapse" href="#collapseOne" aria-expanded="true" aria-controls="collapseOne">' +
            'View Changelog' +
            '</a></h4></div>' +
            '<div id="collapseOne" class="panel-collapse collapse" role="tabpanel" aria-labelledby="headingOne">' +
            '<div class="panel-body"><div class="pre-scrollable">' + changeLog + '</div></div>' +
            '</div></div>' +
            (!isCurrent ? '<a target="_blank" href="https://github.com/bedrock-viz/bedrock-viz">Click here to go to GitHub and grab the update</a>' : '');

        if (optionsModal) {
            optionsModal.modal('hide');
            optionsModal = null;
        }
        doModal( title, msg);
    }

    /**
     * Get the changelog for the specified version
     * @param newVersion the version to get the changelog for
     */
    function doCheckUpdate_getChangeLog(newVersion) {
        // get data from github
        const url = 'https://raw.githubusercontent.com/bedrock-viz/bedrock-viz/master/CHANGELOG.md';

        $.ajax({
            type: 'GET',
            url: url,
            dataType: 'text',
            cache: false,
            success: (result) => {
                // parse this: Latest Highlight: New stuff added");
                let newVersionHighlight = '(See ChangeLog on GitHub)';
                const res = result.match(/Latest highlight\s*:\s*(.+)/);
                if (res) {
                    newVersionHighlight = res[1];
                }
                showUpdateInfo(newVersion, newVersionHighlight, marked(result));
            },
            error: () => {
                showUpdateInfo(newVersion, '(Sorry, we had a problem checking the ChangeLog -- See ChangeLog on GitHub)', '(See ChangeLog on GitHub)');
            }
        });
    }

    /**
     * Check the changelog on github for a version, and if it is different, fetch a change log.
     */
    function checkForUpdate() {
        const url = 'https://raw.githubusercontent.com/bedrock-viz/bedrock-viz/master/CHANGELOG.md';
        $.ajax({
            type: 'GET',
            url: url,
            dataType: 'text',
            cache: false,
            success: (result) => {
                // parse this: Latest release: X.Y.Z
                const res = result.match(/Latest release\s*:\s*(\S+)/);
                if ( res ) {
                    doCheckUpdate_getChangeLog(res[1]);
                } else {
                    if (optionsModal) {
                        optionsModal.modal('hide');
                        optionsModal = null;
                    }
                    doModal('Error', 'Sorry, failed to find version info on GitHub.');
                }
            },
            error: (jqXHR, textStatus, errorThrown) => {
                if (optionsModal) {
                    optionsModal.modal('hide');
                    optionsModal = null;
                }
                doModal('Error',
                    'Sorry, failed to check for update: Status [' + textStatus + '] error [' + errorThrown + ']');
            }
        });
    }

    /**
     * Sets the active class on any options that are toggled on
     */
    function applyOptionStatesToInterface() {
        Object.keys(optionStates).forEach(optionKey => {
            const element = $('#options-modal-content').find(`.modal-body .option-toggle[data-option="${optionKey}"]`);
            if (!!element && !!optionStates[optionKey]) {
                element.addClass('active');
            } else {
                element.removeClass('active');
            }
        });
    }

    /**
     * Show a modal dialog with a form that allows interaction with the map options.
     * Uses the element #options-modal-content, expected to be in the HTML document.
     */
    function showOptions() {
        optionsModal = $('#options-modal-content').first().modal({});
        applyOptionStatesToInterface();
    }
};
ol.inherits(OptionsControl, ol.control.Control);
