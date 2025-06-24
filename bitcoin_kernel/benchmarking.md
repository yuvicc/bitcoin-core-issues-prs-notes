# Benchmarking Script Verify


```./build/bin/bench_bitcoin --filter=VerifyScriptBench```

|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|          116,861.88 |            8,557.11 |    0.7% |    1,545,635.56 |      493,654.62 |  3.131 |      32,734.56 |    0.6% |      0.01 | `VerifyScriptKernelApiBench`

```./build/bin/bench_bitcoin --filter=VerifyScriptBench```

|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|           34,589.45 |           28,910.55 |    0.7% |      457,313.14 |      146,211.04 |  3.128 |       9,465.18 |    0.5% |      0.01 | `VerifyScriptBench`
-------------------------------------------------------------------------------------------
Turns out I was not using the same scripts, so I used same scripts in both the benchmarking test:

```./build/bin/bench_bitcoin --filter=VerifyScriptKernelApiBench```
|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|              938.21 |        1,065,864.46 |    0.3% |       11,763.03 |        3,979.07 |  2.956 |       2,318.00 |    0.1% |      0.01 | `VerifyScriptKernelApiBench`


```./build/bin/bench_bitcoin --filter=VerifyScriptBench```


|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|           34,162.74 |           29,271.66 |    0.2% |      457,313.10 |      144,978.85 |  3.154 |       9,465.17 |    0.5% |      0.01 | `VerifyScriptBench`

I think we are missing some core operations in kernel api benchmarking.

