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

#include "graphviewer.h"
#include <cppitertools/range.hpp>
#include <qmat/QMatAll>
#include <QDesktopWidget>
#include <QGLViewer/qglviewer.h>
#include <nabo/nabo.h>
#include <QApplication>
#include "graph.h"
#include "graphnode.h"
#include "graphedge.h"

GraphViewer::GraphViewer()
{
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);
	scene.setSceneRect(-200, -200, 400, 400);
	this->setScene(&scene);
	this->setCacheMode(CacheBackground);
	this->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	this->setViewportUpdateMode(BoundingRectViewportUpdate);
	this->setRenderHint(QPainter::Antialiasing);
	this->setTransformationAnchor(AnchorUnderMouse);
	this->scale(qreal(0.8), qreal(0.8));
	this->setMinimumSize(400, 400);
	this->fitInView(scene.sceneRect(), Qt::KeepAspectRatio );
	this->adjustSize();
 	QRect availableGeometry(QApplication::desktop()->availableGeometry());
 	this->move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
}

void GraphViewer::setGraph(std::shared_ptr<Graph> graph_, QScrollArea *scrollArea)
{
	this->graph = graph_;
	this->setParent(scrollArea);
	scrollArea->setWidget(this);
	scrollArea->setMinimumSize(600,600);
	
	for(auto &[node_id, node_value] : *graph)
	{
			// get attrs from graph
			auto &[node_atts, node_draw_attrs, node_fanout, node_fanin] = node_value;
			float node_posx = graph->attr<float>(node_draw_attrs["posx"]);
			float node_posy = graph->attr<float>(node_draw_attrs["posy"]);
			std::string color_name = graph->attr<std::string>(node_draw_attrs["color"]);
			QString qname = QString::fromStdString(graph->attr<std::string>(node_draw_attrs["name"]));
			
			//create graphic nodes 
			auto gnode = new GraphNode(this);
			gnode->setColor(color_name.c_str());
			scene.addItem(gnode);
			gnode->setPos(node_posx, node_posy);
			gnode->setTag(qname);
			
			//add to graph
			node_draw_attrs["gnode"] = gnode;
	}		
	
	// add edges after all nodes have been created
	for(auto &[node_id, node_value] : *graph)
	{
			auto &[node_atts, node_draw_attrs, node_fanout, node_fanin] = node_value;
			auto node_origen = graph->attr<GraphNode*>(node_draw_attrs["gnode"]); 
			for( auto &[node_adj, edge_atts] : node_fanout)
			{
				auto [node_dest_atts, node_dest_draw_attrs, node_dest_fanout, node_dest_fanin] = graph->node(node_adj);
				auto node_dest = graph->attr<GraphNode*>(node_dest_draw_attrs["gnode"]); 
				auto edge_tag = graph->attr<std::string>(edge_atts.draw_attrs["name"]);
				scene.addItem(new GraphEdge(node_origen, node_dest, edge_tag.c_str()));
			}
	}
}

void GraphViewer::draw()
{
	//std::cout << this->width() << " " << this->height() << std::endl;
	//this->fitInView(scene.sceneRect(), Qt::KeepAspectRatio );
	show();
}

void GraphViewer::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}

