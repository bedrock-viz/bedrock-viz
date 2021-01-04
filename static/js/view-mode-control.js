/**
 * View Mode Control
 * Show an interface to interact with the various map views (Overview, Biome, Height) as well as change view Dimension
 *
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
let ViewModeControl = function(opt_options) {
    const options = opt_options || {};

    // timers to manage showing/hiding the preview vs. the real interface
    let timerIn;
    let timerOut;

    // available view modes (aka "images" aka "layers")
    const viewModes = [
        {title: 'Overview', id: 0, icon: 'images/view-mode-assets/overview-icon.png'},
        {title: 'Biome', id: 1, icon: 'images/view-mode-assets/biome-icon.png'},
        {title: 'Height', id: 2, icon: 'images/view-mode-assets/height-icon.png'},
        {title: 'Greyscale Height', id: 3, icon: 'images/view-mode-assets/height-greyscale-icon.png'},
        {title: 'Block Light', id: 4, icon: 'images/view-mode-assets/block-light-icon.png'},
        {title: 'Grass Color', id: 5, icon: 'images/view-mode-assets/grass-color-icon.png'},
    ];

    // available dimensions
    const dimensions = [
        {title: 'Overworld', id: 0, icon: 'images/view-mode-assets/overworld-icon.png'},
        {title: 'Nether', id: 1, icon: 'images/view-mode-assets/nether-icon.png'},
        {title: 'The End', id: 2, icon: 'images/view-mode-assets/the-end-icon.png'}
    ];

    // create an element to hold the preview
    const $displayWrapper = $(document.createElement('div'))
        .addClass('display-preview-wrapper');

    // create an image with the default view mode icon
    const $viewModeDisplayIcon = $(document.createElement('img'))
        .attr('src', 'images/view-mode-assets/overview-icon.png')
        .addClass('view-mode-preview');

    // create an image with the default dimension icon
    const $dimensionDisplayIcon = $(document.createElement('img'))
        .attr('src', 'images/view-mode-assets/overworld-icon.png')
        .addClass('dimension-preview');

    // add both images to the preview wrapper
    $displayWrapper.append($viewModeDisplayIcon);
    $displayWrapper.append($dimensionDisplayIcon);

    // build a container to hold the available dimensions
    const $dimensionWrapper = $(document.createElement('div'))
        .addClass('dimension-switcher');

    // loop available dimensions, add an element to select each one
    dimensions.forEach(item => {
        const $dimensionItem = $(document.createElement('div'));
        $dimensionItem
            .addClass('mytooltip dimension-select')
            .attr({
                title: item.title,
                'data-id': item.id
            })
            .html(item.title)
            .css('backgroundImage', `url(${item.icon})`);
        $dimensionWrapper.append($dimensionItem);
    });

    // attach an event delegate to the wrapper, to catch clicks on an item
    $dimensionWrapper.delegate('.dimension-select', 'click', (e) => {
        const id = +$(e.currentTarget).attr('data-id');
        setDimensionById(id);
        const dimensionNow = dimensions.find(i => i.id === id);
        if (dimensionNow) {
            $dimensionDisplayIcon.attr('src', dimensionNow.icon);
        }
    });

    // create a container to hold the available view modes
    const $viewModeWrapper = $(document.createElement('div'));
    $viewModeWrapper.addClass('view-switcher');

    // loop the available view modes, add an element to select each one
    viewModes.forEach(item => {
        const $viewModeItem = $(document.createElement('div'));
        $viewModeItem
            .addClass('mytooltip view-mode-select')
            .attr({
                title: item.title,
                'data-id': item.id
            })
            .html(item.title)
            .css('backgroundImage', `url(${item.icon})`);
        $viewModeWrapper.append($viewModeItem);
    });

    // attach an event delegate to the wrapper, to catch clicks on an item
    $viewModeWrapper.delegate('.view-mode-select', 'click', (e) => {
        const id = +$(e.currentTarget).attr('data-id');
        setLayerById(id);
        const viewModeNow = viewModes.find(i => i.id === id);
        if (viewModeNow) {
            $viewModeDisplayIcon.attr('src', viewModeNow.icon);
        }
    });

    $dimensionWrapper.hide();
    $viewModeWrapper.hide();

    // create the main map control element
    const element = document.createElement('div');

    // build the interface
    $(element)
        .attr('id', 'view-control')
        .append($displayWrapper)
        .append($dimensionWrapper)
        .append($viewModeWrapper);

    // listen for the mouse leaving the element, hide the interface and go back to the preview
    $(element).mouseleave((e) => {
        // todo: there is still a way to sneak your mouse out of the element without it collapsing, if you use the tooltip as a lane.
        // make sure we are really leaving the element, and not getting a weird signal from mousing around the child elements
        if (
            $(e.fromElement).attr('id') !== $element.attr('id') &&
            !!e.toElement.tagName &&
            String(e.toElement.tagName).toLowerCase() !== 'canvas'
        ) {
            return;
        }

        // clean up any current timers
        if (!!timerIn) {
            clearTimeout(timerIn);
        }
        if (!!timerOut) {
            clearTimeout(timerOut);
        }

        // remove the active class and hide the selection interface
        $element.removeClass('active');
        $dimensionWrapper.hide();
        $viewModeWrapper.hide();

        // pause a bit and show the preview
        timerIn = setTimeout(() => {
            $displayWrapper.show();
            timerIn = null;
        }, 200);
    });

    // when a mouse enters the preview, add the active class, hide the preview, show the selection interface
    $displayWrapper.mouseenter((e) => {
        $element.addClass('active');
        $displayWrapper.hide();

        // clean up any current timers
        if (!!timerIn) {
            clearTimeout(timerIn);
        }
        if (!!timerOut) {
            clearTimeout(timerOut);
        }

        // pause a bit and show the selection interface
        timerOut = setTimeout(() => {
            $dimensionWrapper.fadeIn();
            $viewModeWrapper.fadeIn();
            timerOut = null;
        }, 500);
    });

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

};
ol.inherits(ViewModeControl, ol.control.Control);
