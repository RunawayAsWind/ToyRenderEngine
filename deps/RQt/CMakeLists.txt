set(deps_libfilelist "")

#Set prjabsdir prjname prjindir
set(prjabsdir "${CMAKE_CURRENT_LIST_DIR}") #工程文件夹
get_filename_component(prjname "${prjabsdir}" NAME) #文件夹名位工程名
get_filename_component(prjfather "${prjabsdir}" DIRECTORY) 
string(REPLACE "${PROJECT_SOURCE_DIR_FATHER}" "" root2prj_reladir "${prjfather}")	#项目根目录到工程的相对路径父文件夹
message("\n\n |    CMakeList ${prjname}   |")

#Set libfilelist dllfilelist codefilelist
find_package(Qt5Widgets)  
find_package(Qt5Core)
find_package(Qt5Gui)
set(qt_libfilelist Qt5::Widgets Qt5::Core Qt5::Gui)
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

#Create
AddTarget(LIB PNAME "${prjname}" CODERES ${codefilelist} LIBRES ${libfilelist} INCLUDE_DIRS "${prjabsdir}/Include" ${Qt5Gui_PRIVATE_INCLUDE_DIRS} IDE_PRJINDIR "${root2prj_reladir}")
