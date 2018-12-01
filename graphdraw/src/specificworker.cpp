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
	const int nNodes = 2;
	for(std::uint32_t i=0; i<nNodes; i++)
	
		graph->addNode(i); 
	
	 srand (time(NULL));
	
	for(std::uint32_t i=0; i<nNodes; i++)
	{
		Graph::Attribs atts;
		auto rd = QVec::uniformVector(2,-200,200);
		atts.insert(std::pair("posx", rd[0]));
		atts.insert(std::pair("posy", rd[1]));
		atts.insert(std::pair("name", std::string("mynode")));
		atts.insert(std::pair("color", std::string("red")));
		graph->addNodeDrawAttribs(i, atts);
	}
	
	for (auto i : iter::range(nNodes)) 
	{
		auto rd = QVec::uniformVector(2,0,nNodes);
		graph->addEdge(rd[0], rd[1]);
		graph->addEdgeDrawAttribs(rd[0], rd[1], Graph::Attribs{std::pair("name", std::string("edge"))});
	}
}

void SpecificWorker::initializeXML(std::string file_name)
{
	//version = 0;

	// Open file and make initial checks
	xmlDocPtr doc;
	if ((doc = xmlParseFile(file_name.c_str())) == NULL)
	{
		fprintf(stderr,"Can't read XML file - probably a syntax error. \n");
		exit(1);
	}
	xmlNodePtr root;
	if ((root = xmlDocGetRootElement(doc)) == NULL)
	{
		fprintf(stderr,"Can't read XML file - empty document\n");
		xmlFreeDoc(doc);
		exit(1);
	}
	if (xmlStrcmp(root->name, (const xmlChar *) "AGMModel"))
	{
		fprintf(stderr,"Can't read XML file - root node != AGMModel");
		xmlFreeDoc(doc);
		exit(1);
	}

	// Read symbols (just symbols, then links in other loop)
	for (xmlNodePtr cur=root->xmlChildrenNode; cur!=NULL; cur=cur->next)
	{
		if (xmlStrcmp(cur->name, (const xmlChar *)"symbol") == 0)
		{
			xmlChar *stype = xmlGetProp(cur, (const xmlChar *)"type");
			xmlChar *sid = xmlGetProp(cur, (const xmlChar *)"id");
			
			//AGMModelSymbol::SPtr s = newSymbol(atoi((char *)sid), (char *)stype);
			std::uint32_t node_id = std::atoi((char *)sid);
			graph->addNode(node_id);
			graph->addNodeAttribs(node_id, Graph::Attribs{ std::pair("name", std::string((char *)stype))});
			graph->addNodeAttribs(node_id, Graph::Attribs{ std::pair("type", std::string((char *)stype))});
			Graph::Attribs atts;
			auto rd = QVec::uniformVector(2,-200,200);
			atts.insert(std::pair("name", std::string((char *)stype)));
			atts.insert(std::pair("posx", rd[0]));
			atts.insert(std::pair("posy", rd[1]));
			atts.insert(std::pair("color", std::string{"coral"}));
			graph->addNodeDrawAttribs(node_id, atts);
			std::cout << node_id << " " <<  std::string((char *)stype) << std::endl;
			
			xmlFree(sid);
			xmlFree(stype);

			for (xmlNodePtr cur2=cur->xmlChildrenNode; cur2!=NULL; cur2=cur2->next)
			{
				if (xmlStrcmp(cur2->name, (const xmlChar *)"attribute") == 0)
				{
					xmlChar *attr_key   = xmlGetProp(cur2, (const xmlChar *)"key");
					xmlChar *attr_value = xmlGetProp(cur2, (const xmlChar *)"value");
					
					//s->setAttribute(std::string((char *)attr_key), std::string((char *)attr_value));
					graph->addNodeAttribs(node_id, Graph::Attribs{ std::pair(std::string((char *)attr_key), std::string((char *)attr_value))});
					
					
					xmlFree(attr_key);
					xmlFree(attr_value);
				}
				else if (xmlStrcmp(cur2->name, (const xmlChar *)"comment") == 0) { }           // coments are always ignored
				else if (xmlStrcmp(cur2->name, (const xmlChar *)"text") == 0) { }     // we'll ignore 'text'
				else { printf("unexpected tag inside symbol: %s\n", cur2->name); exit(-1); } // unexpected tags make the program exit
			}
		}
		else if (xmlStrcmp(cur->name, (const xmlChar *)"link") == 0) { }     // we'll ignore links in this first loop
		else if (xmlStrcmp(cur->name, (const xmlChar *)"text") == 0) { }     // we'll ignore 'text'
		else if (xmlStrcmp(cur->name, (const xmlChar *)"comment") == 0) { }  // coments are always ignored
		else { printf("unexpected tag #1: %s\n", cur->name); exit(-1); }      // unexpected tags make the program exit
	}

	// Read links
	for (xmlNodePtr cur=root->xmlChildrenNode; cur!=NULL; cur=cur->next)
	{
		if (xmlStrcmp(cur->name, (const xmlChar *)"link") == 0)
		{
			xmlChar *srcn = xmlGetProp(cur, (const xmlChar *)"src");
			if (srcn == NULL) { printf("Link %s lacks of attribute 'src'.\n", (char *)cur->name); exit(-1); }
			int a = atoi((char *)srcn);
			xmlFree(srcn);

			xmlChar *dstn = xmlGetProp(cur, (const xmlChar *)"dst");
			if (dstn == NULL) { printf("Link %s lacks of attribute 'dst'.\n", (char *)cur->name); exit(-1); }
			int b = atoi((char *)dstn);
			xmlFree(dstn);

			xmlChar *label = xmlGetProp(cur, (const xmlChar *)"label");
			if (label == NULL) { printf("Link %s lacks of attribute 'label'.\n", (char *)cur->name); exit(-1); }
			std::string edgeName((char *)label);
			xmlFree(label);

			std::map<std::string, std::string> attrs;
			for (xmlNodePtr cur2=cur->xmlChildrenNode; cur2!=NULL; cur2=cur2->next)
			{
				if (xmlStrcmp(cur2->name, (const xmlChar *)"linkAttribute") == 0)
				{
					xmlChar *attr_key   = xmlGetProp(cur2, (const xmlChar *)"key");
					xmlChar *attr_value = xmlGetProp(cur2, (const xmlChar *)"value");
					attrs[std::string((char *)attr_key)] = std::string((char *)attr_value);
					xmlFree(attr_key);
					xmlFree(attr_value);
				}
				else if (xmlStrcmp(cur2->name, (const xmlChar *)"comment") == 0) { }           // coments are always ignored
				else if (xmlStrcmp(cur2->name, (const xmlChar *)"text") == 0) { }     // we'll ignore 'text'
				else { printf("unexpected tag inside symbol: %s ==> %s\n", cur2->name,xmlGetProp(cur2, (const xmlChar *)"id") ); exit(-1); } // unexpected tags make the program exit
			}
			//qDebug() << a << b;
			graph->addEdge(a, b);
 			graph->addEdgeAttribs(a, b, Graph::Attribs{std::pair("name", edgeName)});
			graph->addEdgeDrawAttribs(a, b, Graph::Attribs{std::pair("name", edgeName)});
 			Graph::Attribs edge_attribs;
 			for(auto &r : attrs)
 				edge_attribs.insert(r);
 			graph->addEdgeAttribs(a, b, edge_attribs);
			
	//	  graph->addEdgeByIdentifiers(a, b, edgeName, attrs);
		}
		else if (xmlStrcmp(cur->name, (const xmlChar *)"symbol") == 0) { }   // symbols are now ignored
		else if (xmlStrcmp(cur->name, (const xmlChar *)"text") == 0) { }     // we'll ignore 'text'
		else if (xmlStrcmp(cur->name, (const xmlChar *)"comment") == 0) { }  // comments are always ignored
		else { printf("unexpected tag #2: %s\n", cur->name); exit(-1); }      // unexpected tags make the program exit
	}
	
}

void SpecificWorker::initialize(int period)
{
	graph = std::make_shared<Graph>();
	//initializeFromInnerModel(innerModel);
	//initializeRandom();
	initializeXML("initialModel_hybrid.xml");
	graph->print();
	
	std::cout << __FILE__ << __FUNCTION__ << "Initializing graphic graph" << std::endl;
	graph_viewer.setGraph(graph, scrollArea);
	std::cout << __FILE__ << __FUNCTION__ << "Graph set OK" << std::endl;
	graph_viewer.show();	
	std::cout << __FILE__ << __FUNCTION__ << "Graph shown OK" << std::endl;
	//graph_viewer.applyForces(graph);
	
	this->Period = 100;
	timer.start(Period);  
	
	std::cout<< __FILE__ << __FUNCTION__ << ":  Initialize OK" << std::endl;
}

void SpecificWorker::compute()
{
// 	graph_viewer.applyForces(graph);
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
