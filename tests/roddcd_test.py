#!/usr/bin/env python3

from testUtils import Account
from testUtils import Utils
from Cluster import Cluster
from WalletMgr import WalletMgr
from Node import Node
from Node import ReturnType
from Node import BlockType
from TestHelper import TestHelper
from TestHelper import AppArgs
from testUtils import BlockLogAction
import json
import sys
import signal
import time
import subprocess
import os
import shutil

###############################################################
# roddcd_test.py
#
# roddcd integration test 
# 
# This test creates a producer node with state history plugin and a 
# roddcd process with a test filter to connect to the producer. Pushes 
# a few transactions to the producer and query roddcd get_block endpoint
# to see if it sees one of the block containing the pushed transaction. 
# Lastly, it verifies if roddcd get_info endpoint returns a head_block_num. 
#
###############################################################

# Parse command line arguments
args = TestHelper.parse_args({"-v","--clean-run","--dump-error-details","--leave-running","--keep-logs"})
Utils.Debug = args.v
killAll=args.clean_run
dumpErrorDetails=args.dump_error_details
dontKill=args.leave_running
killDcdInstances=not dontKill
killWallet=not dontKill
keepLogs=args.keep_logs
stateHistoryEndpoint = "127.0.0.1:8080"

loggingFile="logging.json"


logging="""{
  "includes": [],
  "appenders": [{
      "name": "stderr",
      "type": "console",
      "args": {
        "stream": "std_error",
        "level_colors": [{
            "level": "debug",
            "color": "green"
          },{
            "level": "warn",
            "color": "brown"
          },{
            "level": "error",
            "color": "red"
          }
        ]
      },
      "enabled": true
    }
  ],
  "loggers": [{
      "name": "default",
      "level": "debug",
      "enabled": true,
      "additivity": false,
      "appenders": [
        "stderr"
      ]
    },{
      "name": "state_history",
      "level": "debug",
      "enabled": true,
      "additivity": false,
      "appenders": [
        "stderr"
      ]
    }
  ]
}"""

walletMgr=WalletMgr(True)
cluster=Cluster(walletd=True)
cluster.setWalletMgr(walletMgr)

testSuccessful = False

with open(loggingFile, "w") as textFile:
        print(logging,file=textFile)

class Roddcd:
    def __init__(self, stateHistoryEndpoint, filterName, filterWasm):
        self.roddcdDir = os.path.join(os.getcwd(), 'var/lib/roddcd')
        shutil.rmtree(self.roddcdDir, ignore_errors=True)
        os.makedirs(self.roddcdDir, exist_ok=True)
        self.stateHistoryEndpoint = stateHistoryEndpoint
        self.filterName = filterName
        self.filterWasm = filterWasm
        self.roddcd = None
        self.roddcdStdout = None
        self.roddcdStderr = None
        self.keepLogs = keepLogs

    def __enter__(self):
        self.endpoint = "http://127.0.0.1:8880/"
        self.roddcdStdout = open(os.path.join(self.roddcdDir, "stdout.out"), "w")
        self.roddcdStderr = open(os.path.join(self.roddcdDir, "stderr.out"), "w")
        self.roddcd = subprocess.Popen(['./programs/roddcd/roddcd', '--rdb-database', os.path.join(self.roddcdDir,'rocksdb'), '--data-dir', os.path.join(self.roddcdDir,'data'),
                               '--clone-connect-to',  self.stateHistoryEndpoint , '--filter-name', self.filterName , '--filter-wasm', self.filterWasm ],
                stdout=self.roddcdStdout, 
                stderr=self.roddcdStderr)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if self.roddcd is not None:
            self.roddcd.kill()
        if self.roddcdStdout is not None:
            self.roddcdStdout.close()
        if self.roddcdStderr is not None:
            self.roddcdStderr.close()
        if testSuccessful and not keepLogs:
            shutil.rmtree(self.roddcdDir, ignore_errors=True)

    def waitTillReady(self):
        Utils.waitForTruth(lambda:  Utils.runCmdArrReturnStr(['curl', '-H', 'Accept: application/json', self.endpoint + 'v1/chain/get_info'], silentErrors=True) != "" , timeout=30)

    def get_block(self, blockNum):
        request_body = { "block_num_or_id": blockNum }
        return Utils.runCmdArrReturnJson(['curl', '-X', 'POST', '-H', 'Content-Type: application/json', '-H', 'Accept: application/json', self.endpoint + 'v1/chain/get_block', '--data', json.dumps(request_body)])
        
    def get_info(self):
        return Utils.runCmdArrReturnJson(['curl', '-H', 'Accept: application/json', self.endpoint + 'v1/chain/get_info'])


