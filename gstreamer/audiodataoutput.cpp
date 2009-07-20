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

    if (!m_backend->isValid()) qWarning() << "fuuu, no valid backend";
    if (m_backend->isValid()) {
        // Initialize a new container
        //m_audioBin = gst_bin_new (NULL);
        //gst_object_ref (GST_OBJECT (m_audioBin));  // boilerplate (we own this object now)
        //gst_object_sink (GST_OBJECT (m_audioBin)); // sweet boilerplate

        // We use this to leech audio data
        //GstElement *convert = gst_element_factory_make ("audioconvert", NULL);
        //gst_bin_add(GST_BIN (m_audioBin), convert);

        // We need a queue to handle tee-connections from parent node
        m_queue = gst_element_factory_make ("queue", NULL);
        //gst_bin_add(GST_BIN (m_audioBin), queue);

        //if (queue && convert && m_audioBin) {
            // Link up our wonderful little path
            //gst_bin_add_many (GST_BIN (m_audioBin), m_audioSink, (const char*)NULL);
            //if (gst_element_link_many (queue, convert, (const char*)NULL)) {

                // Add ghost sink for our audiobin
                //GstPad *audiopad = gst_element_get_pad (queue, "sink");
                //gst_element_add_pad (m_audioBin, gst_ghost_pad_new ("sink", audiopad));
                //gst_object_unref(audiopad);

                // add our probe
                //GstPad *audiopad = gst_element_get_pad (m_audioTee, "src");
                //GstPad *audiopad = gst_element_get_pad (root()->audioElement(), "src");
                //gst_pad_add_buffer_probe (audiopad, G_CALLBACK(processBuffer), this);

                // add ghost source for our audio bin
                //gst_element_add_pad (m_audioBin, gst_ghost_pad_new ("src", audiopad));
                //gst_object_unref (audiopad);
                //m_isValid = true; // Initialization ok, accept input
            //}
            //else qWarning() << "Link many failed";
        //}
        //else qWarning() << "we no has all elementos";
    }
    m_isValid = true;
    //if (!m_isValid) qWarning() << Q_FUNC_INFO << ": initialization failed";
}

AudioDataOutput::~AudioDataOutput()
{
    //gst_object_unref(m_audioSink);
    //gst_object_unref(m_audioBin);
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
//    if (m_format == Phonon::AudioDataOutput::FloatFormat)
//    {
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
        that->m_pendingData.append(buffer->data[i]*(1<<8));

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
    if (event->type() == MediaNodeEvent::MediaObjectConnected && root()){
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

