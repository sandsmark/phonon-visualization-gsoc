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

#ifndef SEEKSLIDER_P_H
#define SEEKSLIDER_P_H

#include "seekslider.h"
#include <QBoxLayout>
#include <QSlider>
#include <QLabel>
#include <kicontheme.h>
#include <kiconloader.h>
#include <QPixmap>
#include <kicon.h>
#include <QStyle>

namespace Phonon
{
class AbstractMediaProducer;
class SeekSliderPrivate
{
    Q_DECLARE_PUBLIC(SeekSlider)
    protected:
        SeekSliderPrivate(SeekSlider *parent)
            : layout(QBoxLayout::LeftToRight, parent),
            slider(Qt::Horizontal, parent),
            iconLabel(parent),
            media(0),
            ticking(false),
            icon("player-time")
        {
            const int e = parent->style()->pixelMetric(QStyle::PM_ButtonIconSize);
            iconSize = QSize(e, e);

            slider.setPageStep(5000); // 5 sec
            slider.setSingleStep(500); // 0.5 sec

            layout.setMargin(0);
            layout.setSpacing(2);
            layout.addWidget(&iconLabel, 0, Qt::AlignVCenter);
            layout.addWidget(&slider, 0, Qt::AlignVCenter);

            setEnabled(false);
        }

        SeekSlider *q_ptr;

    private:
        void setEnabled(bool);
        void _k_stateChanged(Phonon::State);
        void _k_mediaDestroyed();
        void _k_seek(int);
        void _k_tick(qint64);
        void _k_length(qint64);
        void _k_seekableChanged(bool);

        QBoxLayout layout;
        QSlider slider;
        QLabel iconLabel;
        AbstractMediaProducer *media;
        bool ticking;
        KIcon icon;
        QSize iconSize;
};
} // namespace Phonon

#endif // SEEKSLIDER_P_H
// vim: sw=4 ts=4 tw=80
