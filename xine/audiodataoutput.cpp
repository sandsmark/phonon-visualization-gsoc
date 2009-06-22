/*  This file is part of the KDE project
    Copyright (C) 2004 Max Howell <max.howell@methylblue.com>
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2009 Martin Sandsmark <sandsmark@samfundet.no>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "audiodataoutput.h"
#include <QVector>
#include <QMap>
#include "sourcenode.h"
#include "audiooutput.h"

extern "C" {
    #define this __this__ //HACK, yeah! (Xine uses “this” as a name for certain variables)
    #include <xine.h>
    #include <xine/xine_internal.h>
    #undef this
}

namespace Phonon
{
namespace Xine
{


AudioDataOutputXT::AudioDataOutputXT(AudioDataOutput *output) :
                    SinkNodeXT(),
                    SourceNodeXT(),
                    m_frontend(output),
                    m_audioPort(0)

{
    m_xine = Backend::xine();

    m_plugin = (scope_plugin_t*)qMalloc(sizeof(scope_plugin_t));
    post_plugin_t  *post_plugin  = (post_plugin_t*)m_plugin;

    {
        post_in_t         *input;
        post_out_t        *output;

        //1 audio input, 0 video inputs
        _x_post_init(post_plugin, 1, 0);

        m_port = _x_post_intercept_audio_port(post_plugin, m_audioPort, &input, &output);

        if (!m_port) {
            qWarning() << Q_FUNC_INFO << "unable to allocate port";
            delete post_plugin;
            return;
        }

        m_port->new_port.open       = openPort;
        m_port->new_port.close      = closePort;
        m_port->new_port.put_buffer = putBufferCallback;
        reinterpret_cast<post_audio_port_t*>(&m_port->new_port)->original_port = &m_port->new_port; // hahahaha, oh, wow, hackish.
        m_audioPort = &m_port->new_port;
        reinterpret_cast<post_audio_port_t*>(m_audioPort)->original_port->set_property; // see? fine!

        post_plugin->xine_post.audio_input[0] = &m_port->new_port;
        post_plugin->xine_post.type = PLUGIN_POST;

        post_plugin->dispose = dispose;
    }

    /* code is straight from xine_init_post()
    can't use that function as it only dlopens the plugins
    and our plugin is statically linked in */

    post_plugin->running_ticket = (*m_xine).port_ticket;
    post_plugin->xine = m_xine;

    m_plugin->audioDataOutput = this;
}

void AudioDataOutputXT::rewireTo(SourceNodeXT *source)
{
    debug() << Q_FUNC_INFO << "rewiring to " << source;
    if (!source->audioOutputPort()) { // I can't get no satisfaction
        qWarning() << Q_FUNC_INFO << ": No audio port in source!";
        return;
    }
    source->assert();

    xine_post_in_t *target = (xine_post_in_t*)xine_post_input(
                                    &((post_plugin_t*)m_plugin)->xine_post,
                                    const_cast<char*>("audio in"));

    if (!xine_post_wire(source->audioOutputPort(), target)) {
        qWarning() << Q_FUNC_INFO << ": Failed to rewire!";
        return;
    }

    source->assert();
    SinkNodeXT::assert();
}

xine_post_out_t *AudioDataOutputXT::audioOutputPort() const
{
    xine_post_out_t* aop = xine_post_output(
                                    &((post_plugin_t*)m_plugin)->xine_post,
                                    const_cast<char*>("audio out"));
    if (!aop) qWarning() << Q_FUNC_INFO << ": Warning! NULL audio output port!";
    return aop;
}

int AudioDataOutputXT::openPort(xine_audio_port_t *port_gen, xine_stream_t *stream, uint32_t bits, uint32_t rate, int mode )
{
    AudioDataOutputXT *that = ((scope_plugin_t*)((post_audio_port_t*)port_gen)->post)->audioDataOutput;
    post_audio_port_t *port = (post_audio_port_t*)port_gen;

    _x_post_rewire((post_plugin_t*)port->post);
    _x_post_inc_usage(port);

    port->stream = stream;
    port->bits = bits;
    port->rate = rate;
    port->mode = mode;

    that->m_channels = _x_ao_mode2channels(mode);
    that->m_frontend->setChannels(that->m_channels);

    return port->original_port->open( port->original_port, stream, bits, rate, mode );
}

