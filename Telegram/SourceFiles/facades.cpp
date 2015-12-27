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
Copyright (c) 2014-2015 John Preston, https://desktop.telegram.org
*/
#include "stdafx.h"

#include "window.h"
#include "mainwidget.h"

#include "layerwidget.h"

namespace App {

	void sendBotCommand(const QString &cmd, MsgId replyTo) {
		if (MainWidget *m = main()) m->sendBotCommand(cmd, replyTo);
	}

	void insertBotCommand(const QString &cmd) {
		if (MainWidget *m = main()) m->insertBotCommand(cmd);
	}

	void searchByHashtag(const QString &tag, PeerData *inPeer) {
		if (MainWidget *m = main()) m->searchMessages(tag + ' ', (inPeer && inPeer->isChannel()) ? inPeer : 0);
	}

	void openPeerByName(const QString &username, bool toProfile, const QString &startToken) {
		if (MainWidget *m = main()) m->openPeerByName(username, toProfile, startToken);
	}

	void joinGroupByHash(const QString &hash) {
		if (MainWidget *m = main()) m->joinGroupByHash(hash);
	}

	void stickersBox(const QString &name) {
		if (MainWidget *m = main()) m->stickersBox(MTP_inputStickerSetShortName(MTP_string(name)));
	}

	void openLocalUrl(const QString &url) {
		if (MainWidget *m = main()) m->openLocalUrl(url);
	}

	bool forward(const PeerId &peer, ForwardWhatMessages what) {
		if (MainWidget *m = main()) return m->onForward(peer, what);
		return false;
	}

	void removeDialog(History *history) {
		if (MainWidget *m = main()) m->removeDialog(history);
	}

	void showSettings() {
		if (Window *win = wnd()) win->showSettings();
	}

}

namespace Ui {

	void showStickerPreview(DocumentData *sticker) {
		if (MainWidget *m = App::main()) m->ui_showStickerPreview(sticker);
	}

	void hideStickerPreview() {
		if (MainWidget *m = App::main()) m->ui_hideStickerPreview();
	}

	void showLayer(LayeredWidget *box, ShowLayerOptions options) {
		if (Window *w = App::wnd()) {
			w->ui_showLayer(box, options);
		} else {
			delete box;
		}
	}

	void hideLayer(bool fast) {
		if (Window *w = App::wnd()) w->ui_showLayer(0, ShowLayerOptions(CloseOtherLayers) | (fast ? ForceFastShowLayer : AnimatedShowLayer));
	}

	bool isLayerShown() {
		if (Window *w = App::wnd()) return w->ui_isLayerShown();
		return false;
	}

	bool isMediaViewShown() {
		if (Window *w = App::wnd()) return w->ui_isMediaViewShown();
		return false;
	}

	bool isGifBeingChosen() {
		if (MainWidget *m = App::main()) return m->ui_isGifBeingChosen();
		return false;
	}

	void clipRepaint(ClipReader *reader) {
		const GifItems &items(App::gifItems());
		GifItems::const_iterator it = items.constFind(reader);
		if (it != items.cend()) {
			if (reader->currentDisplayed()) {
				return;
			}
			Ui::repaintHistoryItem(it.value());
		}
		if (Window *w = App::wnd()) w->ui_clipRepaint(reader);
	}

	void repaintHistoryItem(const HistoryItem *item) {
		if (!item) return;
		if (MainWidget *m = App::main()) m->ui_repaintHistoryItem(item);
	}

	void repaintSavedGif(const LayoutSavedGif *layout) {
		if (!layout) return;
		if (MainWidget *m = App::main()) m->ui_repaintSavedGif(layout);
	}

	bool isSavedGifVisible(const LayoutSavedGif *layout) {
		if (MainWidget *m = App::main()) return m->ui_isSavedGifVisible(layout);
		return false;
	}

	void showPeerHistory(const PeerId &peer, MsgId msgId, bool back) {
		if (MainWidget *m = App::main()) m->ui_showPeerHistory(peer, msgId, back);
	}

	void showPeerHistoryAsync(const PeerId &peer, MsgId msgId) {
		if (MainWidget *m = App::main()) {
			QMetaObject::invokeMethod(m, SLOT(ui_showPeerHistoryAsync(quint64,qint32)), Qt::QueuedConnection, Q_ARG(quint64, peer), Q_ARG(qint32, msgId));
		}
	}

}

namespace Notify {

	void userIsBotChanged(UserData *user) {
		if (MainWidget *m = App::main()) m->notify_userIsBotChanged(user);
	}

	void userIsContactChanged(UserData *user, bool fromThisApp) {
		if (MainWidget *m = App::main()) m->notify_userIsContactChanged(user, fromThisApp);
	}

	void botCommandsChanged(UserData *user) {
		if (MainWidget *m = App::main()) m->notify_botCommandsChanged(user);
	}

	void migrateUpdated(PeerData *peer) {
		if (MainWidget *m = App::main()) m->notify_migrateUpdated(peer);
	}

	void clipStopperHidden(ClipStopperType type) {
		if (MainWidget *m = App::main()) m->notify_clipStopperHidden(type);
	}

	void clipReinit(ClipReader *reader) {
		const GifItems &items(App::gifItems());
		GifItems::const_iterator it = items.constFind(reader);
		if (it != items.cend()) {
			HistoryItem *item = it.value();

			bool stopped = false;
			if (reader->paused()) {
				if (MainWidget *m = App::main()) {
					if (!m->isItemVisible(item)) { // stop animation if it is not visible
						if (HistoryMedia *media = item->getMedia()) {
							media->stopInline(item);
							if (DocumentData *document = media->getDocument()) { // forget data from memory
								document->forget();
							}
							stopped = true;
						}
					}
				}
			}
			if (!stopped) {
				item->initDimensions(); // can delete reader and items entry it
				Notify::historyItemResized(item);
				Notify::historyItemLayoutChanged(item);
			}
		}
		if (Window *w = App::wnd()) w->notify_clipReinit(reader);
	}

	void historyItemResized(const HistoryItem *item, bool scrollToIt) {
		if (MainWidget *m = App::main()) m->notify_historyItemResized(item, scrollToIt);
	}

	void historyItemLayoutChanged(const HistoryItem *item) {
		if (MainWidget *m = App::main()) m->notify_historyItemLayoutChanged(item);
	}

}
