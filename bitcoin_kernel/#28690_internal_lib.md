# build: Introduce internal kernel library ([#28690](https://github.com/bitcoin/bitcoin/pull/28690))

## Background
Static libraries are the type of libraries in which bundling of all the programs are done in a single executable which happens in the compile time whereas dynamic libraries 
are collections of programs that are linked at the runtime, the code is not included at compile it is done during the runtime. 

Bitcoin Core has list of [libraries](https://github.com/bitcoin/bitcoin/blob/master/doc/design/libraries.md) which are internal and static, some of them are `libbitcoin_cli`, 
`libbitcoin_node` and `libbitcoin_common`, these libraries are meant to be used by the internal code of Bitcoin Core software and cannot be used externally. Previously we had two
dynamic libraries which are `libbitcoinkernel` and `libbitcoinconsensus` but due to less of adoption of `libbitcoinconsensus` it has been removed from Bitcoin Core, see [here](https://github.com/bitcoin/bitcoin/pull/29189).

So in this PR [#28690](https://github.com/bitcoin/bitcoin/pull/28690) we introduce a new static library libbitcoin_kernel in existence to external libbitcoinkernel library.
Currently we have removed the external `libbitcoinconsensus` library from Bitcoin Core, so it would be better to merge all the source files in the `libbitcoin_kernel` internal library, this
would it would create confusion in `libbitcoin_kernel` and `libbitcoin_consensus` internal library. 

commit diff [e4d80d721b8336047fcd75eb1891e08ac156a903](https://github.com/bitcoin/bitcoin/pull/28690/commits/e4d80d721b8336047fcd75eb1891e08ac156a903):

```bash
  
+++ b/src/CMakeLists.txt
@@ -124,6 +124,57 @@ target_link_libraries(bitcoin_consensus
     secp256k1
 )
 
+add_library(bitcoin_kernel STATIC EXCLUDE_FROM_ALL
+  chain.cpp
+  coins.cpp
+  compressor.cpp
+  consensus/tx_verify.cpp
+  core_read.cpp
+  dbwrapper.cpp
+  deploymentinfo.cpp
+  deploymentstatus.cpp
+  flatfile.cpp
+  kernel/chain.cpp
+  kernel/chainparams.cpp
+  kernel/checks.cpp
+  kernel/coinstats.cpp
+  kernel/context.cpp
+  kernel/cs_main.cpp
+  kernel/disconnected_transactions.cpp
+  kernel/mempool_removal_reason.cpp
+  node/blockstorage.cpp
+  node/chainstate.cpp
+  node/utxo_snapshot.cpp
+  policy/ephemeral_policy.cpp
+  policy/feerate.cpp
+  policy/packages.cpp
+  policy/policy.cpp
+  policy/rbf.cpp
+  policy/settings.cpp
+  policy/truc_policy.cpp
+  pow.cpp
+  script/sigcache.cpp
+  script/solver.cpp
+  signet.cpp
+  +  txdb.cpp
+  txgraph.cpp
+  txmempool.cpp
+  validation.cpp
+  validationinterface.cpp
+  versionbits.cpp
+)
+target_link_libraries(bitcoin_kernel
+  PRIVATE
+    core_interface
+    bitcoin_consensus
+    bitcoin_kernel_util
+    leveldb
+    secp256k1
+    Boost::headers
+    $<TARGET_NAME_IF_EXISTS:USDT::headers>
+    $<$<PLATFORM_ID:Windows>:ws2_32>
+)
+
 if(WITH_ZMQ)
   add_subdirectory(zmq)
 endif()
@@ -137,7 +188,6 @@ add_library(bitcoin_common STATIC EXCLUDE_FROM_ALL
   chain.cpp
   chainparams.cpp
   chainparamsbase.cpp
-  coins.cpp
   common/args.cpp
   common/bloom.cpp
   common/config.cpp
@@ -151,13 +201,9 @@ add_library(bitcoin_common STATIC EXCLUDE_FROM_ALL
   common/signmessage.cpp
   common/system.cpp
   common/url.cpp
-  compressor.cpp
-  core_read.cpp
   core_write.cpp
   -  deploymentinfo.cpp
   external_signer.cpp
   init/common.cpp
-  kernel/chainparams.cpp
   key.cpp
   key_io.cpp
   merkleblock.cpp
@@ -166,9 +212,6 @@ add_library(bitcoin_common STATIC EXCLUDE_FROM_ALL
   netaddress.cpp
   netbase.cpp
   outputtype.cpp
-  policy/feerate.cpp
-  policy/policy.cpp
-  pow.cpp
   protocol.cpp
   psbt.cpp
   rpc/rawtransaction_util.cpp
@@ -180,7 +223,6 @@ add_library(bitcoin_common STATIC EXCLUDE_FROM_ALL
   script/parsing.cpp
   script/sign.cpp
   script/signingprovider.cpp
-  script/solver.cpp
 )
 target_link_libraries(bitcoin_common
   PRIVATE
@@ -211,7 +253,7 @@ if(ENABLE_WALLET)
       core_interface
       bitcoin_wallet
       bitcoin_common
-      bitcoin_util
+      bitcoin_kernel
       Boost::headers
     )
     install_binary_component(bitcoin-wallet HAS_MANPAGE)
@@ -227,10 +269,6 @@ add_library(bitcoin_node STATIC EXCLUDE_FROM_ALL
   bip324.cpp
   blockencodings.cpp
      blockfilter.cpp
-  consensus/tx_verify.cpp
-  dbwrapper.cpp
-  deploymentstatus.cpp
-  flatfile.cpp
   headerssync.cpp
   httprpc.cpp
   httpserver.cpp
@@ -240,22 +278,13 @@ add_library(bitcoin_node STATIC EXCLUDE_FROM_ALL
   index/coinstatsindex.cpp
   index/txindex.cpp
   init.cpp
-  kernel/chain.cpp
-  kernel/checks.cpp
-  kernel/coinstats.cpp
-  kernel/context.cpp
-  kernel/cs_main.cpp
-  kernel/disconnected_transactions.cpp
-  kernel/mempool_removal_reason.cpp
   mapport.cpp
   net.cpp
   net_processing.cpp
   netgroup.cpp
   node/abort.cpp
   node/blockmanager_args.cpp
-  node/blockstorage.cpp
   node/caches.cpp
-  node/chainstate.cpp
   node/chainstatemanager_args.cpp
   node/coin.cpp
   node/coins_view_args.cpp
@@ -278,16 +307,11 @@ add_library(bitcoin_node STATIC EXCLUDE_FROM_ALL
   node/transaction.cpp
   node/txdownloadman_impl.cpp
   node/txreconciliation.cpp
-  node/utxo_snapshot.cpp
   node/warnings.cpp
      noui.cpp
   policy/ephemeral_policy.cpp
   policy/fees.cpp
   policy/fees_args.cpp
-  policy/packages.cpp
-  policy/rbf.cpp
-  policy/settings.cpp
-  policy/truc_policy.cpp
   rest.cpp
   rpc/blockchain.cpp
   rpc/external_signer.cpp
@@ -302,17 +326,9 @@ add_library(bitcoin_node STATIC EXCLUDE_FROM_ALL
   rpc/server_util.cpp
   rpc/signmessage.cpp
   rpc/txoutproof.cpp
-  script/sigcache.cpp
-  signet.cpp
   torcontrol.cpp
-  txdb.cpp
-  txgraph.cpp
-  txmempool.cpp
   txorphanage.cpp
   txrequest.cpp
-  validation.cpp
-  validationinterface.cpp
-  versionbits.cpp
   $<$<TARGET_EXISTS:bitcoin_wallet>:wallet/init.cpp>
   $<$<NOT:$<TARGET_EXISTS:bitcoin_wallet>>:dummywallet.cpp>
 )
@@ -323,6 +339,7 @@ target_link_libraries(bitcoin_node
     bitcoin_util
     $<TARGET_NAME_IF_EXISTS:bitcoin_zmq>
     leveldb
+    bitcoin_kernel
     minisketch
     univalue
     Boost::headers
     @@ -407,6 +424,7 @@ if(BUILD_CLI)
     core_interface
     bitcoin_cli
     bitcoin_common
+    bitcoin_kernel
     bitcoin_util
     libevent::core
     libevent::extra
@@ -422,6 +440,7 @@ if(BUILD_TX)
   target_link_libraries(bitcoin-tx
     core_interface
     bitcoin_common
+    bitcoin_kernel
     bitcoin_util
     univalue
   )
@@ -436,6 +455,7 @@ if(BUILD_UTIL)
   target_link_libraries(bitcoin-util
     core_interface
     bitcoin_common
+    bitcoin_kernel
     bitcoin_util
   )
   install_binary_component(bitcoin-util HAS_MANPAGE)
diff --git a/src/kernel/CMakeLists.txt b/src/kernel/CMakeLists.txt
index 52e91671dd..00ca314f50 100644
--- a/src/kernel/CMakeLists.txt
+++ b/src/kernel/CMakeLists.txt
@@ -10,76 +10,23 @@ include(GNUInstallDirs)
 #       which are absolutely necessary.
 add_library(bitcoinkernel
   bitcoinkernel.cpp
-  chain.cpp
-  checks.cpp
-  chainparams.cpp
-  coinstats.cpp
-  context.cpp
-  -  cs_main.cpp
-  disconnected_transactions.cpp
-  mempool_removal_reason.cpp
-  ../arith_uint256.cpp
-  ../chain.cpp
-  ../coins.cpp
-  ../compressor.cpp
-  ../consensus/merkle.cpp
-  ../consensus/tx_check.cpp
-  ../consensus/tx_verify.cpp
-  ../core_read.cpp
-  ../dbwrapper.cpp
-  ../deploymentinfo.cpp
-  ../deploymentstatus.cpp
-  ../flatfile.cpp
-  ../hash.cpp
-  ../logging.cpp
-  ../node/blockstorage.cpp
-  ../node/chainstate.cpp
-  ../node/utxo_snapshot.cpp
-  ../policy/ephemeral_policy.cpp
-  ../policy/feerate.cpp
-  ../policy/packages.cpp
-  ../policy/policy.cpp
-  ../policy/rbf.cpp
-  ../policy/settings.cpp
-  ../policy/truc_policy.cpp
-  ../pow.cpp
-  ../primitives/block.cpp
-  ../primitives/transaction.cpp
-  ../pubkey.cpp
-  ../random.cpp
-  ../randomenv.cpp
-  ../script/interpreter.cpp
-  ../script/script.cpp
-  ../script/script_error.cpp
-  ../script/sigcache.cpp
-  -  ../script/solver.cpp
-  ../signet.cpp
-  ../streams.cpp
-  ../support/lockedpool.cpp
-  ../sync.cpp
-  ../txdb.cpp
-  ../txmempool.cpp
-  ../uint256.cpp
-  ../util/chaintype.cpp
-  ../util/check.cpp
-  ../util/feefrac.cpp
-  ../util/fs.cpp
-  ../util/fs_helpers.cpp
-  ../util/hasher.cpp
-  ../util/moneystr.cpp
-  ../util/rbf.cpp
-  ../util/serfloat.cpp
-  ../util/signalinterrupt.cpp
-  ../util/strencodings.cpp
-  ../util/string.cpp
-  ../util/syserror.cpp
-  ../util/threadnames.cpp
-  ../util/time.cpp
-  ../util/tokenpipe.cpp
-  ../validation.cpp
-  ../validationinterface.cpp
-  ../versionbits.cpp
 )
+
+get_target_property(kernel_sources bitcoin_kernel SOURCES)
+foreach(src ${kernel_sources})
+  target_sources(bitcoinkernel PRIVATE "${CMAKE_SOURCE_DIR}/src/${src}")
+endforeach()
+
+get_target_property(util_sources bitcoin_kernel_util SOURCES)
+foreach(src ${util_sources})
+  target_sources(bitcoinkernel PRIVATE "${CMAKE_SOURCE_DIR}/src/util/${src}")
+  +endforeach()
+
+get_target_property(consensus_sources bitcoin_consensus SOURCES)
+foreach(src ${consensus_sources})
+  target_sources(bitcoinkernel PRIVATE "${CMAKE_SOURCE_DIR}/src/${src}")
+endforeach()
+
 target_link_libraries(bitcoinkernel
   PRIVATE
     core_interface
diff --git a/src/test/fuzz/CMakeLists.txt b/src/test/fuzz/CMakeLists.txt
index 20a5109a3c..c4be827241 100644
--- a/src/test/fuzz/CMakeLists.txt
+++ b/src/test/fuzz/CMakeLists.txt
@@ -139,6 +139,7 @@ target_link_libraries(fuzz
   test_fuzz
   bitcoin_cli
   bitcoin_common
+  bitcoin_kernel
   bitcoin_util
   minisketch
   leveldb
diff --git a/src/util/CMakeLists.txt b/src/util/CMakeLists.txt
index fdf0081c26..dcaf7c18e3 100644
--- a/src/util/CMakeLists.txt
+++ b/src/util/CMakeLists.txt
@@ -2,15 +2,9 @@
 # Distributed under the MIT software license, see the accompanying
 # file COPYING or https://opensource.org/license/mit/.
 
-add_library(bitcoin_util STATIC EXCLUDE_FROM_ALL
-  asmap.cpp
-  batchpriority.cpp
-  bip32.cpp
-  bytevectorhash.cpp
+add_library(bitcoin_kernel_util STATIC EXCLUDE_FROM_ALL
   chaintype.cpp
      check.cpp
-  exec.cpp
-  exception.cpp
   feefrac.cpp
   fs.cpp
   fs_helpers.cpp
@@ -19,13 +13,9 @@ add_library(bitcoin_util STATIC EXCLUDE_FROM_ALL
   rbf.cpp
   readwritefile.cpp
   serfloat.cpp
+  syserror.cpp
   signalinterrupt.cpp
-  sock.cpp
   strencodings.cpp
-  string.cpp
-  syserror.cpp
-  thread.cpp
-  threadinterrupt.cpp
   threadnames.cpp
   time.cpp
   tokenpipe.cpp
@@ -37,7 +27,7 @@ add_library(bitcoin_util STATIC EXCLUDE_FROM_ALL
   ../sync.cpp
 )
 
-target_link_libraries(bitcoin_util
+target_link_libraries(bitcoin_kernel_util
   PRIVATE
     core_interface
     bitcoin_clientversion
@@ -46,3 +36,25 @@ target_link_libraries(bitcoin_util
     $<$<PLATFORM_ID:Windows>:iphlpapi>
     $<$<PLATFORM_ID:Windows>:bcrypt>
 )
+
+add_library(bitcoin_util STATIC EXCLUDE_FROM_ALL
+  asmap.cpp
+  +  batchpriority.cpp
+  bip32.cpp
+  bytevectorhash.cpp
+  exec.cpp
+  exception.cpp
+  sock.cpp
+  string.cpp
+  thread.cpp
+  threadinterrupt.cpp
+)
+
+target_link_libraries(bitcoin_util
+  PRIVATE
+    core_interface
+    bitcoin_clientversion
+    bitcoin_crypto
+    bitcoin_kernel_util
+    $<$<PLATFORM_ID:Windows>:ws2_32>
+)
diff --git a/src/wallet/CMakeLists.txt b/src/wallet/CMakeLists.txt
index 8ec381df5a..fe8c0a655d 100644
--- a/src/wallet/CMakeLists.txt
+++ b/src/wallet/CMakeLists.txt
@@ -40,6 +40,7 @@ target_link_libraries(bitcoin_wallet
     bitcoin_common
     $<TARGET_NAME_IF_EXISTS:unofficial::sqlite3::sqlite3>
     $<TARGET_NAME_IF_EXISTS:SQLite::SQLite3>
+    bitcoin_kernel
     univalue
     Boost::headers
     $<TARGET_NAME_IF_EXISTS:USDT::headers>
```





 








