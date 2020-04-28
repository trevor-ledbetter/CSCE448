#!/bin/sh

# Defaults
MACHINE_ADDR="localhost"
ROUTER_ADDR="localhost"
MASTER_1="6000"
MASTER_2="6001"
MASTER_3="6002"
ROUTER="7000"
CLIENTNAME="defaultuser"

# Handle make
make_executables()
{
  make
}

# Handle Master/Slave
master_slave()
{
  make_executables
  echo "Running Master and Slave server on: $MACHINE_ADDR:$MASTER_1 connected to router: $ROUTER_ADDR:$ROUTER"
  gnome-terminal -- /bin/sh -c "./fbsd $MACHINE_ADDR $MASTER_1 $ROUTER_ADDR $ROUTER & ./fbss $MACHINE_ADDR $MASTER_1 $ROUTER_ADDR $ROUTER; exec bash"
#  gnome-terminal -- /bin/sh -c "./fbss $MASTER_1 $ROUTER; exec bash"
}

# Handle Router
router_only()
{
  make_executables
  echo "Running Router server on $ROUTER_ADDR:$ROUTER"
  gnome-terminal -- /bin/sh -c "./fbrs $ROUTER_ADDR $ROUTER; exec bash"
}

# Handle Client
run_client()
{
  make_executables
  echo "Running Client with username: $CLIENTNAME connecting to router at: $ROUTER_ADDR:$ROUTER"
  gnome-terminal -- /bin/sh -c "./fbc $ROUTER_ADDR $ROUTER $CLIENTNAME; exec bash"
}

# Handle Run All
run_all()
{
  make_executables
  echo "Running Full Deployment with the following:"
  echo "   [1] Master on Port: $MASTER_1"
  echo "   [2] Master on Port: $MASTER_2"
  echo "   [3] Master on Port: $MASTER_3"
  echo "   [4] Router on Port: $ROUTER"
  gnome-terminal -- /bin/sh -c "./fbrs $ROUTER & \
    ./fbsd $MACHINE_ADDR $MASTER_1 $ROUTER_ADDR $ROUTER & \
    ./fbss $MACHINE_ADDR $MASTER_1 $ROUTER_ADDR $ROUTER & \
    ./fbsd $MACHINE_ADDR $MASTER_2 $ROUTER_ADDR $ROUTER & \
    ./fbss $MACHINE_ADDR $MASTER_2 $ROUTER_ADDR $ROUTER & \
    ./fbsd $MACHINE_ADDR $MASTER_3 $ROUTER_ADDR $ROUTER & \
    ./fbss $MACHINE_ADDR $MASTER_3 $ROUTER_ADDR $ROUTER ; exec bash"
#  gnome-terminal -- /bin/sh -c "./fbsd $MASTER_1 $ROUTER; exec bash"
#  gnome-terminal -- /bin/sh -c "./fbss $MASTER_1 $ROUTER; exec bash"
#  gnome-terminal -- /bin/sh -c "./fbsd $MASTER_2 $ROUTER; exec bash"
#  gnome-terminal -- /bin/sh -c "./fbss $MASTER_2 $ROUTER; exec bash"
#  gnome-terminal -- /bin/sh -c "./fbsd $MASTER_3 $ROUTER; exec bash"
#  gnome-terminal -- /bin/sh -c "./fbss $MASTER_3 $ROUTER; exec bash"
}

# Set from parameters
if [ "$1" = "s" -a ! -z "$2" -a ! -z "$3" ]; then
  MACHINE_ADDR=$2
  ROUTER_ADDR=$3
  master_slave
elif [ "$1" = "r" -a ! -z "$2" ]; then
  ROUTER_ADDR=$2
  router_only
elif [ "$1" = "f" ]; then
#  MASTER_1=`expr 6000 + 1`
#  MASTER_2=`expr 6000 + 2`
#  MASTER_3=`expr 6000 + 3`
#  ROUTER=`expr 6000 + 4`
  run_all
elif [ "$1" = "c" -a ! -z "$2" -a ! -z "$3" ]; then
  ROUTER_ADDR=$2
  CLIENTNAME=$3
  run_client
elif [ "$1" = "z" ]; then
  MASTER_1="6000"
  MASTER_2="6100"
  MASTER_3="6200"
  ROUTER="7000"
  run_all
else
  echo "Usage:"
  echo "   4 Machine Deployment: ./launch.sh f [starting_port_number]"
  echo "           Master/Slave: ./launch.sh s [current_machine_address] [routing_machine_address]"
  echo "                 Router: ./launch.sh r [current_machine_address]"
  echo "                 Client: ./launch.sh c [router_address] [username]"
fi