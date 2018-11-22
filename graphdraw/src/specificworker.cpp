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
	this->Period = period;
	timer.start(Period);
	
	scene.setSceneRect(0, 0, 5000, 5000);
	view.setScene(&scene);
	scrollArea->setWidgetResizable(true);
	view.setParent(scrollArea);
	scrollArea->setWidget(&view);
	//view.setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	view.fitInView(scene.sceneRect(), Qt::KeepAspectRatio );

	qDebug() << __FILE__ << __FUNCTION__ <<  __cplusplus ;

	//Crear el grafo
	const int nNodes = 10;
	for(std::uint32_t i=0; i<nNodes; i++)
			graph.addNode(i); 
	
	 srand (time(NULL));
	
	for(std::uint32_t i=0; i<nNodes; i++)
	{
		Graph::Attribs atts;
		auto rd = QVec::uniformVector(2,0,4800);
		atts.insert(std::pair("posx", rd[0]));
		atts.insert(std::pair("posy", rd[1]));
		atts.insert(std::pair("color", "red"));
		graph.addNodeAttribs(i, atts);
	}
	
	for (auto i : iter::range(nNodes)) 
	{
		auto rd = QVec::uniformVector(2,0,nNodes);
		graph.addEdge((int)rd[0], (int)rd[1]);
	}
		
	// Pintarlo
	for(auto &[key, value] : graph)
	{
			auto &[atts, neighs] = value;
			
			auto el = scene.addEllipse(std::any_cast<float>(atts["posx"])-100, std::any_cast<float>(atts["posy"])-100, 200, 200, QPen(QBrush(Qt::magenta),30),QBrush(Qt::white));
			el->setZValue(1);
			atts["ellipse"] = el;
			for( auto &[adj, adjatts] : neighs)
			{
				auto [destAtts, destNeighs] = graph.getNode(adj);
				auto line = scene.addLine(std::any_cast<float>(atts["posx"]), std::any_cast<float>(atts["posy"]), 
							  std::any_cast<float>(destAtts["posx"]), std::any_cast<float>(destAtts["posy"]), QPen(QBrush(Qt::blue),50));
				adjatts["edgeline"] = line;
			}
	}
	
	graph_forces(graph);
 	view.show();
	graph_forces(graph);
}

void SpecificWorker::compute()
{
 	graph_forces(graph);
 	view.show();
}
	
//Computes repelling forces among nodes
void SpecificWorker::graph_forces(Graph &g)
{
	// points in 2D fron graph
	Eigen::MatrixXf M = Eigen::MatrixXf(2, graph.size());
	for( auto &[key, value] : graph)
	{
		auto &[atts, neighs] = value; 
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
	
	std::int32_t incx, incy;
	for (auto i : iter::range(indices.cols()))
	{
		incx=0; incy=0;
		for (auto j : iter::range(indices.rows()))
		{
			incx += M(0,i) - M(0,indices(j,i));		
			incy += M(1,i) - M(1,indices(j,i));
			
			//check threshold
			if(incx > 10000) incx = 10000;
			if(incy > 10000) incy = 10000;
			
			//check borders
		}
		//Move
		auto &atts = std::get<0>(graph.getNode(i));
		float rx = std::any_cast<float>(atts["posx"]) + (incx/100.);
		float ry = std::any_cast<float>(atts["posy"]) + (incy/100.);
		std::cout << rx << " " << ry << std::endl;
		atts.insert_or_assign("posx", rx);
		atts.insert_or_assign("posy", ry);
		std::any_cast<QGraphicsEllipseItem *>(atts["ellipse"])->setRect(rx-100,ry-100,200,200);	
	}
	
	std::cout << "NEW-----------------------" << std::endl;
// 	std::cout << indices << std::endl;
// 	std::cout << "----------" << std::endl;
// 	std::cout << dists2 << std::endl;
// 	std::cout << "----------" << std::endl;
// 	std::cout << M << std::endl;
// 	std::cout << "----------" << std::endl;
// 	
	
	
	delete nns;	
	
}

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
// 	for(auto &[from, to] : graph.edges())
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
