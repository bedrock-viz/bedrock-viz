/**
 * View Mode Control
 * Show an interface to interact with the various map views (Overview, Biome, Height) as well as change view Dimension
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
let ViewModeControl = function(opt_options) {
    const options = opt_options || {};

    let timerIn;
    let timerOut;

    const viewModes = [
        {title: 'Overview', id: 0, icon: 'images/view-mode-assets/overview-icon.png'},
        {title: 'Biome', id: 1, icon: 'images/view-mode-assets/biome-icon.png'},
        {title: 'Height', id: 2, icon: 'images/view-mode-assets/height-icon.png'},
        {title: 'Greyscale Height', id: 3, icon: 'images/view-mode-assets/height-greyscale-icon.png'},
        {title: 'Block Light', id: 4, icon: 'images/view-mode-assets/block-light-icon.png'},
        {title: 'Grass Color', id: 5, icon: 'images/view-mode-assets/grass-color-icon.png'},
    ];

    const dimensions = [
        {title: 'Overworld', id: 0, icon: 'images/view-mode-assets/overworld-icon.png'},
        {title: 'Nether', id: 1, icon: 'images/view-mode-assets/nether-icon.png'},
        {title: 'The End', id: 2, icon: 'images/view-mode-assets/the-end-icon.png'}
    ];

    const $displayWrapper = $(document.createElement('div'));
    $displayWrapper.addClass('display-preview-wrapper');

    const $viewModeDisplayIcon = $(document.createElement('img'));
    $viewModeDisplayIcon.attr('src', 'images/view-mode-assets/overview-icon.png');
    $viewModeDisplayIcon.addClass('view-mode-preview');

    const $dimensionDisplayIcon = $(document.createElement('img'));
    $dimensionDisplayIcon.attr('src', 'images/view-mode-assets/overworld-icon.png');
    $dimensionDisplayIcon.addClass('dimension-preview');

    $displayWrapper.append($viewModeDisplayIcon);
    $displayWrapper.append($dimensionDisplayIcon);

    const $dimensionWrapper = $(document.createElement('div'));
    $dimensionWrapper.addClass('dimension-switcher');

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
    $dimensionWrapper.delegate('.dimension-select', 'click', (e) => {
        const id = +$(e.currentTarget).attr('data-id');
        setDimensionById(id);
        const dimensionNow = dimensions.find(i => i.id === id);
        if (dimensionNow) {
            $dimensionDisplayIcon.attr('src', dimensionNow.icon);
        }
    });

    const $viewModeWrapper = $(document.createElement('div'));
    $viewModeWrapper.addClass('view-switcher');
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
    $viewModeWrapper.delegate('.view-mode-select', 'click', (e) => {
        const id = +$(e.currentTarget).attr('data-id');
        setLayerById(id);
        const viewModeNow = viewModes.find(i => i.id === id);
        if (viewModeNow) {
            $viewModeDisplayIcon.attr('src', viewModeNow.icon);
        }
    });

    const $element = $(document.createElement('div'));
    $element.attr('id', 'view-control');

    $dimensionWrapper.hide();
    $viewModeWrapper.hide();

    $element.append($displayWrapper);
    $element.append($dimensionWrapper);
    $element.append($viewModeWrapper);

    $element.mouseleave((e) => {
        // todo: there is still a way to sneak your mouse out of the element without it collapsing, if you use the tooltip as a lane.
        if (
            $(e.fromElement).attr('id') !== $element.attr('id') &&
            !!e.toElement.tagName &&
            String(e.toElement.tagName).toLowerCase() !== 'canvas'
        ) {
            return;
        }
        if (!!timerIn) {
            clearTimeout(timerIn);
        }
        if (!!timerOut) {
            clearTimeout(timerOut);
        }
        $element.removeClass('active');
        $dimensionWrapper.hide();
        $viewModeWrapper.hide();
        timerIn = setTimeout(() => {
            $displayWrapper.show();
            timerIn = null;
        }, 200);
    });

    $displayWrapper.mouseenter((e) => {
        $element.addClass('active');
        $displayWrapper.hide();
        if (!!timerIn) {
            clearTimeout(timerIn);
        }
        if (!!timerOut) {
            clearTimeout(timerOut);
        }
        timerOut = setTimeout(() => {
            $dimensionWrapper.fadeIn();
            $viewModeWrapper.fadeIn();
            timerOut = null;
        }, 500);
    });

    ol.control.Control.call(this, {
        element: $element[0],
        target: options.target
    });

};
ol.inherits(ViewModeControl, ol.control.Control);
