set(deps_libfilelist "RGraph" "RTmpl")

#Set prjabsdir prjname prjindir
set(prjabsdir "${CMAKE_CURRENT_LIST_DIR}") #工程文件夹
get_filename_component(prjname "${prjabsdir}" NAME) #文件夹名位工程名
get_filename_component(prjfather "${prjabsdir}" DIRECTORY) 
string(REPLACE "${PROJECT_SOURCE_DIR_FATHER}" "" root2prj_reladir "${prjfather}")	#项目根目录到工程的相对路径父文件夹
message("\n\n |    CMakeList ${prjname}   |")

#Set libfilelist dllfilelist codefilelist
FolderTra(RCS JUSTFILE RESULT src_codefilelist DIRS "${prjabsdir}/" KEYS ${FILEKEYS_INIDE})
FileGroup(RCS SHOWINFO BASEDIR "${prjabsdir}/" IDE_DIRINPRJ "/" KEYS ${FILEKEYS_INIDE})
set(codefilelist ${src_codefilelist})
set(libfilelist ${deps_libfilelist})

#Create
AddTarget(LIB PNAME "${prjname}" CODERES ${codefilelist} LIBRES ${libfilelist} INCLUDE_DIRS "${prjabsdir}/Include" IDE_PRJINDIR "${root2prj_reladir}")
