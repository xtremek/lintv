project(lintv)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

add_subdirectory( doc )
add_subdirectory( icons )
add_subdirectory( pics )
add_subdirectory( src )

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} src ../global src/backend src/ui)

set ( global_SRCS
    ../global/lintvprocess.cpp
    ../global/lintvdeviceinfo.cpp
    ../global/lintvdevicectl.cpp
    ../global/lintvglobal.cpp
    ../global/lintvtools.cpp
    ../global/ivtvutils.cpp
    ../global/lintvprogram.cpp
    ../global/lintvmixer.cpp
)


set ( backend_SRCS 
    src/backend/lintvrecord.cpp
    src/backend/lintvplay.cpp
)

set ( ui_SRCS
    src/ui/lintvview.cpp
    src/ui/lintvconfigdialog.cpp
    src/ui/lintvpages.cpp
    src/ui/lintvcolordialog.cpp
    src/ui/lintvscreenshotwidgets.cpp
    src/ui/lintvrenamedialog.cpp
    src/ui/lintvdeviceinfodialog.cpp
    src/ui/lintvstatuswidget.cpp
)

set ( lintv_SRCS
    ${global_SRCS}
    ${backend_SRCS}
    ${ui_SRCS}
    src/lintv.cpp
    src/lintvmain.cpp
)

kde4_add_ui_files ( lintv_SRCS 
    src/ui/snapshot_settings.ui
    src/ui/lintvview_base.ui
    src/ui/audio_settings.ui
    src/ui/video_settings.ui
    src/ui/device_info.ui
)

kde4_add_kcfg_files ( lintv_SRCS src/lintvsettings.kcfgc )

qt4_add_dbus_adaptor(lintv_SRCS src/org.kde.LinTV.xml src/lintv.h LinTV )

kde4_add_executable ( lintv ${lintv_SRCS} )

target_link_libraries(lintv ${KDE4_KDEUI_LIBS} ${KDE4_KDE3SUPPORT_LIBS} 
                                                ${QT_QTUITOOLS_LIBRARY}
                                                ${KDE4_KIO_LIBS} )

install(TARGETS lintv ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES lintv.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES src/lintv.kcfg DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES src/lintvui.rc src/lintv.notifyrc DESTINATION  ${DATA_INSTALL_DIR}/lintv )
install( FILES src/ui/record_settings.ui DESTINATION ${DATA_INSTALL_DIR}/lintv/ui )