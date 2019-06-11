/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "ui/abstract_button.h"

#include <rpl/filter.h>
#include <rpl/mappers.h>
#include "core/application.h"

namespace Ui
{
	AbstractButton::AbstractButton(QWidget* parent) :
		RpWidget(parent)
	{
		setMouseTracking(true);

		using namespace rpl::mappers;
		shownValue()
			| rpl::filter(_1 == false)
			| rpl::start_with_next([this]
			{
				clearState();
			}, lifetime());
	}

	void AbstractButton::leaveEventHook(QEvent* e)
	{
		if (_state & StateFlag::Down) return;

		setOver(false, StateChangeSource::ByHover);
		return TWidget::leaveEventHook(e);
	}

	void AbstractButton::enterEventHook(QEvent* e)
	{
		checkIfOver(mapFromGlobal(QCursor::pos()));
		return TWidget::enterEventHook(e);
	}

	void AbstractButton::setAcceptBoth(bool acceptBoth)
	{
		_acceptBoth = acceptBoth;
	}

	void AbstractButton::checkIfOver(QPoint localPos)
	{
		auto over = rect().marginsRemoved(getMargins()).contains(localPos);
		setOver(over, StateChangeSource::ByHover);
	}

	void AbstractButton::mousePressEvent(QMouseEvent* e)
	{
		checkIfOver(e->pos());
		if (_acceptBoth || (e->buttons() & Qt::LeftButton))
		{
			if ((_state & StateFlag::Over) && !(_state & StateFlag::Down))
			{
				auto was = _state;
				_state |= StateFlag::Down;
				onStateChanged(was, StateChangeSource::ByPress);

				e->accept();
			}
		}
	}

	void AbstractButton::mouseMoveEvent(QMouseEvent* e)
	{
		if (rect().marginsRemoved(getMargins()).contains(e->pos()))
		{
			setOver(true, StateChangeSource::ByHover);
		}
		else
		{
			setOver(false, StateChangeSource::ByHover);
		}
	}

	void AbstractButton::mouseReleaseEvent(QMouseEvent* e)
	{
		if (_state & StateFlag::Down)
		{
			const auto was = _state;
			_state &= ~State(StateFlag::Down);

			auto weak = make_weak(this);
			onStateChanged(was, StateChangeSource::ByPress);
			if (!weak)
			{
				return;
			}

			if (was & StateFlag::Over)
			{
				_modifiers = e->modifiers();
				if (const auto callback = _clickedCallback)
				{
					callback();
				}
				else
				{
					emit clicked();
				}
				if (weak)
				{
					_clicks.fire(e->button());
				}
			}
			else
			{
				setOver(false, StateChangeSource::ByHover);
			}
		}
	}

	void AbstractButton::setPointerCursor(bool enablePointerCursor)
	{
		if (_enablePointerCursor != enablePointerCursor)
		{
			_enablePointerCursor = enablePointerCursor;
			updateCursor();
		}
	}

	void AbstractButton::setOver(bool over, StateChangeSource source)
	{
		if (over && !(_state & StateFlag::Over))
		{
			auto was = _state;
			_state |= StateFlag::Over;
			Core::App().registerLeaveSubscription(this);
			onStateChanged(was, source);
		}
		else if (!over && (_state & StateFlag::Over))
		{
			auto was = _state;
			_state &= ~State(StateFlag::Over);
			Core::App().unregisterLeaveSubscription(this);
			onStateChanged(was, source);
		}
		updateCursor();
	}

	void AbstractButton::updateCursor()
	{
		auto pointerCursor = _enablePointerCursor && (_state & StateFlag::Over);
		setCursor(pointerCursor ? style::cur_pointer : style::cur_default);
	}

	void AbstractButton::setDisabled(bool disabled)
	{
		auto was = _state;
		if (disabled && !(_state & StateFlag::Disabled))
		{
			_state |= StateFlag::Disabled;
			onStateChanged(was, StateChangeSource::ByUser);
		}
		else if (!disabled && (_state & StateFlag::Disabled))
		{
			_state &= ~State(StateFlag::Disabled);
			onStateChanged(was, StateChangeSource::ByUser);
		}
	}

	void AbstractButton::clearState()
	{
		auto was = _state;
		_state = StateFlag::None;
		onStateChanged(was, StateChangeSource::ByUser);
	}
} // namespace Ui
