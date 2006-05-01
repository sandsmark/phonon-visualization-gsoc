/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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
#ifndef PHONON_IFACES_VIDEODATAOUTPUT_H
#define PHONON_IFACES_VIDEODATAOUTPUT_H

#include "abstractvideooutput.h"
#include "../videodataoutput.h"

template<class T> class QVector;

namespace Phonon
{
	class IntDataConsumer;
	class FloatDataConsumer;
namespace Ifaces
{
	/**
	 * \author Matthias Kretz <kretz@kde.org>
	 */
	class VideoDataOutput : virtual public AbstractVideoOutput
	{
		public:
			virtual Phonon::VideoDataOutput::Format format() const = 0;
			virtual int frameRate() const = 0;
			virtual void setFormat( Phonon::VideoDataOutput::Format format ) = 0;

		protected: //signals
			virtual void frameReady( const Phonon::VideoFrame& frame ) = 0;
			virtual void endOfMedia() = 0;
	};
}} //namespace Phonon::Ifaces

// vim: sw=4 ts=4 tw=80 noet
#endif // PHONON_IFACES_VIDEODATAOUTPUT_H
