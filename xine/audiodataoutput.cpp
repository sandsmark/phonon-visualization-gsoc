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

extern "C" {
    #define XINE_ENGINE_INTERNAL
    #define this __this__
    #include <xine.h>
    #include <xine/post.h>
    #include <xine/xine_internal.h>
    #undef this
    #undef XINE_ENGINE_INTERNAL
}

namespace Phonon
{
namespace Xine
{
class AudioDataOutputXT : public SinkNodeXT
{
public:
    AudioDataOutputXT(AudioDataOutput *output) { m_output = output; }

    AudioDataOutput *m_output;

private:
    void rewireTo(SourceNodeXT *);

    static int openPort(xine_audio_port_t*, xine_stream_t*, uint32_t, uint32_t, int);
    static void closePort(xine_audio_port_t *, xine_stream_t *);
    static void putBufferCallback(xine_audio_port_t *, audio_buffer_t *buf, xine_stream_t *stream);

    static QMap <xine_audio_port_t*, AudioDataOutputXT*> objectMapper;
};


AudioDataOutput::AudioDataOutput(QObject *parent)
    : AbstractAudioOutput(new AudioDataOutputXT(this), parent)
    , m_format(Phonon::Experimental::AudioDataOutput::FloatFormat)
    , m_dataSize(0)
{
}

AudioDataOutput::~AudioDataOutput()
{
}

void AudioDataOutputXT::rewireTo(SourceNodeXT *source)
{
    post_plugin_t *post_plugin = (post_plugin_t*)qMalloc(sizeof(post_plugin_t));
    xine_audio_port_s *audioPort = (xine_audio_port_s*)qMalloc(sizeof(xine_audio_port_s));// = source->audioOutputPort(); //xine_open_audio_driver(m_xine, XineCfg::outputPlugin().local8Bit(), NULL);

    if (xine_post_wire_audio_port(source->audioOutputPort(), audioPort))
    {
        post_in_t         *input;
        post_out_t        *output;
        post_audio_port_t *port;
        
        _x_post_init(post_plugin, 1, 0);
        
        port = _x_post_intercept_audio_port(post_plugin, audioPort, &input, &output);
        port->new_port.open       = openPort;
        port->new_port.close      = closePort;
        port->new_port.put_buffer = putBufferCallback;
        
        post_plugin->xine_post.audio_input[0] = &port->new_port;
        post_plugin->xine_post.type = PLUGIN_POST;

        objectMapper.insert(audioPort, this);
        
        //post_plugin->dispose = m_output->~AudioDataOutput; //meh TODO
    }
    
    /* code is straight from xine_init_post()
    can't use that function as it only dlopens the plugins
    and our plugin is statically linked in */
    
    post_plugin->running_ticket = (*m_xine).port_ticket;
    post_plugin->xine = m_xine;

    //xine_post_wire_audio_port(source->outputPort(), m_audioPort);
}

int AudioDataOutputXT::openPort(xine_audio_port_t *port_gen, xine_stream_t *stream, uint32_t bits, uint32_t rate, int mode )
{
    post_audio_port_t *port = (post_audio_port_t*)port_gen;
    
    _x_post_rewire((post_plugin_t*)port->post);
    _x_post_inc_usage(port);
    
    port->stream = stream;
    port->bits = bits;
    port->rate = rate;
    port->mode = mode;
    
    //m_output->setChannels(_x_ao_mode2channels(mode));
    
    return port->original_port->open( port->original_port, stream, bits, rate, mode );
}
void AudioDataOutputXT::closePort(xine_audio_port_t *port_gen, xine_stream_t *stream)
{
    post_audio_port_t *port = (post_audio_port_t*)port_gen;
    
    port->stream = NULL;
    port->original_port->close( port->original_port, stream );
    
    _x_post_dec_usage( port );

    //TODO: empty buffers
}

void AudioDataOutputXT::putBufferCallback(xine_audio_port_t * audioPort, audio_buffer_t *buf, xine_stream_t *stream)
{
    QVector<qint16> buffer(buf->num_frames);
    
    for (int i=0; i < buf->num_frames; ++i)
        buffer[i] = buf->mem[i];
    
    objectMapper[audioPort]->m_output->convertAndEmit(buffer);
}

Phonon::Experimental::AudioDataOutput::Format AudioDataOutput::format() const
{
    return m_format;
}

int AudioDataOutput::dataSize() const
{
    return m_dataSize;
}

int AudioDataOutput::sampleRate() const
{
    return 44100;
}

void AudioDataOutput::setFormat(Phonon::Experimental::AudioDataOutput::Format format)
{
    m_format = format;
}

void AudioDataOutput::setDataSize(int size)
{
    m_dataSize = size;
}

typedef QMap<Phonon::Experimental::AudioDataOutput::Channel, QVector<float> > FloatMap;
typedef QMap<Phonon::Experimental::AudioDataOutput::Channel, QVector<qint16> > IntMap;

inline void AudioDataOutput::convertAndEmit(QVector<qint16>& buffer)
{
    //TODO: support floats

    //if (m_format == Phonon::Experimental::AudioDataOutput::FloatFormat) {
    IntMap map;
    map.insert(Phonon::Experimental::AudioDataOutput::LeftChannel, buffer);
    map.insert(Phonon::Experimental::AudioDataOutput::RightChannel, buffer);
    emit dataReady(map);
    //}
    /*else
    {
        IntMap map;
        QVector<qint16> intBuffer(m_dataSize);
        for (int i = 0; i < m_dataSize; ++i)
            intBuffer[i] = static_cast<qint16>(buffer[i] * static_cast<float>(0x7FFF));
        map.insert(Phonon::Experimental::AudioDataOutput::LeftChannel, intBuffer);
        map.insert(Phonon::Experimental::AudioDataOutput::RightChannel, intBuffer);
        emit dataReady(map);
    }*/
}

}} //namespace Phonon::Xine

#include "audiodataoutput.moc"
// vim: sw=4 ts=4
