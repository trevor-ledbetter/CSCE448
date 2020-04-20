#!/bin/sh

# Defaults
MASTER_1="6000"
MASTER_2="6001"
MASTER_3="6002"
ROUTER="7000"

# Handle make
make_executables()
{
  make
}

# Handle Master/Slave
master_slave()
{
  make_executables
  echo "Running Master and Slave server on port: $MASTER_1 connected to router: $ROUTER"
  gnome-terminal -- /bin/sh -c "./fbsd $MASTER_1 $ROUTER; exec bash"
  gnome-terminal -- /bin/sh -c "./fbss $MASTER_1 $ROUTER; exec bash"
}

# Handle Router
router_only()
{
  make_executables
  echo "Running Router server on port $ROUTER"
  gnome-terminal -- /bin/sh -c "./fbrs $ROUTER; exec bash"
}

# Handle Client
run_client()
{
  make_executables
  echo "Running Client with username: $MASTER_1 connecting to router at: $ROUTER"
  gnome-terminal -- /bin/sh -c "./fbc localhost $ROUTER $MASTER_1; exec bash"
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
  gnome-terminal -- /bin/sh -c "./fbrs $ROUTER; exec bash"
  gnome-terminal -- /bin/sh -c "./fbsd $MASTER_1 $ROUTER; exec bash"
  gnome-terminal -- /bin/sh -c "./fbss $MASTER_1 $ROUTER; exec bash"
  gnome-terminal -- /bin/sh -c "./fbsd $MASTER_2 $ROUTER; exec bash"
  gnome-terminal -- /bin/sh -c "./fbss $MASTER_2 $ROUTER; exec bash"
  gnome-terminal -- /bin/sh -c "./fbsd $MASTER_3 $ROUTER; exec bash"
  gnome-terminal -- /bin/sh -c "./fbss $MASTER_3 $ROUTER; exec bash"
}

# Set from parameters
if [ "$1" = "s" -a ! -z "$2" -a ! -z "$3" ]; then
  MASTER_1=$2
  ROUTER=$3
  master_slave
elif [ "$1" = "r" -a ! -z "$2" ]; then
  ROUTER=$2
  router_only
elif [ "$1" = "f" -a ! -z "$2" ]; then
  MASTER_1=`expr $2 + 1`
  MASTER_2=`expr $2 + 2`
  MASTER_3=`expr $2 + 3`
  ROUTER=`expr $2 + 4`
  run_all
elif [ "$1" = "c" -a ! -z "$2" -a ! -z "$3" ]; then
  MASTER_1=$2
  ROUTER=$3
  run_client
elif [ "$1" = "z" ]; then
  MASTER_1="6000"
  MASTER_2="6100"
  MASTER_3="6200"
  ROUTER="7000"
  run_all
else
  echo "Usage:"
  echo "   Full (4 Machine) Deployment: ./launch.sh f [starting_port_number]"
  echo "                  Master/Slave: ./launch.sh s [master_port] [router_port]"
  echo "                        Router: ./launch.sh r [router_port]"
  echo "                        Client: ./launch.sh c [username] [router_port]"
fi