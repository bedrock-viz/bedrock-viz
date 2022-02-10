/*
  bedrock_viz openlayers viewer
    maintained by   : https://github.com/jasper-wan
    contributions by: https://github.com/paulgrahek, https://github.com/tomnolan, https://github.com/bazfp, https://github.com/DazWorrall
    forked from     : https://github.com/Plethora777/mcpe_viz
  

  todobig

  * tiling - see test-xyz* dirs for code; BUT tiling breaks the elevation overlay
  -- it appears that since we fake the Z in tiles that we always get resolution=1 data in the shade() function.  Sigh.

  * introduce layers -- e.g. rail; leaves; water -- that can be toggled on/off.  could be quite cool.  
  -- requires re-think of bedrock_viz program -- need to produce a base layer (solid blocks) and then overlay layers with other things (e.g. rail; leaves; water; torches: etc)
  -- how to prioritize layers? hmmm... 
  -- ideas for layers:
  - trees: leaves; saplings; vines; cocoa; wood that is up/down?
  - rails: all types
  - flowers etc: grasses; dead bush; dandelion; flower; cactus; lily pad; large flowers
  - light: torch; jack o'lantern; redstone lamp; glowstone; fire?
  - chests; chest; trapped chest
  - redstone: redstone wire; lever; pressure plates; r.torch; buttons; tripwire; tripwire hooks; daylight sensor
  - crops: wheat; sugar cane; pumpkin stem; melon stem; netherwart; carrot; potato; beetroot;  mushrooms
  - tools: crafting table; stone cutter; furnace; brewing stand; enchantment table; anvil
  - fences: fences; fencegates; walls
  - decorations: bed; flower pot; mob head; carpet; cake; sign; cobweb
  - doors: doors; trapdoors
  - glass: glass; glass pane
  - ???: ladder; ironbars; snow
  - water


  todo

  * toggle btwn overworld / nether - show player est. spot in nether even if they are in overworld
  -- some tool to help w/ planning portals (e.g. mark a spot in overworld and show where it maps in nether)

  * todobig -- Chrome appears to be demented about serving local files.  You get CORS errors.  Not at all clear that this can be resolved w/o really ugly workarounds (e.g. disabling chrome security); This could be the case with MS Edge on win10 also.
  -- http://stackoverflow.com/questions/3102819/disable-same-origin-policy-in-chrome
  -- https://chrome.google.com/webstore/detail/allow-control-allow-origi/nlfbmbojpeacfghkpbjhddihlkkiljbi?hl=en
  
  * some mods in test-all2 -- combine raw layer + regular layer selector; experiments w/ multilevel dropdown for mobs

  * goto X -- e.g. Player; World Origin; World Spawn; Player Spawn; etc

  * improve measure tools ui

  * improve the ui - it's sorta clunky :)

  * some reporting of details in geojson -- counts of different items?

  * idea from /u/sturace -- filter by pixel value (e.g. show only oak trees)

  * when in raw layer mode, auto load layer before/after the layer that gets loaded? (improve perceived speed)

  * how to get navbar + popovers in full screen?

  * web: why does click on feature not always work?

  * ability to hover over a pixel and get info (e.g. "Jungle Biome - Watermelon @ (X,Z,Y)"); switch image layers
  -- hidden layers for more info in mouse pos window: biomes + height
  -- this could be very easy when tiling works

  * rewrite/adapt to use the google closure tools?

  */


/*
  todo - interesting openlayers examples:

  color manipulation -- color space conversion funcs
  http://openlayers.org/en/v3.11.0/examples/color-manipulation.html

  vector icons
  http://openlayers.org/en/v3.9.0/examples/icon.html

  nav controls
  http://openlayers.org/en/v3.9.0/examples/navigation-controls.html

  overview map
  http://openlayers.org/en/v3.9.0/examples/overviewmap.html
  http://openlayers.org/en/v3.9.0/examples/overviewmap-custom.html
*/

// todo - do a separate geojson file for nether + overworld?

// todo - it might be cool to use ONE projection for both overworld and nether -- nether would auto-adjust?


'use strict';

var map = null, projection = null, extent = null;
var popover = null;

var globalMousePosition = null;
var pixelData = null, pixelDataName = '';

var layerRawIndex = 63;
var layerMain = null, srcLayerMain = null;
var layerSlimeChunks = null, srcLayerSlimeChunks = null;

var measureControl = null;
var mousePositionControl = null;

var globalDimensionId = -1;
var globalLayerMode = 0, globalLayerId = 0;
var globalStyleBlock = null, globalStyleBlockUp = null, globalStyleBlockDown = null, globalStyleBlockSame = null;

var listEntityToggle = [];
var listTileEntityToggle = [];
var listBlockToggle = [];
var spawnableEnableFlag = false;

var globalCORSWarning = 'Bedrock Viz Hint: If you are loading files from the local filesystem, your browser might not be allowing us to load additional files or examine pixels in maps.  Firefox does not have this limitation.  See README for more info...';
var globalCORSWarningFlag = false;

var globalItemStyleSelector = null;
var globalItemStyleCount = 0;

var globalWarnSlimeChunks = false;

var showChunkCoordinatesFlag = false;
var chunkGridFlag = false;

var showNetherCoordinatesFlag = false;

var doCheckPlayerDistanceFlag = false;

// this removes the hideous blurriness when zoomed in
var setCanvasSmoothingMode = function(evt) {
    evt.context.mozImageSmoothingEnabled = false;
    evt.context.webkitImageSmoothingEnabled = false;
    evt.context.msImageSmoothingEnabled = false;
    evt.context.imageSmoothingEnabled = false;
};
var resetCanvasSmoothingMode = function(evt) {
    evt.context.mozImageSmoothingEnabled = true;
    evt.context.webkitImageSmoothingEnabled = true;
    evt.context.msImageSmoothingEnabled = true;
    evt.context.imageSmoothingEnabled = true;
};
var disableLayerSmoothing = function(layer) {
    layer.on('precompose', setCanvasSmoothingMode);
    layer.on('postcompose', resetCanvasSmoothingMode);
};



function map_addLayer(layer) {
    var a = map.getLayers();
    var insertIndex = -1;

    // todo - check for not defined myStackOrder

    // current myStackOrder values:
    // 000 map.addLayer(layerMain);
    // 100 map.addLayer(layerShadedReliefStatic);
    // 110 map.addLayer(layerElevation);
    // 120 map.addLayer(layerElevationAlpha);
    // 200 map.addLayer(layerChunkGrid);
    // 210 map.addLayer(layerSlimeChunks);
    // 300 map.addLayer(vectorPoints);
    // 400 map.addLayer(layerDraw);
    
    var layerStackOrder = +layer.get('myStackOrder');
    a.forEach( function (el, index, arr) {
        var elStackOrder = +el.get('myStackOrder');
        if ( (elStackOrder > layerStackOrder) && (insertIndex < 0) ) {
            insertIndex = index;
        }
    });

    if ( insertIndex < 0 ) {
        insertIndex = a.getLength();
    }
    map.getLayers().insertAt(insertIndex, layer);

    // debug
    if ( false ) {
        a = map.getLayers();
        a.forEach( function (el, index, arr) {
            console.log('layer: index=' + index + ' myStackOrder=' + el.get('myStackOrder') + ' obj=' + el);
        });
    }
}



/**
 * @constructor
 * @param {ol.Map} xmap
 */
var MeasureTool = function(xmap) {

    var this_ = this;

    var map = xmap;

    var sourceDraw = null;
    var layerDraw = null;
    var overlays = [];


    /**
     * Currently drawn feature.
     * @type {ol.Feature}
     */
    var sketch;


    /**
     * The help tooltip element.
     * @type {Element}
     */
    var helpTooltipElement;


    /**
     * Overlay to show the help messages.
     * @type {ol.Overlay}
     */
    var helpTooltip;


    /**
     * The measure tooltip element.
     * @type {Element}
     */
    var measureTooltipElement;


    /**
     * Overlay to show the measurement.
     * @type {ol.Overlay}
     */
    var measureTooltip;


    /**
     * Handle pointer move.
     * @param {ol.MapBrowserEvent} evt
     */
    var pointerMoveHandler = function(evt) {
        if (evt.dragging) {
            return;
        }
        /** @type {string} */
        var helpMsg = 'Click to start drawing; Press ESC to quit measurement mode';

        if (sketch) {
            var geom = (sketch.getGeometry());
            if (geom instanceof ol.geom.Polygon) {
                helpMsg = 'Click to continue drawing the polygon; Double Click to complete';
            } else if (geom instanceof ol.geom.LineString) {
                helpMsg = 'Click to continue drawing the line; Double Click to complete';
            } else if (geom instanceof ol.geom.Circle) {
                helpMsg = 'Click to complete circle';
            }
        }

        helpTooltipElement.innerHTML = helpMsg;
        helpTooltip.setPosition(evt.coordinate);

        $(helpTooltipElement).removeClass('hidden');
    };

    var hideHelpTooltip = function() {
        $(helpTooltipElement).addClass('hidden');
    };

    /**
     * format length output
     * @param {ol.geom.LineString} line
     * @return {string}
     */
    var formatLength = function(line) {
        var length = Math.round(line.getLength() * 100) / 100;

        var output;
        if (length > 1000) {
            output = (Math.round(length / 1000 * 100) / 100) +
                ' ' + 'km';
        } else {
            output = (Math.round(length * 100) / 100) +
                ' ' + 'm';
        }
        return output;
    };


    /**
     * format circle radius output
     * @param {ol.geom.Circle} circle
     * @return {string}
     */
    var formatRadius = function(circle) {
        var length = Math.round(circle.getRadius() * 100) / 100;

        var output;
        if (length > 1000) {
            output = (Math.round(length / 1000 * 100) / 100) +
                ' ' + 'km';
        } else {
            output = (Math.round(length * 100) / 100) +
                ' ' + 'm';
        }
        return output;
    };


    /**
     * format length output
     * @param {ol.geom.Polygon} polygon
     * @return {string}
     */
    var formatArea = function(polygon) {
        var area = polygon.getArea();

        var output;
        if (area > 10000) {
            output = (Math.round(area / 1000000 * 100) / 100) +
                ' ' + 'km<sup>2</sup>';
        } else {
            output = (Math.round(area * 100) / 100) +
                ' ' + 'm<sup>2</sup>';
        }
        return output;
    };


    /**
     * Creates a new help tooltip
     */
    var createHelpTooltip = function() {
        if (helpTooltipElement) {
            helpTooltipElement.parentNode.removeChild(helpTooltipElement);
        }
        helpTooltipElement = document.createElement('div');
        helpTooltipElement.className = 'measureTooltip hidden';
        helpTooltip = new ol.Overlay({
            element: helpTooltipElement,
            offset: [15, 0],
            positioning: 'center-left'
        });
        overlays.push(helpTooltip);
        map.addOverlay(helpTooltip);
    };


    /**
     * Creates a new measure tooltip
     */
    var createMeasureTooltip = function() {
        if (measureTooltipElement) {
            measureTooltipElement.parentNode.removeChild(measureTooltipElement);
        }
        measureTooltipElement = document.createElement('div');
        measureTooltipElement.className = 'measureTooltip measureTooltip-measure';
        measureTooltip = new ol.Overlay({
            element: measureTooltipElement,
            offset: [0, -15],
            positioning: 'bottom-center'
        });
        overlays.push(measureTooltip);
        map.addOverlay(measureTooltip);
    };


    var drawType = 'LineString';

    var draw;

    var createInteraction = function() {
        draw = new ol.interaction.Draw({
            source: sourceDraw,
            type: /** @type {ol.geom.GeometryType} */ (drawType)
            /*
              style: new ol.style.Style({
              fill: new ol.style.Fill({
              color: 'rgba(255, 255, 255, 0.2)'
              }),
              stroke: new ol.style.Stroke({
              color: 'rgba(0, 0, 0, 0.5)',
              lineDash: [3, 3],
              width: 2
              }),
              image: new ol.style.Circle({
              radius: 5,
              stroke: new ol.style.Stroke({
              color: 'rgba(0, 0, 0, 0.7)'
              }),
              fill: new ol.style.Fill({
              color: 'rgba(255, 255, 255, 0.2)'
              })
              })
              })
            */
        });
        map.addInteraction(draw);
        
        createMeasureTooltip();
        createHelpTooltip();

        var listener;
        draw.on('drawstart',
                function(evt) {
                    // set sketch
                    sketch = evt.feature;
                    
                    /** @type {ol.Coordinate|undefined} */
                    var tooltipCoord = evt.coordinate;

                    listener = sketch.getGeometry().on('change', function(evt) {
                        var geom = evt.target;
                        var output;
                        if (geom instanceof ol.geom.Polygon) {
                            output = formatArea(/** @type {ol.geom.Polygon} */ (geom));
                            tooltipCoord = geom.getInteriorPoint().getCoordinates();
                        } else if (geom instanceof ol.geom.LineString) {
                            output = formatLength( /** @type {ol.geom.LineString} */ (geom));
                            tooltipCoord = geom.getLastCoordinate();
                            // we move the coordinate slightly (this avoids the "not able to continue line" problem)
                            var res = map.getView().getResolution() * 1.5;
                            tooltipCoord[0] += res;
                            tooltipCoord[1] += res;
                        } else if (geom instanceof ol.geom.Circle) {
                            output = formatRadius( /** @type {ol.geom.Circle} */ (geom));
                            tooltipCoord = geom.getLastCoordinate();
                        } else {
                            //console.log("unknown geom type in sketch listener");
                        }
                        measureTooltipElement.innerHTML = output;
                        measureTooltip.setPosition(tooltipCoord);
                    }, this_);
                }, this_);
        draw.on('drawend',
                function(evt) {
                    measureTooltipElement.className = 'measureTooltip measureTooltip-static';
                    measureTooltip.setOffset([0, -7]);
                    // unset sketch
                    sketch = null;
                    // unset tooltip so that a new one can be created
                    measureTooltipElement = null;
                    createMeasureTooltip();
                    ol.Observable.unByKey(listener);
                    return true;
                }, this_);
    };
    

    /**
     * Let user change the geometry type.
     * @param {string} dt LineString, Circle, etc
     */
    this.setDrawTypeReal = function(dt) {
        sketch = null;
        drawType = dt;
        map.removeInteraction(draw);
        createInteraction();
    };


    var updateState = function() {
        if ( this_.enabled ) {
            sourceDraw = new ol.source.Vector();
            var width = 3;
            var color = '#f22929';
            layerDraw = new ol.layer.Vector({
                myStackOrder: 400,
                source: sourceDraw,
                style: [ 
                    new ol.style.Style({
                        fill: new ol.style.Fill({
                            color: 'rgba(255, 255, 255, 0.2)'
                        }),
                        stroke: new ol.style.Stroke({ color: '#ffffff', width: width + 2 }),
                        image: new ol.style.Circle({
                            radius: 7,
                            fill: new ol.style.Fill({
                                color: color
                            })
                        })
                    }),
                    new ol.style.Style({
                        stroke: new ol.style.Stroke({ color: color, width: width })
                    })
                ]
            });
            map_addLayer(layerDraw);
            map.on('pointermove', pointerMoveHandler);
            $(map.getViewport()).on('mouseout', hideHelpTooltip);
            createInteraction();
        } else {
            map.removeLayer(layerDraw);
            map.un('pointermove', pointerMoveHandler);
            $(map.getViewport()).off('mouseout', hideHelpTooltip);      
            map.removeInteraction(draw);
            map.removeOverlay(helpTooltip);
            map.removeOverlay(measureTooltip);
            // destroy objects so that they are removed from map
            draw = null;
            layerDraw = null;
            sourceDraw = null;
            for (var i in overlays) {
                map.removeOverlay(overlays[i]);
                overlays[i] = null;
            }
            overlays = [];
        }
    };

    this.toggleEnable = function() {
        this_.enabled = !this_.enabled;
        updateState();
    };

    this.forceStart = function() {
        this_.enabled = true;
        updateState();
    };
    
    this.forceStop = function() {
        this_.enabled = false;
        updateState();
    };

};


