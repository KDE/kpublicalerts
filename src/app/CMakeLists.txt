# SPDX-FileCopyrightText: 2022 Volker Krause
# SPDX-License-Identifier: BSD-2-Clause

add_executable(publicalerts)

target_sources(publicalerts PRIVATE
    alertsmanager.cpp
    alertssortproxymodel.cpp
    areamodel.cpp
    caputil.cpp
    main.cpp
    qml.qrc
    restapi.cpp
    subscription.cpp
    subscriptionmanager.cpp
)
target_include_directories(publicalerts PRIVATE ${CMAKE_BINARY_DIR})
if (ANDROID)
    target_sources(publicalerts PRIVATE notifyrc.qrc)
endif()

target_link_libraries(publicalerts PRIVATE
    KF5::KWeatherCore
    KUnifiedPush
    KF5::Notifications
    KF5::GuiAddons
    KF5::I18n
    Qt::Quick
    Qt::QuickControls2
    Qt::Positioning
)
if (NOT ANDROID)
    target_link_libraries(publicalerts PRIVATE Qt::Widgets KF5::DBusAddons)
else()
    target_link_libraries(publicalerts PRIVATE OpenSSL::SSL Qt::Svg KF5::Kirigami2)
    kirigami_package_breeze_icons(ICONS
        car
        cloudstatus
        cross-shape
        earthquake
        hotspot
        internet-services
        list-add
        network-wireless-hostspot
        office-chart-polar-stacked
        security-high-symbolic
        weather-storm
    )
endif()

install(TARGETS publicalerts ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
if (NOT ANDROID)
    install(PROGRAMS org.kde.publicalerts.desktop DESTINATION ${KDE_INSTALL_APPDIR})
    configure_file(org.kde.publicalerts.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.publicalerts.service)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.publicalerts.service DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})
    install(FILES publicalerts.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
endif()
