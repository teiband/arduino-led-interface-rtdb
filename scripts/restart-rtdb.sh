#!/bin/sh
rtdb-stop
sleep 2
killall -9 kogmo-rtdb-man
sleep 1
killall -9 IceServerViewer
sleep 1
killall -9 GuiViewer
sleep 1
killall -9 IceServerStarter
sleep 1
killall -9 GuiStarter
sleep 1

rtdb-start

IceServerViewer & GuiViewer & IceServerStarter & GuiStarter &
