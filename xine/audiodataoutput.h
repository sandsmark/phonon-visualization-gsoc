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
#include <QVector>
#include <phonon/experimental/audiodataoutputinterface.h>
#include <phonon/experimental/audiodataoutput.h>
#include <phonon/experimental/abstractaudiodataoutput.h>
#include <xine/audio_out.h>

namespace Phonon
{
namespace Xine
{

    class AudioDataOutput : public QObject,
                            public Phonon::Xine::SinkNode,
                            //public Phonon::Experimental::AudioDataOutput,
                            //public Phonon::Experimental::AbstractAudioDataOutput,
                            public Phonon::Experimental::AudioDataOutputInterface
                            
{
    Q_OBJECT
    Q_INTERFACES(Phonon::Experimental::AudioDataOutputInterface Phonon::Xine::SinkNode)

    public:
        AudioDataOutput(QObject *parent);
        ~AudioDataOutput();

        Experimental::AbstractAudioDataOutput *frontendObject() const;
        void setFrontendObject(Experimental::AbstractAudioDataOutput *);

        MediaStreamTypes inputMediaStreamTypes() const { return Phonon::Xine::Audio; }

        friend class AudioDataOutputXT;

    public slots:
        Phonon::Experimental::AudioDataOutput::Format format() const;
        int sampleRate() const;
        int channels() const { return m_channels; }
        void setChannels(int channels) { m_channels = channels; m_pendingData.clear(); }
        int dataSize() { return m_dataSize; }
        void setDataSize(int ds) { m_dataSize = ds; }

    signals:
        void dataReady(const QMap<Phonon::Experimental::AudioDataOutput::Channel, QVector<qint16> > &data);
        void dataReady(const QMap<Phonon::Experimental::AudioDataOutput::Channel, QVector<float> > &data);
        void endOfMedia(int remainingSamples);

    private:
        void packetReady(QVector< qint16 > buffer);

        Experimental::AudioDataOutput::Format m_format;
        int m_channels;
        int m_dataSize;
        QVector<qint16> m_pendingData;
        Experimental::AbstractAudioDataOutput *m_frontend;
};

}} //namespace Phonon::Xine

// vim: sw=4 ts=4 tw=80
#endif // Phonon_XINE_AUDIODATAOUTPUT_H
