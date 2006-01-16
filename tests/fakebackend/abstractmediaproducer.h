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
#ifndef Kdem2m_FAKE_ABSTRACTMEDIAPRODUCER_H
#define Kdem2m_FAKE_ABSTRACTMEDIAPRODUCER_H

#include <QObject>
#include "../../ifaces/abstractmediaproducer.h"

class QTimer;

namespace Kdem2m
{
namespace Fake
{
	class AbstractMediaProducer : public QObject, virtual public Ifaces::AbstractMediaProducer
	{
		Q_OBJECT
		public:
			AbstractMediaProducer( QObject* parent );
			virtual ~AbstractMediaProducer();
			virtual bool addVideoPath( Ifaces::VideoPath* videoPath );
			virtual bool addAudioPath( Ifaces::AudioPath* audioPath );
			virtual State state() const;
			virtual bool hasVideo() const;
			virtual bool seekable() const;
			virtual long currentTime() const;
			virtual long tickInterval() const;
			virtual long setTickInterval( long newTickInterval );
			virtual void play();
			virtual void pause();
			virtual void stop();
			virtual void seek( long time );

		signals:
			void stateChanged( Kdem2m::State newstate, Kdem2m::State oldstate );
			void tick( long time );

		public:
			virtual QObject* qobject() { return this; }
			virtual const QObject* qobject() const { return this; }

		protected:
			void setState( State );

		private slots:
			void emitTick();

		private:
			State m_state;
			QTimer* m_tickTimer;
			long m_fakeTime;
	};
}} //namespace Kdem2m::Fake

// vim: sw=4 ts=4 tw=80 noet
#endif // Kdem2m_FAKE_ABSTRACTMEDIAPRODUCER_H
