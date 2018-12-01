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

GraphEdge::GraphEdge(GraphNode *sourceNode, GraphNode *destNode, const QString &edge_name) : arrowSize(10)
{
    setAcceptedMouseButtons(0);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
		tag = new QGraphicsSimpleTextItem(edge_name, this);
		tag->setBrush(QBrush(QColor("coral")));
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
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void GraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
		{
				// Draw the line itself
				painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
				QRectF rectangle(sourcePoint.x()-20, sourcePoint.y()-20, 20.0, 20.0);
				int startAngle = 35;
				int spanAngle = 270 * 16;
				painter->drawArc(rectangle, startAngle, spanAngle);
				tag->setX(line.center().x()-40);
				tag->setY(line.center().y()-30);
				double alpha = (60*16*2*M_PI)/5760;
				alpha = 0;
				double r = 20/2.f;
				QPointF pnt(r*cos(alpha) + rectangle.center().x(), r*sin(alpha) + rectangle.center().y());
				QLineF line(pnt, destPoint );
				double angle = std::atan2(-line.dy(), line.dx());
				QPointF destArrowP1 = destPoint + QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
				QPointF destArrowP2 = destPoint + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);
				painter->drawPolygon(QPolygonF() << pnt << destArrowP1 << destArrowP2);
		}
		else
		{
			// Draw the line itself
			painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
			painter->drawLine(line);

			// Draw the arrows
			double angle = std::atan2(-line.dy(), line.dx());
			QPointF destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 3) * arrowSize, cos(angle - M_PI / 3) * arrowSize);
			QPointF destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize, cos(angle - M_PI + M_PI / 3) * arrowSize);
			painter->setBrush(Qt::black);
			painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
			tag->setX(line.center().x());
			tag->setY(line.center().y());
		}
	
}
