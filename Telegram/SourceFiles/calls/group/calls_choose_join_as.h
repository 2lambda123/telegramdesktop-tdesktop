/*
This file is part of rabbitGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/rabbitGramDesktop/rabbitGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/weak_ptr.h"
#include "base/object_ptr.h"

class PeerData;

namespace Ui {
class Show;
class BoxContent;
} // namespace Ui

namespace Calls::Group {

struct JoinInfo;

class ChooseJoinAsProcess final {
public:
	ChooseJoinAsProcess() = default;
	~ChooseJoinAsProcess();

	enum class Context {
		Create,
		CreateScheduled,
		Join,
		JoinWithConfirm,
		Switch,
	};

	void start(
		not_null<PeerData*> peer,
		Context context,
		std::shared_ptr<Ui::Show> show,
		Fn<void(JoinInfo)> done,
		PeerData *changingJoinAsFrom = nullptr);

private:
	void requestList();
	void processList(std::vector<not_null<PeerData*>> &&list);
	void finish(JoinInfo info);

	struct ChannelsListRequest {
		not_null<PeerData*> peer;
		std::shared_ptr<Ui::Show> show;
		Fn<void(JoinInfo)> done;
		base::has_weak_ptr guard;
		QPointer<Ui::BoxContent> box;
		rpl::lifetime lifetime;
		Context context = Context();
		mtpRequestId id = 0;
		PeerData *changingJoinAsFrom = nullptr;
	};
	std::unique_ptr<ChannelsListRequest> _request;

};

} // namespace Calls::Group
