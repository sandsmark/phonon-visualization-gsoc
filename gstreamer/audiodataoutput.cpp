/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2009 Martin Sandsmark <sandsmark@samfundet.no>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "audiodataoutput.h"
#include "gsthelper.h"
#include "medianode.h"
#include "mediaobject.h"
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <phonon/audiooutput.h>

namespace Phonon
{
namespace Gstreamer
{
AudioDataOutput::AudioDataOutput(Backend *backend, QObject *parent)
    : QObject(parent),
    MediaNode(backend, AudioSink | AudioSource),
    m_format(Phonon::AudioDataOutput::IntegerFormat)
{
    static int count = 0;
    m_name = "AudioDataOutput" + QString::number(count++);

    m_queue = gst_element_factory_make ("queue", NULL);
    m_isValid = true;
}

AudioDataOutput::~AudioDataOutput()
{
    gst_object_unref(m_queue);
}

Phonon::AudioDataOutput::Format AudioDataOutput::format() const
{
    return Phonon::AudioDataOutput::IntegerFormat;
}

int AudioDataOutput::dataSize() const
{
    return m_dataSize;
}

int AudioDataOutput::sampleRate() const
{
    return 44100;
}

void AudioDataOutput::setFormat(Phonon::AudioDataOutput::Format)
{
//    m_format = format;
}

void AudioDataOutput::setDataSize(int size)
{
    m_dataSize = size;
}

typedef QMap<Phonon::AudioDataOutput::Channel, QVector<float> > FloatMap;
typedef QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > IntMap;

inline void AudioDataOutput::convertAndEmit(const QVector<qint16> &buffer)
{
    //TODO: Floats
    IntMap map;
    map.insert(Phonon::AudioDataOutput::LeftChannel, buffer);
    map.insert(Phonon::AudioDataOutput::RightChannel, buffer);
    emit dataReady(map);

/*    }
    else
    {
        IntMap map;
        QVector<qint16> intBuffer(m_dataSize);
        for (int i = 0; i < m_dataSize; ++i)
            intBuffer[i] = static_cast<qint16>(buffer[i] * static_cast<float>(0x7FFF));
        map.insert(Phonon::AudioDataOutput::LeftChannel, intBuffer);
        map.insert(Phonon::AudioDataOutput::RightChannel, intBuffer);
        emit dataReady(map);
    }*/
}

void AudioDataOutput::processBuffer(GstPad*, GstBuffer* buffer, gpointer gThat)
{
    // TODO emit endOfMedia
    AudioDataOutput *that = reinterpret_cast<AudioDataOutput*>(gThat);
    that->m_pendingData.resize(that->m_pendingData.size() + buffer->size);

    for (uint i=0; i<buffer->size; i++)
        that->m_pendingData.append(buffer->data[i] * 255);

    if (that->m_pendingData.size() < that->m_dataSize)
        return;

    if (that->m_pendingData.size() == that->m_dataSize)
        that->convertAndEmit(that->m_pendingData);
    else
    {
        QVector<qint16> intBuffer(that->m_dataSize);
        while (that->m_pendingData.size() >= that->m_dataSize)
        {
            memcpy(intBuffer.data(), that->m_pendingData.constData(), that->m_dataSize * sizeof(qint16));
            that->convertAndEmit(intBuffer);
            int newSize = that->m_pendingData.size() - that->m_dataSize;
            memmove(that->m_pendingData.data(), that->m_pendingData.constData() + that->m_dataSize, newSize * sizeof(qint16));
            that->m_pendingData.resize(newSize);
        }
    }
}

void AudioDataOutput::mediaNodeEvent(const MediaNodeEvent *event)
{
    if (event->type() == MediaNodeEvent::MediaObjectConnected && root()) {
        GstPad *audiopad = gst_element_get_pad (root()->audioElement(), "src");
        gst_pad_add_buffer_probe (audiopad, G_CALLBACK(processBuffer), this);
        gst_object_unref (audiopad);
        return;
    }

    MediaNode::mediaNodeEvent(event);
}

}} //namespace Phonon::Gstreamer

#include "moc_audiodataoutput.cpp"
// vim: sw=4 ts=4

