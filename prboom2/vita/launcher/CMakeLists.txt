# PrBoom-Plus Vita Launcher

set(VLAUNCHER_SOURCES
    configs.c
    files.c
    input.c
    main.c
    net.c
    screen.c
    ui.c
    ui_audio.c
    ui_input.c
    ui_main.c
    ui_misc.c
    ui_net.c
    ui_pwads.c
    ui_video.c
    utils.c
)

add_executable(vlauncher.elf ${VLAUNCHER_SOURCES})

target_include_directories(vlauncher.elf PRIVATE ${CMAKE_BINARY_DIR})
target_compile_options(vlauncher.elf PRIVATE -std=gnu99 -Wno-declaration-after-statement -Wno-format-truncation -Wno-stringop-truncation)
target_link_libraries(vlauncher.elf PRIVATE vita2d png jpeg freetype z m c)
target_link_libraries(vlauncher.elf PRIVATE SceLibKernel_stub SceAppMgr_stub SceSysmodule_stub SceCtrl_stub SceTouch_stub SceNet_stub SceNetCtl_stub)
target_link_libraries(vlauncher.elf PRIVATE SceAppUtil_stub ScePower_stub SceCommonDialog_stub SceAudio_stub SceGxm_stub SceDisplay_stub SceHid_stub)
target_link_libraries(vlauncher.elf PRIVATE ScePgf_stub)

vita_create_self(vlauncher.self vlauncher.elf)
