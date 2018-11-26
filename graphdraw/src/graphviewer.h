/*
 * Copyright 2018 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <memory>
#include "graph.h"
#if Qt5_FOUND
	#include <QtWidgets>
#else
	#include <QtGui>
#endif
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QDesktopWidget>
#include <QGLViewer/qglviewer.h>
#include <nabo/nabo.h>
#include <cppitertools/range.hpp>


class GraphViewer : public QGraphicsView
{
	public:
    void setGraph(std::shared_ptr<Graph> graph_,  QScrollArea *scrollArea);
		void draw();
		void applyForces(std::shared_ptr<Graph> g);
		void applyForces2(std::shared_ptr<Graph> g);
		
	private:
		std::shared_ptr<Graph> graph;
		QGraphicsScene scene;
	  QGraphicsEllipseItem *node;

};

#endif // GRAPHVIEWER_H
