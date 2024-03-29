/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#include "mediacontrols.h"
#include "../mediaobject.h"
#include "../audiooutput.h"
#include "../phonondefs_p.h"
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <QtGui/QBoxLayout>
#include <QtGui/QToolButton>
#include "../seekslider.h"
#include "../volumeslider.h"
#include <kicon.h>
#include "mediacontrols_p.h"

namespace Phonon
{

MediaControls::MediaControls(QWidget *parent)
    : QWidget(parent),
    k_ptr(new MediaControlsPrivate(this))
{
    setMaximumHeight(32);
}

MediaControls::~MediaControls()
{
    delete k_ptr;
}

bool MediaControls::isSeekSliderVisible() const
{
    K_D(const MediaControls);
    return d->seekSlider.isVisible();
}

bool MediaControls::isVolumeControlVisible() const
{
    K_D(const MediaControls);
    return d->volumeSlider.isVisible();
}

bool MediaControls::isLoopControlVisible() const
{
    K_D(const MediaControls);
    return d->loopButton.isVisible();
}

void MediaControls::setMediaObject(MediaObject *media)
{
    K_D(MediaControls);
    if (d->media) {
        disconnect(d->media, SIGNAL(destroyed()), this, SLOT(_k_mediaDestroyed()));
        disconnect(d->media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this,
                SLOT(_k_stateChanged(Phonon::State, Phonon::State)));
        disconnect(d->media, SIGNAL(finished()), this, SLOT(_k_finished()));
        disconnect(&d->playButton, SIGNAL(clicked()), d->media, SLOT(play()));
        disconnect(&d->pauseButton, SIGNAL(clicked()), d->media, SLOT(pause()));
        disconnect(&d->stopButton, SIGNAL(clicked()), d->media, SLOT(stop()));
    }
    d->media = media;
    if (media) {
        connect(media, SIGNAL(destroyed()), SLOT(_k_mediaDestroyed()));
        connect(media, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
                SLOT(_k_stateChanged(Phonon::State, Phonon::State)));
        connect(d->media, SIGNAL(finished()), this, SLOT(_k_finished()));
        connect(&d->playButton, SIGNAL(clicked()), media, SLOT(play()));
        connect(&d->pauseButton, SIGNAL(clicked()), media, SLOT(pause()));
        connect(&d->stopButton, SIGNAL(clicked()), media, SLOT(stop()));
    }

    d->seekSlider.setMediaObject(media);
}

void MediaControls::setAudioOutput(AudioOutput *audioOutput)
{
    K_D(MediaControls);
    d->volumeSlider.setAudioOutput(audioOutput);
    d->volumeSlider.setVisible(audioOutput != 0);
}

void MediaControls::setSeekSliderVisible(bool vis)
{
    K_D(MediaControls);
    d->seekSlider.setVisible(vis);
}

void MediaControls::setVolumeControlVisible(bool vis)
{
    K_D(MediaControls);
    d->volumeSlider.setVisible(vis);
}

void MediaControls::setLoopControlVisible(bool vis)
{
    K_D(MediaControls);
    d->loopButton.setVisible(vis);
}

void MediaControlsPrivate::_k_stateChanged(State newstate, State)
{
    switch(newstate)
    {
    case Phonon::LoadingState:
    case Phonon::PausedState:
    case Phonon::StoppedState:
        playButton.show();
        pauseButton.hide();
        break;
    case Phonon::BufferingState:
    case Phonon::PlayingState:
        playButton.hide();
        pauseButton.show();
        break;
    case Phonon::ErrorState:
        return;
    }
}

void MediaControlsPrivate::_k_mediaDestroyed()
{
    media = 0;
}

void MediaControlsPrivate::_k_finished()
{
    if (loopButton.isChecked()) {
        Q_ASSERT(media->state() == Phonon::StoppedState);
        media->play();
    }
}

} // namespace Phonon

// vim: sw=4 ts=4
