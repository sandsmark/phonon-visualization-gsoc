/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef STREAMINTERFACE_P_H
#define STREAMINTERFACE_P_H

#include "streaminterface.h"
#include "streameventqueue_p.h"
#include "mediasource.h"
#include <QtCore/QAbstractEventDispatcher>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

namespace Phonon
{

class StreamInterfacePrivateHelper;

class StreamInterfacePrivate : public LockFreeQueueBase::DataReadyHandler
{
    friend class StreamInterface;
    friend class StreamInterface2;
    friend class StreamInterfacePrivateHelper;

    public:
        void disconnectMediaStream();

    protected:
        inline StreamInterfacePrivate()
            : qobject(0),
            streamEventQueue(0),
            eventDispatcher(QAbstractEventDispatcher::instance()),
            connected(false),
            awakeSignalConnected(false)
        {
            Q_ASSERT(eventDispatcher);
        }

        inline ~StreamInterfacePrivate()
        {
            delete qobject;
        }

        virtual void dataReady();
        void _k_handleStreamEvent();
        void handleStreamEvent(QObject *sender);

        MediaSource mediaSource;
        QObject *qobject;
        StreamInterface *q;
        StreamEventQueue *streamEventQueue;
        QAbstractEventDispatcher *eventDispatcher;
        quint8 connected : 1;
        quint8 awakeSignalConnected : 1;
};

class StreamInterfacePrivateHelper : public QObject
{
    Q_OBJECT
    public:
        StreamInterfacePrivateHelper(StreamInterfacePrivate *qq) : q(qq) {}

    private:
        StreamInterfacePrivate *const q;
};

} // namespace Phonon

QT_END_NAMESPACE

#endif // STREAMINTERFACE_P_H
// vim: sw=4 sts=4 et tw=100
