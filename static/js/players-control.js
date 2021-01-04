/**
 * Players Control
 * Show an interface to toggle on/off player visiblity, and to center on the Local Player
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
var PlayersControl = function(opt_options) {
    var options = opt_options || {};
    var showPlayersButton;

    const playerEntityId = 63;

    this.showPlayers = function(e) {
        var cflag = true;
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

        if (!showPlayersButton) {
            return;
        }
        if ($(showPlayersButton).hasClass('pin-active')) {
            $(showPlayersButton).removeClass('pin-active');
        } else {
            $(showPlayersButton).addClass('pin-active');
        }
    };

    this.centerLocalPlayer = function(e) {
        var view = new ol.View({
            projection: projection,
            center: [
                dimensionInfo[globalDimensionId].playerPosX,
                dimensionInfo[globalDimensionId].playerPosY
            ],
            resolution: 1
        });
        map.setView(view);
    };

    showPlayersButton = document.createElement('button');
    showPlayersButton.innerHTML = '<img src="images/map-control-assets/player-icon.png" class="interface-icon" />';
    showPlayersButton.addEventListener('click', this.showPlayers, false);
    showPlayersButton.addEventListener('touchstart', this.showPlayers, false);
    $(showPlayersButton).addClass('mytooltip inline-block').attr('title', 'Show Players');

    var centerLocalPlayerButton = document.createElement('button');
    centerLocalPlayerButton.innerHTML = '<img src="images/map-control-assets/target-location.png" class="interface-icon" />';
    centerLocalPlayerButton.addEventListener('click', this.centerLocalPlayer, false);
    centerLocalPlayerButton.addEventListener('touchstart', this.centerLocalPlayer, false);
    $(centerLocalPlayerButton).addClass('mytooltip inline-block').attr('title', 'Center Map on Player');

    var element = document.createElement('div');
    element.className = 'players ol-unselectable ol-control';
    element.appendChild(showPlayersButton);
    element.appendChild(centerLocalPlayerButton);

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

};
ol.inherits(PlayersControl, ol.control.Control);
