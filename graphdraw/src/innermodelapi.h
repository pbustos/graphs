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


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 													InnerModel API
///////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef INNERMODELAPI_H
#define INNERMODELAPI_H

#include <iostream>
#include <qmat/QMatAll>
#include "graph.h"

using DSR::IDType;
using IMType = QString;

class InnerModelAPI 
{
	public:	
        // copy shared_ptr to Graph
        void setGraph( std::shared_ptr<DSR::Graph> graph_) { graph = graph_; }
        
        // ///////////////////////////////////////////////////
        // /// Tree update methods
        // ///////////////////////////////////////////////////
        // void setRoot(InnerModelNode *node);
        // void cleanupTables();
        void updateTransformValues(const IMType &transformId, float tx, float ty, float tz, float rx, float ry, float rz, const IMType &parentId = 0);
        void innerModelTreeWalk(IDType);

        // ////////////////////////////////////////////
        // /// Transformation matrix retrieval methods
        // ///////////////////////////////////////////
        using ABLists = std::tuple< std::list<IDType>, std::list<IDType>>;

        ABLists setLists(const IDType &origId, const IDType &destId);
        RMat::RTMat getTransformationMatrix(const IDType &to, const IDType &from);
        RMat::QVec transform(const IDType &destId, const QVec &initVec, const IDType &origId);
        //RTMat getTransformationMatrix(const QString &destId, const QString &origId);
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
       std::shared_ptr<DSR::Graph> graph;
};
#endif // INNERMODELAPI_H
