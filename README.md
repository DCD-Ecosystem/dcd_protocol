
# DCD Protocol. Divide trust. Unite players
Welcome to the DCD Ecosystem source code repository! This software is actively developed by the Graphene Lab team based on previous internal developments, as well as using a number of public technologies, including EOSIO, Ethereum and others.

### Build dcd_protocol and setup yourself as a validator 

###### 0. Clone the repository 
```
git clone https://github.com/DCD-Ecosystem/dcd_protocol.git
```

#####  1. Build and run the local node using docker instructions  from the docker/Readme.md (https://github.com/DCD-Ecosystem/dcd_protocol/blob/master/docker/Readme.md). In case the connection was established succesfuly you will see your node accepting the blocks in the log.

##### 2. Log in to your node container in in interactive mode
```
docker exec -it usernode bash
```
##### 3. Run the wallet service
```
dcdksd --data-dir path_to_data_dir --http-max-response-time-ms 99999 >> /node/log/dcdksd.log 2>&1 &
```
**--data-dir path_to_data_dir** - path to save wallet to, may be ommited

##### 4. Create a wallet to hold your private keys for transaction signing. You can have multiple wallets if required. 
```
dcdcli wallet create -n name_wallet -f path_to_wall_password_file
```
**name_wallet**  - desired wallet name to create
**path_to_wall_password_file** - path to the file to store the password for your created wallet

##### 5. Read the password into any local variable
```
walletpasswd=$(cat path_to_wall_password_file)
```
**path_to_wall_password_file** - path to the file to store the password for the wallet you want to use

##### 6. Ensure your wallet is unlocked or unlock it 
```
dcdcli wallet unlock -n name_wallet --password $walletpasswd
```
**name_wallet** - name of the wallet to use

##### 7.Import your private key into your created wallet
```
dcdcli wallet import -n name_wallet --private-key acc_private_key
```
**name_wallet** - name of the wallet to use
**acc_private_key** - your private key received at account registration

##### 8. Check your account balance
```
dcdcli --url http://127.0.0.1:8022 get account acc_name
```
**acc_name** - your account name

##### 9.Stake some amount of DCD tokens
```
dcdcli --url http://127.0.0.1:8022 system delegatebw acc_name acc_name "100.00000 DCD" "100.00000 DCD"
```
**acc_name** - your account name
The amount of DCD can be different, note however to input full precision 0.00000

##### 10. Register yourself as a validator inside blockchain
```
dcdcli --url http://127.0.0.1:8022 system regproducer acc_name acc_public_key
```
**acc_name** - your account name
**acc_public_key** - your public key

##### 11. Contact us and suggest your account name for a validator role

##### 12. After we vote for your account, you shoud your node producing the new blocks according to the global validator rating


## License

DCD is released under the open source [MIT](./LICENSE) license and is offered “AS IS” without warranty of any kind, express or implied. Any security provided by the DCD software depends in part on how it is used, configured, and deployed. DCD is built upon many third-party libraries such as WABT (Apache License) and WAVM (BSD 3-clause) which are also provided “AS IS” without warranty of any kind. Without limiting the generality of the foregoing, Block.one makes no representation or guarantee that DCD or any third-party libraries will perform as intended or will be free of errors, bugs or faulty code. Both may fail in large or small ways that could completely or partially limit functionality or compromise computer systems. If you use or implement DCD, you do so at your own risk. In no event will Block.one be liable to any party for any damages whatsoever, even if it had been advised of the possibility of damage.  

# DCD NODES SPECIFICATIONS 

### PRODUCTION MAINNET NODES
* Processor: Intel® Core™ i7-8700 Hexa-Core (incl. Hyper-Threading Technology)
* RAM: 64 GB DDR4 RAM
* Storage: 2 x 1 TB NVMe SSD
* Connection: 1 GBit/s port

### BACKUP PRODUCTION NODES
* Processor: Intel® Xeon® E3-1270 v6 Octa-Core (Hyper-Threading Technology)
* RAM: 32 GB DDR4 RAM
* Storage: 500 GB SSD
* Connection: 1 GBit/s port

### TESTNET PRODUCTION NODES (SUPER MINIMAL)
* Processor: Minimum 2 cores
* RAM: Minimum 4-8 GB DDR4 RAM (better 8 GB)
* Storage: 30 GB HDD
* Connection: 1 GBit/s port

### API/HTTPS NODES
* Processor: Intel® Core™ i7-8700 Hexa-Core (incl. Hyper-Threading Technology)
* RAM: 64 GB DDR4 RAM
* Storage: 2 x 1 TB NVMe SSD
* Connection: 1 GBit/s port

### BACKUP API/HTTPS NODES
* Processor: Intel® Xeon® E3-1270 v6 Octa-Core (Hyper-Threading Technology)
* RAM: 32 GB DDR4 RAM
* Storage: 500 GB SSD
* Connection: 1 GBit/s port

### SEED NODES
* Processor: Intel® Xeon® E3-1270 v6 Octa-Core (Hyper-Threading Technology)
* RAM: 32 GB DDR4 RAM
* Storage: 500 GB SSD
* Connection: 1 GBit/s port
