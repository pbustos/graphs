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

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <unordered_map>
#include <any>
#include <memory>
#include <vector>
#include <variant>
#include <qmat/QMatAll>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;


namespace DSR
{
	using MTypes = std::variant<std::uint32_t, std::int32_t, float, std::string, std::vector<float>, RMat::RTMat>;		
	using Attribs = std::unordered_map<std::string, MTypes>;
	using DrawAttribs = std::unordered_map<std::string, std::any>;
	struct EdgeAttrs
	{ 
		Attribs attrs; 
		DrawAttribs draw_attrs;
	};
	using FanOut = std::unordered_map<std::uint32_t, EdgeAttrs>;
	using FanIn = std::vector<std::uint32_t>;

	class Graph 
	{
		public:
			 struct Value
			{
				Attribs attrs;
				DrawAttribs draw_attrs;
				FanOut fanout;
				FanIn fanin;
			}; 
			using Nodes = std::unordered_map<std::uint32_t, Value>;
			
			////////////////////////////////////////////////////////////////////////////////////////////
			//									Graph API 
			////////////////////////////////////////////////////////////////////////////////////////////
			
			typename Nodes::iterator begin() 					{ return nodes.begin(); };
			typename Nodes::iterator end() 						{ return nodes.end();   };
			typename Nodes::const_iterator begin() const  		{ return nodes.begin(); };
			typename Nodes::const_iterator end() const 	 		{ return nodes.begin(); };
			
			size_t size() const 								{ return nodes.size();  };
			void addNode(std::uint32_t id) 						{ nodes.insert(std::pair(id, Value()));};
			void addEdge(std::uint32_t from, std::uint32_t to) 			
			{ 
				nodes[from].fanout.insert(std::pair(to, EdgeAttrs()));
				nodes[to].fanin.push_back(from);
			};
			void addNodeAttribs(std::uint32_t id, const Attribs &att)
			{ 
				for(auto &[k,v] : att)
					nodes[id].attrs.insert_or_assign(k,v);
			};
			void addNodeDrawAttribs(std::uint32_t id, const DrawAttribs &att)
			{ 
				for(auto &[k,v] : att)
					nodes[id].draw_attrs.insert_or_assign(k,v);
			};
			void addEdgeAttribs(std::uint32_t from, std::uint32_t to, const Attribs &att)
			{ 
					auto &edgeAtts = nodes[from].fanout.at(to);
					for(auto &[k,v] : att)
						edgeAtts.attrs.insert_or_assign(k,v);
			};
			void addEdgeDrawAttribs(std::uint32_t from, std::uint32_t to, const DrawAttribs &att)
			{ 
					auto &edgeAtts = nodes[from].fanout.at(to);
					for(auto &[k,v] : att)
						edgeAtts.draw_attrs.insert_or_assign(k,v);
			};

			std::string printVisitor(const MTypes &t)
			{			
				return std::visit(overload
				{
    				[](RTMat m) -> std::string				{ return m.asQString("RT").toStdString(); },
    				[](std::vector<float> a)-> std::string	
					{ 
						std::string str;
						for(auto &f : a)
							str += std::to_string(f) + " ";
						return  str += "\n"; 
					},
					[](std::string a) -> std::string		{ return  a; },
					[](auto a) -> std::string				{ return std::to_string(a);} 
				}, t);
    		};

			void print()
			{
				std::cout << "------------Printing Graph: " << nodes.size() << " elements -------------------------" << std::endl;
				for( auto &[k,v] : nodes)
				{
					std::cout << "[" << attr<std::string>(v.draw_attrs["name"]) << "] : " << std::endl;
					std::cout << "	attrs:	";
					for( auto &[ka, kv] : v.attrs)
					{
						std::cout << ka << " -> " << printVisitor(kv)  << " , ";
					}
					std::cout << std::endl << "	edges:	";
					for( auto &[kf,vf] : v.fanout)
					{
						std::cout << printVisitor(vf.attrs["name"]) << "( " << printVisitor(nodes[kf].attrs["name"]) << " ) " << std::endl;
						std::cout << "			edge attrs: ";
						for( auto &[ke, ve] : vf.attrs)
						{
							std::cout << ke << " -> " << printVisitor(ve) << " , ";
						}
						std::cout << std::endl << "		";
					}
					std::cout << std::endl;
				}
				std::cout << "---------------- graph ends here --------------------------" << std::endl;
			}
			
		  
			//Value node(std::uint32_t id) const			{ return nodes.at(id); };
			//Value& node(std::uint32_t id)  	 				{ return nodes.at(id); };
			FanOut fanout(std::uint32_t id) const   			{ return nodes.at(id).fanout;};
			FanOut& fanout(std::uint32_t id)            		{ return nodes.at(id).fanout;};
			FanIn fanin(std::uint32_t id) const    				{ return nodes.at(id).fanin;};
			FanIn& fanin(std::uint32_t id)                		{ return nodes.at(id).fanin;};
			template <typename Ta> 
			Ta edgeAttrib(std::uint32_t from, std::uint32_t to, const std::string &tag) const 	
			{ 	auto &attrs = nodes.at(from).fanout.at(to).attrs;
				if(attrs.count(tag) > 0)
					return std::get<Ta>(attrs.at(tag));
				else return Ta();
			};
			Attribs attrs(std::uint32_t id) const  	 			{ return nodes.at(id).attrs;};
			Attribs& attrs(std::uint32_t id)       				{ return nodes.at(id).attrs;};
			DrawAttribs nodeDrawAttrs(std::uint32_t id) const	{ return nodes.at(id).draw_attrs;};
			DrawAttribs& nodeDrawAttrs(std::uint32_t id)      	{ return nodes.at(id).draw_attrs;};
			Attribs& edgeAttrs(std::uint32_t from, std::uint32_t to) 		{ return nodes.at(from).fanout.at(to).attrs;};
			Attribs edgeAttrs(std::uint32_t from,  std::uint32_t to) const	{ return nodes.at(from).fanout.at(to).attrs;};
			
