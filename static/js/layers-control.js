/**
 * LayersControl Control
 * Show an interface to interact with the map layers.
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
var LayersControl = function(opt_options) {
    var options = opt_options || {};
    var this_ = this;
    this._selfReference = null;

    var $layerCurrentDisplay;

    var doLayerUp = function() {
        const delta = 1;
        layerRawIndex += delta;
        if (layerRawIndex < 0) { layerRawIndex = 0; }
        if (layerRawIndex > 255) { layerRawIndex = 255; }
        layerGoto(layerRawIndex);
        $layerCurrentDisplay.html(layerRawIndex);
    };

    var doLayerDown = function() {
        const delta = -1;
        layerRawIndex += delta;
        if (layerRawIndex < 0) { layerRawIndex = 0; }
        if (layerRawIndex > 255) { layerRawIndex = 255; }
        layerGoto(layerRawIndex);
        $layerCurrentDisplay.html(layerRawIndex);
    };

    var doJumpToLayer = function (layerNumber) {
        layerGoto(layerNumber);
        $layerCurrentDisplay.html(layerRawIndex);
    }

    var removeLayerJumpEntry = function () {
        $('.layer-jump-entry-box-wrapper').remove();
    };

    var getJumpLayerValue = function ($el) {
        let intValue = parseInt($el.val());
        if (intValue < 0) {
            $el.val('0');
            intValue =  0;
        }
        if (intValue > 255) {
            $el.val('255');
            intValue =  255;
        }

        return intValue;
    }

    var showLayerJumpEntry = function () {
        var $layerJumpEntryWrapper = $(document.createElement('div'));
        $layerJumpEntryWrapper.addClass('layer-jump-entry-box-wrapper');

        var $layerJumpEntry = $(document.createElement('input'));
        $layerJumpEntry.attr({
            'type': 'number',
            'min': '0',
            'max': '255'
        });
        $layerJumpEntry.val(layerRawIndex);
        $layerJumpEntry.addClass('layer-jump-entry-box');
        $layerJumpEntryWrapper.append($layerJumpEntry);
        $layerJumpEntry.keyup(e => {
            console.log('keyup', e);
            const lowerCaseKeyName = String(e.key).toLowerCase();
            if (lowerCaseKeyName === "enter") {
                const layerValue = getJumpLayerValue($layerJumpEntry);
                doJumpToLayer(layerValue);
                removeLayerJumpEntry();
            }
        });

        $layerJumpEntry.change(e => {
            console.log('change', e);
            const layerValue = getJumpLayerValue($layerJumpEntry);
            doJumpToLayer(layerValue);
        });

        $layerJumpEntry.blur(e => {
            removeLayerJumpEntry();
        });

        var $layerJumpEntrySubmit = $(document.createElement('buton'));
        $layerJumpEntrySubmit.attr('role', 'buttn=on');
        $layerJumpEntrySubmit.addClass('btn btn-primary');
        $layerJumpEntrySubmit.html('GO');
        $layerJumpEntryWrapper.append($layerJumpEntrySubmit);
        $layerJumpEntrySubmit.click(e => {
            const layerValue = getJumpLayerValue($layerJumpEntry);
            doJumpToLayer(layerValue);
            removeLayerJumpEntry();
        });

        $(element).before($layerJumpEntryWrapper);
        $layerJumpEntry.focus();
    };

    $layerCurrentDisplay = $(document.createElement('span'));
    $layerCurrentDisplay.html('-');
    $layerCurrentDisplay.addClass('mytooltip layer-current-display').attr('title', 'Current Layer');

    var layerUpButton = document.createElement('button');
    layerUpButton.innerHTML = '<img src="images/map-control-assets/layer-up-icon.png" class="interface-icon" />';
    layerUpButton.addEventListener('click', doLayerUp, false);
    layerUpButton.addEventListener('touchstart', doLayerUp, false);
    $(layerUpButton).addClass('mytooltip inline-block').attr('title', 'Go up a Layer');

    var layerJumpButton = document.createElement('button');
    layerJumpButton.innerHTML = '<img src="images/map-control-assets/layer-jump-icon.png" class="interface-icon" />';
    layerJumpButton.addEventListener('click', showLayerJumpEntry, false);
    layerJumpButton.addEventListener('touchstart', showLayerJumpEntry, false);
    $(layerJumpButton).addClass('mytooltip inline-block').attr('title', 'Jump to Layer');

    var layerDownButton = document.createElement('button');
    layerDownButton.innerHTML = '<img src="images/map-control-assets/layer-down-icon.png" class="interface-icon" />';
    layerDownButton.addEventListener('click', doLayerDown, false);
    layerDownButton.addEventListener('touchstart', doLayerDown, false);
    $(layerDownButton).addClass('mytooltip inline-block').attr('title', 'Go down a Layer');

    var element = document.createElement('div');
    element.className = 'layers ol-unselectable ol-control';
    $(element).append($layerCurrentDisplay);
    element.append(layerUpButton);
    element.appendChild(layerJumpButton);
    element.appendChild(layerDownButton);

/*    $('.layerGoto').click(function() {
        var id = +$(this).attr('data-id');
        layerGoto(id);
    });*/

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

};
ol.inherits(LayersControl, ol.control.Control);
