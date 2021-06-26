/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "platform/linux/linux_gtk_integration.h"

#include "base/platform/linux/base_linux_gtk_integration.h"
#include "base/platform/linux/base_linux_gtk_integration_p.h"
#include "platform/linux/linux_gtk_integration_p.h"
#include "platform/linux/linux_gdk_helper.h"
#include "platform/linux/linux_gtk_open_with_dialog.h"

namespace Platform {
namespace internal {

using namespace Platform::Gtk;
using BaseGtkIntegration = base::Platform::GtkIntegration;

namespace {

bool GetImageFromClipboardSupported() {
	return (gtk_clipboard_get != nullptr)
		&& (gtk_clipboard_wait_for_contents != nullptr)
		&& (gtk_clipboard_wait_for_image != nullptr)
		&& (gtk_selection_data_targets_include_image != nullptr)
		&& (gtk_selection_data_free != nullptr)
		&& (gdk_pixbuf_get_pixels != nullptr)
		&& (gdk_pixbuf_get_width != nullptr)
		&& (gdk_pixbuf_get_height != nullptr)
		&& (gdk_pixbuf_get_rowstride != nullptr)
		&& (gdk_pixbuf_get_has_alpha != nullptr)
		&& (gdk_atom_intern != nullptr);
}

} // namespace

GtkIntegration::GtkIntegration() {
}

GtkIntegration *GtkIntegration::Instance() {
	if (!BaseGtkIntegration::Instance()) {
		return nullptr;
	}

	static GtkIntegration instance;
	return &instance;
}

void GtkIntegration::load() {
	static bool Loaded = false;
	Expects(!Loaded);

	if (!BaseGtkIntegration::Instance()->loaded()) {
		return;
	}

	auto &lib = BaseGtkIntegration::Instance()->library();

	LOAD_GTK_SYMBOL(lib, gtk_widget_show);
	LOAD_GTK_SYMBOL(lib, gtk_widget_get_window);
	LOAD_GTK_SYMBOL(lib, gtk_widget_realize);
	LOAD_GTK_SYMBOL(lib, gtk_widget_destroy);
	LOAD_GTK_SYMBOL(lib, gtk_clipboard_get);
	LOAD_GTK_SYMBOL(lib, gtk_clipboard_wait_for_contents);
	LOAD_GTK_SYMBOL(lib, gtk_clipboard_wait_for_image);
	LOAD_GTK_SYMBOL(lib, gtk_selection_data_targets_include_image);
	LOAD_GTK_SYMBOL(lib, gtk_selection_data_free);

	LOAD_GTK_SYMBOL(lib, gdk_atom_intern);

	LOAD_GTK_SYMBOL(lib, gdk_display_get_default);
	LOAD_GTK_SYMBOL(lib, gdk_display_get_monitor);
	LOAD_GTK_SYMBOL(lib, gdk_display_get_primary_monitor);
	LOAD_GTK_SYMBOL(lib, gdk_monitor_get_scale_factor);

	LOAD_GTK_SYMBOL(lib, gdk_pixbuf_get_has_alpha);
	LOAD_GTK_SYMBOL(lib, gdk_pixbuf_get_pixels);
	LOAD_GTK_SYMBOL(lib, gdk_pixbuf_get_width);
	LOAD_GTK_SYMBOL(lib, gdk_pixbuf_get_height);
	LOAD_GTK_SYMBOL(lib, gdk_pixbuf_get_rowstride);

	GdkHelperLoad(lib);

	LOAD_GTK_SYMBOL(lib, gtk_app_chooser_dialog_new);
	LOAD_GTK_SYMBOL(lib, gtk_app_chooser_get_app_info);
	LOAD_GTK_SYMBOL(lib, gtk_app_chooser_get_type);

	Loaded = true;
}

std::optional<int> GtkIntegration::scaleFactor() const {
	if ((gdk_display_get_default == nullptr)
		|| (gdk_display_get_monitor == nullptr)
		|| (gdk_display_get_primary_monitor == nullptr)
		|| (gdk_monitor_get_scale_factor == nullptr)) {
		return std::nullopt;
	}

	const auto display = gdk_display_get_default();
	if (!display) {
		return std::nullopt;
	}

	const auto monitor = [&] {
		if (const auto primary = gdk_display_get_primary_monitor(display)) {
			return primary;
		}
		return gdk_display_get_monitor(display, 0);
	}();

	if (!monitor) {
		return std::nullopt;
	}

	return gdk_monitor_get_scale_factor(monitor);
}

bool GtkIntegration::showOpenWithDialog(const QString &filepath) const {
	return File::internal::ShowGtkOpenWithDialog(filepath);
}

QImage GtkIntegration::getImageFromClipboard() const {
	QImage data;

	if (!GetImageFromClipboardSupported()) {
		return data;
	}

	const auto clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (!clipboard) {
		return data;
	}

	auto gsel = gtk_clipboard_wait_for_contents(
		clipboard,
		gdk_atom_intern("TARGETS", true));

	if (gsel) {
		if (gtk_selection_data_targets_include_image(gsel, false)) {
			auto img = gtk_clipboard_wait_for_image(clipboard);

			if (img) {
				data = QImage(
					gdk_pixbuf_get_pixels(img),
					gdk_pixbuf_get_width(img),
					gdk_pixbuf_get_height(img),
					gdk_pixbuf_get_rowstride(img),
					gdk_pixbuf_get_has_alpha(img)
						? QImage::Format_RGBA8888
						: QImage::Format_RGB888).copy();

				g_object_unref(img);
			}
		}

		gtk_selection_data_free(gsel);
	}

	return data;
}

} // namespace internal
} // namespace Platform
