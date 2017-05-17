# Usage
## Building
make  

## Inserting the driver
sudo insmod interruptor.ko

## Generating incorrect results
Launch tester and rtcspikers for each logical cpu.  
 
sudo su

./tester &

./tester &

./tester &

./tester &

./tester &

./tester &

./tester &

./tester &

./rtcspiker &

./rtcspiker &

./rtcspiker &

./rtcspiker &

./rtcspiker &

./rtcspiker &

./rtcspiker &

./rtcspiker &


## Results
E5-2650:  common 1's and 2's

i7-6700:  occasional 118's and 119's