roddcd = None
try:
    TestHelper.printSystemInfo("BEGIN")
    cluster.killall(allInstances=killAll)
    cluster.cleanup()

    assert cluster.launch(
        pnodes=1,
        prodCount=1,
        totalProducers=1,
        totalNodes=1,
        useBiosBootFile=False,
        loadSystemContract=False,
        specificExtradcdnodeArgs={
            0: ("--plugin dcd::state_history_plugin --trace-history --chain-state-history --disable-replay-opts --state-history-stride 20 --max-retained-history-files 3 " 
                "--state-history-endpoint {} --plugin dcd::net_api_plugin --wasm-runtime dcd-vm-jit -l logging.json").format(stateHistoryEndpoint)})

    producerNodeIndex = 0
    producerNode = cluster.getNode(producerNodeIndex)
   
    # Create a transaction to create an account
    Utils.Print("create a new account payloadless from the producer node")
    payloadlessAcc = Account("payloadless")
    payloadlessAcc.ownerPublicKey = "DCD6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV"
    payloadlessAcc.activePublicKey = "DCD6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV"
    producerNode.createAccount(payloadlessAcc, cluster.dcdAccount)


    contractDir="unittests/test-contracts/payloadless"
    wasmFile="payloadless.wasm"
    abiFile="payloadless.abi"
    Utils.Print("Publish payloadless contract")
    trans = producerNode.publishContract(payloadlessAcc, contractDir, wasmFile, abiFile, waitForTransBlock=True)

    trx = {
        "actions": [{"account": "payloadless", "name": "doit", "authorization": [{
          "actor": "payloadless", "permission": "active"}], "data": ""}],
        "context_free_actions": [{"account": "payloadless", "name": "doit", "data": ""}],
        "context_free_data": ["a1b2c3", "1a2b3c"],
    } 

    cmd = "push transaction '{}' -p payloadless".format(json.dumps(trx))
    trans = producerNode.processdcdcliCmd(cmd, cmd, silentErrors=False)
    assert trans, "Failed to push transaction with context free data"
    
    cfTrxBlockNum = int(trans["processed"]["block_num"])
    cfTrxId = trans["transaction_id"]

    # Wait until the cfd trx block is executed to become irreversible 
    producerNode.waitForIrreversibleBlock(cfTrxBlockNum, timeout=30) 
    
    Utils.Print("verify the account payloadless from producer node")
    trans = producerNode.getDcdAccount("payloadless", exitOnError=False)
    assert trans["account_name"], "Failed to get the account payloadless"

    Utils.Print("verify the context free transaction from producer node")
    trans_from_full = producerNode.getTransaction(cfTrxId)
    assert trans_from_full, "Failed to get the transaction with context free data from the producer node"

    with Roddcd(stateHistoryEndpoint, 'test.filter', './tests/test_filter.wasm') as roddcd:
        roddcd.waitTillReady()
        head_block_num = 0
        Utils.Print("Verify roddcd get_info endpoint works")
        while head_block_num < cfTrxBlockNum:
            response = roddcd.get_info()
            assert 'head_block_num' in response, "Reddcd response does not contain head_block_num, response body = {}".format(json.dumps(response))
            head_block_num = int(response['head_block_num'])
            time.sleep(1)
        
        response = roddcd.get_block(cfTrxBlockNum)
        assert response["block_num"] == cfTrxBlockNum, "Roddcd responds with wrong block"
    
    testSuccessful = True
finally:
    TestHelper.shutdown(cluster, walletMgr, testSuccessful, killDcdInstances, killWallet, keepLogs, killAll, dumpErrorDetails)
    
exitCode = 0 if testSuccessful else 1
exit(exitCode)
