#!/bin/bash
dcdport=8888
dcdhost=localhost

noddr=/node
wddir=$noddr/dcd_wall
wdaddr=localhost:8899
wdurl=http://$wdaddr

logfile=$wddir/genesislog.txt

walletpasswd=""

CONTRDIR="/contracts"
DEFCONTRDIR="/usr/local/dcd_protocol/etc/dcd/contracts"
CONTR_tokn_DIR=$CONTRDIR/dcd.token
CONTR_syst_DIR=$CONTRDIR/dcd.system
CONTR_boot_DIR=$CONTRDIR/dcd.boot
CONTR_bios_DIR=$CONTRDIR/dcd.bios
CONTR_msig_DIR=$CONTRDIR/dcd.msig
CONTR_wrap_DIR=$CONTRDIR/dcd.wrap

DCD_STD_priv_KEY=5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

if [ -e $wddir ]; then
    rm -rf $wddir
fi
mkdir $wddir

########################### begin #############################
echo BEGIN GENESIS SCRIPT
step=1
#configure wallet
echo "http-server-address = $wdaddr" > $wddir/config.ini

dcdksd --config-dir $wddir --data-dir $wddir --http-max-response-time-ms 99999 2> $wddir/wdlog.txt &
echo $$ > $wddir/dcd_wallet.pid
echo dcdksd log in $wddir/wdlog.txt >> $logfile
sleep 1

dcdcmd () {
    echo ===== Start: $step ============ >> $logfile
    echo executing: dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport $* | tee -a $logfile
    echo ----------------------- >> $logfile
    dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport $* >> $logfile 2>&1
    echo ==== End: $step ============== >> $logfile
    step=$(($step + 1))
}

wcmd () {
    dcdcmd wallet $*
}

cacmd () {
    dcdcmd system newaccount dcd --transfer $*
    dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport transfer dcd $1 "300.00000 DCD" "Comission!" >> $logfile 2>&1
    dcdcmd system regproducer $1 $2
    dcdcmd system voteproducer prods $1 $1
}


#echo create wallet
wcmd create -n dcd -f $noddr/wallpass.txt

walletpasswd=$(cat $noddr/wallpass.txt)

#echo unlock wall 
wcmd unlock -n dcd --password $walletpasswd

# import dcd std key
wcmd import -n dcd --private-key $DCD_STD_priv_KEY

# create system accounts
#syspassfile=$wddir/sysacckey.txt

createnewkeys() {
    passfile=$wddir'/'$1'pass.txt';
    dcdcmd create key -f $passfile
    pubacckey=`grep "^Public key:" $passfile | tail -1 | sed "s/^Public key://"`
    priacckey=`grep "^Private key:" $passfile | tail -1 | sed "s/^Private key://"`
    echo dcd.* keys: $priacckey $pubacckey >> $logfile
    wcmd import -n dcd --private-key $priacckey
}

pingdcd() {
    dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport get account dcd 2>&1
}

echo test acc dcd >> $logfile 2>&1
maxrepeat=20
repeat=0
while pingdcd | grep "Failed" 
do
    if [ $repeat -gt $maxrepeat ]; then
        break
    fi
    sleep 1
    echo . >> $logfile 2>&1
    repeat=$(( $repeat + 1 ))
done
echo esoio is work!! >> $logfile 2>&1

createnewkeys $sysacc

dcdcmd create account dcd dcd.bpay $pubacckey $pubacckey
dcdcmd create account dcd dcd.msig $pubacckey $pubacckey
dcdcmd create account dcd dcd.names $pubacckey $pubacckey
dcdcmd create account dcd dcd.ram $pubacckey $pubacckey
dcdcmd create account dcd dcd.ramfee $pubacckey $pubacckey
dcdcmd create account dcd dcd.saving $pubacckey $pubacckey
dcdcmd create account dcd dcd.stake $pubacckey $pubacckey
dcdcmd create account dcd dcd.token $pubacckey $pubacckey
dcdcmd create account dcd dcd.vpay $pubacckey $pubacckey
dcdcmd create account dcd dcd.wrap $pubacckey $pubacckey
dcdcmd create account dcd dcd.rex $pubacckey $pubacckey
dcdcmd create account dcd dcd.feebank $pubacckey $pubacckey

