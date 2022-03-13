message(STATUS "include PathHelp.cmake")

get_filename_component(PROJECT_SOURCE_DIR_FATHER "${PROJECT_SOURCE_DIR}" DIRECTORY)
get_filename_component(PROJECT_BINARY_DIR_FATHER "${PROJECT_BINARY_DIR}" DIRECTORY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR_FATHER}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR_FATHER}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR_FATHER}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR_FATHER}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR_FATHER}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR_FATHER}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR_FATHER}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR_FATHER}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR_FATHER}/lib")

set(FILEKEYS_CODE "*.h" "*.hpp" "*.cpp" "*.c" "*.inl")
set(FILEKEYS_SHADER "*.hlsl")
set(FILEKEYS_INIDE ${FILEKEYS_CODE} ${FILEKEYS_SHADER} "*.ui" "*.qrc" "*.dds" "*.fbx")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakePredefinedTargets")

#         |是否递归| |只获取文件/文件夹/两者都获取|  |是否打印搜索字符|     |是否打印搜索结果|   |返回的搜索结果|                         |搜索的目录列表|                                               |搜索的后缀列表|
#FolderTra(RCS/NORCS      JUSTFILE/JUSTDIR/None         SHOWKEY/None           SHOWRES/None      RESULT returnlist      DIRS "${CMAKE_CURRENT_LIST_DIR}/A" "${CMAKE_CURRENT_LIST_DIR}/B"             KEYS "*.h" "*.cpp")
function(FolderTra)

	set(options RCS NORCS JUSTDIR JUSTFILE SHOWKEY SHOWRES)
    set(oneValueArgs RESULT)
    set(multiValueArgs DIRS KEYS)
	cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	if((ARGS_NORCS OR ARGS_RCS) AND ARGS_RESULT AND ARGS_DIRS AND ARGS_KEYS)
		
		foreach(dirpath ${ARGS_DIRS})
			foreach(key ${ARGS_KEYS})
				list(APPEND keylist "${dirpath}${key}")
			endforeach()
		endforeach()
		
		#非递归遍历文件夹
		if(ARGS_NORCS)
			file(GLOB gbres LIST_DIRECTORIES true ${keylist})
		endif()
		
		#递归遍历文件夹
		if(ARGS_RCS)
			file(GLOB_RECURSE gbres LIST_DIRECTORIES true ${keylist})
		endif()
		
		if(ARGS_JUSTDIR)	#只要文件夹
			foreach(fileordir ${gbres})
			if (IS_DIRECTORY ${fileordir})    
				LIST(APPEND realres ${fileordir})
			endif()
			endforeach()
		elseif(ARGS_JUSTFILE)	#只要文件
			foreach(fileordir ${gbres})
			if (NOT IS_DIRECTORY ${fileordir})    
				LIST(APPEND realres ${fileordir})
			endif()
			endforeach()
		else()
			set(${realres} ${gbres} PARENT_SCOPE)
		endif()
		
		set(${ARGS_RESULT} ${realres} PARENT_SCOPE)
		
		#打印输出
		if(ARGS_SHOWKEY OR ARGS_SHOWRES)
		
			message("")
			message("FolderTra")
			if(ARGS_SHOWKEY)
				message("")
				foreach(key ${keylist})
					message("${key}")
				endforeach()
				message("")
			endif()
			
			if(ARGS_SHOWRES)
				message("")
				foreach(res ${realres})
					message("${res}")
				endforeach()
				message("")
			endif()
			
		endif()
		
	endif()
	
endfunction()

#        |递归/非递归|         |是否打印映射信息|               |映射根目录|                  |根目录对应IDE的Project下的哪个目录|     |映射的目标文件后缀列表|
#FileGroup(RCS/NORCS          SHOWINFO/None         BASEDIR "${CMAKE_CURRENT_LIST_DIR}/"               IDE_DIRINPRJ "/"                   KEYS "*.h" "*.cpp")
function(FileGroup)
	set(options RCS NORCS SHOWINFO)
    set(oneValueArgs BASEDIR IDE_DIRINPRJ)
    set(multiValueArgs KEYS)
	cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	

	if((ARGS_NORCS OR ARGS_RCS) AND ARGS_BASEDIR AND ARGS_IDE_DIRINPRJ AND ARGS_KEYS)
	
		#非递归IDE映射
		if(ARGS_NORCS)
			
			if(ARGS_SHOWINFO)
				message("")
				message("FileGroup")
			endif()
			
			FolderTra(NORCS JUSTFILE RESULT filelist DIRS "${ARGS_BASEDIR}" KEYS ${ARGS_KEYS})
			list(LENGTH filelist len)
			if (${len} STRGREATER 0)  
				source_group(${ARGS_IDE_DIRINPRJ} FILES ${filelist})
			endif()
			
			if(ARGS_SHOWINFO)
				message("${ARGS_IDE_DIRINPRJ}---------->IDE: ${ARGS_IDE_DIRINPRJ}     ${len}")
			endif()
			
		endif()
		
		#递归IDE映射
		if(ARGS_RCS)
		
			if(ARGS_SHOWINFO)
				message("")
				message("FileGroup")
			endif()
			
			FolderTra(NORCS JUSTFILE RESULT filelist DIRS "${ARGS_BASEDIR}" KEYS ${ARGS_KEYS})
			list(LENGTH filelist len)
			if (${len} STRGREATER 0)  
				source_group(${ARGS_IDE_DIRINPRJ} FILES ${filelist})
			endif()
			
			if(ARGS_SHOWINFO)
				message("${ARGS_IDE_DIRINPRJ}---------->IDE: ${ARGS_IDE_DIRINPRJ}     ${len}")
			endif()
			
			
			
			FolderTra(RCS JUSTDIR RESULT dirlist DIRS "${ARGS_BASEDIR}" KEYS "*")

			foreach(dir ${dirlist})
				
				string(REPLACE "${ARGS_BASEDIR}" "" relabasedir "${dir}")
				
				FolderTra(NORCS JUSTFILE RESULT filelist DIRS "${dir}/" KEYS ${ARGS_KEYS})
				list(LENGTH filelist len)
				if (${len} STRGREATER 0)  
					source_group("${ARGS_IDE_DIRINPRJ}${relabasedir}" FILES ${filelist})
				endif()
				
				if(ARGS_SHOWINFO)
					message("${dir}---------->IDE: ${ARGS_IDE_DIRINPRJ}${relabasedir}     ${len}")
				endif()
			endforeach()
			
			
			
		endif()
		
		
	endif()
