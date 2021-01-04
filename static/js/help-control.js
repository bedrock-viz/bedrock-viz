/**
 * Help Control
 * Show an interface invoke the Help tour
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
var HelpControl = function(opt_options) {

    var options = opt_options || {};
    var this_ = this;
    this._selfReference = null;

    var doTour = function () {
        var featureExtra = '<br/><br/>' +
            'You can click on mobs/objects on the map to get information about them.<br/>';

        var tour = new Tour({
            backdropContainer: 'body',
            storage: false,
            container: 'body',
            orphan: true,
            // backdrop: true,

            steps: [
                {
                    title: 'Welcome to Bedrock Viz!',
                    content: 'This tour will show you the features of the <span class="nobreak"><b>Bedrock Viz Viewer</b></span> web app.<br/>' +
                        '<br/>' +
                        'You can naviate the steps of this tour with your arrow keys (<kbd>&larr;</kbd> and <kbd>&rarr;</kbd>).  You can stop it by pressing <kbd>ESC</kbd>.'
                },
                {
                    element: '#map',
                    title: 'The Map',
                    content: 'The main area of the web app.<br/>' +
                        '<br/>' +
                        'Some ways to interact with the map:<br/><ul>' +
                        '<li>Drag the map to move it</li>' +
                        '<li>Mouse Wheel up and down to zoom</li>' +
                        '<li>Double click to zoom in</li>' +
                        '<li><kbd>Shift</kbd> + Double click to zoom out</li>' +
                        '<li>Rotate map with <kbd>Shift</kbd> + <kbd>Alt</kbd> + drag</li>' +
                        '</ul>' +
                        '<i>Tips:</i><br/><ul>' +
                        '<li>Press "G" to toggle chunk grid</li>' +
                        '<li>Press "N" to toggle showing Nether coordinate conversion</li>' +
                        '</ul>'
                },
                {
                    element: '.ol-mouse-position',
                    placement: 'left',
                    title: 'Mouse Position',
                    content: 'Position of the mouse is shown here as well as information on the block under the mouse cursor.'
                },
                {
                    element: '.ol-full-screen',
                    title: 'Full Screen',
                    placement: 'left',
                    content: 'You can toggle on/off full screen mode with this control.'
                },
                {
                    element: '.ol-control.help',
                    title: 'Help',
                    placement: 'left',
                    content: 'Access this tour using this control.'
                },
                {
                    element: '.ol-control.settings',
                    placement: 'left',
                    title: 'Options',
                    content: 'You can set options here.<br/>' +
                        '<br/><ul>' +
                        '<li><b>Show Chunk Grid</b> overlays a grid showing the chunk boundaries in your world.</li>' +
                        '<li><b>Show Slime Chunks</b> overlays green on slime chunks.  <i>Note: we\'re currently using the MCPC slime chunk calculation.  It is not known if this is accurate for MCPE.</i></li>' +
                        '<li><b>Show Height Shading</b> overlays a shading based on height.  Higher blocks are brighter than lower blocks.</li>' +
                        '<li><b>Show Shaded Relief</b> overlays a shaded relief elevation map.  <i>Pre-generated</i> is generated when bedrock_viz runs.  <i>Dynamic</i> is generated in the web app.  You can alter the settings to change the display.  Note that due to bugs in one of the support libraries, <i>Dynamic</i> is not available when in tiled image mode.</li>' +
                        '<li><b>Check for update</b> checks for an <b>Bedrock Viz</b> update on GitHub.</li>' +
                        '</ul>'
                },
                {
                    element: '.measure',
                    placement: 'left',
                    title: 'Measurement Tools',
                    content: 'You can measure distances with Lines and Circles.<br/>' +
                        '<br/>' +
                        'You can quit measurement mode by pressing <kbd>ESC</kbd><br/>' +
                        '<br/>' +
                        'You can quickly access Line measurement mode by pressing <kbd>L</kbd><br/>' +
                        '<br/>' +
                        'You can quickly access Circle measurement mode by pressing <kbd>C</kbd><br/>' +
                        ''
                },
                {
                    element: '.ol-control.layers',
                    placement: 'left',
                    title: 'Select Layer',
                    content: 'You can view individual layers of your world, if you ran Bedrock Viz in "html-all" mode.'
                },
                {
                    element: '.ol-zoom-extent',
                    title: 'Zoom to Full Extent',
                    placement: 'left',
                    content: 'Zoom out so that the entire world is visible.'
                },
                {
                    element: '.ol-zoom',
                    title: 'Zoom Controls',
                    placement: 'left',
                    content: 'You can also zoom in and out with these buttons.'
                },
                {
                    element: '.ol-control.players',
                    title: 'Player Controls',
                    placement: 'left',
                    content: 'Toggle off/on player display on the map, and center the map on the player location.'
                },
                {
                    element: '.ol-scale-line',
                    title: 'Map Scale',
                    placement: 'left',
                    content: 'Shows a visual representation of the map scale.'
                },

                {
                    element: '#sidebar-toggle',
                    title: 'Entity View Sidebar',
                    placement: 'right',
                    content: 'You can toggle Entity visibility on the map, displaying indicators for the location of an Entity.'
                },
                {
                    element: '#view-control',
                    placement: 'right',
                    title: 'Select Dimension and View Mode',
                    content: 'You can switch between dimensions (e.g. Overworld, The Nether).<br/>' +
                        'You can also switch between different views of your world.<br/>' +
                        '<br/><ul>' +
                        '<li><b>Overview</b> is an image of the highest blocks in your world.</li>' +
                        '<li><b>Biome</b> is an image of the biomes in your world.</li>' +
                        '<li><b>Height</b> is an image of the heights of the highest blocks in your world.  Red is below sea level, Green is above.</li>' +
                        '<li><b>Height (Grayscale)</b> is an image of the heights of the highest blocks in your world in grayscale.</li>' +
                        '<li><b>Block Light</b> is an image of the block light levels of the highest blocks in your world.</li>' +
                        '<li><b>Grass Color</b> is an image of the color of grass in all parts of your world.</li>' +
                        '</ul>'
                },
                {
                    title: 'About Bedrock Viz',
                    content: globalAboutMessage
                }
            ]});
        tour.init();
        tour.start();
    };

    var helpButton = document.createElement('button');
    helpButton.innerHTML = '<img src="images/map-control-assets/help-icon.png" class="interface-icon" />';
    helpButton.addEventListener('click', doTour, false);
    helpButton.addEventListener('touchstart', doTour, false);
    $(helpButton).addClass('mytooltip inline-block').attr('title', 'Help');

    var element = document.createElement('div');
    element.className = 'help ol-unselectable ol-control';
    element.appendChild(helpButton);

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

};
ol.inherits(HelpControl, ol.control.Control);
