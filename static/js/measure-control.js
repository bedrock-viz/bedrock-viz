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
    button.innerHTML = '<img src="images/map-control-assets/measure-icon.png" class="interface-icon" />';
    button.addEventListener('click', this.handleMeasure, false);
    button.addEventListener('touchstart', this.handleMeasure, false);
    $(button).addClass('mytooltip measure-start-button').attr('title', 'Measurement Tools - Press ESC to quit');

    var btnLine = document.createElement('button');
    btnLine.innerHTML = 'L';
    btnLine.addEventListener('click', function() { setDrawType('LineString'); }, false);
    btnLine.addEventListener('touchstart', function() { setDrawType('LineString'); }, false);
    $(btnLine).addClass('mytooltip measureSubcontrol').attr('title', 'Lines - Hotkey L').hide();

    var btnCircle = document.createElement('button');
    btnCircle.innerHTML = 'C';
    btnCircle.addEventListener('click', function() { setDrawType('Circle'); }, false);
    btnCircle.addEventListener('touchstart', function() { setDrawType('Circle'); }, false);
    $(btnCircle).addClass('mytooltip measureSubcontrol').attr('title', 'Circles - Hotkey C').hide();

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
