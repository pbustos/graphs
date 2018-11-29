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
	//THE FOLLOWING IS JUST AN EXAMPLE
	//To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
	try
	{
		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
		std::string innermodel_path = par.value;
		innerModel = new InnerModel(innermodel_path);
	}
	catch(std::exception e) { qFatal("Error reading config params"); }

	return true;
}

void SpecificWorker::initializeFromInnerModel(InnerModel *inner)
{
	walkTree(inner->getRoot());
}

void SpecificWorker::walkTree(InnerModelNode *node)
{
	static uint16_t node_id = 0;
	if (node == nullptr)
		node = innerModel->getRoot();
	
	graph->addNode(node_id);
	auto node_parent = node_id;
	Graph::Attribs atts;
	auto rd = QVec::uniformVector(2,0,3000);
	auto ids = node->id.toStdString();
	atts.insert(std::pair("name", ids));
	atts.insert(std::pair("posx", rd[0]));
	atts.insert(std::pair("posy", rd[1]));
	if(ids == "base")
		atts.insert(std::pair("color", std::string{"coral"}));
	if(ids == "laser")
		atts.insert(std::pair("color", std::string{"darkgreen"}));
	else
		atts.insert(std::pair("color", std::string{"steelblue"}));
		
	graph->addNodeDrawAttribs(node_id, atts);
	node_id++;
	
	for(auto &it : node->children)	
	{	
		graph->addEdge(node_parent, node_id);
		graph->addEdgeDrawAttribs(node_parent, node_id, Graph::Attribs{std::pair("name", std::string{"parentOf"})});
		graph->addEdge(node_id, node_parent);
		graph->addEdgeDrawAttribs(node_id, node_parent, Graph::Attribs{std::pair("name", std::string{"childOf"})});
		walkTree(it);
	}
}

void SpecificWorker::initializeRandom()
{
	std::cout << "Initialize random" << std::endl;
	
	//Crear el grafo
	const int nNodes = 10;
	for(std::uint32_t i=0; i<nNodes; i++)
	
		graph->addNode(i); 
	
	 srand (time(NULL));
	
	for(std::uint32_t i=0; i<nNodes; i++)
	{
		Graph::Attribs atts;
		auto rd = QVec::uniformVector(2,1000,1100);
		atts.insert(std::pair("posx", rd[0]));
		atts.insert(std::pair("posy", rd[1]));
		atts.insert(std::pair("name", std::string("red")));
		graph->addNodeDrawAttribs(i, atts);
	}
	
	for (auto i : iter::range(nNodes)) 
	{
		auto rd = QVec::uniformVector(2,0,nNodes);
		graph->addEdge((int)rd[0], (int)rd[1]);
	}
}

void SpecificWorker::initialize(int period)
{
	graph = std::make_shared<Graph>();
	initializeFromInnerModel(innerModel);
	//initializeRandom();
	graph->print();
	graph_viewer.setGraph(graph, scrollArea);
	graph_viewer.show();	
	//graph_viewer.applyForces(graph);
	
	this->Period = 100;
	timer.start(Period);  
}

void SpecificWorker::compute()
{
	graph_viewer.applyForces(graph);
	graph_viewer.draw();
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
