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

void GraphViewer::setGraph(std::shared_ptr<Graph> graph_, QScrollArea *scrollArea)
{
	this->graph = graph_;
	
	scene.setSceneRect(0, 0, 3000, 3000);
	this->setScene(&scene);
	//scrollArea->setWidgetResizable(true);
	this->setParent(scrollArea);
	scrollArea->setWidget(this);
	this->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	setMinimumSize(500,500);
	this->fitInView(scene.sceneRect(), Qt::KeepAspectRatio );
	adjustSize();
	QRect availableGeometry(QApplication::desktop()->availableGeometry());
	move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
	
	for(auto &[node_id, node_value] : *graph)
		{
				auto &[node_atts, node_fanout, node_fanin] = node_value;
				float node_posx = std::any_cast<float>(node_atts["posx"]);
				float node_posy = std::any_cast<float>(node_atts["posy"]);
				//add circle
				auto ellipse = scene.addEllipse(node_posx-200, node_posy-200, 400, 400, QPen(), QBrush(Qt::cyan));
				ellipse->setZValue(1);
				// node tag
				QString qname = QString::fromStdString(std::any_cast<std::string>(node_atts["name"]));
				auto node_tag = new QGraphicsSimpleTextItem(QString::number(node_id) + " " + qname, ellipse);
				node_tag->setX(node_posx-80);
				node_tag->setY(node_posy-80);
				node_tag->setScale(8);
				node_atts["ellipse"] = ellipse;
				node_atts["ellipse_text"] = node_tag;
				for( auto &[node_adj, edge_atts] : node_fanout)
				{
					auto [node_dest_atts, node_dest_fanout, node_dest_fanin] = graph->node(node_adj);
					auto qline = QLine(node_posx, node_posy, 
														std::any_cast<float>(node_dest_atts["posx"]), std::any_cast<float>(node_dest_atts["posy"]));
					auto line = scene.addLine(qline, QPen(QBrush(Qt::blue), 10));
					edge_atts["edge_line"] = line;
					// edge tags
					auto edge_tag = new QGraphicsSimpleTextItem(QString::number(node_id) + "->" + QString::number(node_adj), line);
					edge_tag->setX(qline.center().x());	edge_tag->setY(qline.center().y());
					edge_tag->setScale(8);
					edge_atts["edge_line_text"] = edge_tag;
				}
		}
}

void GraphViewer::draw()
{
	//std::cout << this->width() << " " << this->height() << std::endl;
	this->fitInView(scene.sceneRect(), Qt::KeepAspectRatio );
	show();
}

