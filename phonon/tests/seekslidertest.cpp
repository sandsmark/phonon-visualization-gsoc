/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "qtesthelper.h"
#include "loadfakebackend.h"
#include "../seekslider.h"
#include "../mediaobject.h"
#include "../path.h"
#include "../audiooutput.h"

#include <QtCore/QObject>
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QSlider>

#include <QtTest/QTest>

#include <cstdlib>

class SeekSliderTest : public QObject
{
    Q_OBJECT
    private Q_SLOTS:
        void initTestCase();
        void testEnabled();
        void setMedia();
        void playMedia();
        void seekWithSlider();
        void cleanupTestCase();
    private:
        Phonon::SeekSlider *ss;
        QSlider *qslider;
        QLabel *qlabel;
        Phonon::MediaObject *media;
};

using namespace Phonon;

void SeekSliderTest::initTestCase()
{
    Phonon::loadFakeBackend();
    ss = new SeekSlider;
    QVERIFY(ss != 0);
    qslider = ss->findChild<QSlider *>();
    qlabel = ss->findChild<QLabel *>();
    QVERIFY(qslider != 0);
    QVERIFY(qlabel != 0);
    media = new MediaObject(this);
    createPath(media, new AudioOutput(Phonon::MusicCategory, media));
}

void SeekSliderTest::testEnabled()
{
    QVERIFY(!qslider->isEnabled());
    ss->setMediaObject(0);
    QVERIFY(!qslider->isEnabled());
}

void SeekSliderTest::setMedia()
{
    QVERIFY(media->state() == Phonon::LoadingState);
    QUrl url(testUrl());
    media->setCurrentSource(url);
    ss->setMediaObject(media);
    QVERIFY(!qslider->isEnabled());
    ss->setMediaObject(0);
    QVERIFY(!qslider->isEnabled());
    ss->setMediaObject(media);
    QVERIFY(!qslider->isEnabled());
}

void SeekSliderTest::playMedia()
{
    media->play();
    QSignalSpy stateSpy(media, SIGNAL(stateChanged(Phonon::State, Phonon::State)));
    while (media->state() != Phonon::PlayingState) {
        QVERIFY(QTest::kWaitForSignal(media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), 4000));
        QVERIFY(!stateSpy.isEmpty());
        switch (qvariant_cast<Phonon::State>(stateSpy.last().first())) {
        case Phonon::PlayingState:
        case Phonon::PausedState:
        case Phonon::BufferingState:
            QVERIFY(qslider->isEnabled());
            ss->setMediaObject(0);
            QVERIFY(!qslider->isEnabled());
            ss->setMediaObject(media);
            QVERIFY(qslider->isEnabled());
            break;
        case Phonon::ErrorState:
        case Phonon::StoppedState:
        case Phonon::LoadingState:
            QVERIFY(!qslider->isEnabled());
            break;
        }
    }
}

void SeekSliderTest::seekWithSlider()
{
    // click on the slider to seek
}

void SeekSliderTest::cleanupTestCase()
{
    delete media;
    qslider = 0;
    delete ss;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("seekslidertest");
    SeekSliderTest tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "seekslidertest.moc"
// vim: ts=4
