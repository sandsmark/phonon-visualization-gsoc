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
#include "mediaobject.h"


namespace Phonon
{
namespace Xine
{


AudioDataOutputXT::AudioDataOutputXT(AudioDataOutput *output) :
                    SinkNodeXT("AudioDataOutput"),
                    SourceNodeXT("AudioDataOutput"),
                    m_frontend(output),
                    m_audioPort(0)

{
    m_xine = Backend::xine();

    // Dummy audio input port, until we get the proper one
    xine_audio_port_t *port = xine_open_audio_driver(m_xine, "none", 0);

    // Allocate a new scope plugin
    m_plugin = (scope_plugin_t*)qMalloc(sizeof(scope_plugin_t));

    // It is also a post plugin
    post_plugin_t *post_plugin  = (post_plugin_t*)m_plugin;

    //1 audio input, 0 video inputs
    _x_post_init(post_plugin, 1, 0);

    // Intercept the null audio port (until we get the proper one)
    intercept(port);

    /* code is straight from xine_init_post()
    can't use that function as it only dlopens the plugins
    and our plugin is statically linked in */

    post_plugin->running_ticket = (*m_xine).port_ticket;
    post_plugin->xine = m_xine;

    // Store a reference to our own object in the post plugin struct
    m_plugin->audioDataOutput = this;
}

AudioDataOutputXT::~AudioDataOutputXT()
{
    // Let Xine dispose of our post plugin
    ((post_plugin_t*)m_plugin)->dispose((post_plugin_t*)m_plugin);

    // Free the memory held by our plugin
    delete m_plugin;
}

/// Rewires this node to the specified sourcenode
void AudioDataOutputXT::rewireTo(SourceNodeXT *source)
{
    debug() << Q_FUNC_INFO << "rewiring to " << source;
    if (!source->audioOutputPort()) { // I can't get no satisfaction
        debug() << Q_FUNC_INFO << " no audioport in source";
        return;
    }

    // Make sure the source is sane
    source->assert();

    // Get the audio input port in our post plugin
    xine_post_in_t *target = (xine_post_in_t*)xine_post_input(
                                    &((post_plugin_t*)m_plugin)->xine_post,
                                    const_cast<char*>("audio in"));

    if (!xine_post_wire(source->audioOutputPort(), target)) {
        qWarning() << Q_FUNC_INFO << ": Failed to rewire!";
        return;
    }
    m_postOutput = source->audioOutputPort();

    // Make sure things went okay
    source->assert();
    SinkNodeXT::assert();
}

/// Returns this Source's audio output port
xine_post_out_t *AudioDataOutputXT::audioOutputPort() const
{
    return m_postOutput;
}

/// Intercepts a given Xine audio port (called from AudioOutput)
void AudioDataOutputXT::intercept(xine_audio_port_t *p) {

    if (p == m_audioPort) // we're already intercepting this one
        return;
    m_audioPort = p;

    post_in_t  *input;
    post_out_t *output;

    post_plugin_t *post_plugin  = (post_plugin_t*)m_plugin;

    // Populate the port with dummy functions
    post_audio_port_t *port = _x_post_intercept_audio_port(post_plugin, m_audioPort, &input, &output);
    /* TODO:
     *  Do we leak these ports? Or is the Xine reference counting enough?
     *  Stay tuned for valgrind and interesting questions!
     */

    if (!port) {
        qWarning() << Q_FUNC_INFO << "unable to allocate port! (out of memory?)";
        delete post_plugin;
        return;
    }

    // Put in our own callbacks
    port->new_port.open       = openPort;
    port->new_port.close      = closePort;
    port->new_port.put_buffer = putBufferCallback;

    // Store the audio port for future use
    m_audioPort = &port->new_port;

    // Wire in the port input into our post plugin
    post_plugin->xine_post.audio_input[0] = &port->new_port;
    post_plugin->xine_post.type = PLUGIN_POST;
}

/// Callback function, opens the xine port
int AudioDataOutputXT::openPort(xine_audio_port_t *port_gen,
                                xine_stream_t *stream,
                                uint32_t bits,
                                uint32_t rate,
                                int mode)
{
    // Reference to the relevant object
    AudioDataOutputXT *that = ((scope_plugin_t*)((post_audio_port_t*)port_gen)->post)->audioDataOutput;

    post_audio_port_t *port = (post_audio_port_t*)port_gen;

    _x_post_rewire((post_plugin_t*)port->post);
    _x_post_inc_usage(port);

    port->stream = stream;
    port->bits = bits;
    port->rate = rate;
    port->mode = mode;

    // Set the new audio stream parameters
    that->m_channels = _x_ao_mode2channels(mode);
    that->m_frontend->setChannels(that->m_channels);
    that->m_frontend->m_sampleRate = rate;

    return port->original_port->open(port->original_port, stream, bits, rate, mode);
}

/// Callback function, closes the xine port
void AudioDataOutputXT::closePort(xine_audio_port_t *port_gen, xine_stream_t *stream)
{
    debug() << Q_FUNC_INFO << " closing port " << port_gen;
    post_audio_port_t *port = (post_audio_port_t*)port_gen;

    // This is the same as closing the port, according to comments in the Xine source
    port->stream = NULL;
    port->original_port->close(port->original_port, stream);

    _x_post_dec_usage(port);
}

/// Callback function, receives audio data
void AudioDataOutputXT::putBufferCallback(xine_audio_port_t * port_gen, audio_buffer_t *buf, xine_stream_t *stream)
{
    AudioDataOutputXT *that = ((scope_plugin_t*)((post_audio_port_t*)port_gen)->post)->audioDataOutput;

    // Get the number of samples (audio frames * audio channels)
    int samples = buf->num_frames * that->m_channels;

    // Store the audio data in a QVector
    QVector<qint16> buffer(samples);
    for (int i=0; i<samples; ++i)
        buffer[i] = buf->mem[i];

    // Present the audio data to our frontend
    that->m_frontend->packetReady(buffer);

    /* Send the audio buffer back to the original port.
       This notifies Xine that we have finished processing
       this buffer, so Xine can give us a new one */
    post_audio_port_t *port = (post_audio_port_t*)port_gen;
    port->original_port->put_buffer(port->original_port, buf, stream);
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
    K_XT(AudioDataOutput);
    delete xt;
}

inline void AudioDataOutput::packetReady(const QVector<qint16> buffer)
{
    //TODO: support floats, we currently only handle ints
    if (m_format == Phonon::AudioDataOutput::FloatFormat)
        return;

    // Add the new audio data to our pool
    m_pendingData += buffer;

    // While we have enough audio data for one signal
    while (m_pendingData.size() / m_channels > dataSize())
    {
        QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > map;

        /* If there's only one channel, we duplicate the data
           into both channels in the QMap */
        if (m_channels==1)
        {
            QVector<qint16> data = m_pendingData.mid(0, dataSize());
            m_pendingData.remove(0, dataSize());
            map.insert(Phonon::AudioDataOutput::LeftChannel, data);
            map.insert(Phonon::AudioDataOutput::RightChannel, data);
            emit dataReady(map);
        }
        else if (m_channels==2)
        {
            QVector<qint16> left, right;

            /* Copy out the interleaved data into the
               appropriate channels.
               This isn't very optimal. */
            for (int i=0; i < dataSize()*2; i+=2)
            {
                left.append(m_pendingData[i]);
                right.append(m_pendingData[i+1]);
            }
            // Remove the data we just copied out
            m_pendingData.remove(0, dataSize() * 2);

            // Insert into the map, and emit
            map.insert(Phonon::AudioDataOutput::LeftChannel, left);
            map.insert(Phonon::AudioDataOutput::RightChannel, right);
            emit dataReady(map);
        }
    }
}

void AudioDataOutput::upstreamEvent(Event *e)
{
    Q_ASSERT(e);
    if (e->type() == Event::IsThereAXineEngineForMe) {
        // yes there is
        MediaObject *mediaObject = dynamic_cast<MediaObject*>(m_source);
        if (mediaObject)
            SourceNode::downstreamEvent(new HeresYourXineStreamEvent(mediaObject->stream()));
    } else
        SourceNode::upstreamEvent(e);
}

}} //namespace Phonon::Xine

#include "audiodataoutput.moc"
// vim: sw=4 ts=4
