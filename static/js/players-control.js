/**
 * Players Control
 * Show an interface to toggle on/off player visibility, and to center on the Local Player
 *
 * Does not implement any options.
 *
 * References the global scope to change:
 *  - projection
 *  - dimensionInfo
 *  - globalDimensionId
 *  - map (could easily get this through the map control API)
 *
 * @todo Global scope reference
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
const PlayersControl = function(opt_options) {
    const options = opt_options || {};

    // toggle button to show/hide players on the map
    let $showPlayersButton;

    // the entityID of "the player" -- ie, the local player that generated the map
    const playerEntityId = 63;

    // create players toggle button and icon, bind click event
    $showPlayersButton = $(document.createElement('button'))
        .on('click touchstart', showPlayers)
        .addClass('mytooltip inline-block').attr('title', 'Show Players')
        .append(
            $(document.createElement('img'))
                .addClass('interface-icon')
                .attr('src', 'images/map-control-assets/player-icon.png')
        );

    // create center on player button and icon, bind click event
    const $centerLocalPlayerButton = $(document.createElement('button'))
        .on('click touchstart', centerLocalPlayer)
        .addClass('mytooltip inline-block').attr('title', 'Center Map on Player')
        .append(
            $(document.createElement('img'))
                .addClass('interface-icon')
                .attr('src', 'images/map-control-assets/target-location.png')
        )

    const element = document.createElement('div');

    $(element)
        .addClass('players ol-unselectable ol-control')
        .append($showPlayersButton)
        .append($centerLocalPlayerButton);

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

    // private methods

    /**
     * Toggle the visibility of the local player Entity, and add/remove a class to the button to indicate its status
     */
    function showPlayers() {
        let cflag = true;
        if (vectorPoints === null) {
            cflag = false;
            loadVectors();
        }
        if (listEntityToggle[playerEntityId] === undefined) {
            listEntityToggle[playerEntityId] = true;
        } else {
            listEntityToggle[playerEntityId] = !listEntityToggle[playerEntityId];
        }
        if ( cflag ) {
            vectorPoints.changed();
        }

        if (!$showPlayersButton) {
            return;
        }
        if ($showPlayersButton.hasClass('pin-active')) {
            $showPlayersButton.removeClass('pin-active');
        } else {
            $showPlayersButton.addClass('pin-active');
        }
    }

    /**
     * Create a map view and center it on the current player X/Y
     */
    function centerLocalPlayer() {
        const view = new ol.View({
            projection: projection,
            center: [
                dimensionInfo[globalDimensionId].playerPosX,
                dimensionInfo[globalDimensionId].playerPosY
            ],
            resolution: 1
        });
        map.setView(view);
    }
};
ol.inherits(PlayersControl, ol.control.Control);
