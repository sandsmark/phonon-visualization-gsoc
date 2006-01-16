/*  This file is part of the KDE project
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>

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
#include "audioeffect.h"
#include "ifaces/audioeffect.h"
#include "factory.h"

namespace Kdem2m
{
class AudioEffect::Private
{
	public:
		Private()
		{ }

		QString type;
};
KDEM2M_OBJECT_IMPL( AudioEffect )

QString AudioEffect::type() const
{
	return m_iface ? m_iface->type() : d->type;
}

void AudioEffect::setType( const QString& type )
{
	if( m_iface )
		m_iface->setType( type );
	else
		d->type = type;
}

bool AudioEffect::aboutToDeleteIface()
{
	if( m_iface )
		d->type = m_iface->type();
	return true;
}

void AudioEffect::setupIface()
{	if( !m_iface )
		return;

	// set up attributes
	m_iface->setType( d->type );
}

} //namespace Kdem2m

#include "audioeffect.moc"

// vim: sw=4 ts=4 tw=80 noet
