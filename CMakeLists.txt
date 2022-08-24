# SPDX-FileCopyrightText: 2022 Volker Krause
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

project(kpublicalerts)

set(PROJECT_VERSION "22.08")

set(CMAKE_CXX_STANDARD 20)
set(QT5_MIN_VERSION 5.15)
set(KF5_MIN_VERSION 5.96)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMPoQmTools)
include(ECMAddTests)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX KPUBLICALERTS
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt${QT_MAJOR_VERSION} ${QT5_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Svg Positioning)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Kirigami2 DBusAddons I18n GuiAddons Notifications)
find_package(KF5KWeatherCore REQUIRED)
find_package(KUnifiedPush REQUIRED)
if (NOT ANDROID)
    find_package(Qt${QT_MAJOR_VERSION} ${QT5_MIN_VERSION} REQUIRED COMPONENTS Widgets)
endif()

ki18n_install(po)

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
