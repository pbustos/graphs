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
#include <QGLViewer/qglviewer.h>
#include <unordered_map>
#include <any>
#include <QGraphicsEllipseItem>
#include <string>
#include <memory>
#include <cppitertools/range.hpp>
#include <nabo/nabo.h>
#include "graph.h"
#include "graphviewer.h"
/*
	template<typename T>
	auto inline begin(std::shared_ptr<T> ptr) -> typename T::iterator { return ptr->begin();	}
	template<typename T>
	auto inline end(std::shared_ptr<T> ptr) -> typename T::iterator { return ptr->end(); }*/

// class Graph 
// {
// 	public:
// 		using Attribs = std::unordered_map<std::string, std::any>;
// 		using Neighs = std::unordered_map<std::uint32_t, Attribs>;
// 		using Value = std::tuple<Attribs, Neighs>;
// 		using Nodes = std::unordered_map<std::uint32_t, Value>;
// 		
// 		typename Nodes::iterator begin() 												{ return nodes.begin(); };
// 		typename Nodes::iterator end() 													{ return nodes.end();   };
// 		typename Nodes::const_iterator begin() const  					{ return nodes.begin(); };
// 		typename Nodes::const_iterator end() const 	 						{ return nodes.begin(); };
// 		size_t size() const 																		{ return nodes.size();  };
// 		void addNode(std::uint32_t id) 													{ Value v; nodes.insert(std::pair(id, v));};
// 		void addEdge(std::uint32_t from, std::uint32_t to) 			{ Attribs a; std::get<1>(nodes[from]).insert(std::pair(to,a));};
// 		void addNodeAttribs(std::uint32_t id, const Attribs &att) 									
// 		{ 
// 			for(auto &[k,v] : att)
// 				std::get<0>(nodes[id]).insert_or_assign(k,v);
// 		};
// 		void addEdgeAttribs(std::uint32_t from, std::uint32_t to, const Attribs &att)
// 		{ 
// 				auto &edgeAtts = std::get<1>(nodes[from]).at(to);
// 				for(auto &[k,v] : att)
// 					edgeAtts.insert_or_assign(k,v);
// 		};
//         //helpers
// 		Value node(std::uint32_t id) const		{ return nodes.at(id); };
// 		Value& node(std::uint32_t id)  	 			{ return nodes.at(id); };
// 		Neighs edges(const Value &v) const    { return std::get<1>(v);};
//     Neighs& edges(Value &v)               { return std::get<1>(v);};
// 		Attribs attrs(const Value &v) const   { return std::get<0>(v);};
//     Attribs& attrs(Value &v)              { return std::get<0>(v);};
//     template<typename T>
//       T attr(const std::any &s) const     { return std::any_cast<T>(s);};
// 		
// 	private:
// 		Nodes nodes;
// };

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

private:
	InnerModel *innerModel;
	
	//QGraphicsScene scene;
	//QGraphicsView view;
	//QGraphicsEllipseItem *node;

	std::shared_ptr<Graph> graph;
	GraphViewer graph_viewer;
	
	//void graph_forces(Graph &);
	

	
};

#endif
