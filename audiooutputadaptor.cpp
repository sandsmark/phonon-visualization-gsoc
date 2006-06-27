/*
 * This file was generated by dbusidl2cpp version 0.4
 * when processing input file org.kde.Phonon.AudioOutput.xml
 *
 * dbusidl2cpp is Copyright (C) 2006 Trolltech AS. All rights reserved.
 *
 * This is an auto-generated file.
 */

#include "audiooutputadaptor.h"
#include "audiooutput.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <kdebug.h>

/*
 * Implementation of adaptor class AudioOutputAdaptor
 */

AudioOutputAdaptor::AudioOutputAdaptor(QObject *parent)
   : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

AudioOutputAdaptor::~AudioOutputAdaptor()
{
    // destructor
}

double AudioOutputAdaptor::volume() const
{
    // get the value of property volume
    return qvariant_cast<float>(parent()->property("volume"));
}

void AudioOutputAdaptor::setVolume(double value)
{
    // set the value of property volume
    parent()->setProperty("volume", QVariant::fromValue(static_cast<float>(value)));
}

QString AudioOutputAdaptor::categoryName()
{
    // handle method call org.kde.Phonon.AudioOutput.categoryName
    QString categoryName;
    //QMetaObject::invokeMethod(parent(), "categoryName", Q_RETURN_ARG(QString, categoryName));

    // Alternative:
    categoryName = static_cast<Phonon::AudioOutput *>(parent())->categoryName();
    return categoryName;
}

QString AudioOutputAdaptor::name()
{
    // handle method call org.kde.Phonon.AudioOutput.name
    QString name;
    //QMetaObject::invokeMethod(parent(), "name", Q_RETURN_ARG(QString, name));

    // Alternative:
    name = static_cast<Phonon::AudioOutput *>(parent())->name();
    return name;
}


#include "audiooutputadaptor.moc"