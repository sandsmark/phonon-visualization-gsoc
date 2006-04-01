/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "backend.h"
#include "mediaobject.h"
#include "avcapture.h"
#include "bytestream.h"
#include "audiopath.h"
#include "audioeffect.h"
#include "audiooutput.h"
#include "audiodataoutput.h"
#include "videopath.h"
#include "videoeffect.h"

#include <kgenericfactory.h>
#include "volumefadereffect.h"
#include <QSet>

typedef KGenericFactory<Phonon::Fake::Backend, Phonon::Ifaces::Backend> FakeBackendFactory;
K_EXPORT_COMPONENT_FACTORY( phonon_fake, FakeBackendFactory( "fakebackend" ) )

namespace Phonon
{
namespace Fake
{

Backend::Backend( QObject* parent, const char*, const QStringList& )
	: Ifaces::Backend( parent )
{
}

Backend::~Backend()
{
}

Ifaces::MediaObject*      Backend::createMediaObject( QObject* parent )
{
	return new MediaObject( parent );
}

Ifaces::AvCapture*        Backend::createAvCapture( QObject* parent )
{
	return new AvCapture( parent );
}

Ifaces::ByteStream*       Backend::createByteStream( QObject* parent )
{
	return new ByteStream( parent );
}

Ifaces::AudioPath*        Backend::createAudioPath( QObject* parent )
{
	return new AudioPath( parent );
}

Ifaces::AudioEffect*      Backend::createAudioEffect( QObject* parent )
{
	return new AudioEffect( parent );
}

Ifaces::VolumeFaderEffect*      Backend::createVolumeFaderEffect( QObject* parent )
{
	return new VolumeFaderEffect( parent );
}

Ifaces::AudioOutput*      Backend::createAudioOutput( QObject* parent )
{
	AudioOutput* ao = new AudioOutput( parent );
	m_audioOutputs.append( ao );
	return ao;
}

Ifaces::AudioDataOutput*  Backend::createAudioDataOutput( QObject* parent )
{
	return new AudioDataOutput( parent );
}

Ifaces::VideoPath*        Backend::createVideoPath( QObject* parent )
{
	return new VideoPath( parent );
}

Ifaces::VideoEffect*      Backend::createVideoEffect( QObject* parent )
{
	return new VideoEffect( parent );
}

bool Backend::supportsVideo() const
{
	return true;
}

bool Backend::supportsOSD() const
{
	return false;
}

bool Backend::supportsSubtitles() const
{
	return false;
}

const KMimeType::List& Backend::knownMimeTypes() const
{
	if( m_supportedMimeTypes.isEmpty() )
		const_cast<Backend*>( this )->m_supportedMimeTypes
			<< KMimeType::mimeType( "audio/vorbis" )
			<< KMimeType::mimeType( "audio/x-mp3" )
			<< KMimeType::mimeType( "audio/x-wav" )
			<< KMimeType::mimeType( "video/x-ogm" );
	return m_supportedMimeTypes;
}

QSet<int> Backend::audioOutputDeviceIndexes() const
{
	QSet<int> set;
	set << 10000 << 10001;
	return set;
}

QString Backend::audioOutputDeviceName( int index ) const
{
	switch( index )
	{
		case 10000:
			return "internal Soundcard";
		case 10001:
			return "USB Headset";
	}
	return QString();
}

QString Backend::audioOutputDeviceDescription( int index ) const
{
	Q_UNUSED( index );
	return QString(); // no description
}

QSet<int> Backend::audioCaptureDeviceIndexes() const
{
	QSet<int> set;
	set << 20000 << 20001;
	return set;
}

QString Backend::audioCaptureDeviceName( int index ) const
{
	switch( index )
	{
		case 20000:
			return "Soundcard";
		case 20001:
			return "DV";
		default:
			return QString();
	}
}

QString Backend::audioCaptureDeviceDescription( int index ) const
{
	switch( index )
	{
		case 20000:
			return "first description";
		case 20001:
			return "second description";
		default:
			return QString();
	}
}

int Backend::audioCaptureDeviceVideoIndex( int index ) const
{
	switch( index )
	{
		case 20001:
			return 30001;
		default:
			return -1;
	}
}

QSet<int> Backend::videoCaptureDeviceIndexes() const
{
	QSet<int> set;
	set << 30000 << 30001;
	return set;
}

QString Backend::videoCaptureDeviceName( int index ) const
{
	switch( index )
	{
		case 30000:
			return "USB Webcam";
		case 30001:
			return "DV";
		default:
			return QString();
	}
}

QString Backend::videoCaptureDeviceDescription( int index ) const
{
	switch( index )
	{
		case 30000:
			return "first description";
		case 30001:
			return "second description";
		default:
			return QString();
	}
}

int Backend::videoCaptureDeviceAudioIndex( int index ) const
{
	switch( index )
	{
		case 30001:
			return 20001;
		default:
			return -1;
	}
}

const QStringList& Backend::availableAudioEffects() const
{
	if( m_audioEffects.isEmpty() )
		const_cast<Backend*>( this )->m_audioEffects << "audioEffect1" << "audioEffect2";
	return m_audioEffects;
}

const QStringList& Backend::availableVideoEffects() const
{
	if( m_videoEffects.isEmpty() )
		const_cast<Backend*>( this )->m_videoEffects << "videoEffect1" << "videoEffect2";
	return m_videoEffects;
}

const char* Backend::uiLibrary() const
{
	return "phonon_fakeui";
}

void Backend::freeSoundcardDevices()
{
	foreach( QPointer<AudioOutput> ao, m_audioOutputs )
		if( ao )
			ao->closeDevice();
}

}}

#include "backend.moc"

// vim: sw=4 ts=4 noet
