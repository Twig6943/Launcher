set PLAYERNAME=UsernameGoesHere
set IPADDRESS=192.168.1.91:25200

start GW2.Main_Win64_Retail.exe ^
-playerName %PLAYERNAME% ^
-console ^
#-password password goes here if the server has a password ^
-runMultipleGameInstances ^
-Client.ServerIp %IPADDRESS% ^
#-dataPath "D:\Origin Games\Garden Warfare 2 Server\ModData\Default"
