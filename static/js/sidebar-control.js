/**
 * Sidebar Control
 * Shows a slide in/out sidebar overlay on map with toggles for object and mob visibility.
 *
 * Does not implement any options.
 *
 * References the global scope to change:
 *  - listEntityToggle
 *  - listTileEntityToggle
 *  - listBlockToggle
 *  - spawnableEnableFlag
 *  - vectorPoints
 *  - loadVectors() function
 *
 * @todo Global scope reference
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
let SidebarControl = function(opt_options) {
    const options = opt_options || {};

    // known types of entities, any other type that comes from config will be unknown and not work
    const knownEntityTypes = ['entity', 'tile', 'block', 'spawnable-block'];

    // the main map control element
    let element;

    // if the visible-only filter is active
    let filterVisibleActive = false;

    // main content area element
    let $sidebarContent;

    // filter input
    let $sidebarFilter;

    // Sidebar toggle element
    const $sidebarToggle = $(document.createElement('div'))
        .attr('id', 'sidebar-toggle')
        .append(
            $(document.createElement('img'))
                .attr('src', 'images/sidebar-assets/arrow_left.png')
        );

    // when the toggle is clicked, either open or close the sidebar
    $sidebarToggle.click(function (e) {
        $sidebarFilter.val('');
        if (!$(element).hasClass('open')) {
            $('body').addClass('sidebar-open');
            doUpdateEntityList();
            $(element).addClass('open');
            $sidebarToggle.find('img').addClass('open');
            $sidebarFilter.focus();
        } else {
            $('body').removeClass('sidebar-open');
            $sidebarContent.find('.sidebar-content-group p').remove();
            $(element).removeClass('open');
            $sidebarToggle.find('img').removeClass('open');
        }
    });

    // Brand and logo area
    const $sidebarHeaderWrapper = $(document.createElement('div'))
        .addClass('sidebar-header-wrapper');

    const $sidebarLogoImage = $(document.createElement('img'))
        .attr('src', 'images/sidebar-assets/bedrock-viz-logo.png');

    const $sidebarLogoText = $(document.createElement('div'))
        .html('Bedrock Viz')
        .addClass('sidebar-brand');

    // Filter label, input, and clear button
    const $sidebarFilterWrapper = $(document.createElement('div'))
        .addClass('sidebar-filter-wrapper');

    const $sidebarFilterText = $(document.createElement('label'))
        .html('Search for Entity');

    $sidebarFilter = $(document.createElement('input'))
        .attr({
            'placeholder': 'type 2 or more characters to search',
            'type': 'search'
        })
        .keyup(e => {
            doUpdateEntityList();
        });

    const $sidebarFilterClear = $(document.createElement('button'))
        .html('X')
        .addClass('sidebar-filter-clear')
        .attr('role', 'button')
        .click(e => {
            $sidebarFilter.val('');
            doUpdateEntityList();
        });

    // build filter area
    $sidebarFilterWrapper
        .append($sidebarFilterText)
        .append($sidebarFilter)
        .append($sidebarFilterClear);

    // wrapper for "quick links", utilities like expand/collapse all
    const $sidebarQuickLinkWrapper = $(document.createElement('div'))
        .addClass('sidebar-quick-link-wrapper');

    const $sidebarQuickLink1 = $(document.createElement('a'))
        .html('Expand All')
        .click(() => {
            $sidebarContent.find('.sidebar-content-group').each((idx, ele) => {
                if ($(ele).find('.collapse.in').length === 0) {
                    $(ele).find('.sidebar-group-heading').click();
                }
            });
        });

    const $sidebarQuickLink2 = $(document.createElement('a'))
        .html('Collapse All')
        .click(() => {
            $sidebarContent.find('.sidebar-content-group').each((idx, ele) => {
                if ($(ele).find('.collapse.in').length > 0) {
                    $(ele).find('.sidebar-group-heading').click();
                }
            });
        });

    const $sidebarQuickLink3 = $(document.createElement('a'))
        .html('Clear All Visible')
        .click(() => {
            $('#sidebar .entity-toggle').each((idx, ele) => {
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

    const $sidebarQuickLink4 = $(document.createElement('a'))
        .html('Filter Visible')
        .click(() => {
            filterVisibleActive = !filterVisibleActive;
            if (filterVisibleActive) {
                $sidebarQuickLink4.addClass('pin-active');
            } else {
                $sidebarQuickLink4.removeClass('pin-active');
            }
            doUpdateEntityList();
        });

    // add the links to the wrapper
    $sidebarQuickLinkWrapper.append($sidebarQuickLink1)
        .append(' | ')
        .append($sidebarQuickLink2)
        .append(' | ')
        .append($sidebarQuickLink3)
        .append(' | ')
        .append($sidebarQuickLink4);

    // build header area
    $sidebarHeaderWrapper.append($sidebarLogoImage)
        .append($sidebarLogoText)
        .append($sidebarFilterWrapper)
        .append($sidebarQuickLinkWrapper);

    // main content container
    $sidebarContent = $(document.createElement('div'))
        .attr('id', 'sidebar-content');

    // add logo/filter header
    $sidebarContent.append($sidebarHeaderWrapper);

    // main element for control
    element = document.createElement('div');

    // build the interface
    $(element)
        .attr('id', 'sidebar')
        .append($sidebarToggle)
        .append($sidebarContent);

    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

    // Public API

    /**
     * Update the list of entity toggles on the sidebar
     */
    this.updateEntityList = function() {
        doUpdateEntityList();
    };

    // private methods

    /**
     * Checks the given entity, by id, for visibility state
     * @param id id of entity to check
     * @param type the entity type, must be a type that is in knownEntityTypes
     * @returns {boolean}
     */
    function checkEntityVisibility(id, type) {
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

    /**
     * Sets the given entity's visibility state to the indicated state
     * @param id id of entity to check
     * @param type the entity type, must be a type that is in knownEntityTypes
     * @param state true/false visible
     * @returns {boolean} if the state was actually changed
     */
    function setEntityVisibility(id, type, state) {
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
    }

    /**
     * Update the entity list, with filter text if any. If there are no groups displayed, show a "not found" message.
     * If there is only one group, force it open.
     *
     * @param filterText optional text to filter the entity list
     */
    function drawEntities(filterText) {
        // kill lingering tooltips
        $sidebarContent.find('[aria-describedby]').each((idx, ele) => {
            const tooltipId = $(ele).attr('aria-describedby');
            $(`#${tooltipId}`).remove();
        });

        $sidebarContent.find('.sidebar-content-group').remove();

        Array.from(sidebarControlConfig).forEach(sidebarItemGroup => {
            const $newGroupElement = buildOneSidebarContentGroup(sidebarItemGroup, filterText);
            if (!!$newGroupElement) {
                $sidebarContent.append($newGroupElement);
            }
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
    }

    /**
     * Updates the entity list, with or without a filter
     */
    function doUpdateEntityList() {
        if ($sidebarFilter.val().length > 2) {
            drawEntities(String($sidebarFilter.val()).toLowerCase());
        } else {
            drawEntities();
        }
    }

    /**
     * Constructs the UI for one entity group, including the accordion and each selectable item. Uses sessionStorage to remember the open/closed
     * state of individual groups.
     *
     * @param group the group to render
     * @param filterText optional filter text to reduce the number of items shown
     * @returns {jQuery|HTMLElement}
     */
    function buildOneSidebarContentGroup(group, filterText) {
        let itemsToShow = Array.from(group.items);

        // if there is a filter, apply it to the items
        if (!!filterText) {
            itemsToShow = itemsToShow.filter(i => String(i.label).toLowerCase().includes(filterText));
        }

        // if the filter visible is active, only show visible items
        if (filterVisibleActive) {
            itemsToShow = itemsToShow.filter(item => checkEntityVisibility(item.id, item.type));
        }

        // don't draw the group if there aren't any items
        if (itemsToShow.length === 0) {
            return null;
        }

        // name used for IDs and to group DOM elements
        const keyName = String(group.title).toLowerCase().replace(' ', '');

        // figure out if this should start expanded (default yes)
        let restoreState = sessionStorage.getItem(keyName);
        restoreState = restoreState !== 'false';

        // this element will eventually hold the items
        const $contentGroupItemWrapper = $(document.createElement('div'));

        // build the accordion elements
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
            .html(group.title)
            .append(
                $(document.createElement('img'))
                    .attr('src', 'images/sidebar-assets/arrow_left.png')
            )
            .click(() => {
                $contentGroupCollapser.collapse('toggle');
                $contentGroupHeading.find('img').toggleClass('open');
                const currentState = $contentGroupHeading.find('img').hasClass('open');
                sessionStorage.setItem(keyName, String(currentState));
            });

        if (restoreState) {
            $contentGroupCollapser.addClass('in');
            $contentGroupHeading.find('img').addClass('open');
        }

        // build the toggle all on/off UI for this group
        const $contentGroupToggleAllWrapper = $(document.createElement('div'))
            .addClass('feature-toggle-all-wrapper');
        $contentGroupCollapser.append($contentGroupToggleAllWrapper);

        const $contentGroupShowAll = $(document.createElement('a'))
            .addClass('feature-toggle-all feature-toggle-add-all')
            .attr({
                'data-group': keyName,
                'href': 'javascript: void(0)'
            })
            .html('Show All')
            .click(() => {
                $contentGroupItemWrapper.find('.entity-toggle').each((index, ele) => {
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
        $contentGroupToggleAllWrapper.append($contentGroupShowAll);

        $contentGroupToggleAllWrapper.append(' | ');

        const $contentGroupHideAll = $(document.createElement('a'))
            .addClass('feature-toggle-all feature-toggle-remove-all')
            .attr({
                'data-group': keyName,
                'href': 'javascript: void(0)'
            })
            .html('Hide All')
            .click(() => {
                $contentGroupItemWrapper.find('.entity-toggle').each((index, ele) => {
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
        $contentGroupToggleAllWrapper.append($contentGroupHideAll);

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
                    .html();
                const $thisItemImage = $(document.createElement('img'));
                $thisItemImage
                	.addClass('entity-image')
                	.attr('src', item.icon)
                	.html();
                const $thisItemLabel = $(document.createElement('div'));
                $thisItemLabel
                	.addClass('entity-label')
                	.html(item.label);
				$thisItem.append($thisItemImage);
                $thisItem.append($thisItemLabel);

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

        // bind an event delegate to catch clicking on the individual elements
        $contentGroupItemWrapper.delegate('.entity-toggle', 'click touchstart', (e) => {
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

        // finish putting the elements together
        $contentGroupWrapper.append($contentGroupHeading);
        $contentGroupWrapper.append($contentGroupCollapser);

        // bind up any tooltips in the newly created group
        $contentGroupWrapper.find('.mytooltip').tooltip({
            trigger: 'hover',
            container: 'body'
        });

        return $contentGroupWrapper
    }
};
ol.inherits(SidebarControl, ol.control.Control);
