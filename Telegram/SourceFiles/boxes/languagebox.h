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

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014 John Preston, https://desktop.telegram.org
*/
#pragma once

#include "layerwidget.h"

class LanguageBox : public LayeredWidget {
	Q_OBJECT

public:

	LanguageBox();
	void parentResized();
	void animStep(float64 ms);
	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void paintEvent(QPaintEvent *e);
	void startHide();
	~LanguageBox();

public slots:

	void onChange();
	void onRestore();
	void onSave();
	void onClose();

private:

	void hideAll();
	void showAll();

	QVector<FlatRadiobutton*> _langs;
	int32 _width, _height;
	BottomButton _done;

	bool _hiding;
	QPixmap _cache;

	anim::fvalue a_opacity;
};
