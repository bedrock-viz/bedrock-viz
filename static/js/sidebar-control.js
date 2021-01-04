/**
 * Sidebar Control
 * Shows a slide in/out sidebar overlayed on map with toggles for object and mob visibility
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
let SidebarControl = function(opt_options) {
    const options = opt_options || {};

    const knownEntityTypes = ['entity', 'tile', 'block', 'spawnable-block'];

    let filterVisibleActive = false;

    // main content area element
    let $sidebarContent;

    // filter input
    let $sidebarFilter;

    const checkEntityVisibility = function(id, type) {
        if (!knownEntityTypes.includes(type)) {
            console.error(`Sidebar: Cannot check visibility of ${id} from type ${type} -- unknown type`);
            return false;
        }

        if (type === 'entity') {
            return !!listEntityToggle[id];
        } else if (type === 'tile') {
            return !!listTileEntityToggle[id];
        } else if (type === 'block') {
            return !!listBlockToggle[id];
        } else if (type === 'spawnable-block') {
            return spawnableEnableFlag;
        }

        return false;
    }

    const setEntityVisibility = function(id, type, state) {
        if (!knownEntityTypes.includes(type)) {
            console.error(`Sidebar: Cannot toggle visibility of ${id} from type ${type} -- unknown type`);
            return false;
        }

        let cflag = true;
        if ( vectorPoints === null ) {
            cflag = false;
            loadVectors();
        }

        if (type === 'entity') {
            listEntityToggle[id] = state;
        } else if (type === 'tile') {
            listTileEntityToggle[id] = state;
        } else if (type === 'block') {
            listBlockToggle[id] = state;
        } else if (type === 'spawnable-block') {
            spawnableEnableFlag = state;
        }

        if ( cflag ) {
            vectorPoints.changed();
        }

        return true;
    };

    const drawEntities = function (filterText) {
        $sidebarContent.find('[aria-describedby]').each((idx, ele) => {
            const tooltipId = $(ele).attr('aria-describedby');
            $(`#${tooltipId}`).remove();
        }); // kill lingering tooltips

        $sidebarContent.find('.sidebar-content-group').remove();

        Array.from(sidebarControlConfig).forEach(sidebarItemGroup => {
            const $newGroupElement = buildOneSidebarContentGroup(sidebarItemGroup, filterText);
            $sidebarContent.append($newGroupElement);
        });

        const numberOfVisibleGroups = $sidebarContent.find('.sidebar-content-group').length;

        // if there is only one group and it isn't expanded, force it open without changing the restoreState
        if (numberOfVisibleGroups === 1) {
            $sidebarContent.find('.sidebar-content-group').first().find('.collapse').addClass('in');
            $sidebarContent.find('.sidebar-content-group').first().find('.sidebar-group-heading img').addClass('open');
        }

        if (numberOfVisibleGroups > 0) {
            return;
        }

        const $nothingFoundWrapper = $(document.createElement('div'));
        $nothingFoundWrapper.addClass('sidebar-content-group');

        if (!!filterText || filterVisibleActive) {
            if (!!filterText) {
                const $textLine1 = $(document.createElement('p'));
                $textLine1.html(`No Entities found matching the search term <i>"${filterText}"</i>. Try modifying your search term.`);
                $nothingFoundWrapper.append($textLine1);
            } else {
                const $textLine1 = $(document.createElement('p'));
                $textLine1.html('No Entities to list.');
                $nothingFoundWrapper.append($textLine1);
            }

            if (filterVisibleActive) {
                const $textLine1a = $(document.createElement('p'));
                $textLine1a.html('<b>Note</b>: The filter toggle to show only visible Entities is enabled.');
                $nothingFoundWrapper.append($textLine1a);
            }

            if (!!filterText) {
                const $textLine2 = $(document.createElement('p'));
                $textLine2.html('It is also possible the Entity you are looking for is not available yet in Bedrock Viz.');
                $nothingFoundWrapper.append($textLine2);
            }
        } else {
            const $textLine1 = $(document.createElement('p'));
            $textLine1.html('No Entities to list. Possibly missing a sidebar configuration file.');
            $nothingFoundWrapper.append($textLine1);
        }

        $sidebarContent.append($nothingFoundWrapper);
    };

    const doUpdateEntityList = function() {
        if ($sidebarFilter.val().length > 2) {
            drawEntities(String($sidebarFilter.val()).toLowerCase());
        } else {
            drawEntities();
        }
    };

    const buildOneSidebarContentGroup = function (group, filterText) {
        let itemsToShow = Array.from(group.items);
        if (!!filterText) {
            itemsToShow = itemsToShow.filter(i => String(i.label).toLowerCase().includes(filterText));
        }

        if (filterVisibleActive) {
            itemsToShow = itemsToShow.filter(item => checkEntityVisibility(item.id, item.type));
        }

        if (itemsToShow.length === 0) {
            return;
        }

        const keyName = String(group.title).toLowerCase().replace(' ', '');

        // figure out if this should start expanded (default yes)
        let restoreState = sessionStorage.getItem(keyName);
        if (restoreState !== 'false') {
            restoreState = true;
        } else {
            restoreState = false;
        }

        const $contentGroupWrapper = $(document.createElement('div'));
        $contentGroupWrapper
            .addClass('sidebar-content-group')
            .attr('data-name', group.title)

        const $contentGroupHeading = $(document.createElement('div'));

        const $contentGroupCollapser = $(document.createElement('div'));
        $contentGroupCollapser
            .addClass('collapse')
            .attr('id', `${keyName}Toggles`);

        $contentGroupHeading
            .addClass('sidebar-group-heading')
            .attr({
                'data-toggle': 'collapse',
                'data-target': `#${keyName}Toggles`,
                'data-group': keyName,
                'aria-expanded': 'false',
                'aria-controls': `${keyName}Toggles`
            })
            .html(group.title);

        const $contentGroupHeadingIndicator = $(document.createElement('img'));
        $contentGroupHeadingIndicator.attr('src', 'images/sidebar-assets/arrow_left.png');
        $contentGroupHeading.append($contentGroupHeadingIndicator);

        $contentGroupHeading.click(() => {
            $contentGroupCollapser.collapse('toggle');
            $contentGroupHeadingIndicator.toggleClass('open');
            const currentState = $contentGroupHeadingIndicator.hasClass('open');
            sessionStorage.setItem(keyName, String(currentState));
        });

        if (restoreState) {
            $contentGroupCollapser.addClass('in');
            $contentGroupHeadingIndicator.addClass('open');
        }

        const $contentGroupToggleAllWrapper = $(document.createElement('div'));
        $contentGroupToggleAllWrapper.addClass('feature-toggle-all-wrapper');
        $contentGroupCollapser.append($contentGroupToggleAllWrapper);

        const $contentGroupItemWrapper = $(document.createElement('div'));

        const $contentGroupShowAll = $(document.createElement('a'));
        $contentGroupShowAll
            .addClass('feature-toggle-all feature-toggle-add-all')
            .attr({
                'data-group': keyName,
                'href': 'javascript: void(0)'
            })
            .html('Show All');
        $contentGroupToggleAllWrapper.append($contentGroupShowAll);

        $contentGroupToggleAllWrapper.append(' | ');

        const $contentGroupHideAll = $(document.createElement('a'));
        $contentGroupHideAll
            .addClass('feature-toggle-all feature-toggle-remove-all')
            .attr({
                'data-group': keyName,
                'href': 'javascript: void(0)'
            })
            .html('Hide All');
        $contentGroupToggleAllWrapper.append($contentGroupHideAll);

        $contentGroupShowAll.click((e) => {
            const dataGroup = $(e.target).attr('data-group');
            $contentGroupItemWrapper.find(`.entity-toggle`).each((index, ele) => {
                const id = $(ele).attr('data-id');
                const type = $(ele).attr('data-type');
                if (setEntityVisibility(id, type, true)) {
                    $(ele).addClass('active');
                    if (filterVisibleActive) {
                        doUpdateEntityList();
                    }
                }
            });
        });

        $contentGroupHideAll.click((e) => {
            const dataGroup = $(e.target).attr('data-group');
            $contentGroupItemWrapper.find(`.entity-toggle`).each((index, ele) => {
                const id = $(ele).attr('data-id');
                const type = $(ele).attr('data-type');
                if (setEntityVisibility(id, type, false)) {
                    $(ele).removeClass('active');
                    if (filterVisibleActive) {
                        doUpdateEntityList();
                    }
                }
            });
        });

        $contentGroupItemWrapper.addClass('entity-toggle-wrapper');
        $contentGroupCollapser.append($contentGroupItemWrapper);

        // now add items to $contentGroupItemWrapper
        itemsToShow
            .sort((a, b) => (a.label > b.label) ? 1 : -1)
            .forEach(item => {
                const $thisItem = $(document.createElement('div'));
                let checkIconLoad = true;

                if (!item.icon) {
                    item.icon = 'images/sidebar-assets/Unknown.png';
                    checkIconLoad = false;
                }

                $thisItem
                    .addClass('entity-toggle mytooltip')
                    .attr({
                        'data-group': keyName,
                        'data-type': item.type,
                        'data-id': item.id,
                        'title': item.label
                    })
                    .css('backgroundImage', `url(${item.icon})`)
                    .html(item.label);

                // check that the icon is going to load, if it doesn't, fall back to Unknown.png
                if (checkIconLoad) {
                    $('<img/>')
                        .attr('src', item.icon)
                        .error(() => {
                            $thisItem.css('backgroundImage', 'url("images/sidebar-assets/Unknown.png")');
                            $(this).remove();
                        });
                }

                if (checkEntityVisibility(item.id, item.type)) {
                    $thisItem.addClass('active');
                }

                $contentGroupItemWrapper.append($thisItem);
            });

        $contentGroupItemWrapper.delegate('.entity-toggle', 'click', (e) => {
            const id = $(e.target).attr('data-id');
            const type = $(e.target).attr('data-type');
            const currentState = $(e.target).hasClass('active');
            const targetState = !currentState;

            setEntityVisibility(id, type, targetState);

            if (targetState) {
                $(e.target).addClass('active');
            } else {
                $(e.target).removeClass('active');
            }
            if (filterVisibleActive) {
                doUpdateEntityList();
            }
        });

        $contentGroupWrapper.append($contentGroupHeading);
        $contentGroupWrapper.append($contentGroupCollapser);

        $contentGroupWrapper.find('.mytooltip').tooltip({
            trigger: 'hover',
            container: 'body'
        });

        return $contentGroupWrapper
    };

    // Sidebar toggle element
    const $sidebarToggle = $(document.createElement('div'));
    $sidebarToggle.attr('id', 'sidebar-toggle');

    const $sidebarToggleIcon = $(document.createElement('img'));
    $sidebarToggleIcon.attr('src', 'images/sidebar-assets/arrow_left.png');
    $sidebarToggle.append($sidebarToggleIcon);

    $sidebarToggle.click(function (e) {
        $sidebarFilter.val('');
        if (!$(element).hasClass('open')) {
            $('body').addClass('sidebar-open');
            doUpdateEntityList();
            $(element).addClass('open');
            $sidebarToggleIcon.addClass('open');
            $sidebarFilter.focus();
        } else {
            $('body').removeClass('sidebar-open');
            $sidebarContent.find('.sidebar-content-group p').remove();
            $(element).removeClass('open');
            $sidebarToggleIcon.removeClass('open');
        }
    });

    // Brand and logo area
    const $sidebarHeaderWrapper = $(document.createElement('div'));
    $sidebarHeaderWrapper.addClass('sidebar-header-wrapper');

    const $sidebarLogoImage = $(document.createElement('img'));
    $sidebarLogoImage.attr('src', 'images/sidebar-assets/bedrock-viz-logo.png');

    const $sidebarLogoText = $(document.createElement('div'));
    $sidebarLogoText.html('Bedrock Viz');
    $sidebarLogoText.addClass('sidebar-brand');

    // Filter label, input, and clear button
    const $sidebarFilterWrapper = $(document.createElement('div'));
    $sidebarFilterWrapper.addClass('sidebar-filter-wrapper');

    const $sidebarFilterText = $(document.createElement('label'));
    $sidebarFilterText.html('Search for Entity');
    $sidebarFilter = $(document.createElement('input'));
    $sidebarFilter.attr({
        'placeholder': 'type 2 or more characters to search',
        'type': 'search'
    });
    $sidebarFilter.keyup(e => {
        doUpdateEntityList();
    });

    const $sidebarFilterClear = $(document.createElement('button'));
    $sidebarFilterClear.html('X');
    $sidebarFilterClear.addClass('sidebar-filter-clear');
    $sidebarFilterClear.attr('role', 'button');
    $sidebarFilterClear.click(e => {
        $sidebarFilter.val('');
        doUpdateEntityList();
    });

    // build filter area
    $sidebarFilterWrapper.append($sidebarFilterText);
    $sidebarFilterWrapper.append($sidebarFilter);
    $sidebarFilterWrapper.append($sidebarFilterClear);

    const $sidebarQuickLinkWrapper = $(document.createElement('div'));
    $sidebarQuickLinkWrapper.addClass('sidebar-quick-link-wrapper');

    const $sidebarQuickLink1 = $(document.createElement('a'));
    $sidebarQuickLink1.html('Expand All');
    $sidebarQuickLink1.click(() => {
        $sidebarContent.find('.sidebar-content-group').each((idx, ele) => {
            if ($(ele).find('.collapse.in').length === 0) {
                $(ele).find('.sidebar-group-heading').click();
            }
        });
    });

    const $sidebarQuickLink2 = $(document.createElement('a'));
    $sidebarQuickLink2.html('Collapse All');
    $sidebarQuickLink2.click(() => {
        $sidebarContent.find('.sidebar-content-group').each((idx, ele) => {
            if ($(ele).find('.collapse.in').length > 0) {
                $(ele).find('.sidebar-group-heading').click();
            }
        });
    });

    const $sidebarQuickLink3 = $(document.createElement('a'));
    $sidebarQuickLink3.html('Clear All Visible');
    $sidebarQuickLink3.click(() => {
        $(`.entity-toggle`).each((idx, ele) => {
            const id = $(ele).attr('data-id');
            const type = $(ele).attr('data-type');
            if (setEntityVisibility(id, type, false)) {
                $(ele).removeClass('active');
            }
        });
        if (filterVisibleActive) {
            doUpdateEntityList();
        }
    });

    const $sidebarQuickLink4 = $(document.createElement('a'));
    $sidebarQuickLink4.html('Filter Visible');
    $sidebarQuickLink4.click(() => {
        filterVisibleActive = !filterVisibleActive;
        if (filterVisibleActive) {
            $sidebarQuickLink4.addClass('pin-active');
        } else {
            $sidebarQuickLink4.removeClass('pin-active');
        }
        doUpdateEntityList();
    });

    $sidebarQuickLinkWrapper.append($sidebarQuickLink1);
    $sidebarQuickLinkWrapper.append(" | ");
    $sidebarQuickLinkWrapper.append($sidebarQuickLink2);
    $sidebarQuickLinkWrapper.append(" | ");
    $sidebarQuickLinkWrapper.append($sidebarQuickLink3);
    $sidebarQuickLinkWrapper.append(" | ");
    $sidebarQuickLinkWrapper.append($sidebarQuickLink4);

    // build header area
    $sidebarHeaderWrapper.append($sidebarLogoImage);
    $sidebarHeaderWrapper.append($sidebarLogoText);
    $sidebarHeaderWrapper.append($sidebarFilterWrapper);
    $sidebarHeaderWrapper.append($sidebarQuickLinkWrapper);

    // main content container
    $sidebarContent = $(document.createElement('div'));
    $sidebarContent.attr('id', 'sidebar-content');

    // add logo/filter header
    $sidebarContent.append($sidebarHeaderWrapper);

    // main element for control
    const element = document.createElement('div');
    element.id = 'sidebar';

    // build main element
    $(element).append($sidebarToggle);
    $(element).append($sidebarContent);

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

    // Public API
    this.updateEntityList = function() {
        doUpdateEntityList();
    };
};
ol.inherits(SidebarControl, ol.control.Control);
