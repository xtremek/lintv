project(lintvscheduler)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

add_subdirectory( doc )
add_subdirectory( icons )
add_subdirectory( pics )
add_subdirectory( src )
add_subdirectory( stylesheets )

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} src ../global src/ui)

set ( global_SRCS
    ../global/lintvprocess.cpp
    ../global/lintvdeviceinfo.cpp
    ../global/lintvdevicectl.cpp
    ../global/lintvglobal.cpp
    ../global/lintvtools.cpp
    ../global/ivtvutils.cpp
    ../global/lintvprogram.cpp
)

set ( ui_SRCS
    src/ui/lintvprogrameditor.cpp
    src/ui/lintvprogramwidget.cpp
)


# Now let's do LinTVScheduler
set ( lintvscheduler_SRCS
    ${global_SRCS}
    ${ui_SRCS}
    src/lintvscheduler.cpp
    src/lintvschedulermain.cpp
)

kde4_add_ui_files(lintvscheduler_SRCS src/ui/program_editor.ui)
kde4_add_kcfg_files(lintvscheduler_SRCS src/lintvschedulersettings.kcfgc)

qt4_add_dbus_adaptor(lintvscheduler_SRCS src/org.kde.LinTVScheduler.xml 
                                  src/lintvscheduler.h LinTVScheduler )


kde4_add_executable(lintvscheduler ${lintvscheduler_SRCS})

target_link_libraries(lintvscheduler ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} )

install(TARGETS lintvscheduler ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES lintvscheduler.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES src/lintvscheduler.kcfg DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES src/lintvschedulerui.rc DESTINATION  
                ${DATA_INSTALL_DIR}/lintvscheduler )