			template<typename Ta>
			Ta attr(const std::any &s) const     			{ return std::any_cast<Ta>(s);};

			template<typename Ta>
			Ta attr(const MTypes &s) const    	 			{ return std::get<Ta>(s);};
			
			bool nodeExists(std::uint32_t id) const			{ return nodes.count(id) > 0;}
			template<typename Ta>
			bool nodeHasAttrib(std::uint32_t id, const std::string &key, const std::string &value) const 
			{  
				auto &ats = nodes.at(id).attrs;
				return (ats.count(key) > 0) and (this->attr<Ta>(ats.at(key))==value);
			};
			std::vector<std::uint32_t> edgesByLabel(std::uint32_t id, const std::string &tag) 	
			{ 
				std::vector<std::uint32_t> keys;
				for(auto &[k, v] : nodes.at(id).fanout)
					if( attr<std::string>(v.attrs["name"]) == tag )
						keys.push_back(k);
				return keys;
    		};
			
			///////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 													InnerModel API
			///////////////////////////////////////////////////////////////////////////////////////////////////////////

			// ///////////////////////////////////////////////////
			// /// Tree update methods
			// ///////////////////////////////////////////////////
			// void setRoot(InnerModelNode *node);
			// void cleanupTables();
			void updateTransformValues(const std::uint32_t &transformId, float tx, float ty, float tz, float rx, float ry, float rz, const std::uint32_t &parentId = 0);
			void innerModelTreeWalk(std::uint32_t id);

			// ////////////////////////////////////////////
			// /// Transformation matrix retrieval methods
			// ///////////////////////////////////////////
			using ABLists = std::tuple< std::list<std::uint32_t>, std::list<std::uint32_t> >;

			ABLists setLists(const std::uint32_t &origId, const std::uint32_t &destId);
			RMat::RTMat getTransformationMatrix(const std::uint32_t &to, const std::uint32_t &from);
			RMat::QVec transform(const std::uint32_t &destId, const QVec &initVec, const std::uint32_t &origId);
			// RTMat getTransformationMatrix(const QString &destId, const QString &origId);
			// RTMat getTransformationMatrixS(const std::string &destId, const std::string &origId);
			// QMat getRotationMatrixTo(const QString &to, const QString &from);
			// QVec getTranslationVectorTo(const QString &to, const QString &from);
			// QVec rotationAngles(const QString & destId, const QString & origId);
			
			// ////////////////////////////////////////////
			// /// Editing
			// ///////////////////////////////////////////
			// template <class N> N* getNode(const std::string &id) const
			// void removeSubTree(InnerModelNode *item, QStringList *l);
			// void removeNode(const QString & id);
			// void moveSubTree(InnerModelNode *nodeSrc, InnerModelNode *nodeDst);
			// void getSubTree(InnerModelNode *node, QStringList *l);
			// void getSubTree(InnerModelNode *node, QList<InnerModelNode *> *l);
			// void computeLevels(InnerModelNode *node);
			// InnerModelNode *getRoot() { return root; }
			// QString getParentIdentifier(QString id);
			// std::string getParentIdentifierS(std::string id);
			std::uint32_t getNodeLevel(std::uint32_t id)  	{ return std::get<std::uint32_t>(nodes.at(id).attrs["level"]); };
			std::uint32_t getParent(std::uint32_t id)   	{ return std::get<std::uint32_t>(nodes.at(id).attrs["parent"]); };


			// ////////////////////////////
			// // FCL related
			// ////////////////////////////
			// bool collidable(const QString &a);
			// bool collide(const QString &a, const QString &b);
			// float distance(const QString &a, const QString &b);

			// ////////////////////////////
			// // Jacobians
			// ////////////////////////////
			// QMat jacobian(QStringList &listaJoints, const QVec &motores, const QString &endEffector);

		private:
			Nodes nodes;
			// For InnerModel
	};
}
#endif // GRAPH_H




//FUSCA
	// template <typename Ta> 
	// 		Ta attribFanin(std::uint32_t to, const std::string &tag) const 	
	// 		{ 	auto &fin = nodes.at(to).fanin;
	// 			auto edge = std::find_if(fin.begin(), fin.end(), [to, tag, this](auto from){ return edgeAttrs(from, to).count(tag)>0;});
	// 			if(edge != fin.end())
	// 				return std::get<Ta>(edgeAttrs(*edge,to).at(tag));
	// 			else return Ta();
	// 		};

	/* class Value
	{
		public:
			Attribs &attrs() 			{ return node_attrs;};
			DrawAttribs &drawAttrs() 	{ return node_draw_attrs;};
			FanOut &fanout() 			{ return node_fanout;};
			FanIn &fanin()	 			{ return node_fanin;};
		
		private:
			Attribs node_attrs;
			DrawAttribs node_draw_attrs;
			FanOut node_fanout;
			FanIn node_fanin;
	}; */