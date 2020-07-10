1、本代码为计量插座应用层参考代码，需要配合飞燕SDK一起使用
2、代码需要放到Products/Smart_outlet目录
3、代码目前支持BK7231U、ASR5501、RDA5981A
4、编译需要到飞燕SDK根目录下执行，指令如下：
编译方法： ./build.sh example smart_outlet uno-91h SINGAPORE ONLINE 0
第一个参数为help时，输出build.sh当前默认编译参数. 第一个参数为clean时，执行SDK目录下example目录删除，并从仓库恢复，下次编译时，会重新完整编译整个SDK及应用. 第一个参数在Products目录下找不到对应的文件夹时，会继续从Living_SDK/example目录下找，如能找到，则执行编译，成功后，复制编译结果到out目录。
要实现不输入参数，执行./build.sh编译输出需要的应用固件，可更改以下默认参数： default_type="example" default_app="smart_outlet" default_board="uno-91h" default_region=SINGAPORE default_env=ONLINE default_debug=0 default_args="" 以上参数分别对应： 产品类型、应用名称、模组型号、连云区域、连云环境、debug、其他参数（可不填，需要时把参数加到双引号内）。