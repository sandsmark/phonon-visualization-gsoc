/*  This file is part of the KDE project
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>

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
#include "videodataoutput.h"
#include "videodataoutput_p.h"
#include "../factory_p.h"
#include <QtCore/QSize>

#define PHONON_CLASSNAME VideoDataOutput

namespace Phonon
{
namespace Experimental
{

VideoDataOutput::VideoDataOutput(QObject *parent)
    : QObject(parent)
    , AbstractVideoOutput(*new VideoDataOutputPrivate)
{
    K_D(VideoDataOutput);
    d->createBackendObject();
}

void VideoDataOutputPrivate::createBackendObject()
{
    if (m_backendObject)
        return;
    Q_Q(VideoDataOutput);
    m_backendObject = Factory::createVideoDataOutput(q);
    if (m_backendObject) {
        setupBackendObject();
    }
}


PHONON_GETTER(int, latency, d->latency)

bool VideoDataOutputPrivate::aboutToDeleteBackendObject()
{
    Q_ASSERT(m_backendObject);

    return AbstractVideoOutputPrivate::aboutToDeleteBackendObject();
}

void VideoDataOutputPrivate::setupBackendObject()
{
    Q_Q(VideoDataOutput);
    Q_ASSERT(m_backendObject);
    //AbstractVideoOutputPrivate::setupBackendObject();

    //QObject::connect(m_backendObject, SIGNAL(frameReady(const Phonon::Experimental::VideoFrame &)),
    //        q, SIGNAL(frameReady(const Phonon::Experimental::VideoFrame &)));

    QObject::connect(m_backendObject, SIGNAL(displayFrame(qint64, qint64)),
                     q, SIGNAL(displayFrame(qint64, qint64)));
    QObject::connect(m_backendObject, SIGNAL(endOfMedia()), q, SIGNAL(endOfMedia()));
}

bool VideoDataOutput::isRunning() const
{
     //K_D(const VideoDataOutput);
     //return d->m_backendObject->isRunning();
     return false;
}

VideoFrame VideoDataOutput::frameForTime(qint64 timestamp)
{
    //return d->m_backendObject->frameForTime(timestamp);
    return VideoFrame();
}

void VideoDataOutput::setRunning(bool running)
{
    //return d->m_backendObject->setRunning(running);
}

void VideoDataOutput::start()
{
    //return d->m_backendObject->setRunning(true);
}

void VideoDataOutput::stop()
{
    //return d->m_backendObject->setRunning(false);
}

} // namespace Experimental
} // namespace Phonon

#include "videodataoutput.moc"

#undef PHONON_CLASSNAME
// vim: sw=4 ts=4 tw=80
