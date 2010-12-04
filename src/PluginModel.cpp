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

#include "PluginModel.h"

#include <algorithm>

#include "PluginManager.h"

using namespace std;

namespace Map2X { namespace QtGui {

PluginModel::PluginModel(AbstractPluginManager* _manager, int flags, QObject* parent): QAbstractTableModel(parent), manager(_manager), _flags(flags) {
    connect(manager, SIGNAL(loadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)),
            SLOT(loadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)));
    connect(manager, SIGNAL(unloadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)),
            SLOT(unloadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)));

    reload();
}

void PluginModel::reload() {
    if(_flags & LoadedOnly) {
        nameList.clear();
        vector<string> _nameList = manager->nameList();

        for(vector<string>::const_iterator it = _nameList.begin(); it != _nameList.end(); ++it) {
            if(manager->loadState(*it) & (AbstractPluginManager::IsStatic|AbstractPluginManager::LoadOk))
                nameList.push_back(*it);
        }

    } else nameList = manager->nameList();
}

int PluginModel::findPlugin(const QString& name) const {
    vector<string>::const_iterator it = find(nameList.begin(), nameList.end(), name.toStdString());
    if(it == nameList.end()) return -1;
    return it-nameList.begin();
}

QVariant PluginModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case LoadState:     return tr("Load state");
            case Plugin:        return tr("Plugin");
            case Name:          return tr("Name");
            case Description:   return tr("Description");
            case Authors:       return tr("Author(s)");
            case Version:       return tr("Version");
            case Depends:       return tr("Depends on");
            case UsedBy:        return tr("Used by");
            case Replaces:      return tr("Replaces");
            case ReplacedWith:  return tr("Can be replaced with");
        }
    }

    return QVariant();
}

QVariant PluginModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return QVariant();

    string name = nameList[index.row()];
    QString qName = QString::fromStdString(name);

    /* Load state */
    if(index.column() == LoadState) {
        AbstractPluginManager::LoadState state = manager->loadState(name);

        if(role == Qt::CheckStateRole) {
            if(state & (AbstractPluginManager::LoadOk|AbstractPluginManager::UnloadFailed|AbstractPluginManager::IsRequired|AbstractPluginManager::IsStatic))
                return Qt::Checked;
            else
                return Qt::Unchecked;

        } else if(role == Qt::DisplayRole || role == Qt::EditRole) switch(state) {
            case AbstractPluginManager::NotFound:
                return tr("Not found");
            case AbstractPluginManager::WrongPluginVersion:
                return tr("Wrong plugin version");
            case AbstractPluginManager::WrongInterfaceVersion:
                return tr("Wrong interface version");
            case AbstractPluginManager::WrongMetadataFile:
                return tr("Error in metadata file");
            case AbstractPluginManager::UnresolvedDependency:
                return tr("Unresolved dependency");
            case AbstractPluginManager::LoadFailed:
                return tr("Loading failed");
            case AbstractPluginManager::LoadOk:
                return tr("Loaded");
            case AbstractPluginManager::NotLoaded:
                return tr("Not loaded");
            case AbstractPluginManager::UnloadFailed:
                return tr("Unload failed");
            case AbstractPluginManager::IsRequired:
                return tr("Is required by another");
            case AbstractPluginManager::IsStatic:
                return tr("Static plugin");
            case AbstractPluginManager::IsUsed:
                return tr("Is already used");
        }

    /* Plugin "filename" */
    } else if(index.column() == Plugin && (role == Qt::DisplayRole || role == Qt::EditRole))
        return qName;

    /* Plugin name */
    else if(index.column() == Name && (role == Qt::DisplayRole || role == Qt::EditRole))
        return QString::fromStdString(manager->metadata(name)->name());

    /* Plugin description */
    else if(index.column() == Description && (role == Qt::DisplayRole || role == Qt::EditRole))
        return QString::fromStdString(manager->metadata(name)->description());

    /* Author(s) */
    else if(index.column() == Authors && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        QStringList list;
        vector<string> authors = manager->metadata(name)->authors();
        for(vector<string>::const_iterator it = authors.begin(); it != authors.end(); ++it)
            list.append(QString::fromStdString(*it));
        return list.join(", ");

    /* Plugin version */
    } else if(index.column() == Version && (role == Qt::DisplayRole || role == Qt::EditRole))
        return QString::fromStdString(manager->metadata(name)->version());

    /* On what this plugin depends */
    else if(index.column() == Depends && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        QStringList list;
        vector<string> depends = manager->metadata(name)->depends();
        for(vector<string>::const_iterator it = depends.begin(); it != depends.end(); ++it)
            list.append(QString::fromStdString(*it));
        return list.join(", ");

    /* What depends on this plugin */
    } else if(index.column() == UsedBy && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        QStringList list;
        vector<string> usedBy = manager->metadata(name)->usedBy();
        for(vector<string>::const_iterator it = usedBy.begin(); it != usedBy.end(); ++it)
            list.append(QString::fromStdString(*it));
        return list.join(", ");

    /* What this plugin replaces */
    } else if(index.column() == Replaces && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        QStringList list;
        vector<string> replaces = manager->metadata(name)->replaces();
        for(vector<string>::const_iterator it = replaces.begin(); it != replaces.end(); ++it)
            list.append(QString::fromStdString(*it));
        return list.join(", ");

    /* By what can be this plugin replaced */
    } else if(index.column() == ReplacedWith && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        QStringList list;
        vector<string> replacedWith = manager->metadata(name)->replacedWith();
        for(vector<string>::const_iterator it = replacedWith.begin(); it != replacedWith.end(); ++it)
            list.append(QString::fromStdString(*it));
        return list.join(", ");

    /* Something another */
    } return QVariant();
}