void GraphViewer::applyForces(std::shared_ptr<Graph> g)
{
	// natrix of 2D points from graph nodes
	Eigen::MatrixXf M = Eigen::MatrixXf(2, graph->size());
	for( auto &[key, value] : *graph)
	{
		auto &[atts, fanout, fanin] = value; 
		M(0,key) = std::any_cast<float>(atts["posx"]);
		M(1,key) = std::any_cast<float>(atts["posy"]);
	}
	
	// create a kd-tree for M, note that M must stay valid during the lifetime of the kd-tree
	Nabo::NNSearchF* nns = Nabo::NNSearchF::createKDTreeLinearHeap(M);
	
	// query 5 closest points
	const int K = 5;
	Eigen::MatrixXi indices(K, M.cols());
	Eigen::MatrixXf dists2(K, M.cols());
	nns->knn(M, indices, dists2, K, 0, Nabo::NNSearchF::SORT_RESULTS );
	
	// compute result form repelling nodes
	std::int32_t rincx, rincy, aincx, aincy;
	for (auto i : iter::range(indices.cols()))
	{
        auto &node = graph->node(i);
        auto &node_atts = graph->attrs(node);
				rincx=0; rincy=0;
		//compute repulsive forces
		for (auto j : iter::range(indices.rows()))
		{
				rincx += ( M(0,i) - M(0,indices(j,i)));		
				rincy += ( M(1,i) - M(1,indices(j,i)));
		}
		//compute attractive forces from outgoing edges  
		aincx = 0; aincy = 0;
		for( auto &edges : graph->fanout(node))
		{
			auto &[rnode, eatts] = edges;
			auto &dist_atts = graph->attrs(graph->node(rnode));
			aincx += (graph->attr<float>(dist_atts["posx"]) - graph->attr<float>(node_atts["posx"]));
			aincy += (graph->attr<float>(dist_atts["posy"]) - graph->attr<float>(node_atts["posy"]));
		}  
		
		//weighted sum
		float incx = 0.1*rincx + 0.3*aincx;
		float incy = 0.1*rincy + 0.3*aincy;
		
    //check threshold
    if(incx > 10) incx = 10;
		if(incx < -10) incx = -10;
    if(incy > 10) incy = 10;
		if(incy < -10) incy = -10;
		
		float rx = std::any_cast<float>(node_atts["posx"]) + incx;
		float ry = std::any_cast<float>(node_atts["posy"]) + incy;
        
    //check bounds
		if( rx > this->width()) rx = this->width(); 
		if( rx < 0) rx = 0;
		if( ry > this->height()) ry = this->height(); 
		if( ry < 0) ry = 0;
		
		//update node coors
    node_atts.insert_or_assign("posx", rx);
		node_atts.insert_or_assign("posy", ry);
		
		//move circle and text
		auto ellipse_ptr = std::any_cast<QGraphicsEllipseItem *>(node_atts["ellipse"]);
		auto text_ellipse_ptr = std::any_cast<QGraphicsSimpleTextItem *>(node_atts["ellipse_text"]);
		ellipse_ptr->setRect(rx-100,ry-100,200,200);	
		text_ellipse_ptr->setX(rx-80);
		text_ellipse_ptr->setY(ry-80);
	}
	
	//move line edges after finishing nodes
	for(auto &[node_key, node_value] : *graph)
	{
		auto &[node_atts, node_fanout, node_fanin] = node_value;
		for( auto &[node_dest, edge_atts] : node_fanout)
		{
			auto &node_dest_atts = graph->attrs(graph->node(node_dest));
			auto line = std::any_cast<QGraphicsLineItem *>(edge_atts["edge_line"]);
			QLine qline(std::any_cast<float>(node_atts["posx"]), std::any_cast<float>(node_atts["posy"]),	
									std::any_cast<float>(node_dest_atts["posx"]), std::any_cast<float>(node_dest_atts["posy"]));
			line->setLine(qline);
			auto line_text = std::any_cast<QGraphicsSimpleTextItem *>(edge_atts["edge_line_text"]);
			line_text->setX(qline.center().x()-80);
			line_text->setY(qline.center().y()-80);
		}
	}
	delete nns;	
	
}
void GraphViewer::applyForces2(std::shared_ptr<Graph> g)
{
	// compute forces form current edge lenghts
// 	std::int32_t rincx=0, rincy=0, aincx=0, aincy=0;
// 	std::int32_t natural_length = 200;
// 	for( auto &[node_key, node_value] : *graph)
// 	{
// 		auto &node_atts = graph->attrs(node_value);
// 		float px = graph->attr<float>(node_atts["posx"]);
// 		float py = graph->attr<float>(node_atts["posy"]);
// 		for( auto &edges : graph->fanout(node_value))
// 		{
// 			auto &[node_dest, edge_atts] = edges;
// 			auto &node_dist_atts = graph->attrs(graph->node(node_key));
// 			float rpx = graph->attr<float>(node_dist_atts["posx"]);
// 			float rpy = graph->attr<float>(node_dist_atts["posy"]);
// 			float sqrdist = ((px-rpx)*(px-rpx) + (py -rpy)*(py-rpy)) - natural_length;
// 			if( sqrdist > 0) 
// 			{
// 				resx += rpx - px;
// 				resy += rpy - pxy;
// 			}
// 			else
// 			{
// 				resx += px - rpx;
// 				resy += rpy - py;
// 			}
// 		}  
// 		
// 		//weighted sum
// 		float incx = 0.1*rincx + 0.3*aincx;
// 		float incy = 0.1*rincy + 0.3*aincy;
// 		
//     //check threshold
//     if(incx > 10) incx = 10;
//     if(incy > 10) incy = 10;
//         
// 		float rx = std::any_cast<float>(node_atts["posx"]) + incx;
// 		float ry = std::any_cast<float>(node_atts["posy"]) + incy;
//         
//     //check bounds
// 		if( rx > 3000) rx = 3000; 
// 		if( rx < 0) rx = 0;
// 		if( ry > 3000) rx = 3000; 
// 		if( ry < 0) ry = 0;
// 		
// 		//update node coors
//     node_atts.insert_or_assign("posx", rx);
// 		node_atts.insert_or_assign("posy", ry);
// 		
// 		//move circle and text
// 		auto ellipse_ptr = std::any_cast<QGraphicsEllipseItem *>(node_atts["ellipse"]);
// 		auto text_ellipse_ptr = std::any_cast<QGraphicsSimpleTextItem *>(node_atts["ellipse_text"]);
// 		ellipse_ptr->setRect(rx-100,ry-100,200,200);	
// 		text_ellipse_ptr->setX(rx-80);
// 		text_ellipse_ptr->setY(ry-80);
// 	}
// 	
// 	//move line edges after finishing nodes
// 	for(auto &[node_key, node_value] : *graph)
// 	{
// 		auto &[node_atts, node_fanout, node_fanin] = node_value;
// 		for( auto &[node_dest, edge_atts] : node_fanout)
// 		{
// 			auto &node_dest_atts = graph->attrs(graph->node(node_dest));
// 			auto line = std::any_cast<QGraphicsLineItem *>(edge_atts["edge_line"]);
// 			QLine qline(std::any_cast<float>(node_atts["posx"]), std::any_cast<float>(node_atts["posy"]),	
// 									std::any_cast<float>(node_dest_atts["posx"]), std::any_cast<float>(node_dest_atts["posy"]));
// 			line->setLine(qline);
// 			auto line_text = std::any_cast<QGraphicsSimpleTextItem *>(edge_atts["edge_line_text"]);
// 			line_text->setX(qline.center().x()-80);
// 			line_text->setY(qline.center().y()-80);
// 		}
// 	}
}
