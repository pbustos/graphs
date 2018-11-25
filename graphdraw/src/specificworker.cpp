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
#include "specificworker.h"

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(MapPrx& mprx) : GenericWorker(mprx)
{

}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	std::cout << "Destroying SpecificWorker" << std::endl;
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
//       THE FOLLOWING IS JUST AN EXAMPLE
//	To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
//	try
//	{
//		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
//		std::string innermodel_path = par.value;
//		innerModel = new InnerModel(innermodel_path);
//	}
//	catch(std::exception e) { qFatal("Error reading config params"); }

	return true;
}

void SpecificWorker::initialize(int period)
{
	std::cout << "Initialize worker" << std::endl;

// 	scene.setSceneRect(0, 0, 3000, 3000);
// 	view.setScene(&scene);
// 	//scrollArea->setWidgetResizable(true);
// 	view.setParent(scrollArea);
// 	scrollArea->setWidget(&view);
// 	view.setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
// 	view.fitInView(scene.sceneRect(), Qt::KeepAspectRatio );
// 	setMinimumSize(500,500);
// 	adjustSize();
// 	QRect availableGeometry(QApplication::desktop()->availableGeometry());
// 	move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
	
	graph = std::make_shared<Graph>();
	
	//Crear el grafo
	const int nNodes = 10;
	for(std::uint32_t i=0; i<nNodes; i++)
			graph->addNode(i); 
	
	 srand (time(NULL));
	
	for(std::uint32_t i=0; i<nNodes; i++)
	{
		Graph::Attribs atts;
		auto rd = QVec::uniformVector(2,0,3000);
		atts.insert(std::pair("posx", rd[0]));
		atts.insert(std::pair("posy", rd[1]));
		atts.insert(std::pair("color", "red"));
		graph->addNodeAttribs(i, atts);
	}
	
	for (auto i : iter::range(nNodes)) 
	{
		auto rd = QVec::uniformVector(2,0,nNodes);
		graph->addEdge((int)rd[0], (int)rd[1]);
	}
	
	graph_viewer.setGraph(graph, scrollArea);
	// Pintarlo
// 	for(auto &[node_id, node_value] : *graph)
// 	{
// 			auto &[node_atts, node_neighs] = node_value;
// 			float node_posx = std::any_cast<float>(node_atts["posx"]);
// 			float node_posy = std::any_cast<float>(node_atts["posy"]);
// 			//add circle
// 			auto ellipse = scene.addEllipse(node_posx-200, node_posy-200, 400, 400, QPen(), QBrush(Qt::cyan));
// 			ellipse->setZValue(1);
// 			// node tag
// 			auto node_tag = new QGraphicsSimpleTextItem(QString::number(node_id), ellipse);
// 			node_tag->setX(node_posx-80);
// 			node_tag->setY(node_posy-80);
// 			node_tag->setScale(8);
// 			node_atts["ellipse"] = ellipse;
// 			node_atts["ellipse_text"] = node_tag;
// 			for( auto &[node_adj, edge_atts] : node_neighs)
// 			{
// 				auto [node_dest_atts, node_dest_neighs] = graph->node(node_adj);
// 				auto qline = QLine(node_posx, node_posy, 
// 													 std::any_cast<float>(node_dest_atts["posx"]), std::any_cast<float>(node_dest_atts["posy"]));
// 				auto line = scene.addLine(qline, QPen(QBrush(Qt::blue), 10));
// 				edge_atts["edge_line"] = line;
// 				// edge tags
// 				auto edge_tag = new QGraphicsSimpleTextItem(QString::number(node_id) + "->" + QString::number(node_adj), line);
// 				edge_tag->setX(qline.center().x());	edge_tag->setY(qline.center().y());
// 				edge_tag->setScale(8);
// 				edge_atts["edge_line_text"] = edge_tag;
// 			}
// 	}
	//graph_forces(graph);
 	graph_viewer.show();	
	this->Period = 200;
	timer.start(Period);  
}

void SpecificWorker::compute()
{
  //	graph_forces(graph);
  	graph_viewer.show();
}
	