/**
 * @param {string} dt draw type (e.g. "LineString", "Circle", etc)
 */
MeasureTool.prototype.setDrawType = function(dt) {
    this.setDrawTypeReal(dt);
};


/**
 * @return {boolean} true if measure control is active
 */
MeasureTool.prototype.isEnabled = function() {
    //console.log('mt enabled = ' + this.enabled);
    return this.enabled;
};



/**
 * @constructor
 * @extends {ol.control.Control}
 * @param {Object=} opt_options Control options.
 */
var MeasureControl = function(opt_options) {
    
    var options = opt_options || {};

    //this.enabled = false;

    var this_ = this;

    this.measureTool = null;

    var setDrawType = function(dt) { 
        this_.measureTool.setDrawType(dt);
    };

    this.handleMeasure = function(e) {
        if ( this_.measureTool === null ) {
            this_.measureTool = new MeasureTool(this_.getMap());
        }
        this_.measureTool.toggleEnable();
        if (this_.measureTool.isEnabled()) {
            $('.measureSubcontrol').show();
        } else {
            $('.measureSubcontrol').hide();
        }
    };
    
    var button = document.createElement('button');
    button.innerHTML = 'M';
    button.addEventListener('click', this.handleMeasure, false);
    button.addEventListener('touchstart', this.handleMeasure, false);
    $(button).addClass('mytooltip inline-block').attr('title', 'Measurement Tools - Press ESC to quit');
    
    var btnLine = document.createElement('button');
    btnLine.innerHTML = 'L';
    btnLine.addEventListener('click', function() { setDrawType('LineString'); }, false);
    btnLine.addEventListener('touchstart', function() { setDrawType('LineString'); }, false);
    $(btnLine).addClass('mytooltip measureSubcontrol inline-block').attr('title', 'Lines - Hotkey L').hide();
    
    var btnCircle = document.createElement('button');
    btnCircle.innerHTML = 'C';
    btnCircle.addEventListener('click', function() { setDrawType('Circle'); }, false);
    btnCircle.addEventListener('touchstart', function() { setDrawType('Circle'); }, false);
    $(btnCircle).addClass('mytooltip measureSubcontrol inline-block').attr('title', 'Circles - Hotkey C').hide();
    
    var element = document.createElement('div');
    element.className = 'measure ol-unselectable ol-control';
    element.appendChild(button);
    element.appendChild(btnLine);
    element.appendChild(btnCircle);
    
    ol.control.Control.call(this, {
        element: element,
        target: options.target
    });

};
ol.inherits(MeasureControl, ol.control.Control);


/**
 * @return {boolean} true if measure control is active
 */
MeasureControl.prototype.isEnabled = function() {
    if ( this.measureTool !== null ) {
        return this.measureTool.isEnabled();
    }
    return false;
};


/**
 * force measurement control to stop
 */
MeasureControl.prototype.forceStop = function() {
    if ( this.isEnabled() ) {
        this.handleMeasure();
    }
};


/**
 * force measurement tool to start and be in a particular drawing mode
 * @param {string} dt drawing type
 */
MeasureControl.prototype.forceDrawType = function(dt) {
    if ( ! this.isEnabled() ) {
        this.handleMeasure();
    }
    this.measureTool.setDrawType(dt);
};



function doGlobalQuit() {
    // user has pressed ESC - we want to quit any special modes
    if (measureControl.isEnabled()) {
        measureControl.forceStop();
    }
    
    // make popover disappear
    var element = popover.getElement();
    $(element).popover('destroy');

    // todo - is this too broad?
    $('.mytooltip_dyn').tooltip('hide');
    $('.mytooltip').tooltip('hide');

    // clear village doors
    srcVillageVectorPoints.clear();

    // todo - others?
}

var loadEventCount = 0;
function updateLoadEventCount(delta) {
    loadEventCount += delta;
    if (loadEventCount < 0) { loadEventCount = 0; }
    
    if (loadEventCount > 0) {
        var pos = $('#map').offset();
        var x1 = pos.left + 70;
        var y1 = pos.top + 20;
        $('#throbber')
            .css({ position: 'absolute', left: x1, top: y1 })
            .show();
        
        var a = [];
        if (loadEventCount > 0) {
            a.push('Layers remaining: ' + loadEventCount);
        }
        $('#throbber-msg').html(a.join('; '));
    } else {
        $('#throbber').hide();
    }
}

function doParsedItem(obj, sortFlag) {
    var v = [];
    var vold = [];
    for (var j in obj) {
        if (obj[j].Name === 'info_reserved6') {
            // swallow this - it is the player's hot slots
        } else {
            var s = '';
            var fnimg = 'images/unknown.png';
            
            if ( obj[j].imgid !== undefined ) {
                fnimg = imageIconLUT['' + obj[j].imgid];
            }
            
            var title = obj[j].Name;
            if (obj[j].Enchantments !== undefined) {
                var ench = obj[j].Enchantments;
                title += ' (';
                var i = ench.length;
                for (var k in ench) {
                    title += ench[k].Name;
                    if (--i > 0) {
                        title += '; ';
                    }
                }
                title += ')';
            }
            if (obj[j].Count !== undefined) {
                if ( obj[j].Count > 1 ) {
                    title += ' (Qty: ' + obj[j].Count + ')';
                }
            }
            // debug
            //title += ' (' + fnimg + ') (' + obj[j].imgid + ')' + ' (' + j + ')';
            
            s += '<div class="col-xs-1 mytooltip_dyn inventory-grid" title="' + title + '">';
            s += '<img class="pull-left inventory-item-image" height="32" width="32" src="' + fnimg + '" />';
            if (obj[j].Count !== undefined) {
                s += '<span class="inventory-item-count">' + obj[j].Count + '</span>';
                // s += obj[j].Count;
            }
            s += '</div>';

            v.push(s);

            // old version:
            s = '<li>' + obj[j].Name;
            if (obj[j].Count !== undefined) {
                s += ' (' + obj[j].Count + ')';
            }
            if (obj[j].Enchantments !== undefined) {
                var ench = obj[j].Enchantments;
                s += ' (';
                var i = ench.length;
                for (var k in ench) {
                    s += ench[k].Name;
                    if (--i > 0) {
                        s += '; ';
                    }
                }
                s += ')';
            }
            s += '</li>';
            vold.push(s);
        }
    }

    // we sort the items, yay
    if (sortFlag) {
        v.sort();
        vold.sort();
    }

    var ct = globalItemStyleCount++;
    var ret = '<div class="myTabsContainer">' +
        '<div role="group">' +
        '<ul class="nav nav-pills btn-group myTabs" role="tablist">' +
        '<li role="presentation"><a href="#itemsIcon' + ct + '" data-id="itemsIcon" class="itemsViewToggle itemsIcon" aria-controls="itemsIcon' + ct + '" role="tab" data-toggle="tab">Icons</a></li>' +
        '<li role="presentation"><a href="#itemsText' + ct + '" data-id="itemsText" class="itemsViewToggle itemsText" aria-controls="itemsText' + ct + '" role="tab" data-toggle="tab">Text</a></li>' +
        '</ul>' +
        '</div>' +
        '<div class="tab-content">';

    // icon version
    ret += '<div role="tabpanel" class="tab-pane" id="itemsIcon' + ct + '">';
    ret += '<div class="container-fluid"><div class="row-fluid">';
    for (var i = 0; i < v.length; i++) {
        ret += v[i];
        if ( ((i + 1) % 8) == 0 ) {
            ret += '</div><div class="row-fluid">';
        }
    }
    ret += '</div></div></div>';

    // text list version
    ret += '<div role="tabpanel" class="tab-pane" id="itemsText' + ct + '">';
    ret += '<div class="container-fluid"><ul>';
    ret += vold.join('\n');
    ret += '</ul></div></div>';

    ret += '</div></div>';
    
    return ret;
}


// adapted from: http://openlayers.org/en/v3.9.0/examples/vector-layer.html
// todo - this does not work?
var highlightStyleCache = {};
var featureOverlay = new ol.layer.Vector({
    source: new ol.source.Vector(),
    map: map,
    style: function(feature, resolution) {
        var text = resolution < 5000 ? feature.get('Name') : '';
        if (!highlightStyleCache[text]) {
            highlightStyleCache[text] = [new ol.style.Style({
                stroke: new ol.style.Stroke({
                    color: '#f00',
                    width: 1
                }),
                fill: new ol.style.Fill({
                    color: 'rgba(255,0,0,0.1)'
                }),
                text: new ol.style.Text({
                    font: '12px Calibri,sans-serif',
                    text: text,
                    fill: new ol.style.Fill({
                        color: '#000'
                    }),
                    stroke: new ol.style.Stroke({
                        color: '#f00',
                        width: 3
                    })
                })
            })];
        }
        return highlightStyleCache[text];
    }
});

// given coords in mcpe world, return coords for openlayers
function mcpeToOpenlayers(tx, ty, adjustToCenterFlag) {
    var px = tx - dimensionInfo[globalDimensionId].globalOffsetX;
    var py = (dimensionInfo[globalDimensionId].worldHeight - 1 - ty) + dimensionInfo[globalDimensionId].globalOffsetY;
    if ( adjustToCenterFlag ) {
        px += 0.5;
        py += 0.5;
    }
    return [ px, py ];
}

// given coords in openlayers, return coords for mcpe world
function openlayersToMcpe(tx, ty, adjustToCenterFlag) {
    var px = tx + dimensionInfo[globalDimensionId].globalOffsetX;
    // todo - is this correct?
    var py = (dimensionInfo[globalDimensionId].worldHeight - 1 - ty) + dimensionInfo[globalDimensionId].globalOffsetY;
    if ( adjustToCenterFlag ) {
        px += 0.5;
        py += 0.5;
    }
    return [ px, py ];
}

