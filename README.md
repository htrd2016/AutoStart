# AutoStart
AutoStart是运行在windows xp的辅助工具，用来根据系统MAC地址读取配置文件来修改计算机IP地址、网关、DNS、主机名(会重启计算机)，以及启动指定个数的客户端程序。该工具是用导入注册表设置为开机启动的。
AutoStart是visual studio2013工程。
AutoStart.reg：通过运行“reg import XX/AutoStart.reg”方式将AutoStart.exe设置为开机启动。
config.ini：保存所有虚机系统ip，网关，dns，主机名，启动客户端数和服务器ip端口，在AutoStart启动的时候解析。