//Computes repelling forces among nodes
// void SpecificWorker::graph_forces(Graph &g)
// {
// 	// points in 2D fron graph
// 	Eigen::MatrixXf M = Eigen::MatrixXf(2, graph->size());
// 	for( auto &[key, value] : *graph)
// 	{
// 		auto &[atts, neighs] = value; 
// 		M(0,key) = std::any_cast<float>(atts["posx"]);
// 		M(1,key) = std::any_cast<float>(atts["posy"]);
// 	}
// 	
// 	// create a kd-tree for M, note that M must stay valid during the lifetime of the kd-tree
// 	Nabo::NNSearchF* nns = Nabo::NNSearchF::createKDTreeLinearHeap(M);
// 	
// 	// query 5 closest points
// 	const int K = 5;
// 	Eigen::MatrixXi indices(K, M.cols());
// 	Eigen::MatrixXf dists2(K, M.cols());
// 	nns->knn(M, indices, dists2, K, 0, Nabo::NNSearchF::SORT_RESULTS );
// 	
// 	std::int32_t rincx, rincy, aincx, aincy;
// 	for (auto i : iter::range(indices.cols()))
// 	{
//         auto &node = graph->node(i);
//         auto &node_atts = graph->attrs(node);
// 				rincx=0; rincy=0;
// 		//compute repulsive forces
// 		for (auto j : iter::range(indices.rows()))
// 		{
// 				rincx += ( M(0,i) - M(0,indices(j,i)));		
// 				rincy += ( M(1,i) - M(1,indices(j,i)));
// 		}
// 		//compute attractive forces from outgoing edges
// 		aincx = 0; aincy = 0;
// 		for( auto &edges : graph->edges(node))
// 		{
// 			auto &[rnode, eatts] = edges;
// 			auto &dist_atts = graph->attrs(graph->node(rnode));
// 			aincx += (graph->attr<float>(dist_atts["posx"]) - graph->attr<float>(node_atts["posx"]));
// 			aincy += (graph->attr<float>(dist_atts["posy"]) - graph->attr<float>(node_atts["posy"]));
// 		}  
// 		
// 		//weighted sum
// 		float incx = 0.1*rincx + 0.1*rincy;
// 		float incy = 0.1*rincy + 0.1*rincy;
// 		
//     //check threshold
//     if(incx > 10000) incx = 10000;
//     if(incy > 10000) incy = 10000;
//         
// 		float rx = std::any_cast<float>(node_atts["posx"]) + incx;
// 		float ry = std::any_cast<float>(node_atts["posy"]) + incy;
//         
//     //check bounds
// 		
// 		//move node
//     node_atts.insert_or_assign("posx", rx);
// 		node_atts.insert_or_assign("posy", ry);
// 		auto ellipse_ptr = std::any_cast<QGraphicsEllipseItem *>(node_atts["ellipse"]);
// 		auto text_ellipse_ptr = std::any_cast<QGraphicsSimpleTextItem *>(node_atts["ellipse_text"]);
// 		ellipse_ptr->setRect(rx-100,ry-100,200,200);	
// 		text_ellipse_ptr->setX(rx-80);
// 		text_ellipse_ptr->setY(ry-80);
// 		
// 		//move line edges
// 		auto &neighs = std::get<1>(graph->node(i));
// 		for( auto &[node_adj, edge_atts] : neighs)
// 		{
// 			auto &[node_dest_atts, node_dest_neighs] = graph->node(node_adj);
// 			auto line = std::any_cast<QGraphicsLineItem *>(edge_atts["edge_line"]);
// 			QLine qline(std::any_cast<float>(node_atts["posx"]), std::any_cast<float>(node_atts["posy"]), 
// 									std::any_cast<float>(node_dest_atts["posx"]), std::any_cast<float>(node_dest_atts["posy"]));
// 			line->setLine(qline);
// 			auto line_text = std::any_cast<QGraphicsSimpleTextItem *>(edge_atts["edge_line_text"]);
// 			line_text->setX(qline.center().x()-80);
// 			line_text->setY(qline.center().y()-80);
// 			
// 		}
// 	}
// 	delete nns;	
// 	
// }

// 	for(auto &[key, node] : graph)
// 	{
// 		auto &neighs = node.neighboors();
// 		auto &attribs = node.attributes();
// 		
// 		auto px = attribs.getPosX();
// 		auto py = attribs.getPosY();
// 		
// 		auto el = scene.addEllipse(px-200, py-200, 400, 400, QPen(QBrush(Qt::magenta),50),QBrush(Qt::white));
// 		el->setZValue(1);
// 	}
// 
// 	for(auto &[from, to] : graph->edges())
// 	{
// 		auto &fromAttribs = from.attributes()["posx"];
// 		auto &toAttribs = to.attributes();
// 		
// 		auto &fx = fromAttribs.getPosX();
// 		auto &fy = fromAttribs.getPosY();
// 		auto &tx = toAttribs.getPosX();
// 		auto &ty = toAttribs.getPosY();
// 		
// 		scene.addLine(fx, fy, tx, ty, QPen(QBrush(Qt::blue),50));
// 	}