function correctGeoJSONName(feature) {
    var name = feature.get('Name');
    if (name == 'MobSpawner') {
        // rename spawner so that feature shows spawner type
        var props = feature.getProperties();
        name = props[name].Name + ' Spawner';
    }
    else if (name == 'NetherPortal') {
        name = 'Nether Portal';
    }
    else if (name == 'Dropped item') {
        // rename dropped item so that it is visible in map
        var props = feature.getProperties();
        name = 'Drop: ' + props.Item.Name;
        // todo - highlight interesting drops? e.g. armor / weapons (i.e. indication of a player death spot)
    }
    else if (name == 'SignNonBlank' || name == 'SignBlank') {
        // rename sign so that it is visible in map
        var props = feature.getProperties();
        var a = [];
        var pushTrimmedString = function(a, str) {
            if (str) {
                var s = str.trim();
                if (s.length > 0 ) {
                    a.push(s);
                }
            }
        };
        pushTrimmedString(a, props[name].Text1);
        pushTrimmedString(a, props[name].Text2);
        pushTrimmedString(a, props[name].Text3);
        pushTrimmedString(a, props[name].Text4);
        if ( a.length > 0 ) {
            name = 'Sign: ' + a.join(' / ');
        } else {
            name = 'Sign';
        }
    }
    else if (name == 'The Player') {
        var props = feature.getProperties();
        if ( props.playerType === 'Local Player' ) {
            name = 'Local Player';
        } else {
            if ( props.playerName !== undefined ) {
                name = props.playerType + ': ' + props.playerName;
            } else {
                name = props.playerType + ': ' + props.playerId;
            }
        }
    }
    return name;
}

function doParsedDoors(doors) {
    // {"idx":0,"idz":-2,"ts":54461602,"Pos":[118,76,-45]},
    var a = [];
    for (var d in doors) {
        var s = '';
        s += '' + '<a href="#" class="layerGoto_dyn mytooltip_dyn" data-id="' + doors[d]['Pos'][1] + '" title="Click to go to layer">' + JSON.stringify(doors[d]['Pos'], null, 2) + '</a>';
        s +=  ' : ' + doors[d]['ts'] + ' : ' + doors[d]['idx'] + ' ' + doors[d]['idz'];
        // make "Pos" clickable if we have raw layers
        a.push(s);
    }
    return a.join('<br/>');
}

function doFeaturePopover(features, id, coordinate) {
    var feature = features[id];
    
    var element = popover.getElement();
    // todo - is this too broad?
    $('.mytooltip_dyn').tooltip('hide');
    $(element).popover('destroy');

    var props = feature.getProperties();

    var name = correctGeoJSONName(feature);

    var stitle = '<div class="mob">';
    // prev/next buttons
    if ( features.length > 1 ) {
        var enabled;

        stitle += '<div class="btn-group btn-group-xs">';

        enabled = (id > 0) ? '' : ' disabled';
        stitle += '<a href="#" class="btn btn-primary btn-xs mytooltip_dyn featurePrev' + enabled + '" title="Previous"><strong>&lt;</strong></a>';

        enabled = (id < (features.length - 1)) ? '' : ' disabled';
        stitle += '<a href="#" class="btn btn-primary btn-xs mytooltip_dyn featureNext' + enabled + '" title="Next"><strong>&gt;</strong></a>';
        
        stitle += '</div>&nbsp;';
    }
    // mob/item title
    if (props.Entity !== undefined) {
        stitle += '<span class="mob_name">' + name + '</span> <span class="mob_id">(id=' + props.id + ')</span>';
    } else {
        stitle += '<span class="mob_name">' + name + '</span>';
    }
    stitle += '</div>';

    if (name === 'Village') {
        // remove old items
        srcVillageVectorPoints.clear();

        // add doors to map
        for (var d in props['Doors']) {
            var door = props['Doors'][d];

            // adjust point to ol coordinates
            var pts = mcpeToOpenlayers(door.Pos[0], door.Pos[2], true);
            
            var pt = new ol.geom.Point( pts );

            var feature = new ol.Feature({
                name: 'Village Door',
                geometry: pt
            });
            
            // copy properties (for popup)
            for (var dp in door) {
                feature.set(dp, door[dp], true);
            }
            feature.set('Name', 'Village Door');

            featuresVillageVectorPoints.push(feature);
        }

        // add a bounding circle for the village
        var cpts = mcpeToOpenlayers(props.Pos[0], props.Pos[2], true);
        var fcircle = new ol.Feature({
            name: 'Village Radius',
            radius: props['radius'],
            geometry: new ol.geom.Point( cpts ),
            Pos: props.Pos,
            Name: 'Village Radius'
        });
        featuresVillageVectorPoints.push(fcircle);

        // todobig todohere - how to remove this?! (currently only escape key will do it)
    }
    
    var s = '<div class="container-fluid my-scrollable">';

    for (var i in props) {
        // skip geometry property because it contains circular refs; skip others that are uninteresting
        if (i !== 'geometry' &&
            i !== 'TileEntity' &&
            i !== 'Entity' &&
            i !== 'Block' &&
            i !== 'pairchest' &&
            i !== 'player' &&
            i !== 'Name' &&
            i !== 'id' &&
            i !== 'etype' &&
            i !== 'Dimension'
           ) {
            if (typeof(props[i]) === 'object') {
                if (i === 'Armor') {
                    var armor = props[i];
                    s += 'Armor:' + doParsedItem(armor, false);
                }
                else if (i === 'Items') {
                    // items in a chest
                    var items = props[i];
                    s += 'Items:' + doParsedItem(items, true);
                }
                else if (i === 'Inventory') {
                    var inventory = props[i];
                    s += 'Inventory:' + doParsedItem(inventory, true);
                }
                else if (i === 'EnderChest') {
                    var inventory = props[i];
                    s += 'Ender Chest Inventory:' + doParsedItem(inventory, true);
                }
                else if (i === 'ItemInHand') {
                    var itemInHand = props[i];
                    s += 'In Hand:' + doParsedItem([itemInHand], false);
                }
                else if (i === 'Item') {
                    var item = props[i];
                    s += 'Item:' + doParsedItem([item], false);
                }
                else if (i === 'Sign') {
                    s += '<div class="mycenter">' +
                        props[i].Text1 + '<br/>' +
                        props[i].Text2 + '<br/>' +
                        props[i].Text3 + '<br/>' +
                        props[i].Text4 + '<br/>' +
                        '</div>';
                }
                else if (i === 'Notes') {
                    for (var n in props[i]) {
                        s += 'Note: <b>' + props[i][n] + '</b><br/>';
                    }
                }
                else if (i === 'MobSpawner') {
                    s += '' +
                        'Name: <b>' + props[i].Name + '</b><br/>' +
                        'entityId: <b>' + props[i].entityId + '</b><br/>';
                }

                // Village-specific stuff
                else if ( i === 'APos' ) {
                    // hide it
                }
                else if ( i === 'Doors' ) {
                    s += 'Door Count: <b>' + props[i].length + '</b><br/>';
                    s += 'Doors (Pos : TS : IDX IDZ):<br/><div class="my-indent">' + doParsedDoors(props[i]) + '</div>';
                }
                else if ( i === 'Players' ) {
                    // todo - more info here? i've never seen this filled
                    s += 'Players Count: <b>' + props[i].length + '</b><br/>';
                }
                else if ( i === 'Villagers' ) {
                    s += 'Villager Count: <b>' + props[i].length + '</b><br/>';
                }
                
                else if (i === 'Pos') {
                    // make "Pos" clickable if we have raw layers
                    s += '' + i + ': <a href="#" class="layerGoto_dyn mytooltip_dyn" data-id="' + props[i][1] + '" title="Click to go to layer">' + JSON.stringify(props[i], null, 2) + '</a><br/>';
                }
                else {
                    s += '' + i + ': ' + JSON.stringify(props[i], null, 2) + '<br/>';
                }
            } else {
                s += '' + i + ': <b>' + props[i].toString() + '</b><br/>';
            }
        }
    }
    s += '</div>';
    popover.setPosition(coordinate);
    $(element).popover({
        'placement': 'auto right',
        'viewport': {selector: '#map', padding: 0},
        'animation': false,
        'trigger': 'click focus',
        'html': true,
        'title': stitle,
        'content': s
    });
    $(element).popover('show');
    $('.mytooltip_dyn').tooltip({
        // this helps w/ btn groups
        trigger: 'hover',
        container: 'body'
    });

    $('.myTabs').tab();
    if ( globalItemStyleSelector !== null ) {
        // todo - HACKY HACK TOWN
        $('a.' + globalItemStyleSelector ).tab('show');
    } else {
        $('a.itemsIcon').tab('show');
    }
    
    $('.myTabs a').click(function (e) {
        e.preventDefault();
        globalItemStyleSelector = $(this).attr('data-id');
        // $(this).tab('show');
        $('a.' + globalItemStyleSelector ).tab('show');
        // todo - is this the correct way to do this?
        //console.log("tab click: e=" + e.toString() + " h=" + globalItemStyleSelector);
    });

    $('.featurePrev').click(function (e) {
        doFeaturePopover(features, id - 1, coordinate);
    });
    $('.featureNext').click(function (e) {
        doFeaturePopover(features, id + 1, coordinate);
    });

    //todobig - would be cool to support up/down left/right as kbd accel for prev/next
    
    // activate links
    // todo - seems silly that we have to do this - can't use regular ones init'ed in main func?
    $('.layerGoto_dyn').click(function() {
        var id = +$(this).attr('data-id');
        layerGoto(id);
    });
    $('.mytooltip_dyn').tooltip({
        // this helps w/ btn groups
        trigger: 'hover',
        container: 'body'
    });
    
    // todo - disabled because this does not appear to work
    if (false) {
        if (feature !== highlight) {
            if (highlight) {
                featureOverlay.getSource().removeFeature(highlight);
            }
            if (feature) {
                featureOverlay.getSource().addFeature(feature);
            }
            highlight = feature;
        }
    }

    return 0;
}

function doFeatureSelect(features, coordinate) {
    var element = popover.getElement();
    // todo - is this too broad?
    $('.mytooltip_dyn').tooltip('hide');
    $(element).popover('destroy');

    var stitle = '<div class="mob"><span class="mob_name">Multiple Items</span></div>\n';

    features.sort(function(a, b) {
        // we use trickery (/255) to sort by height
        var ap = a.getProperties();
        var astr = ap.Name + ' @ ' + (ap.Pos[1] / 255.0) + ', ' + ap.Pos[0] + ', ' + ap.Pos[2];
        var bp = b.getProperties();
        var bstr = bp.Name + ' @ ' + (bp.Pos[1] / 255.0) + ', ' + bp.Pos[0] + ', ' + bp.Pos[2];
        return astr.localeCompare(bstr);
    });
    
    var s = 'Select item:<div class="list-group my-scrollable">';
    for (var i in features) {
        var feature = features[i];
        var props = feature.getProperties();
        // how to do this?
        s += '<a href="#" data-id="' + i + '" class="list-group-item doFeatureHelper">' +
            correctGeoJSONName(feature) +
            ' @ ' + props.Pos[0] + ', ' + props.Pos[1] + ', ' + props.Pos[2] +
            '</a>';
    }
    s += '</div>';
    
    popover.setPosition(coordinate);
    $(element).popover({
        'placement': 'auto right',
        'viewport': {selector: '#map', padding: 0},
        'animation': false,
        'trigger': 'click focus',
        'html': true,
        'title': stitle,
        'content': s
    });
    $(element).popover('show');
    // setup click helper for the list of items
    $('.doFeatureHelper').click(function() {
        var id = +$(this).attr('data-id');
        doFeaturePopover(features, id, coordinate);
    });
}

var highlight;
var displayFeatureInfo = function(evt) {

    // we don't continue if the measure control is active
    if ( measureControl !== null ) {
        if ( measureControl.isEnabled() ) {
            return;
        }
    }

    var pixel = evt.pixel;
    var coordinate = evt.coordinate;

    // we get a list in case there are multiple items (e.g. stacked chests)
    var features = [];
    map.forEachFeatureAtPixel(pixel, function(feature, layer) {
        var addFlag = true;
        if ( feature.get('Clickable') !== undefined ) {
            if ( feature.get('Clickable') === '0' ) {
                addFlag = false;
            }
        }
        if ( addFlag) { 
            features.push(feature);
        }
    });

    if (features.length > 0) {

        if (features.length === 1) {
            doFeaturePopover(features, 0, coordinate);
        } else {
            // we need to show a feature select list
            doFeatureSelect(features, coordinate);
        }
    } else {
        var element = popover.getElement();
        $(element).popover('destroy');
    }
};

// http://stackoverflow.com/questions/14484787/wrap-text-in-javascript
function stringDivider(str, width, spaceReplacer) {
    if (str.length > width) {
        var p = width;
        for (; p > 0 && (str[p] != ' ' && str[p] != '-'); p--) {
        }
        if (p > 0) {
            var left;
            if (str.substring(p, p + 1) == '-') {
                left = str.substring(0, p + 1);
            } else {
                left = str.substring(0, p);
            }
            var right = str.substring(p + 1);
            return left + spaceReplacer + stringDivider(right, width, spaceReplacer);
        }
    }
    return str;
}

