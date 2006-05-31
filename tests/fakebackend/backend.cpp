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
#include "mediaqueue.h"
#include "avcapture.h"
#include "bytestream.h"
#include "audiopath.h"
#include "audioeffect.h"
#include "audiooutput.h"
#include "audiodataoutput.h"
#include "visualization.h"
#include "videopath.h"
#include "videoeffect.h"
#include "brightnesscontrol.h"

#include <kgenericfactory.h>
#include "volumefadereffect.h"
#include <QSet>
#include "videodataoutput.h"

typedef KGenericFactory<Phonon::Fake::Backend, Phonon::Fake::Backend> FakeBackendFactory;
K_EXPORT_COMPONENT_FACTORY( phonon_fake, FakeBackendFactory( "fakebackend" ) )

namespace Phonon
{
namespace Fake
{

Backend::Backend( QObject* parent, const QStringList& )
	: QObject( parent )
{
}

Backend::~Backend()
{
}

QObject*      Backend::createMediaObject( QObject* parent )
{
	return new MediaObject( parent );
}

QObject*       Backend::createMediaQueue( QObject* parent )
{
	return new MediaQueue( parent );
}

QObject*        Backend::createAvCapture( QObject* parent )
{
	return new AvCapture( parent );
}

QObject*       Backend::createByteStream( QObject* parent )
{
	return new ByteStream( parent );
}

QObject*        Backend::createAudioPath( QObject* parent )
{
	return new AudioPath( parent );
}

QObject*      Backend::createAudioEffect( int effectId, QObject* parent )
{
	return new AudioEffect( effectId, parent );
}

QObject*      Backend::createVolumeFaderEffect( QObject* parent )
{
	return new VolumeFaderEffect( parent );
}

QObject*      Backend::createAudioOutput( QObject* parent )
{
	AudioOutput* ao = new AudioOutput( parent );
	m_audioOutputs.append( ao );
	return ao;
}

QObject*  Backend::createAudioDataOutput( QObject* parent )
{
	return new AudioDataOutput( parent );
}

QObject*    Backend::createVisualization( QObject* parent )
{
	return new Visualization( parent );
}

QObject*        Backend::createVideoPath( QObject* parent )
{
	return new VideoPath( parent );
}

QObject*      Backend::createVideoEffect( int effectId, QObject* parent )
{
	return new VideoEffect( effectId, parent );
}

QObject* Backend::createBrightnessControl( QObject* parent )
{
	return new BrightnessControl( parent );
}

QObject*  Backend::createVideoDataOutput( QObject* parent )
{
	return new VideoDataOutput( parent );
}

bool Backend::supportsVideo() const
{
	return true;
}

bool Backend::supportsOSD() const
{
	return false;
}

bool Backend::supportsFourcc( quint32 fourcc ) const
{
	switch( fourcc )
	{
		case 0x00000000:
			return true;
		default:
			return false;
	}
}

bool Backend::supportsSubtitles() const
{
	return false;
}

QStringList Backend::knownMimeTypes() const
{
	if( m_supportedMimeTypes.isEmpty() )
		const_cast<Backend*>( this )->m_supportedMimeTypes
			<< QLatin1String( "audio/vorbis" )
			//<< QLatin1String( "audio/x-mp3" )
			<< QLatin1String( "audio/x-wav" )
			<< QLatin1String( "video/x-ogm" );
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

QSet<int> Backend::videoOutputDeviceIndexes() const
{
	QSet<int> set;
	set << 40000 << 40001 << 40002 << 40003;
	return set;
}

QString Backend::videoOutputDeviceName( int index ) const
{
	switch( index )
	{
		case 40000:
			return "XVideo";
		case 40001:
			return "XShm";
		case 40002:
			return "X11";
		case 40003:
			return "SDL";
	}
	return QString();
}

QString Backend::videoOutputDeviceDescription( int index ) const
{
	Q_UNUSED( index );
	return QString(); // no description
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

QSet<int> Backend::visualizationIndexes() const
{
	QSet<int> ret;
	return ret;
}

QString Backend::visualizationName( int index ) const
{
	return QString();
}

QString Backend::visualizationDescription( int index ) const
{
	return QString();
}

QSet<int> Backend::audioEffectIndexes() const
{
	QSet<int> ret;
	ret << 0x7F000001;
	return ret;
}

QString Backend::audioEffectName( int index ) const
{
	switch( index )
	{
		case 0x7F000001:
			return "Delay";
	}
	return QString();
}

QString Backend::audioEffectDescription( int index ) const
{
	switch( index )
	{
		case 0x7F000001:
			return "Simple delay effect with time, feedback and level controls.";
	}
	return QString();
}

QSet<int> Backend::videoEffectIndexes() const
{
	QSet<int> ret;
	ret << 0x7E000001;
	return ret;
}

QString Backend::videoEffectName( int index ) const
{
	switch( index )
	{
		case 0x7E000001:
			return "VideoEffect1";
	}
	return QString();
}

QString Backend::videoEffectDescription( int index ) const
{
	switch( index )
	{
		case 0x7E000001:
			return "Description 1";
	}
	return QString();
}

QSet<int> Backend::audioCodecIndexes() const
{
	return QSet<int>();
}

QString Backend::audioCodecName( int index ) const
{
	return QString();
}

QString Backend::audioCodecDescription( int index ) const
{
	return QString();
}

QSet<int> Backend::videoCodecIndexes() const
{
	return QSet<int>();
}

QString Backend::videoCodecName( int index ) const
{
	return QString();
}

QString Backend::videoCodecDescription( int index ) const
{
	return QString();
}

QSet<int> Backend::containerFormatIndexes() const
{
	return QSet<int>();
}

QString Backend::containerFormatName( int index ) const
{
	return QString();
}

QString Backend::containerFormatDescription( int index ) const
{
	return QString();
}

char const* Backend::uiLibrary() const
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
