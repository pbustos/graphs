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


#include <unordered_map>
#include <any>
#include <memory>


class Graph 
{
	public:
		using Attribs = std::unordered_map<std::string, std::any>;
		using Neighs = std::unordered_map<std::uint32_t, Attribs>;
		using Value = std::tuple<Attribs, Neighs>;
		using Nodes = std::unordered_map<std::uint32_t, Value>;
		
		typename Nodes::iterator begin() 												{ return nodes.begin(); };
		typename Nodes::iterator end() 													{ return nodes.end();   };
		typename Nodes::const_iterator begin() const  					{ return nodes.begin(); };
		typename Nodes::const_iterator end() const 	 						{ return nodes.begin(); };
		size_t size() const 																		{ return nodes.size();  };
		void addNode(std::uint32_t id) 													{ Value v; nodes.insert(std::pair(id, v));};
		void addEdge(std::uint32_t from, std::uint32_t to) 			{ Attribs a; std::get<1>(nodes[from]).insert(std::pair(to,a));};
		void addNodeAttribs(std::uint32_t id, const Attribs &att) 									
		{ 
			for(auto &[k,v] : att)
				std::get<0>(nodes[id]).insert_or_assign(k,v);
		};
		void addEdgeAttribs(std::uint32_t from, std::uint32_t to, const Attribs &att)
		{ 
				auto &edgeAtts = std::get<1>(nodes[from]).at(to);
				for(auto &[k,v] : att)
					edgeAtts.insert_or_assign(k,v);
		};
        //helpers
		Value node(std::uint32_t id) const		{ return nodes.at(id); };
		Value& node(std::uint32_t id)  	 			{ return nodes.at(id); };
		Neighs edges(const Value &v) const    { return std::get<1>(v);};
    Neighs& edges(Value &v)               { return std::get<1>(v);};
		Attribs attrs(const Value &v) const   { return std::get<0>(v);};
    Attribs& attrs(Value &v)              { return std::get<0>(v);};
    template<typename T>
      T attr(const std::any &s) const     { return std::any_cast<T>(s);};
		
	private:
		Nodes nodes;
};

#endif // GRAPH_H
