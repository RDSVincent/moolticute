/******************************************************************************
 **  Copyright (c) Raoul Hecky. All Rights Reserved.
 **
 **  Calaos is free software; you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation; either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Calaos is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Foobar; if not, write to the Free Software
 **  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 **
 ******************************************************************************/
#include "MPNode.h"

QByteArray MPNode::EmptyAddress = QByteArray(2, 0);

MPNode::MPNode(const QByteArray &d, QObject *parent, const QByteArray &nodeAddress, const quint32 virt_addr):
    QObject(parent),
    data(std::move(d)),
    address(std::move(nodeAddress)),
    virtualAddress(virt_addr)
{
}

MPNode::MPNode(QObject *parent, const QByteArray &nodeAddress, const quint32 virt_addr):
    QObject(parent),
    address(std::move(nodeAddress)),
    virtualAddress(virt_addr)
{
}

int MPNode::getType() const
{
    if (data.size() > 1)
        return ((quint8)data[1] >> 6) & 0x03;
    return -1;
}

bool MPNode::isValid() const
{
    return getType() != NodeUnknown &&
           data.size() == MP_NODE_SIZE &&
           ((quint8)data[1] & 0x20) == 0;
}

bool MPNode::isDataLengthValid() const
{
    return data.size() == MP_NODE_SIZE;
}

void MPNode::appendData(const QByteArray &d)
{
    data.append(d);
}

QByteArray MPNode::getAddress() const
{
    return address;
}

void MPNode::setVirtualAddress(quint32 addr)
{
    virtualAddress = addr;
}

quint32 MPNode::getVirtualAddress(void) const
{
    return virtualAddress;
}

void MPNode::setPointedToCheck()
{
    pointedToCheck = true;
}

void MPNode::removePointedToCheck()
{
    pointedToCheck = false;
}

bool MPNode::getPointedToCheck() const
{
    return pointedToCheck;
}

QByteArray MPNode::getPreviousParentAddress() const
{
    if (!isValid()) return QByteArray();
    return data.mid(2, 2);
}

quint32 MPNode::getPrevParentVirtualAddress() const
{
    return prevVirtualAddress;
}

void MPNode::setPreviousParentAddress(const QByteArray &d, const quint32 virt_addr)
{
    prevVirtualAddress = virt_addr;
    data[2] = d[0];
    data[3] = d[1];
}

QByteArray MPNode::getNextParentAddress() const
{
    if (!isValid()) return QByteArray();
    return data.mid(4, 2);
}

quint32 MPNode::getNextParentVirtualAddress() const
{
    return nextVirtualAddress;
}

void MPNode::setNextParentAddress(const QByteArray &d, const quint32 virt_addr)
{
    nextVirtualAddress = virt_addr;
    data[4] = d[0];
    data[5] = d[1];
}

QByteArray MPNode::getStartChildAddress() const
{
    if (!isValid()) return QByteArray();
    return data.mid(6, 2);
}

quint32 MPNode::getFirstChildVirtualAddress() const
{
    return firstChildVirtualAddress;
}

void MPNode::setStartChildAddress(const QByteArray &d, const quint32 virt_addr)
{
    firstChildVirtualAddress = virt_addr;
    data[6] = d[0];
    data[7] = d[1];
}

QString MPNode::getService() const
{
    if (!isValid()) return QString();
    return QString::fromUtf8(data.mid(8, MP_NODE_SIZE - 8 - 3));
}

void MPNode::setService(const QString &service)
{
    if (isValid())
    {
        data.replace(8, service.toUtf8());
    }
}

QByteArray MPNode::getStartDataCtr() const
{
    if (!isValid()) return QByteArray();
    return data.mid(129, 3);
}

QByteArray MPNode::getNextChildAddress() const
{
    if (!isValid()) return QByteArray();
    return data.mid(4, 2);
}

void MPNode::setNextChildAddress(const QByteArray &d)
{
    data[4] = d[0];
    data[5] = d[1];
}

QByteArray MPNode::getPreviousChildAddress() const
{
    if (!isValid()) return QByteArray();
    return data.mid(2, 2);
}

void MPNode::setPreviousChildAddress(const QByteArray &d)
{
    data[2] = d[0];
    data[3] = d[1];
}

QByteArray MPNode::getNextChildDataAddress() const
{
    //in data nodes, there is no linked list
    //the only address is the next one
    //It is the same as previous for cred nodes
    return getPreviousChildAddress();
}

void MPNode::setNextChildDataAddress(const QByteArray &d)
{
    data[2] = d[0];
    data[3] = d[1];
}

QByteArray MPNode::getCTR() const
{
    if (!isValid()) return QByteArray();
    return data.mid(34, 3);
}

QString MPNode::getDescription() const
{
    if (!isValid()) return QString();
    return QString::fromUtf8(data.mid(6, 24));
}

QString MPNode::getLogin() const
{
    if (!isValid()) return QString();
    return QString::fromUtf8(data.mid(37, 63));
}

QByteArray MPNode::getPasswordEnc() const
{
    if (!isValid()) return QByteArray();
    return data.mid(100, 32);
}

QDate MPNode::getDateCreated() const
{
    if (!isValid()) return QDate();
    return Common::bytesToDate(data.mid(30, 2));
}

QDate MPNode::getDateLastUsed() const
{
    if (!isValid()) return QDate();
    return Common::bytesToDate(data.mid(32, 2));
}

QByteArray MPNode::getNextDataAddress() const
{
    if (!isValid()) return QByteArray();
    return data.mid(2, 2);
}

void MPNode::setNextDataAddress(const QByteArray &d)
{
    data[2] = d[0];
    data[3] = d[1];
}

QByteArray MPNode::getNodeData() const
{
    if (!isValid()) return QByteArray();
    return data;
}

QByteArray MPNode::getChildData() const
{
    if (!isValid()) return QByteArray();
    return data.mid(4);
}

QJsonObject MPNode::toJson() const
{
    QJsonObject obj;

    if (getType() == NodeParent)
    {
        obj["service"] = getService();

        QJsonArray childs;
        foreach (MPNode *cnode, childNodes)
        {
            QJsonObject cobj = cnode->toJson();
            childs.append(cobj);
        }

        obj["childs"] = childs;
    }
    else if (getType() == NodeParentData)
    {
        obj["service"] = getService();

        QJsonArray childs;
        foreach (MPNode *cnode, childDataNodes)
        {
            QJsonObject cobj = cnode->toJson();
            childs.append(cobj);
        }

        obj["childs"] = childs;
    }
    else if (getType() == NodeChild)
    {
        obj["login"] = getLogin();
        obj["description"] = getDescription();
        obj["password_enc"] = Common::bytesToJson(getPasswordEnc());
        obj["date_created"] = getDateCreated().toString(Qt::ISODate);
        obj["date_last_used"] = getDateLastUsed().toString(Qt::ISODate);
    }
    else if (getType() == NodeChildData)
    {
        obj["data"] = Common::bytesToJson(getChildData());
    }

    return obj;
}
