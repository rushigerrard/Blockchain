PID=`ps -ef | grep "serverstartup" | awk '{print $2}'`
echo "About to shut process id " $PID
sudo kill -9 $PID
