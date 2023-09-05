/*
This file is part of rabbitGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/rabbitGramDesktop/rabbitGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Platform::AutostartTask {

void Toggle(bool enabled, Fn<void(bool)> done);
void RequestState(Fn<void(bool)> callback);
void OpenSettings();

} // namespace Platform::AutostartTask
