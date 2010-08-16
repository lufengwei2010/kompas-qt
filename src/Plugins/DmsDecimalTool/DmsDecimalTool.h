#ifndef Map2X_QtGui_Plugins_DmsDecimalTool_h
#define Map2X_QtGui_Plugins_DmsDecimalTool_h
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
 * @brief Class Map2X::QtGui::Plugins::DmsDecimalTool
 */

#include "../../AbstractTool.h"
#include "DmsDecimalToolDialog.h"

namespace Map2X { namespace QtGui { namespace Plugins {

/** @brief Converting DMS to decimal and back */
class DmsDecimalTool: public AbstractTool {
    public:
        DmsDecimalTool(PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = ""):
            AbstractTool(manager, plugin) {}

        virtual QString menuText() const { return QObject::tr("DMS - Decimal converter"); }
        virtual AbstractToolDialog* create(MainWindow* mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0) const
            { return new DmsDecimalToolDialog(mainWindow, parent, f); }
};

}}}

PLUGIN_REGISTER_STATIC(DmsDecimal, Map2X::QtGui::Plugins::DmsDecimalTool, "cz.mosra.Map2X.QtGui.AbstractTool/0.1")
PLUGIN_SET_NAME("DMS - Decimal converter")
PLUGIN_SET_DESCRIPTION("Tool for converting coordinates from Degree Minute Second format to decimal number and back.")
PLUGIN_FINISH

#endif