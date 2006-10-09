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

#include "globalconfig.h"
#include <QSet>
#include <QList>
#include "factory.h"
#include "objectdescription.h"
#include "phonondefs_p.h"

namespace Phonon
{
GlobalConfig::GlobalConfig( QObject *parent )
	: QObject( parent )
	, m_config( KSharedConfig::openConfig( "phononrc", false, false ) )
{
}

GlobalConfig::~GlobalConfig()
{
}

QList<int> GlobalConfig::audioOutputDeviceListFor( Phonon::Category category ) const
{
	//The devices need to be stored independently for every backend
	//FIXME: backendName() is a translated string
	const KConfigGroup configGroup( const_cast<KSharedConfig*>( m_config.data() ),
			QLatin1String( "AudioOutputDevice" ) + Factory::self()->backendName() );

	//First we lookup the available devices directly from the backend
	QSet<int> deviceIndexes;
	QObject *backendObject = Factory::self()->backend();
	pBACKEND_GET1( QSet<int>, deviceIndexes, "objectDescriptionIndexes", ObjectDescriptionType, Phonon::AudioOutputDeviceType );
	QList<int> defaultList = deviceIndexes.toList();
	qSort( defaultList );

	//Now the list from the phononrc file
	QList<int> deviceList = configGroup.readEntry<QList<int> >( QLatin1String( "Category" ) +
			QString::number( static_cast<int>( category ) ), defaultList );

	QMutableListIterator<int> i( deviceList );
	while( i.hasNext() )
		if( 0 == defaultList.removeAll( i.next() ) )
			//if there are devices in phononrc that the backend doesn't report, remove them from the list
			i.remove();
	//if the backend reports more devices that are not in phononrc append them to the list
	deviceList += defaultList;

	return deviceList;
}

int GlobalConfig::audioOutputDeviceFor( Phonon::Category category ) const
{
	return audioOutputDeviceListFor( category ).first();
}

} // namespace Phonon

#include "globalconfig.moc"

// vim: sw=4 ts=4 noet