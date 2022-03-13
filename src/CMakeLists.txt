cmake_minimum_required (VERSION 3.1)
project (ToyRenderEngine)
set(CMAKE_CXX_STANDARD 17)
include("CMakeHelp/PathHelp.cmake")
include("CMakeHelp/Qt5Help.cmake")

#Set deps_libfilelist and Compile deps
FolderTra(NORCS JUSTDIR RESULT deps_libdirlist DIRS "${PROJECT_SOURCE_DIR_FATHER}/deps/" KEYS "*")
message("\n")
message("---------Deps Lib:---------")
foreach(libpath ${deps_libdirlist})
	get_filename_component(libname "${libpath}" NAME)
	list(APPEND deps_libfilelist "${libname}")
	message("${libname}")
endforeach()
message("\n")
AddSubDirRCS("${PROJECT_SOURCE_DIR_FATHER}/deps" "${PROJECT_BINARY_DIR_FATHER}")

#Set prjabsdir prjname prjindir
set(prjabsdir "${CMAKE_CURRENT_LIST_DIR}")
set(prjname "Demo")
set(prjindir "src")
message("\n\n |    CMakeList ${prjname}   |")



#Set libfilelist dllfilelist codefilelis
#Set Qtlib
find_package(Qt5Widgets)  
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Network)
find_package(Qt5Multimedia)
set(qt_libfilelist Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network Qt5::Multimedia)

#Gen Qtcode
QT5_WRAP_UI_DIY(qt_genheader UIDIR "${prjabsdir}/Ui/UiFile" OUTDIR "${prjabsdir}/Ui/GenCode/UiHeader")
QT5_WRAP_CPP_DIY(qt_genmoc HEADERDIR "${prjabsdir}/Ui/Include" OUTDIR "${prjabsdir}/Ui/GenCode/Moc")
set(qtgen_codefilelist ${qt_genheader} ${qt_genmoc})
source_group("/Ui/GenCode/UiHeader" FILES ${qt_genheader})
source_group("/Ui/GenCode/Moc" FILES ${qt_genmoc})

FolderTra(RCS JUSTFILE RESULT src_codefilelist DIRS "${prjabsdir}/" KEYS ${FILEKEYS_INIDE})
FileGroup(RCS SHOWINFO BASEDIR "${prjabsdir}/" IDE_DIRINPRJ "/" KEYS ${FILEKEYS_INIDE})
set(libfilelist ${deps_libfilelist} ${qt_libfilelist})
set(codefilelist ${src_codefilelist} ${qtgen_codefilelist})


#Forbit VisualStudio Compiling Shader
add_definitions(-D FBXSDK_SHARED)
FolderTra(RCS JUSTFILE RESULT shaderfilelist DIRS "${prjabsdir}/" KEYS ${FILEKEYS_SHADER})
set_source_files_properties(${shaderfilelist} PROPERTIES VS_TOOL_OVERRIDE "None")

#Create
AddTarget(EXE PNAME "${prjname}" CODERES ${codefilelist} LIBRES ${libfilelist} INCLUDE_DIRS "${prjabsdir}/Include" ${Qt5Gui_PRIVATE_INCLUDE_DIRS} IDE_PRJINDIR "${prjindir}")



#Copy Qtdll after build
add_custom_command(TARGET ${prjname} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:${prjname}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:${prjname}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:${prjname}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Network> $<TARGET_FILE_DIR:${prjname}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Multimedia> $<TARGET_FILE_DIR:${prjname}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:${prjname}>/platforms/$<TARGET_FILE_NAME:Qt5::QWindowsIntegrationPlugin>
    )

#Compile Test
#ReplaceCmakeFile("${PROJECT_SOURCE_DIR_FATHER}/test" "CMakeHelp/cmake_example/test_base/CMakeLists.txt")
CopyAssets("${prjabsdir}/Assets")
AddSubDirRCS("${PROJECT_SOURCE_DIR_FATHER}/test" "${PROJECT_BINARY_DIR_FATHER}")
