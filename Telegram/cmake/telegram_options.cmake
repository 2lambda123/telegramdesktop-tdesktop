# This file is part of Telegram Desktop,
# the official desktop application for the Telegram messaging service.
#
# For license and copyright information please follow this link:
# https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL

option(TDESKTOP_FORCE_GTK_FILE_DIALOG "Force using GTK file dialog (Linux only)." OFF)
option(TDESKTOP_DISABLE_REGISTER_CUSTOM_SCHEME "Disable automatic 'tg://' URL scheme handler registration." ${DESKTOP_APP_USE_PACKAGED})
option(TDESKTOP_DISABLE_NETWORK_PROXY "Disable all code for working through Socks5 or MTProxy." OFF)
option(TDESKTOP_DISABLE_DESKTOP_FILE_GENERATION "Disable automatic '.desktop' file generation (Linux only)." ${DESKTOP_APP_USE_PACKAGED})
option(TDESKTOP_DISABLE_GTK_INTEGRATION "Disable all code for GTK integration (Linux only)." OFF)
option(TDESKTOP_USE_PACKAGED_TGVOIP "Find libtgvoip using CMake instead of bundled one." ${DESKTOP_APP_USE_PACKAGED})
option(TDESKTOP_API_TEST "Use test API credentials." OFF)
set(TDESKTOP_API_ID "0" CACHE STRING "Provide 'api_id' for the Telegram API access.")
set(TDESKTOP_API_HASH "" CACHE STRING "Provide 'api_hash' for the Telegram API access.")
set(TDESKTOP_LAUNCHER_FILENAME "" CACHE STRING "Use custom desktop file name (Linux only).")

if (TDESKTOP_API_TEST)
    set(TDESKTOP_API_ID 17349)
    set(TDESKTOP_API_HASH 344583e45741c457fe1862106095a5eb)
endif()

if (TDESKTOP_API_ID STREQUAL "0" OR TDESKTOP_API_HASH STREQUAL "")
    message(FATAL_ERROR
    " \n"
    " PROVIDE: -D TDESKTOP_API_ID=[API_ID] -D TDESKTOP_API_HASH=[API_HASH]\n"
    " \n"
    " > To build your version of Telegram Desktop you're required to provide\n"
    " > your own 'api_id' and 'api_hash' for the Telegram API access.\n"
    " >\n"
    " > How to obtain your 'api_id' and 'api_hash' is described here:\n"
    " > https://core.telegram.org/api/obtaining_api_id\n"
    " >\n"
    " > If you're building the application not for deployment,\n"
    " > but only for test purposes you can use TEST ONLY credentials,\n"
    " > which are very limited by the Telegram API server:\n"
    " >\n"
    " > api_id: 17349\n"
    " > api_hash: 344583e45741c457fe1862106095a5eb\n"
    " >\n"
    " > Your users will start getting internal server errors on login\n"
    " > if you deploy an app using those 'api_id' and 'api_hash'.\n"
    " ")
endif()

if (NOT DESKTOP_APP_SPECIAL_TARGET STREQUAL "")
    set(TDESKTOP_FORCE_GTK_FILE_DIALOG ON)
endif()

if (TDESKTOP_FORCE_GTK_FILE_DIALOG AND TDESKTOP_DISABLE_GTK_INTEGRATION)
    message(FATAL_ERROR "Option TDESKTOP_FORCE_GTK_FILE_DIALOG conflicts with option TDESKTOP_DISABLE_GTK_INTEGRATION.")
endif()

if (DESKTOP_APP_DISABLE_SPELLCHECK)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_DISABLE_SPELLCHECK)
else()
    target_link_libraries(Telegram PRIVATE desktop-app::lib_spellcheck)
endif()

if (disable_autoupdate)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_DISABLE_AUTOUPDATE)
endif()

if (DESKTOP_APP_SPECIAL_TARGET)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_ALLOW_CLOSED_ALPHA)
endif()

if (TDESKTOP_FORCE_GTK_FILE_DIALOG)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_FORCE_GTK_FILE_DIALOG)
endif()

if (TDESKTOP_DISABLE_REGISTER_CUSTOM_SCHEME)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_DISABLE_REGISTER_CUSTOM_SCHEME)
endif()

if (TDESKTOP_DISABLE_NETWORK_PROXY)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_DISABLE_NETWORK_PROXY)
endif()

if (TDESKTOP_DISABLE_DESKTOP_FILE_GENERATION)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_DISABLE_DESKTOP_FILE_GENERATION)
endif()

if (TDESKTOP_DISABLE_GTK_INTEGRATION)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_DISABLE_GTK_INTEGRATION)
endif()

if (TDESKTOP_LAUNCHER_FILENAME)
    target_compile_definitions(Telegram PRIVATE TDESKTOP_LAUNCHER_FILENAME=${TDESKTOP_LAUNCHER_FILENAME})
endif()