void AudioDataOutputXT::closePort(xine_audio_port_t *port_gen, xine_stream_t *stream)
{
    post_audio_port_t *port = (post_audio_port_t*)port_gen;

    port->stream = NULL;
    port->original_port->close(port->original_port, stream);

    _x_post_dec_usage(port);
}

void AudioDataOutputXT::putBufferCallback(xine_audio_port_t * port_gen, audio_buffer_t *buf, xine_stream_t *stream)
{
    AudioDataOutputXT *that = ((scope_plugin_t*)((post_audio_port_t*)port_gen)->post)->audioDataOutput;

    qWarning() << "got called back!";
    int samples = buf->num_frames * that->m_channels;

    QVector<qint16> buffer(samples);
    for (int i=0; i<samples; ++i)
        buffer[i] = buf->mem[i];

    that->m_frontend->packetReady(buffer);

    // Pass on the data to the original port
    post_audio_port_t *port = (post_audio_port_t*)port_gen;
    port->original_port->put_buffer(port->original_port, buf, stream);
}

void AudioDataOutputXT::dispose(post_plugin_t *port_gen)
{
    AudioDataOutputXT *that = ((scope_plugin_t*)((post_audio_port_t*)port_gen)->post)->audioDataOutput;
    delete that;
}




/* BACKEND-FRONT OBJECT */
AudioDataOutput::AudioDataOutput(QObject*)
: SinkNode(new AudioDataOutputXT(this))
, SourceNode(static_cast<AudioDataOutputXT *>(SinkNode::m_threadSafeObject.data()))
, m_format(Phonon::AudioDataOutput::IntegerFormat)
{
}

AudioDataOutput::~AudioDataOutput()
{
}

Phonon::AudioDataOutput::Format AudioDataOutput::format() const
{
    return m_format;
}

int AudioDataOutput::sampleRate() const
{
    return 44100;
}

typedef QMap<Phonon::AudioDataOutput::Channel, QVector<float> > FloatMap;
typedef QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > IntMap;

void AudioDataOutput::setFrontendObject(Phonon::AudioDataOutput *frontend) {
    m_frontend = frontend;
}

Phonon::AudioDataOutput *AudioDataOutput::frontendObject() const {
    return m_frontend;
}


inline void AudioDataOutput::packetReady(const QVector<qint16> buffer)
{
    //TODO: support floats
    m_pendingData += buffer;

    if (m_format == Phonon::AudioDataOutput::FloatFormat)
        return;

    if (m_pendingData.size() / m_channels > dataSize())
    {
        if (m_channels==1)
        {
            IntMap map;
            QVector<qint16> data = m_pendingData.mid(0, dataSize());
            m_pendingData.remove(0, dataSize());
            map.insert(Phonon::AudioDataOutput::LeftChannel, data);
            map.insert(Phonon::AudioDataOutput::RightChannel, data);
            emit dataReady(map);
        }
        else if (m_channels==2)
        {
            IntMap map;
            QVector<qint16> left, right, data = m_pendingData.mid(0, dataSize() * 2);
            m_pendingData.remove(0, dataSize() * 2);
            for (int i=0, j=0; i < dataSize() * 2; i+=2)
            {
                left[j] = m_pendingData[i];
                right[j] = m_pendingData[i+1];
                ++j;
            }
            map.insert(Phonon::AudioDataOutput::LeftChannel, left);
            map.insert(Phonon::AudioDataOutput::RightChannel, right);
            emit dataReady(map);
        }
    }
}

}} //namespace Phonon::Xine

#include "audiodataoutput.moc"
// vim: sw=4 ts=4
