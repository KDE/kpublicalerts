# SPDX-FileCopyrightText: 2022 Volker Krause
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

project(kpublicalerts)

set(PROJECT_VERSION "24.02")

set(CMAKE_CXX_STANDARD 20)
set(QT_MIN_VERSION 6.5)
set(KF_MIN_VERSION 6.8)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMPoQmTools)
include(ECMAddTests)
include(ECMQmlModule)
include(ECMAddAndroidApk)
include(ECMDeprecationSettings)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX KPUBLICALERTS
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Svg Positioning Location)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS CoreAddons Kirigami I18n GuiAddons Notifications)
find_package(KWeatherCore REQUIRED)
find_package(KUnifiedPush REQUIRED)

if (NOT ANDROID)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS DBusAddons)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
    find_package(KF6 ${KF_MIN_VERSION} OPTIONAL_COMPONENTS Crash)
else()
    if (NOT DEFINED BREEZEICONS_DIR AND EXISTS ${CMAKE_SOURCE_DIR}/../breeze-icons)
        set(BREEZEICONS_DIR ${CMAKE_SOURCE_DIR}/../breeze-icons)
    endif()
    find_package(OpenSSL REQUIRED)
endif()

ki18n_install(po)

ecm_set_disabled_deprecation_versions(
    QT 6.8
    KF 6.10
)

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