Qt::ItemFlags PluginModel::flags(const QModelIndex& index) const {
    if(!index.isValid()) return Qt::ItemIsEnabled;

    /* Only load state column is checkable */
    if(index.column() == LoadState) {
        string name = nameList[index.row()];

        /* Static plugins are disabled */
        if(manager->loadState(name) != AbstractPluginManager::IsStatic)
            return QAbstractTableModel::flags(index)|Qt::ItemIsUserCheckable;
    }

    return QAbstractItemModel::flags(index);
}

bool PluginModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if((_flags & LoadedOnly) || !index.isValid() || index.column() != LoadState || role != Qt::CheckStateRole)
        return false;

    string name = nameList[index.row()];

    /* Static plugins cannot be edited */
    if(manager->loadState(name) == AbstractPluginManager::IsStatic)
        return false;

    /* Unload plugin */
    if(manager->loadState(name) & (AbstractPluginManager::LoadOk|AbstractPluginManager::UnloadFailed|AbstractPluginManager::IsRequired)) {
        if(manager->unload(name) != AbstractPluginManager::NotLoaded) return false;
        return true;

    /* Load plugin */
    } else {
        if(manager->load(name) != AbstractPluginManager::LoadOk) return false;
        return true;
    }
}

void PluginModel::loadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after) {
    /* Plugin was loaded */
    if(!(before & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)) &&
       (after & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic))) {

        /* Add to list, if displaying only loaded plugins */
        if(_flags & LoadedOnly) {
            beginInsertRows(QModelIndex(), nameList.size(), nameList.size());
            /** @todo Insert to right place (alphabetically sorted) */
            nameList.push_back(name);
            endInsertRows();

        /* Or just emit signal about data change */
        } else {
            /* Find the name in list */
            vector<string>::const_iterator it = find(nameList.begin(), nameList.end(), name);
            if(it == nameList.end()) return;
            /** @todo Emit dataChanged() for whole row, if metadata changed */
            emit dataChanged(index(it-nameList.begin(), LoadState), index(it-nameList.begin(), LoadState));
        }
    }
}

void PluginModel::unloadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after) {
    /** @bug When trying to unload used plugin the plugin is removed from LoadedOnly model! ... check all states */
    /* Plugin was unloaded */
    if((before & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)) &&
       !(after & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic))) {

        /* Find the name in list */
        vector<string>::iterator it = find(nameList.begin(), nameList.end(), name);
        if(it == nameList.end()) return;

        /* Add to list, if displaying only loaded plugins */
        if(_flags & LoadedOnly) {
            beginRemoveRows(QModelIndex(), it-nameList.begin(), it-nameList.begin());
            nameList.erase(it);
            endRemoveRows();

        /* Or just emit signal about data change */
        } else emit dataChanged(index(it-nameList.begin(), LoadState), index(it-nameList.begin(), LoadState));
    }
}

}}
