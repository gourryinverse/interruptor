# Usage
This driver produces unexpected results on E5-2650, i7-6700.  

Expected: Always 0  

E5-2650 Result: 0, 1, 2, ..., up to 19 and occasionally higher  

i7-6700 Result: 0, 118, 119  

E5-1650 Result: 0

When correct, the driver will not produce any messages in dmesg.  

When the branch count is > 0, it will produce a message "interruptor: %d" where %d is the non-zero value detected. 


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
Expected: Always 0, no output in dmesg.  

E5-2650:  common 1s and 2s - dmesg output "interruptor: 1"

i7-6700:  occasional 118s and 119s - dmesg output "interruptor: 118"
