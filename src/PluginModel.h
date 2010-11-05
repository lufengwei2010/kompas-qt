#ifndef Map2X_QtGui_PluginModel_h
#define Map2X_QtGui_PluginModel_H
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
 * @brief Class Map2X::QtGui::PluginModel
 */

#include <vector>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QStringList>

#include "AbstractPluginManager.h"

namespace Map2X { namespace QtGui {

/**
 * @brief Model for viewing and managing plugins
 * @todo Don't regenerate lists on every data request
 */
class PluginModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        /** @brief Flags */
        enum Flags {
            LoadedOnly  = 0x01      /**< @brief Display only loaded plugins */
        };

        /** @brief Columns */
        enum Column {
            LoadState,
            Plugin,
            Name,
            Description,
            Authors,
            Version,
            Depends,
            UsedBy,
            Replaces,
            ReplacedWith
        };

        /**
         * @brief Constructor
         *
         * @param _manager      Pointer to PluginManager
         * @param flags         Flags
         * @param parent        Parent object
         */
        PluginModel(AbstractPluginManager* _manager, int flags = 0, QObject* parent = 0);

        /** @brief Reload data from PluginManager */
        void reload();

        /**
         * @brief Find plugin with specific name
         * @param name          Plugin name
         * @return Row index of given plugin or -1 if the plugin is not found
         *      in this model.
         */
        int findPlugin(const QString& name) const;

        /** @brief Row count */
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const { return nameList.size(); }

        /** @brief Column count */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const { return 10; }

        /** @brief Header data access */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /** @brief Data read access */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /** @brief Item flags */
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;

        /** @brief Data write access */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    private slots:
        void loadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after);
        void unloadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after);

    private:
        AbstractPluginManager* manager;
        int _flags;
        std::vector<std::string> nameList;
};

}}

#endif
