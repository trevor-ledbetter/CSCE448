Changes to be aware of:
    1. client.cpp takes the routing server's port as input instead of a master server'server
    2. server.cpp takes the routing server's port as an additional input
    3. Master servers now register with the routing on boot/reboot
    4. clients get the master server's port from the routing server using the getServer()
        function called in connectTo()

Currently Implementing:
    1. slave server that repeatedly sends KeepAlive to the master. When master fails it "taddles"
        to the routing server.
    2. In server.cpp implement KeepAlive rpc, and a thread in to receive messages from the slave. 