endfunction()


#           |生成目标类型|     |生成目标名字|    |参与编译的代码列表，例如.cpp .h等的路径|     | 参与编译的库列表，例如.lib(Windows) .a(Linux) .so(Linux)的路径,或者通过add_library CMake内部生成的库等|    相当于添加了环境变量，便于代码内部include用<>直接找到路径        项目在IDE中的哪个文件夹
#AddTarget(  LIB/DLL/EXE      PNAME "Demo"         CODERES ${libfilelist}                                           LIBRES ${libfilelist}                                                                       INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Include"              IDE_PRJINDIR "Lib")
function(AddTarget)
	set(options LIB DLL EXE)
    set(oneValueArgs PNAME IDE_PRJINDIR) 
    set(multiValueArgs CODERES LIBRES INCLUDE_DIRS)
	cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	if((ARGS_LIB OR ARGS_DLL OR ARGS_EXE) AND ARGS_PNAME AND (ARGS_CODERES OR ARGS_LIBRES) OR ARGS_INCLUDE_DIRS AND ARGS_IDE_PRJINDIR)
		if(ARGS_LIB)
			add_library (${ARGS_PNAME} ${ARGS_CODERES})
		elseif(ARGS_DLL)
			add_library (${ARGS_PNAME} SHARED ${ARGS_CODERES})
		elseif(ARGS_EXE)
			add_executable (${ARGS_PNAME} ${ARGS_CODERES})
		endif()
		target_link_libraries(${ARGS_PNAME} ${ARGS_LIBRES})#生成对象链接库
		target_include_directories(${ARGS_PNAME} PUBLIC ${ARGS_INCLUDE_DIRS}) #必须要加了这句，否则include无法用<>找到路径
		set_target_properties(${ARGS_PNAME} PROPERTIES FOLDER ${ARGS_IDE_PRJINDIR}) #设置在Project在IDE内的路径
	endif()
	
endfunction()

#递归basedir文件夹，搜索CMakeLists.txt文件，对含有此文件的文件夹进行add_subdirectory，并使得build生成的文件保留basedir下目录结构输出到buiddir中
function(AddSubDirRCS basedir buiddir)
	
	message("\n")
	message("--------------AddSubDirRCS(Build)-------------")
	
	get_filename_component(basename ${basedir} NAME)
	
	FolderTra(RCS JUSTDIR RESULT dirlist DIRS "${basedir}/" KEYS "*")
	
	set(cmakelist_dirs "")
	foreach(dir ${dirlist})
		if(EXISTS "${dir}/CMakeLists.txt")
			list(APPEND cmakelist_dirs ${dir})
		endif()
	endforeach()
	
	foreach(dir ${cmakelist_dirs})
		string(REPLACE "${basedir}" "" relabasedir "${dir}")
		add_subdirectory("${dir}" "${buiddir}/${basename}${relabasedir}")
		message("${dir}---------->Build: ${buiddir}/${basename}${relabasedir}")
	endforeach()
	
	message("\n")
	
endfunction()

function(ReplaceCmakeFile basedir newcmake)
	
	message("\n")
	message("--------------ReplaceCmakeFile(Test)-------------")
	
	FolderTra(RCS JUSTDIR RESULT dirlist DIRS "${basedir}/" KEYS "*")
	
	foreach(dir ${dirlist})
		if(EXISTS "${dir}/CMakeLists.txt")
			file(COPY ${newcmake} DESTINATION "${dir}")
			message("${newcmake}---------->Build: ${dir}")
		endif()
	endforeach()
	
	message("\n")
	
endfunction()

function(CopyAssets basedir)
	
	message("\n")
	message("--------------CopyAssets(Test)-------------")
	
	set(goalpath "${PROJECT_SOURCE_DIR_FATHER}/build/default/bin" "${PROJECT_SOURCE_DIR_FATHER}/build/default/proj")
	
	FolderTra(RCS JUSTDIR RESULT dirlist DIRS "${PROJECT_SOURCE_DIR_FATHER}/test/" KEYS "*")
	
	foreach(dir ${dirlist})
		if(EXISTS "${dir}/CMakeLists.txt")
			string(REPLACE "${PROJECT_SOURCE_DIR_FATHER}/test/" "${PROJECT_SOURCE_DIR_FATHER}/build/default/test/" relabasedir "${dir}")
			list(APPEND goalpath ${relabasedir})
		endif()
	endforeach()
	
	foreach(path ${goalpath})
		file(COPY ${basedir} DESTINATION ${path})
		message("${basedir}---------->CopyTo: ${path}/Assets")
	endforeach()
	
	message("\n")
	
endfunction()