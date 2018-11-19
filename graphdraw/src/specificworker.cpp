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
	//view.scale(1, -1);
	view.setParent(scrollArea);
	view.setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	view.fitInView(scene.sceneRect(), Qt::KeepAspectRatio );

	qDebug() << __FILE__ << __FUNCTION__ <<  __cplusplus ;

	//Crear el grafo
	for(std::uint32_t i=0; i<10; i++)
			graph.addNode(i); 
	
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
	
	for(std::uint32_t i=0; i<10; i++)
	{
		Graph::Attribs atts;
		atts.insert(std::pair("posx", 10. + qrand() % ((4000 + 1) - 1) + 1 ));
		atts.insert(std::pair("posy", 10. + qrand() % ((4000 + 1) - 1) + 1 ));
		atts.insert(std::pair("color", "red"));
		graph.addNodeAttribs(i, atts);
	}
	
	// Pintarlo
	for(auto &[key, value] : graph)
	{
			auto &[atts, neighs] = value;
			std::cout << std::any_cast<double>(atts["posx"]) << " " << std::any_cast<double>(atts["posy"]) << std::endl;
			scene.addEllipse(std::any_cast<double>(atts["posx"]), std::any_cast<double>(atts["posy"]), 200, 200, QPen(Qt::magenta), QBrush(Qt::magenta));
	}
	view.show();
}

void SpecificWorker::compute()
{

}
