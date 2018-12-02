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

class Graph 
{
	public:
		using Attribs = std::unordered_map<std::string, std::any>;
		struct EdgeAttrs{ Attribs attrs, draw_attrs;};
		using FanOut = std::unordered_map<std::uint32_t, EdgeAttrs>;
		using FanIn = std::vector<std::uint32_t>;
		struct Value
		{
			Attribs attrs;
			Attribs draw_attrs;
			FanOut fanout;
			FanIn fanin;
		};
		using Nodes = std::unordered_map<std::uint32_t, Value>;
		
		typename Nodes::iterator begin() 								{ return nodes.begin(); };
		typename Nodes::iterator end() 									{ return nodes.end();   };
		typename Nodes::const_iterator begin() const  	{ return nodes.begin(); };
		typename Nodes::const_iterator end() const 	 		{ return nodes.begin(); };
		size_t size() const 														{ return nodes.size();  };
		void addNode(std::uint32_t id) 									{ nodes.insert(std::pair(id, Value()));};
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
		void addNodeDrawAttribs(std::uint32_t id, const Attribs &att)
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
		void addEdgeDrawAttribs(std::uint32_t from, std::uint32_t to, const Attribs &att)
		{ 
				auto &edgeAtts = nodes[from].fanout.at(to);
				for(auto &[k,v] : att)
					edgeAtts.draw_attrs.insert_or_assign(k,v);
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
					std::cout << ka << " -> " << std::any_cast<std::string>(kv) << " , ";
				}
				std::cout << std::endl << "	edges:	";
				for( auto &[kf,vf] : v.fanout)
				{
					std::cout << attr<std::string>(vf.attrs["name"]) << "( " << attr<std::string>(nodes[kf].attrs["name"]) << " ) " << std::endl;
					std::cout << "			edge attrs: ";
					for( auto &[ke, ve] : vf.attrs)
						std::cout << ke << " -> " << std::any_cast<std::string>(ve) << " , ";
					std::cout << std::endl << "		";
				}
				std::cout << std::endl;
			}
			std::cout << "---------------- graph ends here --------------------------" << std::endl;
		}
        //helpers
		Value node(std::uint32_t id) const			{ return nodes.at(id); };
		Value& node(std::uint32_t id)  	 				{ return nodes.at(id); };
		FanOut fanout(const Value &v) const   	{ return v.fanout;};
    	FanOut& fanout(Value &v)              { return v.fanout;};
		FanIn fanin(const Value &v) const     	{ return v.fanin;};
		FanIn& fanin(Value &v)                	{ return v.fanin;};
		Attribs attrs(const Value &v) const   	{ return v.attrs;};
		Attribs& attrs(Value &v)              	{ return v.attrs;};
		Attribs drawAttrs(const Value &v) const	{ return v.draw_attrs;};
		Attribs& drawAttrs(Value &v)           	{ return v.draw_attrs;};
   
    template<typename T>
	T attr(const std::any &s) const     		{ return std::any_cast<T>(s);};
		
	private:
		Nodes nodes;
};

#endif // GRAPH_H
