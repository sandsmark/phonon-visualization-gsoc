/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_PACKET_P_H
#define PHONON_PACKET_P_H

#include <QtCore/QAtomicInt>

namespace Phonon
{

class PacketPoolPrivate;
struct PacketPrivate
{
    inline PacketPrivate(char *_data, PacketPoolPrivate *_pool)
        : ref(0), m_size(0), m_data(_data), m_pool(_pool) {}
    inline PacketPrivate() : ref(1), m_size(0), m_data(0), m_pool(0) {}

    QAtomicInt ref;

    int m_size;
    char *const m_data;
    PacketPoolPrivate *const m_pool;
};

} // namespace Phonon

#endif // PHONON_PACKET_P_H
