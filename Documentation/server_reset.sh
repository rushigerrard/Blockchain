PID=`ps -ef | grep "serverstartup" | awk '{print $2}'`
echo "Shutting down serverstartup : "
echo "Killing processes : " $PID
sudo kill -9 $PID
