/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 or version 3 as published by the Free Software
    Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "hardwaredatabase_p.h"
#include <kcomponentdata.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <QtCore/QCache>
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSet>
#include <QtCore/QString>

namespace Phonon
{
namespace HardwareDatabase
{

struct HardwareDatabasePrivate
{
    HardwareDatabasePrivate();
    void readEntry(const QString &udi);

    QSet<QString> knownUdis;
    QCache<QString, Entry> entryCache;
    QString fileName;
};

K_GLOBAL_STATIC(HardwareDatabasePrivate, s_globalDB)

HardwareDatabasePrivate::HardwareDatabasePrivate()
{
    const QString dbFileName = KStandardDirs::locate("data", QLatin1String("libphonon/hardwaredatabase"));
    if (dbFileName.isEmpty()) {
        // no database, we're useless
        return;
    }
    const QString cacheFileName =
        KGlobal::mainComponent().dirs()->saveLocation("cache", QLatin1String("libphonon/"))
        + QLatin1String("hardwaredatabase");
    const QFileInfo dbFileInfo(dbFileName);
    const QFileInfo cacheFileInfo(cacheFileName);
    QFile cacheFile(cacheFileName);
    QString name;
    QString iconName;
    int pref;
    if (!cacheFileInfo.exists() || cacheFileInfo.lastModified() < dbFileInfo.lastModified()) {
        // update cache file
        const KConfig dbFile(dbFileName, KConfig::CascadeConfig);
        const bool opened = cacheFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        Q_ASSERT(opened);
        QDataStream cacheStream(&cacheFile);
        foreach (const QString &udi, dbFile.groupList()) {
            const KConfigGroup group = dbFile.group(udi);
            name = group.readEntry("name", QString());
            iconName = group.readEntry("icon", QString());
            pref = group.readEntry("initialPreference", 0);
            cacheStream << udi << name << iconName << pref;
            knownUdis.insert(udi);
        }
    } else {
        cacheFile.open(QIODevice::ReadOnly);
        QDataStream cacheStream(&cacheFile);
        QString udi;
        while (!cacheStream.atEnd()) {
            cacheStream >> udi >> name >> iconName >> pref;
            knownUdis.insert(udi);
        }
    }
    cacheFile.close();
    fileName = cacheFileName;
}

void HardwareDatabasePrivate::readEntry(const QString &udi)
{
    QFile cacheFile(fileName);
    cacheFile.open(QIODevice::ReadOnly);
    QDataStream cacheStream(&cacheFile);

    QString readUdi;
    QString name;
    QString iconName;
    int pref;

    const QString udiStart = udi.left(udi.length() - 10); /*10 == strlen("playback_9")*/
    while (!cacheStream.atEnd()) {
        cacheStream >> readUdi >> name >> iconName >> pref;
        if (readUdi.startsWith(udiStart)) {
            s_globalDB->entryCache.insert(udi, new Entry(name, iconName, pref));
            if (readUdi == udi) {
                break;
            }
        }
    }
}

bool contains(const QString &udi)
{
    return s_globalDB->knownUdis.contains(udi);
}

Entry entryFor(const QString &udi)
{
    if (!s_globalDB->entryCache.contains(udi)) {
        s_globalDB->readEntry(udi);
    }
    Entry *e = s_globalDB->entryCache[udi];
    if (e) {
        return *e;
    }
    return Entry();
}

} // namespace HardwareDatabase
} // namespace Phonon