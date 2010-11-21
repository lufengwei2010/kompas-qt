#ifndef Map2X_QtGui_AbstractMapView_h
#define Map2X_QtGui_AbstractMapView_h
/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Map2X.

    Map2X is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Map2X is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Map2X::QtGui::AbstractMapView
 */

#include <QtGui/QWidget>

#include "AbsoluteArea.h"
#include "AbstractRasterModel.h"

namespace Map2X { namespace QtGui {

class TileDataThread;

/** @brief Abstract class for map viewer widget plugins */
class AbstractMapView: public QWidget, Map2X::PluginManager::Plugin {
    Q_OBJECT

    #ifndef QT_MOC_RUN
    PLUGIN_INTERFACE("cz.mosra.Map2X.QtGui.AbstractMapView/0.1")
    #endif

    public:
        /** @brief Direction for moving map */
        enum Direction {
            Top, TopLeft, Left, BottomLeft, Bottom, BottimRight, Right, TopRight
        };

        /** @copydoc PluginManager::Plugin::Plugin */
        AbstractMapView(Map2X::PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = "");

        /** @{ @name Map data functions */

    public slots:
        /**
         * @brief Update raster model
         *
         * Connected to MainWindow::rasterModelChanged(). Reloads all map data
         * from new model.
         */
        virtual void updateRasterModel() = 0;

    public:
        /**
         * @brief Set map layer
         * @param layer     Layer
         * @return Whether the layer exists and can be set.
         *
         * Sets map layer from list provided by tile model.
         */
        virtual bool setLayer(const QString& layer) = 0;

        /**
         * @brief Current map layer
         * @return Currently set map layer
         */
        virtual QString layer() const = 0;

        /**
         * @brief Add map overlay
         * @param layer     Overlay
         * @return Whether the overlay exists and can be added.
         *
         * Adds overlay to map from list provided by tile model.
         */
        virtual bool addOverlay(const QString& layer) = 0;

        /**
         * @brief Remove map overlay
         * @param overlay   Overlay
         * @return If the overlay was added and can be removed, returns true.
         *      Otherwise returns false.
         */
        virtual bool removeOverlay(const QString& overlay) = 0;

        /**
         * @brief List of added overlays
         * @return All currently added overlays
         */
        virtual QStringList overlays() const = 0;

        /*@}*/

        /** @{ @name Map movement functions */

        /**
         * @brief Current zoom
         */
        virtual unsigned int zoom() const = 0;

        /**
         * @brief Get current map coordinates
         * @param pos       Position for which compute the coordinates, relative
         *      to map view widget. If the position is null, the coordinates
         *      should be taken from center of view widget.
         */
        virtual Core::Wgs84Coords coords(const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Currently viewed tile area
         * @param area      Area relative to map view widget. If given area is
         *      null, the function returns coordinates for whole visible area
         *      in map view widget.
         * @return Coordinates of tile area. If the map is smaller than view
         *      area, returns coordinates for existing tiles only.
         */
        virtual Core::TileArea viewedArea(const QRect& area = QRect()) = 0;

    public slots:
        /**
         * @brief Zoom map in
         * @param pos       Position which should keep its coordinates after
         *      zooming, relative to map view widget. Useful for zooming to
         *      cursor. If the position is null, the view should zoom to center
         *      of view widget.
         * @return Whether the map can be zoomed in
         */
        virtual bool zoomIn(const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Zoom map out
         * @param pos       Position which should keep its coordinates after
         *      zooming, relative to map view widget. Useful for zooming to
         *      cursor. If the position is null, the view should zoom to center
         *      of view widget.
         * @return Whether the map can be zoomed out
         */
        virtual bool zoomOut(const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Zoom map to given zoom
         * @param zoom      Zoom level
         * @param pos       Position which should keep its coordinates after
         *      zooming, relative to map view widget. Useful for zooming to
         *      cursor. If the position is null, the view should zoom to center
         *      of view widget.
         * @return Whether the zoom level is available
         */
        virtual bool zoomTo(Core::Zoom zoom, const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Move map to given coordinates
         * @param coords Coordinates
         * @param pos       Position to which apply the coordinates, relative to
         *      map view widget. If the position is null, the coordinates should
         *      be applied to center of view widget.
         * @return Whether the map can be moved to given coordinates (they
         *      are valid and they aren't out of available map area).
         *
         * Moves the map so given coordinates are centered in view area.
         */
        virtual bool setCoords(const Core::Wgs84Coords& coords, const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Move map in given direction
         * @param direction Direction
         * @param speed     Speed in pps (pixels per second)
         * @return Whether the map can be moved (whether map area is available)
         */
        virtual bool move(Direction direction, unsigned int speed) = 0;

        /*@}*/

    protected:
        TileDataThread* tileDataThread;         /**< @brief Thread for downloading tile data */

    signals:
        /**
         * @brief Request for tile data
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         *
         * Connected to TileDataThread::getTileData().
         */
        void getTileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords);

        /**
         * @brief Coordinates under mouse
         * @param coords    Coordinates
         *
         * These coordinates are displayed in status bar.
         */
        void currentCoordinates(const Core::Wgs84Coords& coords);

    protected slots:
        /**
         * @brief Tile data
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         * @param data      Tile data
         */
        virtual void tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QByteArray& data) = 0;

        /**
         * @brief Tile data downloading
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         */
        virtual void tileLoading(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) = 0;

        /**
         * @brief Tile data not found
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         */
        virtual void tileNotFound(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) = 0;
};

}}

#endif