// from: http://openlayers.org/en/v3.9.0/examples/vector-labels.html
var getText = function(feature, resolution) {
    var type = 'normal';
    var maxResolution = 2;
    var text = correctGeoJSONName(feature);

    if (false) {
        if (resolution > maxResolution) {
            text = '';
        } else if (type == 'hide') {
            text = '';
        } else if (type == 'shorten') {
            text = text.trunc(12);
        } else if (type == 'wrap') {
            text = stringDivider(text, 16, '\n');
        }
    }
    
    return text;
};

var createTextStyle = function(feature, resolution, textColor) {
    var align = 'left';
    var baseline = 'bottom';
    var size = '14pt';
    var offsetX = 3;
    var offsetY = -2;
    var weight = 'bold';
    var rotation = 0;
    
    // smaller font when we are zoomed out
    if ( resolution > 3 ) {
        size = '8pt';
    } else if ( resolution > 2 ) {
        size = '10pt';
    } else {
        size = '12pt';
    }
    
    var font = weight + ' ' + size + ' Calibri,sans-serif';
    var fillColor = textColor;
    var outlineColor = '#000000';
    var outlineWidth = 3;

    var txt = getText(feature, resolution);
    
    return new ol.style.Text({
        textAlign: align,
        textBaseline: baseline,
        font: font,
        color: textColor,
        text: txt,
        fill: new ol.style.Fill({color: fillColor}),
        stroke: new ol.style.Stroke({color: outlineColor, width: outlineWidth}),
        offsetX: offsetX,
        offsetY: offsetY,
        rotation: rotation
    });
};


function setLayerLoadListeners(src, fn) {
    src.on('imageloadstart', function(event) {
        updateLoadEventCount(1);
    });
    src.on('imageloadend', function(event) {
        updateLoadEventCount(-1);
    });
    src.on('imageloaderror', function(event) {
        updateLoadEventCount(-1);
        doModal('Image Load Error',
                'Could not load file: ' + fn);
    });
}


// originally from: http://openlayers.org/en/v3.10.0/examples/shaded-relief.html
// but that code is actually *quite* insane
// rewritten based on:
//   http://edndoc.esri.com/arcobjects/9.2/net/shared/geoprocessing/spatial_analyst_tools/how_hillshade_works.htm

// todo what does this comment do? (from openlayers version)
// NOCOMPILE


/**
 * Generates a shaded relief image given elevation data.  Uses a 3x3
 * neighborhood for determining slope and aspect.
 * @param {Array.<ImageData>} inputs Array of input images.
 * @param {Object} data Data added in the 'beforeoperations' event.
 * @return {ImageData} Output image.
 */
function shade(inputs, data) {
    try {
        // testing
        //var tstart = Date.now();
        var elevationImage = inputs[0];
        var width = elevationImage.width;
        var height = elevationImage.height;
        var elevationData = elevationImage.data;
        var shadeData = new Uint8ClampedArray(elevationData.length);
        var maxX = width - 1;
        var maxY = height - 1;
        var twoPi = 2 * Math.PI;
        var halfPi = Math.PI / 2;

        // (2)  Zenith_deg = 90 - Altitude
        // (3)  Zenith_rad = Zenith_deg * pi / 180.0
        var zenithRad = (90 - data.sunEl) * Math.PI / 180;

        // (4)  Azimuth_math = 360.0 - Azimuth + 90
        var azimuthMath = 360 - data.sunAz + 90;
        // (5)  if Azimth_math >= 360.0 : Azimuth_math = Azimuth_math - 360.0
        if (azimuthMath >= 360.0) {
            azimuthMath = azimuthMath - 360.0;
        }
        // (6)  Azimuth_rad = Azimuth_math *  pi / 180.0
        var azimuthRad = azimuthMath * Math.PI / 180.0;

        var cosZenithRad = Math.cos(zenithRad);
        var sinZenithRad = Math.sin(zenithRad);

        // todo - since we need to multiply x2 to expand 0..127 to 0..255 we instead halve this (would be 8)
        var dp = data.resolution * 4.0;  // data.resolution * 8; // todo - not totally sure about the use of resolution here

        // notes: negative values simply reverse the sun azimuth; the range of interesting values is fairly narrow - somewhere on (0.001..0.8)
        var zFactor = (data.vert / 10.0) - 0.075;

        var x0, x1, x2, 
            y0, y1, y2, 
            offset,
            z0, z2, 
            dzdx, dzdy, 
            slopeRad, aspectRad, hillshade, fhillshade;

        /* 
           our 3x3 grid:
           a b c
           d e f
           g h i
           
           y0 is row above curr
           y1 is curr
           y2 is row below curr

           x0 is col to left of curr
           x1 is curr
           x2 is col to right of curr
        */

        for (y1 = 0; y1 <= maxY; ++y1) {
            y0 = (y1 === 0) ? 0 : (y1 - 1);
            y2 = (y1 === maxY) ? maxY : (y1 + 1);

            for (x1 = 0; x1 <= maxX; ++x1) {
                x0 = (x1 === 0) ? 0 : (x1 - 1);
                x2 = (x1 === maxX) ? maxX : (x1 + 1);

                // z0 = a + 2d + g
                z0 = 
                    elevationData[(y0 * width + x0) * 4] + 
                    elevationData[(y1 * width + x0) * 4] * 2.0 + 
                    elevationData[(y2 * width + x0) * 4];

                // z2 = c + 2f + i
                z2 = 
                    elevationData[(y0 * width + x2) * 4] + 
                    elevationData[(y1 * width + x2) * 4] * 2.0 + 
                    elevationData[(y2 * width + x2) * 4];
                
                // (7)  [dz/dx] = ((c + 2f + i) - (a + 2d + g)) / (8 * cellsize)
                dzdx = (z2 - z0) / dp;


                // z0 = a + 2b + c
                z0 = 
                    elevationData[(y0 * width + x0) * 4] + 
                    elevationData[(y0 * width + x1) * 4] * 2.0 + 
                    elevationData[(y0 * width + x2) * 4];

                // z2 = g + 2h + i
                z2 = 
                    elevationData[(y2 * width + x0) * 4] + 
                    elevationData[(y2 * width + x1) * 4] * 2.0 + 
                    elevationData[(y2 * width + x2) * 4];

                // (8)  [dz/dy] = ((g + 2h + i) - (a + 2b + c))  / (8 * cellsize)
                dzdy = (z2 - z0) / dp;

                // (9)  Slope_rad = ATAN (z_factor * sqrt ([dz/dx]2 + [dz/dy]2)) 
                slopeRad = Math.atan(zFactor * Math.sqrt(dzdx * dzdx + dzdy * dzdy));

                if (dzdx !== 0.0) { 
                    aspectRad = Math.atan2(dzdy, -dzdx);

                    if (aspectRad < 0) {
                        aspectRad += twoPi;
                    }
                }
                else {
                    if (dzdy > 0.0) {
                        aspectRad = halfPi;
                    } 
                    else if (dzdy < 0.0) {
                        aspectRad = twoPi - halfPi;
                    }
                    else {
                        // aspectRad is fine
                        aspectRad = 0.0; // todo - this is my guess; algo notes are ambiguous
                    }
                }
                
                // (1)  Hillshade = 255.0 * ((cos(Zenith_rad) * cos(Slope_rad)) + 
                //        (sin(Zenith_rad) * sin(Slope_rad) * cos(Azimuth_rad - Aspect_rad)))
                // Note that if the calculation of Hillshade value is < 0, the cell value will be = 0.

                // todo - worth doing a sin/cos LUT?
                fhillshade = 255.0 * ((cosZenithRad * Math.cos(slopeRad)) + (sinZenithRad * Math.sin(slopeRad) * Math.cos(azimuthRad - aspectRad)));

                if (fhillshade < 0.0) {
                    hillshade = 0;
                } else {
                    hillshade = Math.round(fhillshade);
                }

                offset = (y1 * width + x1) * 4;
                shadeData[offset] =
                    shadeData[offset + 1] =
                    shadeData[offset + 2] = hillshade;
                // note: reduce the opacity for brighter parts; idea is to reduce haziness
                shadeData[offset + 3] = 255 - (hillshade / 2);
            }
        }

        // testing:
        //      var tdiff = Date.now() - tstart;
        //      var npixel = width * height;
        //      var tpixel = npixel / tdiff;
        //      console.log('shade() t=' + tdiff + ' pixels=' + npixel + ' p/t=' + tpixel);

        return new ImageData(shadeData, width, height);
    } catch (e) {
        console.log('shade() exception: ' + e.toString());

        // we are probably failing because of CORS
        doModal('CORS Error',
                'Error accessing map pixels.  Disabling elevation overlay.<br/><br/>' +
                'Error: ' + e.toString() + '<br/><br/>' +
                globalCORSWarning);
        map.removeLayer(layerElevation);
    }
    // todobig - how to catch CORS issue here?
}

var srcLayerElevationAlpha = null, layerElevationAlpha = null;

function doElevationAlpha(enableFlag) {
    if (enableFlag) {
        var fn = dimensionInfo[globalDimensionId].fnLayerHeightAlpha;
        if ( useTilesFlag ) {
            srcLayerElevationAlpha = new ol.source.XYZ({
                url: fn,
                //crossOrigin: 'anonymous',
                projection: projection,
                extent: extent,
                //wrapX: false,
                tileSize: [ tileW, tileH ],
                tileGrid: srcLayerMain.getTileGrid()
            });
            layerElevationAlpha = new ol.layer.Tile({
                myStackOrder: 120,
                preload: Infinity,
                projection: projection,
                extent: extent,
                source: srcLayerElevationAlpha,
                opacity: $('#elevationAlphaOpacity').val() / 100.0
            });
        } else {
            srcLayerElevationAlpha = new ol.source.ImageStatic({
                url: fn,
                //crossOrigin: 'anonymous',
                projection: projection,
                imageSize: [dimensionInfo[globalDimensionId].worldWidth, dimensionInfo[globalDimensionId].worldHeight],
                imageExtent: extent
            });
            layerElevationAlpha = new ol.layer.Image({
                myStackOrder: 120,
                opacity: $('#elevationAlphaOpacity').val() / 100.0,
                source: srcLayerElevationAlpha
            });
        }
        setLayerLoadListeners(srcLayerElevationAlpha, fn);
        map_addLayer(layerElevationAlpha);
        disableLayerSmoothing(layerElevationAlpha);
    } else {
        if (layerElevationAlpha !== null ) {
            map.removeLayer(layerElevationAlpha);
            layerElevationAlpha = null;
            srcLayerElevationAlpha = null;
        }
    }
}

var srcLayerShadedReliefStatic = null, layerShadedReliefStatic = null;

function doElevationStatic(enableFlag) {
    if (enableFlag) {
        var fn = dimensionInfo[globalDimensionId].fnLayerShadedRelief;  
        if ( useTilesFlag ) {
            srcLayerShadedReliefStatic = new ol.source.XYZ({
                url: fn,
                //crossOrigin: 'anonymous',
                projection: projection,
                extent: extent,
                //wrapX: false,
                tileSize: [ tileW, tileH ],
                tileGrid: srcLayerMain.getTileGrid()
            });
            layerShadedReliefStatic = new ol.layer.Tile({
                myStackOrder: 100,
                preload: Infinity,
                projection: projection,
                extent: extent,
                source: srcLayerShadedReliefStatic,
                opacity: $('#elevationStaticOpacity').val() / 100.0
            });
        } else {
            srcLayerShadedReliefStatic = new ol.source.ImageStatic({
                url: fn,
                //crossOrigin: 'anonymous',
                projection: projection,
                imageSize: [dimensionInfo[globalDimensionId].worldWidth, dimensionInfo[globalDimensionId].worldHeight],
                imageExtent: extent
            });
            layerShadedReliefStatic = new ol.layer.Image({
                myStackOrder: 100,
                opacity: $('#elevationStaticOpacity').val() / 100.0,
                source: srcLayerShadedReliefStatic
            });
        }
        setLayerLoadListeners(srcLayerShadedReliefStatic, fn);
        map_addLayer(layerShadedReliefStatic);
        disableLayerSmoothing(layerShadedReliefStatic);
    } else {
        if (layerShadedReliefStatic !== null ) {
            map.removeLayer(layerShadedReliefStatic);
            layerShadedReliefStatic = null;
            srcLayerShadedReliefStatic = null;
        }
    }
}

var srcElevation = null, rasterElevation = null, layerElevation = null;

