/**
 * Layers Control
 * An interface to interact with the map layers. Includes an up and down layer button, as well as a button that displays
 * a floating number entry box that will interact with the layer setting.
 *
 * Does not implement any options.
 *
 * References the global scope to change:
 *  - layerRawIndex
 *
 * @todo Global scope reference
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
const LayersControl = function(opt_options) {
    const options = opt_options || {};

    // element for displaying the current layer. Declared as a scope global so utility functions can change the value
    let $layerCurrentDisplay;

    // display area for current layer number
    $layerCurrentDisplay = $(document.createElement('span'))
        .html('-')
        .addClass('mytooltip layer-current-display').attr('title', 'Current Layer');

    // create layer up button and icon, bind click event
    const $layerUpButton = $(document.createElement('button'))
        .on('click touchstart', doLayerUp)
        .addClass('mytooltip')
        .attr('title', 'Go up a Layer')
        .append(
            $(document.createElement('img'))
                .addClass('interface-icon')
                .attr('src', 'images/map-control-assets/layer-up-icon.png')
        );

    // create layer jump button and icon, bind click event
    const $layerJumpButton = $(document.createElement('button'))
        .on('click touchstart', showLayerJumpEntry)
        .addClass('mytooltip')
        .attr('title', 'Jump to Layer')
        .append(
            $(document.createElement('img'))
                .addClass('interface-icon')
                .attr('src', 'images/map-control-assets/layer-jump-icon.png')
        );

    // create layer down button and icon, bind click event
    const $layerDownButton = $(document.createElement('button'))
        .on('click touchstart', doLayerDown)
        .addClass('mytooltip')
        .attr('title', 'Go down a Layer')
        .append(
            $(document.createElement('img'))
                .addClass('interface-icon')
                .attr('src', 'images/map-control-assets/layer-down-icon.png')
        );

    // create the main map control element
    const element = document.createElement('div');

    // build the interface
    $(element)
        .addClass('layers ol-unselectable ol-control')
        .append($layerCurrentDisplay)
        .append($layerUpButton)
        .append($layerJumpButton)
        .append($layerDownButton);

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

    // private methods

    /**
     * Sets the active layer to current + 1. Values greater than 255 will use 255.
     */
    function doLayerUp() {
        layerRawIndex += 1;
        if (layerRawIndex < 0) { layerRawIndex = 0; }
        if (layerRawIndex > 255) { layerRawIndex = 255; }
        layerGoto(layerRawIndex);
        $layerCurrentDisplay.html(layerRawIndex);
    }

    /**
     * Sets the active layer to current - 1. Values less than 0 will use 0.
     */
    function doLayerDown() {
        layerRawIndex += -1;
        if (layerRawIndex < 0) { layerRawIndex = 0; }
        if (layerRawIndex > 255) { layerRawIndex = 255; }
        layerGoto(layerRawIndex);
        $layerCurrentDisplay.html(layerRawIndex);
    }

    /**
     * Sets the active layer to the specified layer number
     * @param layerNumber the layer number to go to
     */
    function doJumpToLayer(layerNumber) {
        layerGoto(layerNumber);
        $layerCurrentDisplay.html(layerRawIndex);
    }

    /**
     * Called when entry is complete in the layer jump input
     */
    function removeLayerJumpEntry() {
        $('.layer-jump-entry-box-wrapper').remove();
    }

    /**
     * Used to extract a value from the layer jump input. If a value greater than 255 is entered, sets the input to 255 and returns 255.
     * If a value less than 0 is entered, sets the input to 0 and returns 0.
     * @param $el the element to read the value from
     * @returns {number} the value of the element or a min/max default from the valid range
     */
    function getJumpLayerValue($el) {
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

    /**
     * Creates a floating input to collect the desired layer to jump to.
     * Value starts off with the current active layer number.
     */
    function showLayerJumpEntry() {
        // create a wrapper for the entry box
        const $layerJumpEntryWrapper = $(document.createElement('div'))
            .addClass('layer-jump-entry-box-wrapper');

        // create an input with events
        const $layerJumpEntry = $(document.createElement('input'))
            .attr({
                'type': 'number',
                'min': '0',
                'max': '255'
            })
            .val(layerRawIndex)
            .addClass('layer-jump-entry-box')

            // when a key is released, check if it is enter. If it is, go to the currently entered layer and remove the box
            .keyup(e => {
                const lowerCaseKeyName = String(e.key).toLowerCase();
                if (lowerCaseKeyName === "enter") {
                    const layerValue = getJumpLayerValue($layerJumpEntry);
                    doJumpToLayer(layerValue);
                    removeLayerJumpEntry();
                }
            })

            // when the value changes, immediately go to the entered layer
            .change(() => {
                const layerValue = getJumpLayerValue($layerJumpEntry);
                doJumpToLayer(layerValue);
            })

            // when focus leaves the entry box, remove this interface
            .blur(() => removeLayerJumpEntry());

        $layerJumpEntryWrapper.append($layerJumpEntry);

        // create a button that will "submit" the layer change, and remove the interface (just like if you press enter in the entry box)
        const $layerJumpEntrySubmit = $(document.createElement('buton'))
            .attr('role', 'buttn=on')
            .addClass('btn btn-primary')
            .html('GO')
            .click(() => {
                const layerValue = getJumpLayerValue($layerJumpEntry);
                doJumpToLayer(layerValue);
                removeLayerJumpEntry();
            });

        $layerJumpEntryWrapper.append($layerJumpEntrySubmit);

        // add the entry box wrapper to dom before the layer control, so it's still "on the map" and works in fullscreen
        $(element).before($layerJumpEntryWrapper);

        // focus the layer input box
        $layerJumpEntry.focus();
    }
};
ol.inherits(LayersControl, ol.control.Control);
