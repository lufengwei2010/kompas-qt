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

#include "RasterPackageModel.h"

#include "MainWindow.h"

using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

void RasterPackageModel::reload() {
    beginResetModel();
    packages.clear();

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    /* All packages */
    if(rasterModel) for(int i = 0; i != rasterModel->packageCount(); ++i) {
        Package p;
        p.name = QString::fromStdString(rasterModel->packageAttribute(i, AbstractRasterModel::Name));
        p.description = QString::fromStdString(rasterModel->packageAttribute(i, AbstractRasterModel::Description));
        p.packager = QString::fromStdString(rasterModel->packageAttribute(i, AbstractRasterModel::Packager));
        packages.append(p);
    }

    MainWindow::instance()->unlockRasterModel();

    endResetModel();
}

QVariant RasterPackageModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() >= columnCount() || index.row() >= rowCount() || role != Qt::DisplayRole)
        return QVariant();

    switch(index.column()) {
        case Name:
            return packages.at(index.row()).name;
        case Description:
            return packages.at(index.row()).description;
        case Packager:
            return packages.at(index.row()).packager;
    }

    return QVariant();
}

}}