function doShadedRelief(enableFlag) {
    try {
        if (enableFlag) {
            var fn = dimensionInfo[globalDimensionId].fnLayerHeightGrayscale;
            if (fn === undefined || fn.length <= 1) {
                doModal('CORS Error',
                        'Data for elevation image is not available -- see README and re-run bedrock_viz<br/>' +
                        '<br/>' +
                        'Hint: You need to run bedrock_viz with --html-most (or --html-all)');
                return -1;
            }
            var doInitFlag = false;
            if (srcElevation === null) {
                doInitFlag = true;
                srcElevation = new ol.source.ImageStatic({
                    url: fn,
                    //crossOrigin: 'anonymous',
                    projection: projection,
                    imageSize: [dimensionInfo[globalDimensionId].worldWidth, dimensionInfo[globalDimensionId].worldHeight],
                    // 'Extent of the image in map coordinates. This is the [left, bottom, right, top] map coordinates of your image.'
                    imageExtent: extent
                });

                setLayerLoadListeners(srcElevation, fn);

                rasterElevation = new ol.source.Raster({
                    sources: [srcElevation],
                    operationType: 'image',
                    operation: shade
                });

                layerElevation = new ol.layer.Image({
                    myStackOrder: 110,
                    opacity: $('#shadeOpacity').val() / 100.0,
                    source: rasterElevation
                });
            }

            map_addLayer(layerElevation);

            if (doInitFlag) {
                var controlIds = ['vert', 'sunEl', 'sunAz', 'shadeOpacity'];
                var controls = {};
                controlIds.forEach(function(id) {
                    var control = document.getElementById(id);
                    var nameOutput = '#' + id + 'Out';
                    control.addEventListener('input', function() {
                        $(nameOutput).html('' + control.value);
                        rasterElevation.changed();
                    });
                    $(nameOutput).html('' + control.value);
                    controls[id] = control;
                });

                rasterElevation.on('beforeoperations', function(event) {
                    // the event.data object will be passed to operations
                    var data = event.data;
                    data.resolution = event.resolution;
                    for (var id in controls) {
                        data[id] = Number(controls[id].value);
                    }
                });
            }
        } else {
            if (layerElevation !== null) {
                map.removeLayer(layerElevation);
            }
        }
    } catch (e) {
        doModal('CORS Error',
                'Error accessing map pixels.<br/><br/>' +
                'Error: ' + e.toString() + '<br/><br/>' +
                globalCORSWarning);
    }
    // todobig - how to catch CORS issue here?
    return 0;
}


function makeChunkGrid(inputs, data) {
    var srcImage = inputs[0];
    var width = srcImage.width;
    var height = srcImage.height;
    var srcData = srcImage.data;
    var gridData = new Uint8ClampedArray(srcData.length);
    var dx = data.resolution;
    var dy = data.resolution;
    
    //console.log('makeChunkGrid w=' + width + ' h=' + height + ' dx='+dx+' dy='+dy);

    // todo - so fiddly.  it's still off a bit (not 100% locked to src pixels)
    
    var truncate = function(value) {
        return Math.floor(value);
    };
    
    var cy = data.extent[3];
    for (var pixelY = 0; pixelY < height; ++pixelY, cy -= dy) {
        var icy = truncate((data.worldHeight - cy) + data.globalOffsetY);
        var chunkY = (icy / 16) | 0;
        var cx = data.extent[0];
        for (var pixelX = 0; pixelX < width; ++pixelX, cx += dx) {
            var offset = (pixelY * width + pixelX) * 4;

            var icx = truncate(cx + data.globalOffsetX);
            var chunkX = (icx / 16) | 0;
            if (((icx % 16) === 0) || ((icy % 16) === 0)) {
                if (chunkX === 0 && chunkY === 0) {
                    gridData[offset] = 255;
                    gridData[offset + 1] = 0;
                    gridData[offset + 2] = 0;
                    gridData[offset + 3] = 128; 
                } else {
                    gridData[offset] = 255;
                    gridData[offset + 1] = 255;
                    gridData[offset + 2] = 255;
                    gridData[offset + 3] = 128;
                }
            } else {
                gridData[offset] = 
                    gridData[offset + 1] = 
                    gridData[offset + 2] = 
                    gridData[offset + 3] = 0;
            }
        }
    }
    return new ImageData(gridData, width, height);
}

var rasterChunkGrid = null, layerChunkGrid = null;

function doChunkGrid(enableFlag) {
    chunkGridFlag = enableFlag;
    if (enableFlag) {
        if (srcLayerMain === null) {
            doModal('Error', 'Weird.  Main layer source is null.  Cannot proceed.');
            return -1;
        }
        var doInitFlag = false;
        if (rasterChunkGrid === null) {
            doInitFlag = true;

            rasterChunkGrid = new ol.source.Raster({
                sources: [srcLayerMain],
                operationType: 'image',
                operation: makeChunkGrid
            });

            layerChunkGrid = new ol.layer.Image({
                myStackOrder: 200,
                opacity: 0.5,
                source: rasterChunkGrid
            });
        }

        map_addLayer(layerChunkGrid);

        if (doInitFlag) {
            rasterChunkGrid.on('beforeoperations', function(event) {
                // the event.data object will be passed to operations
                var data = event.data;
                data.resolution = event.resolution;
                data.extent = event.extent;
                data.worldWidth = dimensionInfo[globalDimensionId].worldWidth;
                data.worldHeight = dimensionInfo[globalDimensionId].worldHeight;
                data.globalOffsetX = dimensionInfo[globalDimensionId].globalOffsetX;
                data.globalOffsetY = dimensionInfo[globalDimensionId].globalOffsetY;
                //console.log('rasterChunkGrid resolution=' + event.resolution + ' extent=' + event.extent);
            });
        }
    } else {
        if (layerChunkGrid !== null) {
            map.removeLayer(layerChunkGrid);
        }
    }
    return 0;
}

function doSlimeChunks(enabled) {
    if ( enabled ) {
        if ( !globalWarnSlimeChunks ) {
            globalWarnSlimeChunks = true;
        }
        var fn = dimensionInfo[globalDimensionId].fnLayerSlimeChunks;
        if ( useTilesFlag ) {
            srcLayerSlimeChunks = new ol.source.XYZ({
                url: fn,
                //crossOrigin: 'anonymous',
                projection: projection,
                extent: extent,
                //wrapX: false,
                tileSize: [ tileW, tileH ],
                tileGrid: srcLayerMain.getTileGrid()
            });
            layerSlimeChunks = new ol.layer.Tile({
                myStackOrder: 210,
                preload: Infinity,
                projection: projection,
                extent: extent,
                source: srcLayerSlimeChunks,
                opacity: 0.65,
            });
        } else {
            srcLayerSlimeChunks = new ol.source.ImageStatic({
                url: fn,
                //crossOrigin: 'anonymous',
                projection: projection,
                imageSize: [dimensionInfo[globalDimensionId].worldWidth, dimensionInfo[globalDimensionId].worldHeight],
                imageExtent: extent
            });
            layerSlimeChunks = new ol.layer.Image({
                myStackOrder: 210,
                opacity: 0.65,
                source: srcLayerSlimeChunks
            });
        }
        setLayerLoadListeners(srcLayerSlimeChunks, fn);
        map_addLayer(layerSlimeChunks);
        disableLayerSmoothing(layerSlimeChunks);
    } else {
        if ( layerSlimeChunks ) {
            map.removeLayer(layerSlimeChunks);
            layerSlimeChunks = null;
            srcLayerSlimeChunks = null;
        }
    }
}

function setLayer(fn, extraHelp) {
    if (fn.length <= 1) {
        if ( extraHelp === undefined ) {
            extraHelp = '';
        } else {
            extraHelp = '<br/><br/>Hint: ' + extraHelp;
        }
        doModal('Error', 'That image is not available -- see README and re-run bedrock_viz.' + extraHelp);
        return -1;
    }
    
    // todo - attribution is small and weird in map - why?
    if ( useTilesFlag ) {
        srcLayerMain = new ol.source.XYZ({
            attributions: [
                new ol.Attribution({
                    html: 'Created by <a href="https://github.com/bedrock-viz/bedrock-viz" target="_blank">bedrock_viz</a>'
                })
            ],
            url: fn,
            //crossOrigin: 'anonymous',
            projection: projection,
            //wrapX: false,
            tileSize: [ tileW, tileH ],
            tileGrid: new ol.tilegrid.TileGrid({
                //extent: [ 0, 0, 39, 25 ],
                extent: extent,
                minZoom: 0,
                // todobig - should this be 0?
                maxZoom: 1,
                tileSize: [ tileW, tileH ],
                resolutions: [ 1 ]
            })
            //imageSize: [dimensionInfo[globalDimensionId].worldWidth, dimensionInfo[globalDimensionId].worldHeight],
            // 'Extent of the image in map coordinates. This is the [left, bottom, right, top] map coordinates of your image.'
            //imageExtent: extent
        });
    } else {
        srcLayerMain = new ol.source.ImageStatic({
            attributions: [
                new ol.Attribution({
                    html: 'Created by <a href="https://github.com/bedrock-viz/bedrock-viz" target="_blank">bedrock_viz</a>'
                })
            ],
            url: fn,
            //crossOrigin: 'anonymous',
            projection: projection,
            imageSize: [dimensionInfo[globalDimensionId].worldWidth, dimensionInfo[globalDimensionId].worldHeight],
            // 'Extent of the image in map coordinates. This is the [left, bottom, right, top] map coordinates of your image.'
            imageExtent: extent
        });
    }

    setLayerLoadListeners(srcLayerMain, fn);
    
    if (layerMain === null) {
        if ( useTilesFlag ) {
            layerMain = new ol.layer.Tile({
                myStackOrder: 0,
                preload: Infinity,
                projection: projection,
                extent: extent,
                source: srcLayerMain
            });
        } else { 
            layerMain = new ol.layer.Image({
                myStackOrder: 0,
                source: srcLayerMain
            });
        }
        map_addLayer(layerMain);

        if ( false ) {
            // for tile debugging...
            map.addLayer( new ol.layer.Tile({
                source: new ol.source.TileDebug({
                    projection: projection,
                    tileGrid: srcLayerMain.getTileGrid()
                })
            }) );
        }
        
        // get the pixel position with every move
        $(map.getViewport()).on('mousemove', function(evt) {
            globalMousePosition = map.getEventPixel(evt.originalEvent);
            // todo - is this too expensive? is there a better way?
            map.render();
        }).on('mouseout', function() {
            globalMousePosition = null;
            map.render();
        });

        layerMain.on('postcompose', function(event) {
            try {
                var ctx = event.context;
                var pixelRatio = event.frameState.pixelRatio;
                pixelDataName = '';
                if (globalMousePosition && 
                    ((globalLayerMode === 0 && (globalLayerId === 0 || globalLayerId === 1)) || globalLayerMode !== 0)) {
                    // todo - this appears to be slightly off at times (e.g. block does not change crisply at src pixel boundaries)
                    var x = globalMousePosition[0] * pixelRatio;
                    var y = globalMousePosition[1] * pixelRatio;
                    var pre = '';
                    pixelData = ctx.getImageData(x, y, 1, 1).data;
                    var cval = (pixelData[0] << 16) | (pixelData[1] << 8) | pixelData[2];
                    if (globalLayerMode === 0 && globalLayerId === 1) {
                        pre = 'Biome';
                        if ( biomeColorLUT['' + cval] !== undefined ) {
                            pixelDataName = biomeColorLUT['' + cval].name;
                        }
                    } else {
                        pre = 'Block';
                        if ( blockColorLUT['' + cval] !== undefined ) {
                            pixelDataName = blockColorLUT['' + cval].name;
                        }
                    }
                    if (pixelDataName === undefined || pixelDataName === '') {
                        if (pixelData[0] === 0 && pixelData[1] === 0 && pixelData[2] === 0) {
                            pixelDataName = '(<i>Here be Monsters</i> -- unexplored chunk)';
                        } else {
                            pixelDataName = '<span class="lgray">' + pre + '</span> ' + 'Unknown RGB: ' + pixelData[0] + ' ' + pixelData[1] + ' ' + pixelData[2] + ' (' + cval + ')';
                        }
                    } else {
                        pixelDataName = '<span class="lgray">' + pre + '</span> ' + pixelDataName;
                    }
                }
            } catch (e) {
                pixelDataName = '<i>Browser will not let us access map pixels - See README</i>';
                if ( ! globalCORSWarningFlag ) {
                    doModal('CORS Error',
                            'Error accessing map pixels.<br/><br/>' +
                            'Error: ' + e.toString() + '<br/><br/>' +
                            globalCORSWarning);
                    globalCORSWarningFlag = true;
                }
            }
        });
        
        disableLayerSmoothing(layerMain);
    } else {
        layerMain.setSource(srcLayerMain);
    }

    if ( vectorPoints ) {
        // update vector points (e.g. up/down/same markers if we are in raw layer mode and blocks are shown)
        vectorPoints.changed();
    }
    
    return 0;
}


function setLayerById(id) {
    var extraHelp = 'You need to run bedrock_viz with --html-most (or --html-all)';
    if (0) {
    } else if (id === 1) {
        if (setLayer(dimensionInfo[globalDimensionId].fnLayerBiome, extraHelp) === 0) {
            globalLayerMode = 0; globalLayerId = 1;
            $('#imageSelectName').html('Biome');
        }
    } else if (id === 2) {
        if (setLayer(dimensionInfo[globalDimensionId].fnLayerHeight, extraHelp) === 0) {
            globalLayerMode = 0; globalLayerId = 2;
            $('#imageSelectName').html('Height');
        }
    } else if (id === 3) {
        if (setLayer(dimensionInfo[globalDimensionId].fnLayerHeightGrayscale, extraHelp) === 0) {
            globalLayerMode = 0; globalLayerId = 3;
            $('#imageSelectName').html('Height (Grayscale)');
        }
    } else {
        // default is overview map
        if (setLayer(dimensionInfo[globalDimensionId].fnLayerTop, '') === 0) {
            globalLayerMode = 0; globalLayerId = 0;
            $('#imageSelectName').html('Overview');
        }
    }
}