void GraphViewer::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<GraphNode *> nodes;
    foreach (QGraphicsItem *item, scene.items()) 
		{
        if (GraphNode *node = qgraphicsitem_cast<GraphNode *>(item))
            nodes << node;
    }
    foreach (GraphNode *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (GraphNode *node, nodes) 
		{
        if (node->advancePosition())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}

// void GraphViewer::applyForces(std::shared_ptr<Graph> g)
// {
// 	if(g->size() == 0)
// 	{	std::cout << __FILE__ << __FUNCTION__ <<	" Empty GRAPH" << std::endl;	return; }
// 	
// 	// natrix of 2D points from graph nodes
// 	Eigen::MatrixXf M = Eigen::MatrixXf(2, graph->size());
// 	std::uint32_t ind = 0;
// 	std::unordered_map<std::uint32_t, std::uint64_t> nodes_to_matrix;
// 	for( auto &[key, value] : *graph)
// 	{
// 		auto &[attrs, draw_attrs, fanout, fanin] = value; 
// 		M(0,ind) = std::any_cast<float>(draw_attrs["posx"]);
// 		M(1,ind) = std::any_cast<float>(draw_attrs["posy"]);
// 		nodes_to_matrix[ind] = key;
// 		ind++;
// 	}
// 	
// 	// create a kd-tree for M, note that M must stay valid during the lifetime of the kd-tree
// 	Nabo::NNSearchF* nns = Nabo::NNSearchF::createKDTreeLinearHeap(M);
// 	
// 	// query 5 closest points
// 	const int K = std::max(2, (int)(M.cols()/10));
// 	Eigen::MatrixXi indices(K, M.cols());
// 	Eigen::MatrixXf dists2(K, M.cols());
// 	nns->knn(M, indices, dists2, K, 0, Nabo::NNSearchF::SORT_RESULTS );
// 		
// 	// compute result form repelling nodes
// 	float rincx, rincy, aincx, aincy, cincx, cincy;
// 	ind = 0;
// 	for (auto i : iter::range(indices.cols()))
// 	{
// 		auto &node = graph->node(nodes_to_matrix.at(i));
// 		auto &node_draw_attrs = graph->drawAttrs(node);
// 		rincx=0; rincy=0;
// 		
// 		//compute repulsive forces
// 		for (auto j : iter::range(indices.rows()))
// 		{
// 			float rx = M(0,i) - M(0,indices(j,i));
// 			float ry = M(1,i) - M(1,indices(j,i));
// 			float mod = QVec::vec2(rx, ry).norm2();
// 			rx = rx / mod;
// 			ry = ry / mod;
// 			rincx += rx*10000/mod;
// 			rincy += ry*10000/mod;
// 		}
// 		
// 		//compute attractive forces from outgoing edges  
// 		float px = graph->attr<float>(node_draw_attrs["posx"]);
// 		float py = graph->attr<float>(node_draw_attrs["posy"]);
// 		
// 		aincx = 0; aincy = 0;
// 		for( auto &edges : graph->fanout(node))
// 		{
// 			auto &[rnode, eatts] = edges;
// 			auto &dist_draw_atts = graph->drawAttrs(graph->node(rnode));
// 			float xl = graph->attr<float>(dist_draw_atts["posx"]) - px;
// 			float yl = graph->attr<float>(dist_draw_atts["posy"]) - py;
// 			float emod = QVec::vec2(xl, yl).norm2();
// 			xl = xl / emod;
// 			yl = yl / emod;
// 			if( emod > 500 ) 
// 			{	
// 				aincx += xl*10;
// 				aincy += yl*10;
// 			}
// 		}  
// 		
// 		//compute attractive forces from central point
// 		cincx = 0; cincy = 0;
// 		float cx = (scene.width()/2.f-px);
// 		float cy = (scene.height()/2.f-py);
// 		float modc = QVec::vec2(cx,cy).norm2();
// 		cx = cx / modc;
// 		cy = cy / modc;
// 		if( modc > 500 ) 
// 		{	
// 			cincx += cx*10;
// 			cincy += cy*10;
// 		}
// 		
// 		//weighted sum
// 		float incx = rincx + aincx + cincx;
// 		float incy = rincy + aincy + cincy;
// 	
// 		// Rozamiento
//  		if( fabs(incx) < 8 ) incx = 0;
//  		if( fabs(incy) < 8 ) incy = 0;
// 
// 		float rx = std::any_cast<float>(node_draw_attrs["posx"]) + incx;
// 		float ry = std::any_cast<float>(node_draw_attrs["posy"]) + incy;
// 		
// 		//check bounds
// 		auto srect = scene.sceneRect();
// 		rx = std::min(std::max(rx,(float)srect.left()+50),(float)srect.right()-50 );
// 		ry = std::min(std::max(ry,(float)srect.top()+50),(float)srect.bottom()-50 );
// 		
// 		
// 		//update node coors
// 		node_draw_attrs.insert_or_assign("posx", rx);
// 		node_draw_attrs.insert_or_assign("posy", ry);
// 
// 		//move circle and text
// 		auto ellipse_ptr = std::any_cast<QGraphicsEllipseItem *>(node_draw_attrs["ellipse"]);
// 		auto text_ellipse_ptr = std::any_cast<QGraphicsSimpleTextItem *>(node_draw_attrs["ellipse_text"]);
// 		//ellipse_ptr->setRect(rx-300,ry-100,600,200);	
// 		ellipse_ptr->setPos(rx-300,ry-100);	
// 		
// 		text_ellipse_ptr->setX(rx-120);
// 		text_ellipse_ptr->setY(ry-80);
// 		
// 	}
// 	
// 	//move line edges after finishing nodes
// 	for(auto &[node_key, node_value] : *graph)
// 	{
// 		auto &[node_atts, node_draw_attrs, node_fanout, node_fanin] = node_value;
// 		for( auto &[node_dest, edge_atts] : node_fanout)
// 		{
// 			auto &node_dest_draw_attrs = graph->drawAttrs(graph->node(node_dest));
// 			auto line = std::any_cast<QGraphicsLineItem *>(edge_atts.draw_attrs["edge_line"]);
// 			QLine qline(std::any_cast<float>(node_draw_attrs["posx"]),
// 						std::any_cast<float>(node_draw_attrs["posy"]),	std::any_cast<float>(node_dest_draw_attrs["posx"]),
// 						std::any_cast<float>(node_dest_draw_attrs["posy"]));
// 			line->setLine(qline);
// 			auto line_text = std::any_cast<QGraphicsSimpleTextItem *>(edge_atts.draw_attrs["edge_line_text"]);
// 			line_text->setX(qline.center().x()-80);
// 			line_text->setY(qline.center().y()-80);
// 		}
// 	}
// 	delete nns;	
// 	
// }

/////////////////////////
/////////////////////////

void GraphViewer::wheelEvent(QWheelEvent *event)
{
		//zoom
// 	  double scaleFactor = pow((double)2, -event->delta() / 240.0);
// 	  qreal factor = this->transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
//     if (factor < 0.07 || factor > 100)
//         return;
//     this->scale(scaleFactor, scaleFactor);
		
	// zoom
	const ViewportAnchor anchor = this->transformationAnchor();
	this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	int angle = event->angleDelta().y();
	qreal factor;
	if (angle > 0) 
	{
		factor = 1.1;
		QRectF r = scene.sceneRect();
		this->scene.setSceneRect(r);
	}
	else
	{
		factor = 0.9;
		QRectF r = scene.sceneRect();
		this->scene.setSceneRect(r);
	}
	this->scale(factor, factor);
	this->setTransformationAnchor(anchor);
	
// 	qDebug() << "scene rect" << scene.sceneRect();
// 	qDebug() << "view rect" << rect();

}

// void GraphViewer::mousePressEvent(QMouseEvent* event)
// {
// 	if (event->button() == Qt::LeftButton)
// 	{
//   	pressed_item = scene.itemAt(mapToScene(event->pos()), QTransform());
// 		if(pressed_item != nullptr)
// 			qDebug() << pressed_item->scenePos() << pressed_item << mapToScene(event->pos()) << pressed_item->mapToScene(0,0);
// 	}
// }
// 
// void GraphViewer::mouseMoveEvent(QMouseEvent* event)
// {
// 	if (event->buttons() & Qt::LeftButton && pressed_item != nullptr)
// 	{
//   	pressed_item->setPos(mapToScene(event->pos()));
// 	}
// }


	// wall repulsion. compute distance from node to all walls using QLine2D
// 		QLine2D top_wall(0, 0, scene.width(), 0);
// 		QLine2D bottom_wall(0, scene.height(), scene.width(), scene.height());
// 		QLine2D left_wall(0, 0, 0, scene.height());
// 		QLine2D right_wall(scene.width(), 0, scene.width(), scene.height());
// 		
// 		QVec vpoint = QVec::vec2(px,py);
// 		std::vector<QLine2D> distances_to_walls { top_wall, bottom_wall, left_wall, right_wall };
// 		auto closest_wall = std::min_element( distances_to_walls.begin(), distances_to_walls.end(), 
// 											  [vpoint](auto &lineA, auto &lineB) { return lineA.perpendicularDistanceToPoint(vpoint) 
// 																				    < lineB.perpendicularDistanceToPoint(vpoint); });
// 		QLine2D  perp = closest_wall->getPerpendicularLineThroughPoint(vpoint);
// 		float dist = closest_wall->perpendicularDistanceToPoint(QVec::vec2(px,py));
// 		float wincx = -perp.getDirectionVector().x() * dist;
// 		float wincy = -perp.getDirectionVector().y() * dist;
// 		wincx = std::max(std::min(wincx, (float)scene.width()), (float)200);
// 		wincy = std::max(std::min(wincy, (float)scene.height()), (float)200);
