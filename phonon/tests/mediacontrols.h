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

#ifndef PHONON_UI_MEDIACONTROLS_H
#define PHONON_UI_MEDIACONTROLS_H

#include "../phonondefs.h"
#include "../phononnamespace.h"
#include <QtGui/QWidget>

namespace Phonon
{
class MediaObject;
class AudioOutput;
class MediaControlsPrivate;

/**
 * \short Simple widget showing buttons to control an MediaProducer
 * object.
 *
 * This widget shows the standard player controls. There's at least the
 * play/pause and stop buttons. If the media is seekable it shows a seek-slider.
 * Optional controls include a volume control and a loop control button.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class MediaControls : public QWidget
{
    Q_OBJECT
    K_DECLARE_PRIVATE(MediaControls)
    /**
     * This property holds whether the slider showing the progress of the
     * playback is visible.
     *
     * By default the slider is visible. It is enabled/disabled automatically
     * depending on whether the media can be seeked or not.
     */
    Q_PROPERTY(bool seekSliderVisible READ isSeekSliderVisible WRITE setSeekSliderVisible)

    /**
     * This property holds whether the slider controlling the volume is visible.
     *
     * By default the slider is visible if an AudioOutput has been set with
     * setAudioOutput.
     *
     * \see setAudioOutput
     */
    Q_PROPERTY(bool volumeControlVisible READ isVolumeControlVisible WRITE setVolumeControlVisible)

    /**
     * This property holds whether the button controlling loop behaviour is
     * visible.
     *
     * By default the loop button is hidden.
     */
    Q_PROPERTY(bool loopControlVisible READ isLoopControlVisible WRITE setLoopControlVisible)
    public:
        /**
         * Constructs a media control widget with a \p parent.
         */
        MediaControls(QWidget *parent = 0);
        ~MediaControls();

        bool isSeekSliderVisible() const;
        bool isVolumeControlVisible() const;
        bool isLoopControlVisible() const;

    public Q_SLOTS:
        void setSeekSliderVisible(bool);
        void setVolumeControlVisible(bool);
        void setLoopControlVisible(bool);

        /**
         * Sets the media producer object to be controlled by this widget.
         */
        void setMediaObject(MediaObject *);

        /**
         * Sets the audio output object to be controlled by this widget.
         */
        void setAudioOutput(AudioOutput *audioOutput);


    private:
        Q_PRIVATE_SLOT(k_func(), void _k_stateChanged(Phonon::State, Phonon::State))
        Q_PRIVATE_SLOT(k_func(), void _k_mediaDestroyed())
        Q_PRIVATE_SLOT(k_func(), void _k_finished())

        MediaControlsPrivate *const k_ptr;
};

} // namespace Phonon

// vim: sw=4 ts=4 tw=80
#endif // PHONON_UI_MEDIACONTROLS_H