function initDimension() {
    // Map views always need a projection.  Here we just want to map image
    // coordinates directly to map coordinates, so we create a projection that uses
    // the image extent in pixels.
    dimensionInfo[globalDimensionId].worldWidth =
        dimensionInfo[globalDimensionId].maxWorldX - dimensionInfo[globalDimensionId].minWorldX + 1;
    dimensionInfo[globalDimensionId].worldHeight =
        dimensionInfo[globalDimensionId].maxWorldY - dimensionInfo[globalDimensionId].minWorldY + 1;

    extent = [0, 0,
              dimensionInfo[globalDimensionId].worldWidth,
              dimensionInfo[globalDimensionId].worldHeight];

    dimensionInfo[globalDimensionId].globalOffsetX = dimensionInfo[globalDimensionId].minWorldX;
    dimensionInfo[globalDimensionId].globalOffsetY = dimensionInfo[globalDimensionId].minWorldY;

    
    // todobig - adjust maxworldX & Y if in tile mode so that we include the right edge and bottom edge tiles?
    if ( false ) {
        extent = [ dimensionInfo[globalLayerId].minWorldX,
                   dimensionInfo[globalLayerId].minWorldY,
                   dimensionInfo[globalLayerId].maxWorldX,
                   dimensionInfo[globalLayerId].maxWorldY ];
        
        dimensionInfo[globalDimensionId].globalOffsetX = 0;
        dimensionInfo[globalDimensionId].globalOffsetY = 0;
    }

    
    console.log('World bounds: dimId=' + globalDimensionId +
                ' w=' + dimensionInfo[globalDimensionId].worldWidth +
                ' h=' + dimensionInfo[globalDimensionId].worldHeight +
                ' offx=' + dimensionInfo[globalDimensionId].globalOffsetX + 
                ' offy=' + dimensionInfo[globalDimensionId].globalOffsetY
               );
    
    projection = new ol.proj.Projection({
        code: 'bedrock_viz-image',
        // todobig - this appears to break loading geojson
        // code: 'EPSG:3857',

        // todobig - 'm' or 'pixels'?
        //units: 'm',
        units: 'pixels',

        extent: extent,
        getPointResolution: function(resolution, coordinate) {
            return resolution;
        }
    });

    if (mousePositionControl === null) {
        mousePositionControl = new ol.control.MousePosition({
            coordinateFormat: coordinateFormatFunction,
            projection: projection,
            // comment the following two lines to have the mouse position be placed within the map.
            // className: 'custom-mouse-position',
            //target: document.getElementById('mouse-position'),
            undefinedHTML: '&nbsp;'
        });
    } else {
        mousePositionControl.setProjection(projection);
    }

    /*
      var attribution = new ol.control.Attribution({
      collapsed: false,
      collapsible: false
      //target: '_blank'
      });               
    */
    
    if (map === null) {
        measureControl = new MeasureControl();
        map = new ol.Map({
            controls: ol.control.defaults({
                attribution: true,
                attributionOptions: { collapsed: false, collapsible: false, target: '_blank' }
            })
                .extend([
                    new ol.control.ZoomToExtent(),
                    new ol.control.ScaleLine(),
                    new ol.control.FullScreen(),
                    measureControl,
                    mousePositionControl
                ]),
            // pixelRatio: 1, // todo - need this?
            target: 'map',
            view: new ol.View({
                projection: projection,
                center: [
                    dimensionInfo[globalDimensionId].playerPosX + Math.floor(dimensionInfo[globalDimensionId].worldWidth / 2)
                   ,dimensionInfo[globalDimensionId].playerPosY + Math.floor(dimensionInfo[globalDimensionId].worldHeight / 2)
                ],
                resolution: 1
            })
        });
    } else {
        var view = new ol.View({
            projection: projection,
            center: [
                dimensionInfo[globalDimensionId].playerPosX + Math.floor(dimensionInfo[globalDimensionId].worldWidth / 2)
               ,dimensionInfo[globalDimensionId].playerPosY + Math.floor(dimensionInfo[globalDimensionId].worldHeight / 2)
            ],
        resolution: 1
        });
        map.setView(view);
    }

    // setup per-dimension block select menu

    // todobig - clear selected items?
    
    // clear existing items
    $('#blockSelectList .blockToggle').remove();
    var newItems = '';
    for (var b in  dimensionInfo[globalDimensionId].geojsonBlocks) {
        var block = dimensionInfo[globalDimensionId].geojsonBlocks[b];
        newItems += '<li><a href="#" class="blockToggle" data-id="' + block + '">' + block + '</a></li>';
    }
    if ( dimensionInfo[globalDimensionId].spawnableFlag) {
        newItems += '<li><a href="#" class="spawnableToggle" data-id="spawnable">Spawnable Blocks</a></li>';
    }
    if ( newItems.length > 0 ) {
        $(newItems).prependTo('#blockSelectList');
    }
    // todobig - could disable button if the menu is empty
    
    $('.blockToggleAddAll').click(function() {
        var cflag = true;
        if ( vectorPoints === null ) {
            cflag = false;
            loadVectors();
        }
        $('.blockToggle').each(function(index) {
            listBlockToggle[$(this).attr('data-id')] = true;
            $(this).parent().addClass('active');
        });
        $('.spawnableToggle').each(function(index) {
            spawnableEnableFlag = true;
            $(this).parent().addClass('active');
        });
        if ( cflag ) {
            vectorPoints.changed();
        }
    });
    $('.blockToggleRemoveAll').click(function() {
        listBlockToggle = [];
        spawnableEnableFlag = false;
        if (vectorPoints !== null) { 
            vectorPoints.changed();
        }
        $('.blockToggle').parent().removeClass('active');
        $('.spawnableToggle').parent().removeClass('active');
    });
    $('.blockToggle').click(function() {
        var id = $(this).attr('data-id');
        blockToggle(id);
        if (listBlockToggle[id]) {
            $('.blockToggle[data-id="' + id + '"]').parent().addClass('active');
        } else {
            $('.blockToggle[data-id="' + id + '"]').parent().removeClass('active');
        }
    });
    $('.spawnableToggle').click(function() {
        var id = $(this).attr('data-id');
        spawnableEnableFlag = ! spawnableEnableFlag;
        spawnableToggle();
        if ( spawnableEnableFlag ) {
            $('.spawnableToggle[data-id="' + id + '"]').parent().addClass('active');
        } else {
            $('.spawnableToggle[data-id="' + id + '"]').parent().removeClass('active');
        }
    });

    
    // finally load the proper layer
    if (globalLayerMode === 0) {
        return setLayerById(globalLayerId);
    } else {
        return layerGoto(layerRawIndex);
    }
}

function setDimensionById(id) {
    var prevDimId = globalDimensionId;
    if (0) {
    }
    else if (id === 1) {
        globalDimensionId = id;
        $('#dimensionSelectName').html('Nether');
        adjustLayerDropDown(0, 128);
        $('.layerGoto[data-id="62"]').hide();
    }
    else if (id === 2) {
        globalDimensionId = id;
        $('#dimensionSelectName').html('The End');
        adjustLayerDropDown(0, 128);
        $('.layerGoto[data-id="62"]').hide();
    }
    else {
        // default to overworld
        globalDimensionId = id;
        $('#dimensionSelectName').html('Overworld');
        adjustLayerDropDown(-64, 319);
    }

    if (prevDimId !== globalDimensionId) {
        initDimension();
    }
}

function adjustLayerDropDown(min, max) {
    var lcv = -64
    for(; lcv <= 319; lcv++) {
        if (lcv < min || lcv > max) {
            $('.layerGoto[data-id="' + lcv + '"]').hide();
        } else {
            $('.layerGoto[data-id="' + lcv + '"]').show();
        }
    }
}

function checkPlayerDistance(feature) {
    if ( ! doCheckPlayerDistanceFlag ) { return true; }

    var playerpos = openlayersToMcpe(dimensionInfo[globalDimensionId].playerPosX,
                                     dimensionInfo[globalDimensionId].playerPosY,
                                     false);
    var featurepos = feature.get('Pos');
    var dx = playerpos[0] - featurepos[0];
    var dy = playerpos[1] - featurepos[2];
    var dist = Math.sqrt( (dx * dx) + (dy * dy) );
    if ( dist <= 128.0 ) {
        return true;
    }
    return false;
}


// color feature labels according to entity type (etype)
function etypeToTextColor(feature) {
    var etype = feature.get('etype');
    if ( etype === 'H' ) {
        return '#ffc0c0';
    }
    if ( etype === 'P' ) {
        return '#a0ffa0';
    }
    return '#ffffff';
}


var createPointStyleFunction = function() {
    return function(feature, resolution) {
        var style;
        var entity = feature.get('Entity');
        var tileEntity = feature.get('TileEntity');
        var block = feature.get('Block');
        // todobig - instead of a prop for 'spawnable' just check the name prop? (save big on geojson filesize)
        var spawnable = feature.get('Spawnable');
        var dimId = feature.get('Dimension');
        var textColor = '#ffffff';
        
        // hack for pre-0.12 worlds
        if (dimId === undefined) {
            dimId = 0;
        } else {
            dimId = +dimId;
        }
        
        if (entity !== undefined) {
            if (dimId === globalDimensionId) {
                var id = +feature.get('id');
                if (listEntityToggle[id] !== undefined) {
                    if (listEntityToggle[id] && checkPlayerDistance(feature)) {
                        textColor = etypeToTextColor(feature);
                        style = new ol.style.Style({
                            image: new ol.style.Circle({
                                radius: 4,
                                fill: new ol.style.Fill({color: 'rgba(255, 255, 255, 1.0)'}),
                                stroke: new ol.style.Stroke({color: 'rgba(0, 0, 0, 1.0)', width: 2})
                            }),
                            text: createTextStyle(feature, resolution, textColor)
                        });
                        return [style];
                    }
                }
            }
        }
        else if (tileEntity !== undefined) {
            if (dimId === globalDimensionId) {
                var Name = feature.get('Name');
                if (listTileEntityToggle[Name] !== undefined) {
                    if (listTileEntityToggle[Name]) { 
                        style = new ol.style.Style({
                            image: new ol.style.Circle({
                                radius: 4,
                                fill: new ol.style.Fill({color: 'rgba(255, 255, 255, 1.0)'}),
                                stroke: new ol.style.Stroke({color: 'rgba(0, 0, 0, 1.0)', width: 2})
                            }),
                            text: createTextStyle(feature, resolution, textColor)
                        });
                        return [style];
                    }
                }
            }
        }
        else if (block !== undefined) {
            if (dimId === globalDimensionId) {
                var Name = feature.get('Name');
                if (listBlockToggle[Name] !== undefined) {
                    if (listBlockToggle[Name]) {
                        // we return a single style to reduce memory consumption
                        if ( globalLayerMode === 1 ) {
                            // we are in raw layer mode, let's see where this block is
                            var Pos = feature.get('Pos');
                            if ( Pos[1] > layerRawIndex ) {
                                return [globalStyleBlockUp];
                            }
                            if ( Pos[1] < layerRawIndex ) {
                                return [globalStyleBlockDown];
                            }
                            return [globalStyleBlockSame];
                        } else {
                            return [globalStyleBlock];
                        }
                    }
                }
            }
        }
        else if (spawnable !== undefined) {
            if (dimId === globalDimensionId) {
                if ( spawnableEnableFlag ) {
                    if ( feature.get('BoundingCircle') ) {
                        var bcradius = feature.get('Radius');
                        return [ new ol.style.Style({
                            image:  new ol.style.Circle({
                                radius: bcradius / resolution,
                                fill: new ol.style.Fill({color: 'rgba(255, 128, 255, 0.15)'}),
                                stroke: new ol.style.Stroke({color: 'rgba(0, 0, 0, 0.15)', width: 2})
                            })
                        }) ];
                    } else {
                        // we return a single style to reduce memory consumption
                        if ( globalLayerMode === 1 ) {
                            // we are in raw layer mode, let's see where this block is
                            var Pos = feature.get('Pos');
                            if ( Pos[1] > layerRawIndex ) {
                                return [globalStyleBlockUp];
                            }
                            if ( Pos[1] < layerRawIndex ) {
                                return [globalStyleBlockDown];
                            }
                            return [globalStyleBlockSame];
                        } else {
                            return [globalStyleBlock];
                        }
                    }
                }
            }
        }
        else {
            console.log('Weird.  Found a GeoJSON item that is not an entity, tileEntitiy or block');
        }
        return null;
    };
};


