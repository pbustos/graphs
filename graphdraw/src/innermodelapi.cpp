/*
 * Copyright 2018 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use graph file except in compliance with the License.
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

#include "innermodelapi.h"

using namespace DSR;

void InnerModelAPI::innerModelTreeWalk(std::uint32_t id)
{
	//std::cout << "id: " << id << std::endl;
	if (graph->nodeExists(id) == false)
	{
		std::cout << __FUNCTION__ << "Non existing node: " << id << std::endl;
		return;
	}
	
	for(auto &child_id : graph->edgesByLabel(id, "RT")) 
	{
		innerModelTreeWalk(child_id);
		//exit(-1);
	} 
}

RMat::QVec InnerModelAPI::transform(const IMType &destId, const QVec &initVec, const std::uint32_t &origId)
{
    if (initVec.size()==3)
	{
		return (getTransformationMatrix(destId, origId) * initVec.toHomogeneousCoordinates()).fromHomogeneousCoordinates();
	}
	else if (initVec.size()==6)
	{
		const QMat M = getTransformationMatrix(destId, origId);
		const QVec a = (M * initVec.subVector(0,2).toHomogeneousCoordinates()).fromHomogeneousCoordinates();
		const Rot3D R(initVec(3), initVec(4), initVec(5));
		const QVec b = (M.getSubmatrix(0,2,0,2)*R).extractAnglesR_min();
		QVec ret(6);
		ret(0) = a(0);
		ret(1) = a(1);
		ret(2) = a(2);
		ret(3) = b(0);
		ret(4) = b(1);
		ret(5) = b(2);
		return ret;
	}
	// else
	// {
	// 	throw InnerModelException("InnerModel::transform was called with an unsupported vector size.");
	// }
}

RMat::RTMat InnerModelAPI::getTransformationMatrix(const IMType &to, const IMType &from)
{
    //search node with "imName" = destIp
    auto to_id = graph->nodeByIMName("imName", to);
    auto from_id = graph->nodeByIMName("imName", from);
    
	RMat::RTMat ret;

	// if (localHashTr.contains(QPair<QString, QString>(to, from)))
	// {
	// 	ret = localHashTr[QPair<QString, QString>(to, from)];
	// }
	// else
	// {
		auto [listA, listB] = setLists(from_id, to_id);
		for (auto to_id = listA.begin(); to_id != std::prev(listA.end()); ++to_id)
		{
			//ret = ((RTMat)(*i)).operator*(ret);
			//std::cout << "List A id " << *to << std::endl;
			ret = graph->edgeAttrib<RMat::RTMat>(*(std::next(to_id,1)), *to_id, "RT") * ret;
		}
		for (auto from = listB.begin(); from != std::prev(listB.end()); ++from_id)
		{
			//ret = i->invert() * ret;
			ret = graph->edgeAttrib<RMat::RTMat>(*from_id, *(std::next(from_id)), "RT").invert() * ret;
			//std::cout << "List B id " << *from << std::endl;
		}
		//localHashTr[QPair<QString, QString>(to, from)] = ret;
	// }
	return ret;
} 
 
InnerModelAPI::ABLists InnerModelAPI::setLists(const std::uint32_t &origId, const std::uint32_t &destId)
{
	//InnerModelNode *a = hash[origId], *b = hash[destId];
    std::list<std::uint32_t> listA, listB;
	auto a = origId;
	auto b = destId;
	
/* 	if (!a)
		throw InnerModelException("Cannot find node: \""+ origId.toStdString()+"\"");
	if (!b)
		throw InnerModelException("Cannot find node: "+ destId.toStdString()+"\"");
 */
	std::uint32_t a_level = graph->getNodeLevel(origId);
	std::uint32_t b_level = graph->getNodeLevel(destId);
	std::uint32_t min_level = std::min(a_level,b_level);
	
	listA.clear();
	while (a_level >= min_level)
	{
		listA.push_back(a);
		if(graph->getParent(a) == 0)
			break;
		a = graph->getParent(a);
	}

	listB.clear();
	while (b_level >= min_level)
	{
		listB.push_front(b);
		if(graph->getParent(b) == 0)
			break;
		b = graph->getParent(b);
	}
	while (b != a)
	{
		listA.push_back(a);
		listB.push_front(b);
		a = graph->getParent(a);
		b = graph->getParent(b);
	}

	for(auto a : listA)
		std::cout << "list A " << a << std::endl;
	for(auto a : listB)
		std::cout << "list B " << a << std::endl;
	return std::make_pair(listA, listB);
}

/////////////////////////////////////////////////////
///// Tree update methods
/////////////////////////////////////////////////////

void InnerModelAPI::updateTransformValues(const std::uint32_t &transformId, float tx, float ty, float tz, float rx, float ry, float rz, const std::uint32_t &parentId)
{	
//	cleanupTables();
//	InnerModelTransform *aux = dynamic_cast<InnerModelTransform *>(hash[transformId]);
	if(graph->nodeExists(transformId) and graph->nodeHasAttrib<std::string>(transformId, "imType", "transform"))
	{
		if(parentId != 0)
		{
			//InnerModelTransform *auxParent = dynamic_cast<InnerModelTransform *>(hash[parentId]);
			if (graph->nodeExists(parentId) and graph->nodeHasAttrib<std::string>(parentId, "imType", "transform"))
			{
				RTMat Tbi;
				Tbi.setTr(tx,ty,tz);
				Tbi.setR (rx,ry,rz);
				//RTMat Tpb = getTransformationMatrix( getNode (transformId)->parent->id,parentId );
                RTMat Tpb = getTransformationMatrix( graph->getParent(transformId), parentId );
				RTMat Tpi = Tpb*Tbi;
				QVec angles = Tpi.extractAnglesR();
				QVec tr = Tpi.getTr();
				rx = angles.x();
				ry = angles.y();
				rz = angles.z();
				tx = tr.x();
				ty = tr.y();
				tz = tr.z();
			}
			else
			{
				qDebug() << "There is no such" << parentId << "node";
			}
		}
		//always update
		//aux->update(tx,ty,tz,rx,ry,rz);
        Attribs attr{ std::pair("tx",tx), std::pair("ty",ty), std::pair("tz",tz), std::pair("rx",rx), std::pair("ry",ry), std::pair("rz",rz) };
        graph->addNodeAttribs(transformId, attr);
	}
	else
	{
		qDebug() << "There is no such" << transformId << "node";
	}
}