# SPDX-FileCopyrightText: 2022 Volker Krause
# SPDX-License-Identifier: BSD-2-Clause

add_executable(publicalerts)

target_compile_definitions(publicalerts PRIVATE -DTRANSLATION_DOMAIN=\"kpublicalerts\")
target_sources(publicalerts PRIVATE
    alertsmanager.cpp
    alertssortproxymodel.cpp
    application.cpp
    areamodel.cpp
    caputil.cpp
    geomath.cpp
    main.cpp
    polygonsimplifier.cpp
    restapi.cpp
    subscription.cpp
    subscriptionmanager.cpp
)
target_include_directories(publicalerts PRIVATE ${CMAKE_BINARY_DIR})
if (ANDROID)
    target_sources(publicalerts PRIVATE notifyrc.qrc)
endif()

set_source_files_properties(OSMPlugin.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
ecm_add_qml_module(publicalerts URI "org.kde.publicalerts" NO_PLUGIN
    QML_FILES
        AlertPage.qml
        AlertsPage.qml
        Main.qml
        MapView.qml
        OSMPlugin.qml
        SubscriptionPage.qml
        SubscriptionsPage.qml
    DEPENDENCIES
        "org.kde.kirigami"
        "org.kde.weathercore"
        "QtLocation"
        "QtQuick"
)

target_link_libraries(publicalerts PRIVATE
    KWeatherCore
    KUnifiedPush
    KF6::CoreAddons
    KF6::Notifications
    KF6::GuiAddons
    KF6::I18n
    KF6::I18nQml
    Qt::Quick
    Qt::QuickControls2
    Qt::Positioning
)
if (NOT ANDROID)
    target_link_libraries(publicalerts PRIVATE Qt::Widgets KF6::DBusAddons)
    if (TARGET KF6::Crash)
        target_link_libraries(publicalerts PRIVATE KF6::Crash)
    endif()
else()
    target_link_libraries(publicalerts PRIVATE OpenSSL::SSL Qt::Svg KF6::Kirigami)
    kirigami_package_breeze_icons(ICONS
        car
        cloudstatus
        cross-shape
        earthquake
        edit-delete
        help-about
        hotspot
        internet-services
        list-add
        map-globe
        network-wireless-hotspot
        notifications
        office-chart-polar-stacked
        security-high-symbolic
        weather-storm
    )
    ecm_add_android_apk(publicalerts ANDROID_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
endif()

install(TARGETS publicalerts ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
if (NOT ANDROID)
    install(PROGRAMS org.kde.publicalerts.desktop DESTINATION ${KDE_INSTALL_APPDIR})
    configure_file(org.kde.publicalerts.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.publicalerts.service)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.publicalerts.service DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})
    install(FILES publicalerts.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
endif()