var villageCreatePointStyleFunction = function() {
    return function(feature, resolution) {

        if ( feature.get('name') === 'Village Radius' ) {
            var bcradius = feature.get('radius');
            return [ new ol.style.Style({
                image:  new ol.style.Circle({
                    radius: bcradius / resolution,
                    fill: new ol.style.Fill({color: 'rgba(255, 255, 255, 0.1)'}),
                    stroke: new ol.style.Stroke({color: 'rgba(0, 0, 0, 0.1)', width: 2})
                })
            }) ];
        }
        
        var weight = 'bold';
        var size;
        // smaller font when we are zoomed out
        if ( resolution > 3 ) {
            size = '6pt';
        } else if ( resolution > 2 ) {
            size = '8pt';
        } else {
            size = '10pt';
        }
        var font = weight + ' ' + size + ' Calibri,sans-serif';
        var style = new ol.style.Style({
            image: new ol.style.RegularShape({
                fill: new ol.style.Fill({color: '#ad7d28'}),
                stroke: new ol.style.Stroke({color: '#533b12', width: 2}),
                points: 4,
                radius: 6,
                angle: Math.PI / 4
            }),
            text: new ol.style.Text({
                textAlign: 'left',
                textBaseline: 'bottom',
                font: font,
                color: '#ffffff',
                text: 'Door',
                fill: new ol.style.Fill({color: '#efe81f'}),
                stroke: new ol.style.Stroke({color: '#000000', width: 3}),
                offsetX: 3,
                offsetY: -2,
                rotation: 0
            })
        });
        return [style];
    }
};




var vectorPoints = null;
var villageVectorPoints = null;
var srcVillageVectorPoints = null;
var featuresVillageVectorPoints = new ol.Collection();

function loadVectors() {
    if (vectorPoints !== null) {
        map.removeLayer(vectorPoints);
    }

    try {
        var src;
        if ( loadGeoJSONFlag ) { 
            src = new ol.source.Vector({
                url: fnGeoJSON,
                //crossOrigin: 'anonymous',
                format: new ol.format.GeoJSON()
            });
            updateLoadEventCount(1);
        } else {
            // we are loading the geojson directly to work-around silly chrome (et al) CORS issue
            // adapted from ol/featureloader.js
            var format = new ol.format.GeoJSON();
            var features = format.readFeatures(geojson, {featureProjection: projection});
            src = new ol.source.Vector({
                features: features
            });
        }
        
        var listenerKey = src.on('change', function(e) {
            if (src.getState() == 'ready') {
                updateLoadEventCount(-1);
                ol.Observable.unByKey(listenerKey);
            }
            else if (src.getState() == 'error') {
                updateLoadEventCount(-1);
                ol.Observable.unByKey(listenerKey);
                doModal('Image Load Error',
                        'Could not load file: ' + src.url + '<br/>' +
                        globalCORSWarning);
            }
        });
        
        vectorPoints = new ol.layer.Vector({
            myStackOrder: 300,
            source: src,
            style: createPointStyleFunction()
        });
        
        map_addLayer(vectorPoints);

    } catch (e) {
        updateLoadEventCount(-1);
        doModal('Vector Load Error',
                'Error: ' + e.toString() + '<br/>' +
                '<br/>' +
                globalCORSWarning);
    } 
    // todobig - how to catch CORS issue here?

    // add village doors vector layer
    srcVillageVectorPoints = new ol.source.Vector({
        features: featuresVillageVectorPoints
    });
    villageVectorPoints = new ol.layer.Vector({
        myStackOrder: 310,
        source: srcVillageVectorPoints,
        //todobig - could use a func to set door labels w/ more info
        style: villageCreatePointStyleFunction()
    });
    
    map_addLayer(villageVectorPoints);
}



function entityToggle(id) {
    id = +id;
    var cflag = true;
    if (vectorPoints === null) {
        cflag = false;
        loadVectors();
    }
    if (listEntityToggle[id] === undefined) {
        listEntityToggle[id] = true;
    } else {
        listEntityToggle[id] = !listEntityToggle[id];
    }
    if ( cflag ) {
        vectorPoints.changed();
    }
}

function tileEntityToggle(name) {
    var cflag = true;
    if (vectorPoints === null) {
        cflag = false;
        loadVectors();
    }
    if (listTileEntityToggle[name] === undefined) {
        listTileEntityToggle[name] = true;
    } else {
        listTileEntityToggle[name] = !listTileEntityToggle[name];
    }
    if ( cflag ) {
        vectorPoints.changed();
    }
}

function blockToggle(name) {
    var cflag = true;
    if (vectorPoints === null) {
        cflag = false;
        loadVectors();
    }
    if (listBlockToggle[name] === undefined) {
        listBlockToggle[name] = true;
    } else {
        listBlockToggle[name] = !listBlockToggle[name];
    }
    if ( cflag ) {
        vectorPoints.changed();
    }
}

function spawnableToggle() {
    var cflag = true;
    if (vectorPoints === null) {
        cflag = false;
        loadVectors();
    }
    if ( cflag ) {
        vectorPoints.changed();
    }
}

function layerMove(delta) {
    //this_.getMap().getView().setRotation(0);
    layerRawIndex += delta;
    var min = dimensionInfo[globalDimensionId].bottomLayer;
    var max = dimensionInfo[globalDimensionId].topLayer;

    // bound the layer within the dimensions limits
    if (layerRawIndex < min) { layerRawIndex = min; }
    if (layerRawIndex > max) { layerRawIndex = max; }
    layerGoto(layerRawIndex);
}

function layerGoto(layer) {
    // we make sure that layer is an integer (mob positions can have decimal points)
    layer = Math.floor(layer);
    var min = dimensionInfo[globalDimensionId].bottomLayer;
    var max = dimensionInfo[globalDimensionId].topLayer;

    // bound the layer within the dimensions limits
    if (layer < min) { layer = min; }
    if (layer > max) { layer = max; }
    // offset into the list by the min, to map -64 to 0 for overworld
    if (setLayer(dimensionInfo[globalDimensionId].listLayers[layer - min], 'You need to run bedrock_viz with --html-all') === 0) {
        globalLayerMode = 1;
        layerRawIndex = layer;
        $('#layerNumber').html('' + layer);
    }
}


// todo - this is still not quite right
var coordinateFormatFunction = function(coordinate) {
    var cx = coordinate[0] + dimensionInfo[globalDimensionId].globalOffsetX;
    var cy = ((dimensionInfo[globalDimensionId].worldHeight - 1) - coordinate[1]) + dimensionInfo[globalDimensionId].globalOffsetY;

    // todobig - new coordinate handling
    if ( false ) {
        cx = coordinate[0];
        cy = -coordinate[1];
    }
    
    var ix = coordinate[0];
    var iy = (dimensionInfo[globalDimensionId].worldHeight - 1) - coordinate[1];

    // todobig - how to adjust image coord?
    if ( false ) {
        ix = cx - dimensionInfo[globalDimensionId].minWorldX;
        iy = cy - dimensionInfo[globalDimensionId].minWorldY;
    }
    
    var chunkX = Math.floor(cx / 16);
    var chunkY = Math.floor(cy / 16);

    var prec = 1;
    var s = '<span class="lgray">World</span> ' + cx.toFixed(prec) + ' ' + cy.toFixed(prec) + 
        ' <span class="lgray">Image</span> ' + ix.toFixed(prec) + ' ' + iy.toFixed(prec);
    if ( showChunkCoordinatesFlag || chunkGridFlag ) {
        s += '<br/><span class="lgray">Chunk</span> ' + chunkX.toFixed(0) + ' ' + chunkY.toFixed(0);
    }
    if ( showNetherCoordinatesFlag ) {
        var nx = 0, ny = 0;
        var ns = '';
        if ( globalDimensionId === 1 ) {
            // convert from nether to overworld
            nx = cx * 8.0;
            ny = cy * 8.0;
            ns = 'Overworld';
        } else {
            // convert from overworld to nether
            nx = cx / 8.0;
            ny = cy / 8.0;
            ns = 'Nether';
        }
        s += '<br/><span class="lgray">' + ns + '</span> ' + nx.toFixed(prec) + ' ' + ny.toFixed(prec);
    }
    if (pixelDataName.length > 0) {
        s += '<br/>' + pixelDataName;
    }
    return s;
};

// adapted from: http://stackoverflow.com/questions/12887506/cannot-set-maps-div-height-for-openlayers-map
var fixContentHeight = function() {
    var viewHeight = $(window).height();
    var navbar = $('div[data-role="navbar"]:visible:visible');
    // todo - this is not quite right, off by approx 7 pixels - why?
    var newMapH = viewHeight - navbar.outerHeight();
    var curMapSize = map.getSize();
    curMapSize[1] = newMapH;
    map.setSize(curMapSize);
};

function doTour(aboutFlag) {
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
                element: '.ol-zoom',
                title: 'Zoom Controls',
                content: 'You can also zoom in and out with these buttons.'
            },
            {
                element: '.ol-zoom-extent',
                title: 'Zoom to Full Extent',
                content: 'Zoom out so that the entire world is visible.'
            },
            {
                element: '.measure',
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
                element: '.ol-mouse-position',
                placement: 'left',
                title: 'Mouse Position',
                content: 'Position of the mouse is shown here as well as information on the block under the mouse cursor.'
            },
            {
                element: '#dimensionSelectName',
                placement: 'top',
                title: 'Select Dimension',
                content: 'You can switch between dimensions (e.g. Overworld, The Nether).'
            },
            {
                element: '#layerNumber',
                placement: 'top',
                title: 'Select Raw Layer',
                content: 'You can view individual layers of your world, if you ran Bedrock Viz in "html-all" mode.'
            },
            {
                element: '#imageSelectName',
                placement: 'top',
                title: 'Select Image',
                content: 'You can view different images of your world.<br/>' +
                    '<br/><ul>' +
                    '<li><b>Overview</b> is an image of the highest blocks in your world.</li>' +
                    '<li><b>Biome</b> is an image of the biomes in your world.</li>' +
                    '<li><b>Height</b> is an image of the heights of the highest blocks in your world.  Red is below sea level, Green is above.</li>' +
                    '<li><b>Height (Grayscale)</b> is an image of the heights of the highest blocks in your world in grayscale.</li>' +
                    '</ul>'
            },
            {
                element: '#menuPassiveMobs',
                placement: 'top',
                title: 'Passive Mobs',
                content: 'You can display the location of passive mobs here.' +
                    featureExtra
            },
            {
                element: '#menuHostileMobs',
                placement: 'top',
                title: 'Hostile Mobs',
                content: 'You can display the location of hostile mobs here.' + 
                    featureExtra
            },
            {
                element: '#menuObjects',
                placement: 'top',
                title: 'Objects',
                content: 'You can display the location of objects here.' + 
                    featureExtra
            },
            {
                element: '#menuBlocks',
                placement: 'top',
                title: 'Blocks',
                content: 'You can display the location of blocks here.<br/><br/>' +
                    'Please note that to improve performance labels are not shown.  Click on a dot to get information about the block.<br/><br/>' +
                    'You can change which blocks are available by using the command-line switch <i>--geojson-block</i>, or you can add <i>geojson-block</i> items to your config file.  See <i>bedrock_viz.cfg</i> for more details.'
            },
            {
                element: '#menuOptions',
                placement: 'top',
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
                element: '#worldName',
                placement: 'top',
                title: 'World Name',
                content: 'The name of your world, and the date/time that you ran Bedrock Viz on it.'
            },
            {
                title: 'About Bedrock Viz',
                content: '' +
                    '<a href="https://github.com/bedrock-viz/bedrock-viz" target="_blank">Bedrock Viz is developed on GitHub</a><br/>' +
                    'Version ' + creationBedrockVizVersion + '<br/>' +
                    'Please be sure to check back often for updates!</a><br/>' +
                    '<br/>' +
                    '<b>Bedrock Viz Viewer</b> is built using these other fine projects and libraries:<br/><ul>' +
                    '<li><a href="http://openlayers.org/" target="_blank">OpenLayers 3</a></li>' +
                    '<li><a href="http://getbootstrap.com/" target="_blank">Bootstrap</a></li>' +
                    '<li><a href="http://bootstraptour.com/" target="_blank">Bootstrap Tour</a></li>' +
                    '<li><a href="http://jquery.com/" target="_blank">jQuery</a></li>' +
                    '<li>Fork of <a href="https://github.com/Plethora777/mcpe_viz" target="_blank">MCPE Viz by Plethora777</a></li>' +
                    '</ul>' +
                    'Block and Item images are borrowed from the <a href="http://minecraft.gamepedia.com/" target="_blank">Minecraft Wiki</a>.  The textures themselves are copyright Mojang.'
            }
        ]});
    tour.init();
    if ( aboutFlag ) {
        tour._options.template = '<div class="popover tour">' + 
            '<div class="arrow"></div>' + 
            '<h3 class="popover-title"></h3>' +
            '<div class="popover-content"></div>' +
            '<div class="popover-navigation">' +
            '<button class="btn btn-default" data-role="end">Close</button>' +
            '</div></div>';
        tour.goTo( tour._options.steps.length - 1);
    }
    tour.start();
}

function doModal(title, body) {
    $('.modal-title').html(title);
    $('.modal-body').html(body);
    $('.modal').modal({});
}

