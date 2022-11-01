# 我们的编译预期 : 编译动作会新创建一个文件夹`build` , 
#	然后将编译的中间产物按照现有项目文件分布在 build 中做一个相同的分布 ; 
#	我期望的 make file 文件结构的样子:
#		1. 所有的编译产物归总到 build 文件夹中
#		2. 每个文件夹都有一个 makefile 文件
#		3. 通过最外层 makefile 调用内层 makefile 的形式最终组织起整个 makefile 结构
# 	4. 最终一个 make 命令完成代码编译动作


.RECIPEPREFIX = ># 标定命令行前缀,默认命令行前缀是 tab 字符，更改为 ‘>’
g++ = g++ -g -c -std=c++20

make_file_path = $(dir $(shell basename $(dir $(word 1,$(MAKEFILE_LIST)))))
pro_dir = /volume/_document/c_cpp_program/nameless_carpool

# 源代码文件夹
src_dir = $(pro_dir)/src_test
# src 目录下 所有的编译产物都放到这个目录中 , 按照代码目录结构组织这个文件夹
protram_output_dir = $(pro_dir)/build_src_test
build_dir = $(protram_output_dir)/build

objects = $(build_dir)/date_time.o 																\
					$(build_dir)/main.o

dynamic_libs = -lpthread  -lcurl

# region 定义几个便利函数

# 如果文件夹不存在,创建一个文件夹
create_dir = 								\
	if [ ! -d "$(1)" ]; then 	\
		mkdir -p "$(1)"; 				\
	fi												\
# endregion 


all : try_create_all_dependent_dir 						\
			$(protram_output_dir)/execute.run
.PHONY : all


try_create_all_dependent_dir :      
>		$(call create_dir,$(protram_output_dir))
>		$(call create_dir,$(build_dir))


$(protram_output_dir)/execute.run : $(objects)
>   g++ $(objects) $(dynamic_libs) -o $(protram_output_dir)/execute.run
>		echo "build success , start run ... \n"
# >		$(protram_output_dir)/execute.run


date_time_o_out = $(build_dir)/date_time.o
$(date_time_o_out) : $(src_dir)/date_time.cpp $(src_dir)/date_time.h
>		$(g++) $(src_dir)/date_time.cpp -o $(date_time_o_out)

main_o_dependent = $(src_dir)/main.cpp
main_o_out = $(build_dir)/main.o
$(main_o_out) : $(main_o_dependent)
>		$(g++) $(main_o_dependent) -o $(main_o_out)

# 清空编译产物
.PHONY : clean
clean :
>   rm -rf $(protram_output_dir)

# 执行编译结果
run :
>		$(protram_output_dir)/execute.run
