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

#include "graphedge.h"
#include "graphnode.h"
#include <qmath.h>
#include <QPainter>
#include <QDebug>
#include <QTableWidget>
#include <QGraphicsSceneMouseEvent>

GraphEdge::GraphEdge(GraphNode *sourceNode, GraphNode *destNode, const QString &edge_name) : arrowSize(10)
{
    setAcceptedMouseButtons(0);
		//setFlags(QGraphicsItem::ItemIsSelectable);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
		//tag = new QGraphicsSimpleTextItem(edge_name, this);
		tag = edge_name;
		//tag->setBrush(QBrush(QColor("coral")));
		adjust();
}

GraphNode *GraphEdge::sourceNode() const
{
    return source;
}

GraphNode *GraphEdge::destNode() const
{
    return dest;
}

void GraphEdge::adjust()
{
    if (!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) 
		{
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else 
		{
        sourcePoint = destPoint = line.p1();
    }
}

QRectF GraphEdge::boundingRect() const
{
    qreal adjust = 5;
		QLineF p(sourcePoint, destPoint);
    return QRectF( p.center().x() - adjust, p.center().y() - adjust, 10 , 10);
}

QPainterPath GraphEdge::shape() const
{
    QPainterPath path;
		QLineF p(sourcePoint, destPoint);
    path.addEllipse( p.center().x() - 5, p.center().y() - 5, 10, 10);
    return path;
}


void GraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;

    QLineF line(sourcePoint, destPoint);
		
		// self returning edges
    if (qFuzzyCompare(line.length(), qreal(0.)))
		{
				// Draw the line itself
				painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
				QRectF rectangle(sourcePoint.x()-20, sourcePoint.y()-20, 20.0, 20.0);
				int startAngle = 35;
				int spanAngle = 270 * 16;
				painter->drawArc(rectangle, startAngle, spanAngle);
				painter->setPen(QColor("coral"));
				painter->drawText(rectangle.center(), tag);
				double alpha = 0;
				double r = 20/2.f;
				painter->setBrush(Qt::black);
				painter->setPen(Qt::black);
				painter->drawPolygon(QPolygonF() << QPointF(r*cos(alpha) + rectangle.center().x(), r*sin(alpha) + rectangle.center().y())
																				 << QPointF(r*cos(alpha) + rectangle.center().x()-3, r*sin(alpha) + rectangle.center().y()-2) 
																				 << QPointF(r*cos(alpha) + rectangle.center().x()+2, r*sin(alpha) + rectangle.center().y()-2));
		}
		else
		{
			//check if there is another parallel edge 
			// Draw the line itself
			painter->save();
			painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
			double angle = std::atan2(-line.dy(), line.dx());
			painter->translate(line.center().x(), line.center().y());
			painter->rotate(-angle*180/M_PI);
			
			QRectF rectangle(-line.length()*0.5, -10, line.length(), 20);
			painter->drawArc(rectangle, 0, 180*16);
			
			painter->setPen(QColor("coral"));
			painter->drawText(rectangle.center(), tag);
				
			// Draw the arrows
			QPointF destArrowP1 = QPointF(-line.length()*0.5,0) + QPointF(sin(M_PI / 2) * arrowSize, cos(M_PI / 2) * arrowSize);
			QPointF destArrowP2 = QPointF(-line.length()*0.5,0) + QPointF(sin(M_PI / 4) * arrowSize, cos(M_PI / 4) * arrowSize);
			painter->setBrush(Qt::black);
			painter->setPen(Qt::black);
			painter->drawPolygon(QPolygonF() << QPointF(-line.length()*0.5,0) << destArrowP1 << destArrowP2 );
			painter->restore();
		}
	
}


void GraphEdge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
		if(event->button() == Qt::RightButton and tag == "RT")
		{
			//qDebug() << "hola";
			rt_values = new QGraphicsTextItem(this);
			rt_values->setPlainText("Matrix showing RT values");
			rt_values->setX(event->scenePos().x());
			rt_values->setY(event->scenePos().y());
   	}
}

void GraphEdge::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if( rt_values != nullptr)
		delete rt_values;
}

