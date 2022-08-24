# SPDX-FileCopyrightText: 2022 Volker Krause
# SPDX-License-Identifier: BSD-2-Clause

add_executable(publicalerts)

target_sources(publicalerts PRIVATE
    alertsmanager.cpp
    areamodel.cpp
    caputil.cpp
    main.cpp
    qml.qrc
    subscription.cpp
    subscriptionmanager.cpp
)

target_include_directories(publicalerts PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(publicalerts PRIVATE
    KF5::KWeatherCore
    KUnifiedPush
    KF5::Notifications
    KF5::DBusAddons
    KF5::GuiAddons
    KF5::I18n
    Qt::Quick
    Qt::Positioning
)
if (NOT ANDROID)
    target_link_libraries(publicalerts PRIVATE Qt::Widgets)
endif()

install(TARGETS publicalerts ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.publicalerts.desktop DESTINATION ${KDE_INSTALL_APPDIR})
configure_file(org.kde.publicalerts.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.publicalerts.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.publicalerts.service DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})
install(FILES publicalerts.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