function showUpdateInfo(newVersion, newVersionHighlight, releaseData) {
    // todobig - make this a bootstrap dialog box that has a clickable link
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
        msg = 'You are running the latest released version: <b>v' + creationBedrockVizVersion + '</b><br/><br/>';
    }

    msg += '<div class="panel panel-default">' +
           '<div class="panel-heading" role="tab" id="headingOne">' +
           '<h4 class="panel-title">' +
           '<a role="button" data-toggle="collapse" href="#collapseOne" aria-expanded="true" aria-controls="collapseOne">' +
           'View Changelog' +
           '</a></h4></div>' +
           '<div id="collapseOne" class="panel-collapse collapse" role="tabpanel" aria-labelledby="headingOne">' +
           '<div class="panel-body"><div class="pre-scrollable"><pre>' + releaseData['body'] + '</pre></div></div>' +
           '</div></div>' + 
           (!isCurrent ? '<a target="_blank" href="' + releaseData['html_url'] + '">Click here to go to GitHub and grab the update</a>' : '');

    doModal( title, msg);
}

function doCheckUpdate_getChangeLog(newVersion, releaseData) {
    var newVersionHighlight = releaseData['name'];

    // try to take the version off the front since it's redundant when displayed
    var re = new RegExp('v' + newVersion + '\\s*-\\s*(.+)');
    var res = newVersionHighlight.match(re);
    if ( res ) {
        newVersionHighlight = res[1];
    }

    showUpdateInfo(newVersion, newVersionHighlight, releaseData);
}

function doCheckUpdate() {
    // get data from the latest published github release
    var url = 'https://api.github.com/repos/bedrock-viz/bedrock-viz/releases/latest';

    $.ajax({
        type: 'GET',
        url: url,
        dataType: 'text',
        cache: false,
        success: function(result, textStatus, jqxhr) {
            var releaseData = JSON.parse(result);
            if ( releaseData ) {
                var release = releaseData['tag_name'];
                // try to take the v off the front to match our built in version
                var res = releaseData['tag_name'].match(/v(\S+)/);
                if (res) {
                    release = res[1];
                }
                doCheckUpdate_getChangeLog(release, releaseData);
            } else {
                doModal('Error', 'Sorry, failed to find version info on GitHub.');
            }

        },
        error: function(jqXHR, textStatus, errorThrown, execptionObject) {
            doModal('Error',
                    'Sorry, failed to check for update: Status [' + textStatus + '] error [' + errorThrown + ']');
        }
    });
}

function doAbout() {
    doTour(true);
}

$(function() {

    // add the main layer
    setDimensionById(0);

    // create default style for blocks
    // we do this to reduce memory consumption for blocks
    if ( true ) {
        var fill = new ol.style.Fill({color: 'rgba(255, 128, 255, 0.8)'});
        var stroke = new ol.style.Stroke({color: 'rgba(0, 0, 0, 0.8)', width: 1});

        globalStyleBlock = new ol.style.Style({
            image: new ol.style.Circle({
                radius: 4,
                fill: fill,
                stroke: stroke
            })
        });

        var radius = 8;
        
        globalStyleBlockUp = new ol.style.Style({
            image:  new ol.style.RegularShape({
                fill: new ol.style.Fill({color: 'rgba(128, 255, 128, 0.8)'}),
                stroke: stroke,
                points: 3,
                radius: radius,
                rotation: 0,
                angle: 0
            })
        });

        globalStyleBlockDown = new ol.style.Style({
            image:  new ol.style.RegularShape({
                fill: new ol.style.Fill({color: 'rgba(255, 128, 128, 0.8)'}),
                stroke: stroke,
                points: 3,
                radius: radius,
                rotation: Math.PI,
                angle: 0
            })
        });

        globalStyleBlockSame = new ol.style.Style({
            image:  new ol.style.RegularShape({
                fill: new ol.style.Fill({color: 'rgba(255, 255, 255, 0.8)'}),
                stroke: stroke,
                points: 4,
                radius: radius,
                angle: Math.PI / 4
            })
        });
    }
    
    popover = new ol.Overlay({
        element: document.getElementById('popover'),
        autoPan: true,
        autoPanAnimation: {
            duration: 100
        }
    });
    // todo - do we need to do this when we create/update popover? hmmm
    map.addOverlay(popover);

    // todo - refine overview map cfg and add this back?
    if (false) {
        var omap = new ol.control.OverviewMap({
            layers: [layerMain]
        });
        map.addControl(omap);
    }

    map.on('singleclick', function(evt) {
        displayFeatureInfo(evt);
    });

    $('.dimensionSelect').click(function() {
        var id = +$(this).attr('data-id');
        setDimensionById(id);
    });
    
    
    $('.layerGoto').click(function() {
        var id = +$(this).attr('data-id');
        layerGoto(id);
    });
    
    $('#layerPrev').click(function() { layerMove(-1); });
    $('#layerNext').click(function() { layerMove(1); });
    
    $('.imageSelect').click(function() {
        var id = +$(this).attr('data-id');
        setLayerById(id);
    });


    $('.featureToggleAddAll').click(function() {
        var dtype = $(this).attr('data-type');
        var cflag = true;
        if ( vectorPoints === null ) {
            cflag = false;
            loadVectors();
        }
        $('.entityToggle').each(function(index) {
            if ($(this).attr('data-type') === dtype) {
                var id = +$(this).attr('data-id');
                listEntityToggle[id] = true;
                $(this).parent().addClass('active');
            }
        });
        $('.tileEntityToggle').each(function(index) {
            if ($(this).attr('data-type') === dtype) {
                var id = $(this).attr('data-id');
                listTileEntityToggle[id] = true;
                $(this).parent().addClass('active');
            }
        });
        if ( cflag ) {
            vectorPoints.changed();
        }
    });

    $('.featureToggleRemoveAll').click(function() {
        var dtype = $(this).attr('data-type');
        $('.entityToggle').each(function(index) {
            if ($(this).attr('data-type') === dtype) {
                var id = +$(this).attr('data-id');
                listEntityToggle[id] = false;
                $(this).parent().removeClass('active');
            }
        });
        $('.tileEntityToggle').each(function(index) {
            if ($(this).attr('data-type') === dtype) {
                var id = $(this).attr('data-id');
                listTileEntityToggle[id] = false;
                $(this).parent().removeClass('active');
            }
        });
        if (vectorPoints !== null) { 
            vectorPoints.changed();
        }
    });
    
    $('.entityToggle').click(function() {
        var id = $(this).attr('data-id');
        entityToggle(id);
        if (listEntityToggle[id]) {
            $('.entityToggle[data-id="' + id + '"]').parent().addClass('active');
        } else {
            $('.entityToggle[data-id="' + id + '"]').parent().removeClass('active');
        }
    });

    $('.tileEntityToggle').click(function() {
        var id = $(this).attr('data-id');
        tileEntityToggle(id);
        if (listTileEntityToggle[id]) {
            $('.tileEntityToggle[data-id="' + id + '"]').parent().addClass('active');
        } else {
            $('.tileEntityToggle[data-id="' + id + '"]').parent().removeClass('active');
        }
    });

    $('#gridToggle').click(function() {
        if ($('#gridToggle').parent().hasClass('active')) {
            $('#gridToggle').parent().removeClass('active');
            doChunkGrid(false);
        } else {
            $('#gridToggle').parent().addClass('active');
            doChunkGrid(true);
        }
    });

    $('#showNetherCoordinatesToggle').click(function() {
        if ($(this).parent().hasClass('active')) {
            $(this).parent().removeClass('active');
        } else {
            $(this).parent().addClass('active');
        }
        showNetherCoordinatesFlag = !showNetherCoordinatesFlag;
    });
    
    $('#chunkDisplayToggle').click(function() {
        if ($(this).parent().hasClass('active')) {
            $(this).parent().removeClass('active');
        } else {
            $(this).parent().addClass('active');
        }
        showChunkCoordinatesFlag = !showChunkCoordinatesFlag;
    });

    $('#slimeChunksToggle').click(function() {
        if ($('#slimeChunksToggle').parent().hasClass('active')) {
            $('#slimeChunksToggle').parent().removeClass('active');
            doSlimeChunks(false);
        } else {
            $('#slimeChunksToggle').parent().addClass('active');
            doSlimeChunks(true);
        }
    });

    $('#checkPlayerDistanceToggle').click(function() {
        if ($(this).parent().hasClass('active')) {
            $(this).parent().removeClass('active');
        } else {
            $(this).parent().addClass('active');
        }
        doCheckPlayerDistanceFlag = !doCheckPlayerDistanceFlag;
        if ( vectorPoints ) {
            vectorPoints.changed();
        }
    });
    
    $('#elevationToggle').click(function() {
        if ( globalCORSWarningFlag ) {
            doModal('CORS Error', 'Error accessing map pixels.  We cannot enable the elevation overlay.<br/><br/>' +
                    globalCORSWarning);
            return;
        }
        if ($('#elevationToggle').parent().hasClass('active')) {
            $('#elevationToggle').parent().removeClass('active');
            doShadedRelief(false);
        } else {
            $('#elevationToggle').parent().addClass('active');
            doShadedRelief(true);
        }
    });
    $('#elevationReset').click(function() {
        $('#vert').val($('#vert').attr('data-default'));
        $('#sunEl').val($('#sunEl').attr('data-default'));
        $('#sunAz').val($('#sunAz').attr('data-default'));
        $('#shadeOpacity').val($('#shadeOpacity').attr('data-default'));
        if (rasterElevation !== null) {
            rasterElevation.changed();
        }
    });
    $('#shadeOpacity').change(function() {
        if (layerElevation !== null) {
            layerElevation.setOpacity( $('#shadeOpacity').val() / 100.0 );
        }
    });


    $('#elevationAlphaToggle').click(function() {
        if ($('#elevationAlphaToggle').parent().hasClass('active')) {
            $('#elevationAlphaToggle').parent().removeClass('active');
            doElevationAlpha(false);
        } else {
            $('#elevationAlphaToggle').parent().addClass('active');
            doElevationAlpha(true);
        }
    });
    var updateElevationAlphaOpacity = function() {
        $('#elevationAlphaOpacityOut').html($('#elevationAlphaOpacity').val());
        if (layerElevationAlpha !== null) {
            layerElevationAlpha.setOpacity( $('#elevationAlphaOpacity').val() / 100.0 );
        }
    };
    $('#elevationAlphaOpacity').change(function() {
        updateElevationAlphaOpacity();
    });
    updateElevationAlphaOpacity();
    
    $('#elevationStaticToggle').click(function() {
        if ($('#elevationStaticToggle').parent().hasClass('active')) {
            $('#elevationStaticToggle').parent().removeClass('active');
            doElevationStatic(false);
        } else {
            $('#elevationStaticToggle').parent().addClass('active');
            doElevationStatic(true);
        }
    });
    var updateElevationStaticOpacity = function() {
        $('#elevationStaticOpacityOut').html($('#elevationStaticOpacity').val());
        if (layerShadedReliefStatic !== null) {
            layerShadedReliefStatic.setOpacity( $('#elevationStaticOpacity').val() / 100.0 );
        }
    };
    $('#elevationStaticOpacity').change(function() {
        updateElevationStaticOpacity();
    });
    updateElevationStaticOpacity();

    
    $('#btnHelp').click(function() {
        doTour(false);
    });

    $('#btnCheckUpdate').click(function() {
        doCheckUpdate();
    });
    $('#btnAbout').click(function() {
        doAbout();
    });


    // don't close dropdowns when an item in them is clicked
    $('.menu-stay .dropdown-menu').on({
        'click': function(e) {
            e.stopPropagation();
        }
    });
    
    // put the world info
    $('#worldInfo').html(
        '<span id="worldName" class="badge mytooltip" title="World Name">' + worldName + '</span>' +
            '<span id="creationTime" class="label mytooltip" title="Imagery Creation Date">' + creationTime + '</span>'
    );

    // setup tooltips
    $('.mytooltip').tooltip({
        // this helps w/ btn groups
        trigger: 'hover',
        container: 'body'
    });

    if ( useTilesFlag ) {
        // remove dynamic shaded relief -- currently does not work in tile mode
        $('.dynamicShadedRelief').hide();
    }
    
    // setup hotkeys
    $(document).on('keydown', function(evt) {
        var key = String.fromCharCode(evt.which);
        // escape key will quit any special modes
        if ( evt.keyCode == 27 ) {
            doGlobalQuit();
        }
        // 'l' or 'm' will put us in line measurement mode
        else if ( key === 'L' || key === 'M' ) {
            measureControl.forceDrawType('LineString');
        }
        // 'c' will put us in circle measurement mode
        else if ( key === 'C' ) {
            measureControl.forceDrawType('Circle');
        }
        // 'g' will toggle chunk grid
        else if ( key === 'G' ) {
            $('#gridToggle').click();
        }
        // 'n' will toggle nether conversion
        else if ( key === 'N' ) {
            $('#showNetherCoordinatesToggle').click();
        }
        // todobig -- more keyboard toggles/controls here
    });

    // fix map size
    window.addEventListener('resize', fixContentHeight);
    fixContentHeight();
});
