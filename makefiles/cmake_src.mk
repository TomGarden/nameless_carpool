# 之前我们都是使用 makefile 直接编译代码的
# 随着代码量主键增大 , cmake 感觉更方便些
# cmake 使用不够熟练 , 暂时组装命令还是是用 make 来组装粘接


.RECIPEPREFIX = ># 标定命令行前缀,默认命令行前缀是 tab 字符，更改为 ‘>’



make_file_path      = $(dir $(word 1,$(MAKEFILE_LIST)))
pro_dir             = $(realpath $(make_file_path)..)
cmake_output_dir	  = $(pro_dir)/build
tom_doc_file_dir    = $(cmake_output_dir)/tom_doc_file_dir
executable_run_dir  = $(pro_dir)/tmp_dir/run_path

debugInputFileName  = debugInput.json




# region 定义几个便利函数

# 如果文件夹不存在,创建一个文件夹
create_dir = 								\
	if [ ! -d "$(1)" ]; then 	\
		mkdir -p "$(1)"; 				\
	fi

log_black    = echo "\033[30m $(1) \033[0m"
log_rad      = echo "\033[31m $(1) \033[0m"
log_green    = echo "\033[32m $(1) \033[0m"
log_yellow   = echo "\033[33m $(1) \033[0m"
log_blue     = echo "\033[34m $(1) \033[0m"
log_purple   = echo "\033[35m $(1) \033[0m"
log_sky_blue = echo "\033[36m $(1) \033[0m"
log_white    = echo "\033[37m $(1) \033[0m"


# endregion

all : 
# 进入 cmakefile.txt 所在目录
>  cd $(pro_dir) && pwd
>	 $(call log_rad,开始执行 cmake 动作)
# 执行 cmake 指令
>  /usr/bin/cmake  \
  --no-warn-unused-cli \
  -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
  -DCMAKE_BUILD_TYPE:STRING=Debug \
  -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/aarch64-linux-gnu-gcc \
  -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/aarch64-linux-gnu-g++ \
  -S$(pro_dir) \
  -B$(cmake_output_dir) \
   -G "Unix Makefiles" 

>	 $(call log_rad,cmake 执行完毕 ; 开始执行 make 动作,生成可执行文件[$(cmake_output_dir)])
# 进入 cmake 输出文件目录 , 并执行 make 指令生成可执行文件
> cd $(cmake_output_dir) && pwd && make

.PHONY : all


# 清空编译产物
.PHONY : clean
clean :
>	 $(call log_rad,清空所有 cmake make 产物)
>  rm -rf $(cmake_output_dir)

clean_log:
>	 $(call log_rad,清空软件执行日志)
>  rm -rf $(pro_dir)/tmp_dir/run_log/*

copy_doc:


>	 $(call log_rad,拷贝文档到编译目录 [$(tom_doc_file_dir)])

>	 $(call create_dir,$(tom_doc_file_dir))
>  cp $(pro_dir)/doc/nameless_carpool.software.usage.txt   $(cmake_output_dir)
>  cp $(pro_dir)/doc/$(debugInputFileName)  			 				 $(cmake_output_dir)

# 执行编译结果
run :
>	 $(call log_rad,启动可执行文件)
>  cd $(executable_run_dir)
# >  $(cmake_output_dir)/NamelessCarpool.run  --input_file=$(tom_doc_file_dir)/$(debugInputFileName)
>  $(cmake_output_dir)/NamelessCarpool.run  --input_file


reload_cgi :
>	 $(call log_rad,重启 CGI)
>  ssh -i /mount_point/data/_beyourself/_global_config_file/ssh/login_remote_ssh  \
				tom@172.17.0.3 /mount_point/data/_document/nginx_web_server/fcgi_reload.sh 	


test :
> pwd

sub__clean__all__copy_doc__clean_log__reload_cgi__run :
> cd $(pro_dir)
> make --file=makefiles/cmake_src.mk clean all copy_doc clean_log reload_cgi run

sub__clean__all__copy_doc__clean_log______________run :
> cd $(pro_dir)
> make --file=makefiles/cmake_src.mk clean all copy_doc clean_log            run

sub_________all__copy_doc__clean_log______________run :
> cd $(pro_dir)
> make --file=makefiles/cmake_src.mk       all copy_doc clean_log            run

sub_________all__copy_doc__clean_log__reload_cgi__run :
> cd $(pro_dir)
> make --file=makefiles/cmake_src.mk       all copy_doc clean_log reload_cgi run

sub___________________________________reload_cgi__run :
> cd $(pro_dir)
> make --file=makefiles/cmake_src.mk                              reload_cgi run



#		clear && make --file=makefiles/cmake_src.mk clean all copy_doc clean_log reload_cgi run       
#		clear && make --file=makefiles/cmake_src.mk clean all copy_doc clean_log            run
#		clear && make --file=makefiles/cmake_src.mk       all copy_doc clean_log            run       
#		clear && make --file=makefiles/cmake_src.mk       all copy_doc clean_log reload_cgi run       
#		clear && make --file=makefiles/cmake_src.mk                              reload_cgi run       