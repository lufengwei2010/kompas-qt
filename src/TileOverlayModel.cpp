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

#include "TileOverlayModel.h"

#include "AbstractTileModel.h"
#include "AbstractMapView.h"

using namespace std;

namespace Map2X { namespace QtGui {

TileOverlayModel::TileOverlayModel(Core::AbstractTileModel** _tileModel, AbstractMapView** _mapView, int flags, QObject* parent): QAbstractListModel(parent), tileModel(_tileModel), mapView(_mapView), _flags(flags) { reload(); }

void TileOverlayModel::reload() {
    overlays.clear();
    loaded.clear();

    /* Only loaded overlays */
    if(_flags & LoadedOnly) {
        overlays.append((*mapView)->overlays());
        loaded.fill(true, overlays.size());

    /* All available overlays */
    } else {
        vector<string> _overlays = (*tileModel)->overlays();
        QStringList _loaded = (*mapView)->overlays();
        for(vector<string>::const_iterator it = _overlays.begin(); it != _overlays.end(); ++it) {
            overlays.append(QString::fromStdString(*it));

            if(_loaded.contains(overlays.last()))
                loaded.setBit(overlays.size()-1, true);
        }

        /* Make sure loadedOverlays bitarray is as large as overlays list */
        loaded.resize(overlays.size());
    }
}

QVariant TileOverlayModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() != 0 || index.row() >= rowCount()) return QVariant();

    if(role == Qt::DisplayRole)
        return overlays.at(index.row());
    if(role == Qt::CheckStateRole)
        return loaded.at(index.row()) ? Qt::Checked : Qt::Unchecked;

    return QVariant();
}

}}