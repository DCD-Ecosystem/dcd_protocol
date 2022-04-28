#!/bin/bash
echo unlock wall 
dcdcli wallet unlock -n wall1 --password PW5K6ueZ1BpL2SpeQPh5jvXfzvoDseLXdXmz3YHCQCTZ25pnFxc3E

echo "Transfer DCD for new account, comiss"
dcdcli push action dcd.token transfer '[ "dcd", "accountnum11", "1.0000 DCD", "m" ]' -p dcd@active
dcdcli push action dcd.token transfer '[ "dcd", "accountnum12", "1.0000 DCD", "m" ]' -p dcd@active
dcdcli push action dcd.token transfer '[ "dcd", "accountnum13", "1.0000 DCD", "m" ]' -p dcd@active
dcdcli push action dcd.token transfer '[ "dcd", "accountnum14", "1.0000 DCD", "m" ]' -p dcd@active

echo "Register the new account as a producer"
dcdcli system regproducer accountnum11 DCD7z1EdDDSSiaUMp9EtD2H2Vq9sL6beA9xbNzN91KECcD5UbSgvB https://accountnum11.com 643
dcdcli system regproducer accountnum12 DCD8QhMNPv4jQ1SF6EcaTwGvxskjob2mqBvC1fo8Rw79u1ntSVYaA https://accountnum12.com 643
dcdcli system regproducer accountnum13 DCD7mGENPpBmXvVwiE5VQL9ArSf5maqsvykoaHsq7ysw2ZGyTJ5T9 https://accountnum13.com 643
dcdcli system regproducer accountnum14 DCD6Wha4QzgqGaV3cy2QA8vFM47X2gYNGdwtav76Us5mBCXCdigsd https://accountnum14.com 643
# dcdcli system listproducers
echo "Vote for each of the block producers started"
dcdcli system voteproducer prods accountnum11 accountnum11 accountnum12 accountnum13 accountnum14
# user must stake before they can vote
# user must stake before they can vote
echo FINISH SET PRODUCERS