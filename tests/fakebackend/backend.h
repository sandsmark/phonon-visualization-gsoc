/*  This file is part of the KDE project
    Copyright (C) 2004-2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#ifndef Phonon_FAKE_BACKEND_H
#define Phonon_FAKE_BACKEND_H

#include "../../ifaces/backend.h"

class KUrl;

namespace Phonon
{
namespace Ifaces
{
	class MediaObject;
	class AvCapture;
	class ByteStream;

	class AudioPath;
	class AudioEffect;
	class VolumeFaderEffect;
	class AudioOutput;
	class AudioDataOutput;
	class AudioFftOutput;

	class VideoPath;
	class VideoEffect;
}
namespace Fake
{

	class PHONON_EXPORT Backend : public Ifaces::Backend
	{
		Q_OBJECT
		public:
			Backend( QObject* parent, const char*, const QStringList& args );
			virtual ~Backend();

			virtual Ifaces::MediaObject*      createMediaObject( QObject* parent );
			virtual Ifaces::AvCapture*        createAvCapture( QObject* parent );
			virtual Ifaces::ByteStream*       createByteStream( QObject* parent );

			virtual Ifaces::AudioPath*        createAudioPath( QObject* parent );
			virtual Ifaces::AudioEffect*      createAudioEffect( QObject* parent );
			virtual Ifaces::VolumeFaderEffect* createVolumeFaderEffect( QObject* parent );
			virtual Ifaces::AudioOutput*      createAudioOutput( QObject* parent );
			virtual Ifaces::AudioDataOutput*  createAudioDataOutput( QObject* parent );
			virtual Ifaces::AudioFftOutput*   createAudioFftOutput( QObject* parent );

			virtual Ifaces::VideoPath*        createVideoPath( QObject* parent );
			virtual Ifaces::VideoEffect*      createVideoEffect( QObject* parent );

			virtual bool supportsVideo() const;
			virtual bool supportsOSD() const;
			virtual bool supportsSubtitles() const;
			virtual const KMimeType::List& knownMimeTypes() const;

			virtual int audioSourceCount() const;
			virtual QString audioSourceName( int index ) const;
			virtual QString audioSourceDescription( int index ) const;
			virtual int audioSourceVideoIndex( int index ) const;

			virtual int videoSourceCount() const;
			virtual QString videoSourceName( int index ) const;
			virtual QString videoSourceDescription( int index ) const;
			virtual int videoSourceAudioIndex( int index ) const;

			// effects
			virtual const QStringList& availableAudioEffects() const;
			virtual const QStringList& availableVideoEffects() const;

			virtual const char* uiLibrary() const;
			//virtual const char* uiSymbol() const;

		private:
			KMimeType::List m_supportedMimeTypes;
			QStringList m_audioEffects, m_videoEffects;
	};
}} // namespace Phonon::Ifaces

// vim: sw=4 ts=4 noet tw=80
#endif // Phonon_FAKE_BACKEND_H
