/*
 *    Copyright (C)2018 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
       \brief
       @author authorname
*/



#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#include <genericworker.h>
#include <innermodel/innermodel.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDesktopWidget>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QXmlDefaultHandler>
#include <QGLViewer/qglviewer.h>
#include <unordered_map>
#include <any>
#include <QGraphicsEllipseItem>
#include <string>
#include <memory>
#include <cppitertools/range.hpp>
#include <nabo/nabo.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#include "graph.h"
#include "graphviewer.h"

class Handler : public QXmlDefaultHandler
{
	bool fatalError (const QXmlParseException & exception)
	{
    qWarning() << "Fatal error on line" << exception.lineNumber()
               << ", column" << exception.columnNumber() << ':'
               << exception.message();

    return false;
	}
	
};

class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:
	SpecificWorker(MapPrx& mprx);
	~SpecificWorker();
	bool setParams(RoboCompCommonBehavior::ParameterList params);

public slots:
	void compute();
	void initialize(int period);
	void initializeFromInnerModel(InnerModel *inner);
	void initializeRandom();
	void initializeXML(std::string file_name);

	void walkTree(InnerModelNode *node);

private:
	InnerModel *innerModel;

	std::shared_ptr<Graph> graph;
	GraphViewer graph_viewer;
	
};

#endif
