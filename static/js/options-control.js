// todo - make the update checks come from https://api.github.com/repos/bedrock-viz/bedrock-viz/releases/latest for the latest version info.
// todobig - make this a bootstrap dialog box that has a clickable link

/**
 * Options Control
 * Show an interface to interact with map options in a modal
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
var OptionsControl = function(opt_options) {
    var options = opt_options || {};
    var this_ = this;
    this._selfReference = null;

    var optionStates = {};

    function showUpdateInfo(newVersion, newVersionHighlight, changeLog) {
        var isCurrent = newVersion == creationBedrockVizVersion;
        var title;
        var msg;
        if (!isCurrent)
        {
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

        doModal( title, msg);
    }

    function doCheckUpdate_getChangeLog(newVersion) {
        // get data from github
        var url = 'https://raw.githubusercontent.com/bedrock-viz/bedrock-viz/master/CHANGELOG.md';

        $.ajax({
            type: 'GET',
            url: url,
            dataType: 'text',
            cache: false,
            success: function(result, textStatus, jqxhr) {

                // parse this: Latest Highlight: New stuff added");
                var newVersionHighlight = '(See ChangeLog on GitHub)';
                var res = result.match(/Latest highlight\s*:\s*(.+)/);
                if ( res ) {
                    newVersionHighlight = res[1];
                }

                showUpdateInfo(newVersion, newVersionHighlight, marked(result));
            },
            error: function(jqXHR, textStatus, errorThrown, execptionObject) {
                showUpdateInfo(newVersion, '(Sorry, we had a problem checking the ChangeLog -- See ChangeLog on GitHub)', '(See ChangeLog on GitHub)');
            }
        });
    }

    var checkForUpdate = function () {
        var url = 'https://raw.githubusercontent.com/bedrock-viz/bedrock-viz/master/CHANGELOG.md';
        $.ajax({
            type: 'GET',
            url: url,
            dataType: 'text',
            cache: false,
            success: function(result, textStatus, jqxhr) {
                // parse this: Latest release: X.Y.Z
                var res = result.match(/Latest release\s*:\s*(\S+)/);
                if ( res ) {
                    doCheckUpdate_getChangeLog(res[1]);
                } else {
                    doModal('Error', 'Sorry, failed to find version info on GitHub.');
                }

            },
            error: function(jqXHR, textStatus, errorThrown, execptionObject) {
                doModal('Error',
                    'Sorry, failed to check for update: Status [' + textStatus + '] error [' + errorThrown + ']');
            }
        });
    };

    var applyOptionStatesToInterface = function () {
        Object.keys(optionStates).forEach(optionKey => {
            const element = $(`.modal-body .option-toggle[data-option="${optionKey}"]`);
            if (!!element && !!optionStates[optionKey]) {
                element.addClass('active');
            } else {
                element.removeClass('active');
            }
        });
    };

    var showOptions = function () {
        doModal(
            "Map Options",
            $('#options-modal-content').first().html(),
            {
                width: '60W'
            }
        );

        $('.modal-body').find('.elevationAlphaOpacity').val(elevationAlphaOpacity);
        $('.modal-body').find('.elevationAlphaOpacityOut').html(elevationAlphaOpacity);
        $('.modal-body').find('.elevationAlphaOpacity').change(function() {
            elevationAlphaOpacity = $('.modal-body').find('.elevationAlphaOpacity').val();
            $('.modal-body').find('.elevationAlphaOpacityOut').html(elevationAlphaOpacity);
            updateElevationAlphaOpacity();
        });

        $('.modal-body').find('.elevationStaticOpacity').val(elevationStaticOpacity);
        $('.modal-body').find('.elevationStaticOpacityOut').html(elevationStaticOpacity);
        $('.modal-body').find('.elevationStaticOpacity').change(function() {
            elevationStaticOpacity = $('.modal-body').find('.elevationStaticOpacity').val();
            $('.modal-body').find('.elevationStaticOpacityOut').html(elevationStaticOpacity);
            updateElevationStaticOpacity();
        });

        $('.modal-body').find('.shadeOpacity').val(shadeOpacity);
        $('.modal-body').find('.shadeOpacityOut').html(shadeOpacity);
        $('.modal-body').find('.shadeOpacity').change(function() {
            shadeOpacity = $('.modal-body').find('.shadeOpacity').val();
            $('.modal-body').find('.shadeOpacityOut').html(shadeOpacity);
            if (layerElevation !== null) {
                layerElevation.setOpacity(shadeOpacity / 100.0 );
            }
        });

        if (globalCORSWarningFlag) {
            $('.modal-body').find('.dynamicShadedRelief').hide();
        }

        $('.modal-body .option-toggle').click(function (e) {
           const selectedOption = $(e.currentTarget).data('option');
           const currentOptionState = optionStates[selectedOption];
           const targetOptionState = !currentOptionState;

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

        applyOptionStatesToInterface();
    };

    var settingsButton = document.createElement('button');
    settingsButton.innerHTML = '<img src="images/map-control-assets/settings.png" class="interface-icon" />';
    $(settingsButton).addClass('mytooltip').attr('title', 'Map Options');
    settingsButton.addEventListener('click', showOptions, false);
    settingsButton.addEventListener('touchstart', showOptions, false);

    var element = document.createElement('div');
    element.className = 'settings ol-unselectable ol-control';
    element.appendChild(settingsButton);

    $('#gridToggle').click(function() {
        if ($('#gridToggle').parent().hasClass('active')) {
            $('#gridToggle').parent().removeClass('active');
            doChunkGrid(false);
        } else {
            $('#gridToggle').parent().addClass('active');
            doChunkGrid(true);
        }
    });

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

};
ol.inherits(OptionsControl, ol.control.Control);
