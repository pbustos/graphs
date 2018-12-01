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

#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include <QGraphicsItem>

class GraphNode;

class GraphEdge : public QGraphicsItem
{
	public:
    GraphEdge(GraphNode *sourceNode, GraphNode *destNode);
    GraphNode *sourceNode() const;
    GraphNode *destNode() const;
    void adjust();
    enum { Type = UserType + 2 };
    int type() const override { return Type; }

	protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	
	private:
		GraphNode *source, *dest;
    qreal arrowSize;
    QPointF sourcePoint;
    QPointF destPoint;
};

#endif // GRAPHEDGE_H
