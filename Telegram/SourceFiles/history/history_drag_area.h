/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2016 John Preston, https://desktop.telegram.org
*/
#pragma once

#include "ui/twidget.h"
#include "ui/effects/rect_shadow.h"

class DragArea : public TWidget {
	Q_OBJECT

public:
	DragArea(QWidget *parent);

	void setText(const QString &text, const QString &subtext);

	void otherEnter();
	void otherLeave();

	void step_appearance(float64 ms, bool timer);

	bool overlaps(const QRect &globalRect) {
		if (isHidden() || _a_appearance.animating()) return false;

		return QRect(st::dragPadding.left(),
					 st::dragPadding.top(),
					 width() - st::dragPadding.left() - st::dragPadding.right(),
					 height() - st::dragPadding.top() - st::dragPadding.bottom()
					 ).contains(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
	}

	void hideFast();

protected:
	void paintEvent(QPaintEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void dragEnterEvent(QDragEnterEvent *e) override;
	void dragLeaveEvent(QDragLeaveEvent *e) override;
	void dropEvent(QDropEvent *e) override;
	void dragMoveEvent(QDragMoveEvent *e) override;

signals:
	void dropped(const QMimeData *data);

public slots:
	void hideStart();
	void hideFinish();

	void showStart();

private:
	bool _hiding, _in;

	anim::fvalue a_opacity;
	anim::cvalue a_color;
	Animation _a_appearance;

	Ui::RectShadow _shadow;

	QString _text, _subtext;

};
