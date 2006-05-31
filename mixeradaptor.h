/*
 * This file was generated by dbusidl2cpp version 0.3
 * when processing input file org.kde.MixerIface.xml
 *
 * dbusidl2cpp is Copyright (C) 2006 Trolltech AS. All rights reserved.
 *
 * This is an auto-generated file.
 */

#ifndef MIXERADAPTOR_H_257421146496182
#define MIXERADAPTOR_H_257421146496182

#include <QtCore/QObject>
#include <dbus/qdbus.h>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.kde.Phonon.MixerIface
 */
class MixerIfaceAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Phonon.MixerIface")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.kde.Phonon.MixerIface\" >"
"    <method name=\"name\" >"
"      <arg direction=\"out\" type=\"s\" name=\"name\" />"
"    </method>"
"    <method name=\"categoryName\" >"
"      <arg direction=\"out\" type=\"s\" name=\"categoryName\" />"
"    </method>"
"    <property access=\"readwrite\" type=\"d\" name=\"volume\" />"
"  </interface>"
        "")
public:
    MixerIfaceAdaptor(QObject *parent);
    virtual ~MixerIfaceAdaptor();

public: // PROPERTIES
   Q_PROPERTY(double volume READ volume WRITE setVolume)
    double volume() const;
    void setVolume(double value);

public slots: // METHODS
    QString categoryName();
    QString name();
signals: // SIGNALS
};

#endif
