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
#ifndef Phonon_XINE_AUDIODATAOUTPUT_H
#define Phonon_XINE_AUDIODATAOUTPUT_H



#include "abstractaudiooutput.h"
#include "sourcenode.h"
#include <QVector>
#include <phonon/audiodataoutputinterface.h>
#include <phonon/audiodataoutput.h>
//#include <phonon/abstractaudiodataoutput.h>

extern "C" {
    #define this __this__ //HACK, yeah! (Xine uses “this” as a name for certain variables)
    #define XINE_ENGINE_INTERNAL //we need the port_ticket
    #include <xine/audio_out.h>
    #include <xine/post.h>
    #undef XINE_ENGINE_INTERNAL
    #undef this
}


namespace Phonon
{
namespace Xine
{

class AudioDataOutputXT;
class AudioDataOutput;

typedef struct
{
    post_plugin_t post;

    AudioDataOutputXT *audioDataOutput;
} scope_plugin_t;

class AudioDataOutputXT : public SinkNodeXT, public SourceNodeXT
{
    public:
        AudioDataOutputXT(AudioDataOutput *output);

        xine_post_out_t *audioOutputPort() const;
        
        //callback functions
        static int  openPort(xine_audio_port_t*, xine_stream_t*, uint32_t, uint32_t, int);
        static void closePort(xine_audio_port_t *, xine_stream_t *);
        static void putBufferCallback(xine_audio_port_s*, audio_buffer_s* buf, xine_stream_s* stream);
        static void dispose(post_plugin_t*);

        AudioDataOutput *m_frontend;
    private:
        void rewireTo(SourceNodeXT *);

        int m_channels;

        scope_plugin_t         m_scope;
        xine_audio_port_t *m_audioPort;
        scope_plugin_t       *m_plugin;
        post_audio_port_t      *m_port;
        xine_post_out_t      *m_output;
};

class AudioDataOutput : public QObject,
                        public Phonon::Xine::SinkNode,
                        public Phonon::AudioDataOutputInterface

{
    Q_OBJECT

    Phonon::AudioDataOutput* m_frontend;
    Q_INTERFACES(Phonon::AudioDataOutputInterface Phonon::Xine::SinkNode)

    public:
        AudioDataOutput(QObject *parent);
        ~AudioDataOutput();

        Phonon::AudioDataOutput *frontendObject() const;
        void setFrontendObject(Phonon::AudioDataOutput *);

        MediaStreamTypes inputMediaStreamTypes() const { return Phonon::Xine::Audio; }

        friend class AudioDataOutputXT;

    public slots:
        Phonon::AudioDataOutput::Format format() const;
        int sampleRate() const;
        int channels() const { return m_channels; }
        void setChannels(int channels) { m_channels = channels; m_pendingData.clear(); }
        int dataSize() { return m_dataSize; }
        void setDataSize(int ds) { m_dataSize = ds; }

    signals:
        void dataReady(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > &data);
        void dataReady(const QMap<Phonon::AudioDataOutput::Channel, QVector<float> > &data);
        void endOfMedia(int remainingSamples);

    private:
        void packetReady(QVector<qint16> buffer);

        Phonon::AudioDataOutput::Format m_format;
        int m_channels;
        int m_dataSize;
        QVector<qint16> m_pendingData;
};

}} //namespace Phonon::Xine

// vim: sw=4 ts=4 tw=80
#endif // Phonon_XINE_AUDIODATAOUTPUT_H
