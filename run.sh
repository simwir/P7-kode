#! /bin/bash

set -o errexit
set -o pipefail
set -o nounset

./build/bin/port_discovery > pd.log &
./build/bin/com_module > com.log &
./build/bin/order > order.log &
read -p "Press enter when webots has started."
./orchestrator.sh $@
