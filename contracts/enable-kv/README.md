## Summary
This README illustrates the steps for usage of enable-kv.sh script with kv_map as an example. Set your environment variables as follows before beginning the rest of the steps.
1. export EOS_2_1_0=[eos 2.1.0 directory]
1. export DCD_CDT_1_8_0=[dcd.cdt 1.8.0 directory]
1. export PATH=$EOS_2_1_0/build/bin:$PATH

## Steps
Bring up dcdnode in a different terminal
1. export PATH=$EOS_2_1_0/build/bin:$PATH
1. dcdnode -e -p dcd --plugin dcd::producer_plugin --plugin dcd::producer_api_plugin --plugin dcd::chain_api_plugin --plugin dcd::http_plugin --plugin dcd::history_plugin --plugin dcd::history_api_plugin --filter-on=* --access-control-allow-origin=* --contracts-console --http-validate-host=false --verbose-http-errors --max-transaction-time=1000 --backing-store chainbase --data-dir=datadir

In the first terminal
1. $EOS_2_1_0/contracts/enable-kv/enable-kv.sh -c $EOS_2_1_0/build/contracts/contracts/
1. cd $DCD_CDT_1_8_0/examples/kv_map
1. mkdir build
1. cd build
1. cmake .. -DCMAKE_PREFIX_PATH=$EOS_2_1_0/build
1. make
1. dcdcli create account dcd jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
1. dcdcli set contract jane $DCD_CDT_1_8_0/examples/kv_map/build/kv_map -p jane@active
1. dcdcli push action jane upsert '[1, "jane.acct" "jane", "doe", "1 main st", "new york", "NY", "USA", "123"]' -p jane@active
1. dcdcli push action jane get '[1]' -p jane@active