# import dcd.token contract
dcdcmd set contract dcd.token $CONTR_tokn_DIR dcd.token.wasm dcd.token.abi

# create sys token
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd.token create '[ "dcd", "320000000.00000 DCD" ]' -p dcd.token >> $logfile 2>&1
# push tokens into circulation
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd.token issue '[ "dcd", "320000000.00000 DCD", "memo" ]' -p dcd >> $logfile 2>&1

# import dcd.msig contract
dcdcmd set contract dcd.msig $CONTR_msig_DIR dcd.msig.wasm dcd.msig.abi

# import dcd.wrap contract
dcdcmd set contract dcd.wrap $CONTR_wrap_DIR dcd.wrap.wasm dcd.wrap.abi

# PREACTIVATE_FEATURE
curl --request POST \
    --url http://127.0.0.1:8888/v1/producer/schedule_protocol_feature_activations \
    -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}'

sleep 1

# set the dcd.boot contract
dcdcmd set contract dcd $CONTR_boot_DIR dcd.boot.wasm dcd.boot.abi

echo Enable Protocol Features
echo KV_DATABASE
dcdcmd push action dcd activate '["825ee6288fb1373eab1b5187ec2f04f6eacb39cb3a97f356a07c91622dd61d16"]' -p dcd
echo ACTION_RETURN_VALUE
dcdcmd push action dcd activate '["c3a6138c5061cf291310887c0b5c71fcaffeab90d5deb50d3b9e687cead45071"]' -p dcd
echo CONFIGURABLE_WASM_LIMITS
dcdcmd push action dcd activate '["bf61537fd21c61a60e542a5d66c3f6a78da0589336868307f94a82bccea84e88"]' -p dcd
echo BLOCKCHAIN_PARAMETERS
dcdcmd push action dcd activate '["5443fcf88330c586bc0e5f3dee10e7f63c76c00249c87fe4fbf7f38c082006b4"]' -p dcd
echo GET_SENDER
dcdcmd push action dcd activate '["f0af56d2c5a48d60a4a5b5c903edfb7db3a736a94ed589d0b797df33ff9d3e1d"]' -p dcd
echo FORWARD_SETCODE
dcdcmd push action dcd activate '["2652f5f96006294109b3dd0bbde63693f55324af452b799ee137a81a905eed25"]' -p dcd
echo ONLY_BILL_FIRST_AUTHORIZER
dcdcmd push action dcd activate '["8ba52fe7a3956c5cd3a656a3174b931d3bb2abb45578befc59f283ecd816a405"]' -p dcd
echo RESTRICT_ACTION_TO_SELF
dcdcmd push action dcd activate '["ad9e3d8f650687709fd68f4b90b41f7d825a365b02c23a636cef88ac2ac00c43"]' -p dcd
echo DISALLOW_EMPTY_PRODUCER_SCHEDULE
dcdcmd push action dcd activate '["68dcaa34c0517d19666e6b33add67351d8c5f69e999ca1e37931bc410a297428"]' -p dcd
echo FIX_LINKAUTH_RESTRICTION
dcdcmd push action dcd activate '["e0fb64b1085cc5538970158d05a009c24e276fb94e1a0bf6a528b48fbc4ff526"]' -p dcd
echo REPLACE_DEFERRED
dcdcmd push action dcd activate '["ef43112c6543b88db2283a2e077278c315ae2c84719a8b25f25cc88565fbea99"]' -p dcd
echo NO_DUPLICATE_DEFERRED_ID
dcdcmd push action dcd activate '["4a90c00d55454dc5b059055ca213579c6ea856967712a56017487886a4d4cc0f"]' -p dcd
echo ONLY_LINK_TO_EXISTING_PERMISSION
dcdcmd push action dcd activate '["1a99a59d87e06e09ec5b028a9cbb7749b4a5ad8819004365d02dc4379a8b7241"]' -p dcd
echo RAM_RESTRICTIONS
dcdcmd push action dcd activate '["4e7bf348da00a945489b2a681749eb56f5de00b900014e137ddae39f48f69d67"]' -p dcd
echo WEBAUTHN_KEY
dcdcmd push action dcd activate '["4fca8bd82bbd181e714e283f83e1b45d95ca5af40fb89ad3977b653c448f78c2"]' -p dcd
echo WTMSIG_BLOCK_SIGNATURES
dcdcmd push action dcd activate '["299dcb6af692324b899b39f16d5a530a33062804e41f09dc97e9f156b4476707"]' -p dcd

