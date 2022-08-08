# SPDX-FileCopyrightText: 2022 Volker Krause
# SPDX-License-Identifier: BSD-2-Clause

add_executable(publicalerts)

target_sources(publicalerts PRIVATE
    alertsmanager.cpp
    main.cpp
    qml.qrc
    subscriptionmanager.cpp
)

target_include_directories(publicalerts PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(publicalerts PRIVATE
    KF5::KWeatherCore
    KF5::I18n
    Qt::Quick
)
if (NOT ANDROID)
    target_link_libraries(publicalerts PRIVATE Qt::Widgets)
endif()

install(TARGETS publicalerts ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
