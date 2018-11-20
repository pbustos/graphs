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
	for(std::uint32_t i=0; i<100; i++)
			graph.addNode(i); 
	
	 srand (time(NULL));
	
	for(std::uint32_t i=0; i<100; i++)
	{
		Graph::Attribs atts;
		auto rd = QVec::uniformVector(2,0,4800);
		atts.insert(std::pair("posx", rd[0]));
		atts.insert(std::pair("posy", rd[1]));
		atts.insert(std::pair("color", "red"));
		graph.addNodeAttribs(i, atts);
	}
	
	for (auto i : iter::range(100)) 
	{
		auto rd = QVec::uniformVector(2,0,100);
		graph.addEdge((int)rd[0], (int)rd[1]);
	}
		
	// Pintarlo
	for(auto &[key, value] : graph)
	{
			auto &[atts, neighs] = value;
			
			auto el = scene.addEllipse(std::any_cast<float>(atts["posx"])-200, std::any_cast<float>(atts["posy"])-200, 400, 400, QPen(QBrush(Qt::magenta),50),QBrush(Qt::white));
			el->setZValue(1);
			for( auto &[adj, adjatts] : neighs)
			{
				auto [destAtts, destNeighs] = graph.getNode(adj);
				scene.addLine(std::any_cast<float>(atts["posx"]), std::any_cast<float>(atts["posy"]), 
							  std::any_cast<float>(destAtts["posx"]), std::any_cast<float>(destAtts["posy"]), QPen(QBrush(Qt::blue),50));
			}
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
	
 	view.show();
}

void SpecificWorker::compute()
{

}

//Computes repelling forces among nodes
void SpecificWorker::graph_forces(const Graph &g)
{
	
	
}