# Delay for the activation to take hold
sleep 1

# deploy dcd.system contract
dcdcmd set contract dcd $CONTR_syst_DIR

# Transition from single genesis producer to multiple producers
# Designate dcd.msig as privileged account
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd setpriv '["dcd.msig", 1]' -p dcd@active >> $logfile 2>&1
# dcdcli get account dcd.msig

# Initialize system account
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd init '["0", "5,DCD"]' -p dcd@active

createnewacgenesisconfig() {
    if [ -n "$1" ]; then
        accname=$1
    else
        echo "no account name, break."
        return -1
    fi
    
    # $2 - flag export configfile
    if [ -n "$2" ]; then
        genesisaccdir=$noddr/config/$accname
        if [ ! -d $genesisaccdir ]; then
            mkdir -p $genesisaccdir
        fi
    else
        genesisaccdir=$noddr/config/work
    fi    
    
    listfiles="genesis_start.sh hard_replay.sh start.sh"
    for filename in $listfiles
    do
        passfile=$wddir'/'$accname'pass.txt';
        pubacckey=`grep "^Public key:" $passfile | tail -1 | sed "s/^Public key://"`
        priacckey=`grep "^Private key:" $passfile | tail -1 | sed "s/^Private key://"`
        cat $genesisaccdir/$filename.in | sed -e "s/ PUB_KEY/$pubacckey/;s/:PRIV_KEY/:$priacckey/" | sed "s/KEY: /KEY:/" > $genesisaccdir/$filename
        chmod a+x $genesisaccdir/$filename
    done
}

### NEW
setprod () {
    dcdcmd system regproducer $1 $2
    dcdcmd system voteproducer prods accountnum1o $1
}
crootaccountcmd () {
    dcdcmd system newaccount dcd --transfer $*
    dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport transfer dcd $1 "11300000.00000 DCD" "On stake root account" >> $logfile 2>&1
    dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport system delegatebw $1 $1 "5640000.00000 DCD" "5640000.00000 DCD" >> $logfile 2>&1
    dcdcmd system regproducer $1 $2
    dcdcmd system voteproducer prods $1 $1
}


echo CREATE ROOT ACCOUNT accountnum1o >> $logfile 2>&1
createnewkeys accountnum1o
crootaccountcmd accountnum1o $pubacckey
createnewacgenesisconfig accountnum1o
echo run local work node accountnum1o >> $logfile 2>&1
$genesisaccdir/genesis_start.sh >> $logfile 2>&1


echo CREATE PRODUCER ACCOUNTS >> $logfile 2>&1
createacclist="accountnum11 accountnum12"
for createacc in $createacclist
do
    createnewkeys $createacc
    dcdcmd create account dcd $createacc $pubacckey
    createnewacgenesisconfig $createacc y
    echo transfer dcd->$createacc 1000 tokens >> $logfile 2>&1
    dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport transfer dcd $createacc "1000.00000 DCD" "on fee" >> $logfile 2>&1
    echo stake $createacc 200 tokens >> $logfile 2>&1
    dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport system delegatebw $createacc $createacc "100.00000 DCD" "100.00000 DCD" >> $logfile 2>&1
    echo setprod $createacc >> $logfile 2>&1
    dcdcmd system regproducer $createacc $pubacckey
done

echo voite for accountnum1o accountnum11 accountnum12 >> $logfile 2>&1
dcdcmd system voteproducer prods accountnum1o accountnum1o accountnum11 accountnum12

echo CREATE ACCOUNTS FOR NODE
createacclist="accountnum13 accountnum14 accountnum15"
for createacc in $createacclist
do
    createnewkeys $createacc
    dcdcmd create account dcd $createacc $pubacckey
    createnewacgenesisconfig $createacc y
    echo transfer dcd->$createacc 300 tokens >> $logfile 2>&1
    dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport transfer dcd $createacc "300.00000 DCD" "on fee" >> $logfile 2>&1
