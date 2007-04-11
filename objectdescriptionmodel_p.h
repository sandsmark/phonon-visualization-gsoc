/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_OBJECTDESCRIPTIONMODEL_P_H
#define PHONON_OBJECTDESCRIPTIONMODEL_P_H

#include "objectdescriptionmodel.h"
#include <QList>
#include "objectdescription.h"

namespace Phonon
{

template<ObjectDescriptionType type>
class ObjectDescriptionModelPrivate
{
    Q_DECLARE_PUBLIC(ObjectDescriptionModel<type>)
    protected:
        ObjectDescriptionModel<type> *q_ptr;

        QList<ObjectDescription<type> > data;
};

}
#endif // PHONON_OBJECTDESCRIPTIONMODEL_P_H
// vim: sw=4 ts=4 tw=80
