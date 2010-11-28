#ifndef Map2X_QtGui_MainWindow_h
#define Map2X_QtGui_MainWindow_h
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
 * @brief Class Map2X::QtGui::MainWindow
 */

#include <QtCore/QReadWriteLock>
#include <QtGui/QMainWindow>
#include <QtGui/QFrame>

#include "Utility/Configuration.h"
#include "AbstractMapView.h"
#include "AbstractTool.h"
#include "AbstractProjection.h"

class QStackedWidget;
class QAction;
class QMenu;
class QLabel;
class QDockWidget;

namespace Map2X { namespace QtGui {

class AbstractPluginManager;
template<class T> class PluginManager;

class RasterPackageModel;
class RasterLayerModel;
class RasterOverlayModel;
class RasterZoomModel;
class SaveRasterMenuView;

/** @brief %Map2X main window */
class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
        /** @brief Global instance of MainWindow */
        static MainWindow* instance() { return _instance; }

        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param flags     Window flags
         */
        MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

        /**
         * @brief Destructor
         *
         * Destroys all plugin managers.
         */
        virtual ~MainWindow();

        /** @brief Global application configuration */
        inline Utility::Configuration* configuration()
            { return &_configuration; }

        /** @brief Instance of PluginManager for map view plugins */
        inline PluginManager<AbstractMapView>* mapViewPluginManager()
            { return _mapViewPluginManager; }

        /** @brief Instance of PluginManager for projection plugins */
        inline PluginManager<Core::AbstractProjection>* projectionPluginManager()
            { return _projectionPluginManager; }

        /** @brief Instance of PluginManager for tile model plugins */
        inline PluginManager<Core::AbstractRasterModel>* rasterModelPluginManager()
            { return _rasterModelPluginManager; }

        /** @brief Instance of PluginManager for tool plugins */
        inline PluginManager<AbstractTool>* toolPluginManager()
            { return _toolPluginManager; }

        /** @brief Global raster map package model */
        inline RasterPackageModel* rasterPackageModel()
            { return _rasterPackageModel; }

        /** @brief Global raster map layer model */
        inline RasterLayerModel* rasterLayerModel()
            { return _rasterLayerModel; }

        /** @brief Global raster map layer model */
        inline RasterOverlayModel* rasterOverlayModel()
            { return _rasterOverlayModel; }

        /** @brief Global raster map layer model */
        inline RasterZoomModel* rasterZoomModel()
            { return _rasterZoomModel; }

        /**
         * @brief Get tile model for reading
         * @return Pointer to tile model
         *
         * This functions locks tile model for reading. After usage the model
         * have to be unlocked with unlockRasterModel().
         */
        inline const Core::AbstractRasterModel* lockRasterModelForRead() {
            rasterModelLock.lockForRead();
            return _rasterModel;
        }

        /**
         * @brief Get tile model for writing
         * @return Pointer to tile model
         *
         * This functions locks tile model for writing. After usage the model
         * have to be unlocked with unlockRasterModel().
         */
        inline Core::AbstractRasterModel* lockRasterModelForWrite() {
            rasterModelLock.lockForWrite();
            return _rasterModel;
        }

        /**
         * @brief Unlock tile model
         *
         * Unlocks tile model previously locked with lockRasterModelRead() or
         * lockRasterModelWrite().
         */
        inline void unlockRasterModel() {
            rasterModelLock.unlock();
        }

        /**
         * @brief Map view
         * @return Current map view. The pointer should not be stored anywhere
         *      because it can change after loading another map view plugin.
         */
        inline AbstractMapView* mapView() { return _mapView; }

    signals:
        /** @brief Map view has been changed to another */
        void mapViewChanged();

        /** @brief Raster model has been changed to another */
        void rasterModelChanged();

    public slots:
        /**
         * @brief Load default configuration
         *
         * Goes through configuration and if any group or key is missing, sets
         * it to default value.
         */
        void loadDefaultConfiguration();

        /**
         * @brief Set map view
         * @param view      Instance of map view
         *
         * Replaces current map view with given instance.
         */
        void setMapView(AbstractMapView* view);

        /**
         * @brief Set raster model
         * @param model     Instance of raster model
         *
         * Replaces current raster model with given instance.
         */
        void setRasterModel(Core::AbstractRasterModel* model);

        /**
         * @brief Enable or disable online maps
         * @param enabled   Whether disable or enable
         */
        void setOnlineEnabled(bool enabled);

    private slots:
        /**
         * @brief Open raster map file
         *
         * Allows user to select an file through open file dialog. First checks
         * whether the file can be opened with currently active model (if it
         * either supports multiple packages or doesn't have any opened
         * packages), otherwise goes through all available raster plugins
         * and tries to open the file with them.
         */
        void openRaster();

        /**
         * @brief Save offline package of current raster map
         */
        void saveRaster();

        /**
         * @brief Close raster map
         */
        inline void closeRaster() { setRasterModel(0); }

    private:
        static MainWindow* _instance;

        Utility::Configuration _configuration;

        PluginManager<AbstractMapView>* _mapViewPluginManager;
        PluginManager<Core::AbstractProjection>* _projectionPluginManager;
        PluginManager<Core::AbstractRasterModel>* _rasterModelPluginManager;
        PluginManager<AbstractTool>* _toolPluginManager;

        RasterPackageModel* _rasterPackageModel;
        RasterLayerModel* _rasterLayerModel;
        RasterOverlayModel* _rasterOverlayModel;
        RasterZoomModel* _rasterZoomModel;

        AbstractMapView* _mapView;
        Core::AbstractRasterModel* _rasterModel;
        QReadWriteLock rasterModelLock;

        QAction *openRasterAction,
            *saveRasterAction,
            *closeRasterAction,
            *quitAction,
            *zoomInAction,
            *zoomOutAction,
            *pluginDialogAction,
            *configurationDialogAction,
            *aboutQtAction;

        QMenu *openRasterMenu,
            *saveRasterMenu,
            *mapMenu,
            *toolsMenu;

        SaveRasterMenuView* saveRasterMenuView;

        QLabel* coordinateStatus;

        QStackedWidget* centralStackedWidget;
        QDockWidget* mapOptionsDock;

        void createActions();
        void createMenus();

        void loadPluginsAsConfigured(const std::string& group, AbstractPluginManager* manager);

        void displayMapIfUsable();

    private slots:
        void pluginDialog();
        void configurationDialog();
        void currentCoordinates(const Core::Wgs84Coords& coords);
};

}}

#endif
