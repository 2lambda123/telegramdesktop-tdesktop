/*
This file is part of rabbitGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/rabbitGramDesktop/rabbitGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/timer.h"
#include "ui/widgets/input_fields.h"

namespace Ui {

class SentCodeField final : public Ui::InputField {
public:
	SentCodeField(
		QWidget *parent,
		const style::InputField &st,
		rpl::producer<QString> placeholder = nullptr,
		const QString &val = QString());

	void setAutoSubmit(int length, Fn<void()> submitCallback);
	void setChangedCallback(Fn<void()> changedCallback);
	[[nodiscard]] QString getDigitsOnly() const;

private:
	void fix();

	// Flag for not calling onTextChanged() recursively.
	bool _fixing = false;

	int _autoSubmitLength = 0;
	Fn<void()> _submitCallback;
	Fn<void()> _changedCallback;

};

class SentCodeCall final {
public:
	SentCodeCall(
		FnMut<void()> callCallback,
		Fn<void()> updateCallback);

	enum class State {
		Waiting,
		Calling,
		Called,
		Disabled,
	};
	struct Status {
		Status() {
		}
		Status(State state, int timeout) : state(state), timeout(timeout) {
		}

		State state = State::Disabled;
		int timeout = 0;
	};
	void setStatus(const Status &status);

	void callDone();

	[[nodiscard]] QString getText() const;

private:
	Status _status;
	base::Timer _timer;
	FnMut<void()> _call;
	Fn<void()> _update;

};

} // namespace Ui