done
#### END NEW

echo transfer dcd->accountnum11 30000 tokens >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport transfer dcd accountnum11 "30000.00000 DCD" "Hodl!" >> $logfile 2>&1


###################### FEE ##################################
# register oracle
echo push action dcd regoracle '[ "accountnum1o" ]' -p accountnum1o@active >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd regoracle '[ "accountnum1o" ]' -p accountnum1o@active >> $logfile 2>&1
# voite oracle for rate usd/dcd
echo push action dcd setrateorcl '[ "accountnum1o","2" ]' -p accountnum1o@active >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd setrateorcl '[ "accountnum1o","2" ]' -p accountnum1o@active >> $logfile 2>&1
# set list fee from producers
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd propactfee '["accountnum1o", [["dcd.token", "transfer", "1.00000 USD"],["dcd", "defaultfee", "1.00000 USD"] ] ]' -p accountnum1o@active >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd propactfee '["accountnum11", [["dcd.token", "transfer", "1.00000 USD"],["dcd", "defaultfee", "1.00000 USD"] ] ]' -p accountnum11@active >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd propactfee '["accountnum12", [["dcd.token", "transfer", "1.00000 USD"],["dcd", "defaultfee", "1.00000 USD"] ] ]' -p accountnum12@active >> $logfile 2>&1
###### voite producers for list with fee
# producer accountnum1o
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd votefeeprop '["accountnum1o","accountnum11","1"]' -p accountnum1o@active >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd votefeeprop '["accountnum1o","accountnum12","1"]' -p accountnum1o@active >> $logfile 2>&1
# producer accountnum11
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd votefeeprop '["accountnum11","accountnum1o","1"]' -p accountnum11@active >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd votefeeprop '["accountnum11","accountnum12","1"]' -p accountnum11@active >> $logfile 2>&1
# producer accountnum12
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd votefeeprop '["accountnum12","accountnum1o","1"]' -p accountnum12@active >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd votefeeprop '["accountnum12","accountnum11","1"]' -p accountnum12@active >> $logfile 2>&1
########### END FEE ##############
sleep 2



# Waiting for the new node to accept all blocks
pingworknode() {
    dcdcli --wallet-url $wdurl --url http://172.19.0.2:8010 get account accountnum1o 2>&1
}

echo test acc accountnum1o >> $logfile 2>&1
maxrepeat=10
repeat=0
while pingworknode | grep "Failed" 
do
    if [ $repeat -gt $maxrepeat ]; then
        break
    fi
    sleep 1
    echo . >> $logfile 2>&1
    repeat=$(( $repeat + 1 ))
done
echo accountnum1o is working!! >> $logfile 2>&1

dcdcmd create account dcd accounttest1 DCD5sK22SGYSWP3aaRr5GG1JxJodWbAtfsMV1zwMCJFKmpX5rJUoP
wcmd import -n dcd --private-key 5J3Wxum2yXa6HXAZccNjbgVGaQ5iotUkZp2MiMw5ZiffbWDZaSs


sleep 2
echo отключение аккаунтов dcd >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd.prods", "permission": "active"}}]}}' -p dcd@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd.prods", "permission": "active"}}]}}' -p dcd@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.bpay", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.bpay@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.bpay", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.bpay@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.msig", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.msig@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.msig", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.msig@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.names", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.names@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.names", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.names@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.ram", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.ram@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.ram", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.ram@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.ramfee", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.ramfee@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.ramfee", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.ramfee@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.saving", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.saving@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.saving", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.saving@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.stake", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.stake@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.stake", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.stake@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.token", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.token@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.token", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.token@active >> $logfile 2>&1

dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.vpay", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.vpay@owner >> $logfile 2>&1
dcdcli --wallet-url $wdurl --url http://$dcdhost:$dcdport push action dcd updateauth '{"account": "dcd.vpay", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "dcd", "permission": "active"}}]}}' -p dcd.vpay@active >> $logfile 2>&1




echo Stopping dcd node
$noddr/config/genesis/stop.sh
touch $noddr/config/genesis/desabled.lock

echo FINISH INIT
