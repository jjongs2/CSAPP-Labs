```bash
$ make
$ ./mdriver -va -t traces
```

```
Using default tracefiles in traces/
Measuring performance with gettimeofday().

Results for mm malloc:
               trace  valid  util     ops      secs    Kops
      amptjp-bal.rep    yes   98%    5694  0.000104   54593
        cccp-bal.rep    yes   98%    5848  0.000087   66911
     cp-decl-bal.rep    yes   98%    6648  0.000110   60491
        expr-bal.rep    yes   99%    5380  0.000080   67673
  coalescing-bal.rep    yes   96%   14400  0.000145   99174
      random-bal.rep    yes   94%    4800  0.000423   11337
     random2-bal.rep    yes   94%    4800  0.000400   12003
      binary-bal.rep    yes   84%   12000  0.000131   91954
     binary2-bal.rep    yes   75%   24000  0.000299   80160
     realloc-bal.rep    yes  100%   14401  0.000148   97436
    realloc2-bal.rep    yes   86%   14401  0.000141  102207
               Total          93%  112372  0.002068   54333

Perf index = 56 (util) + 40 (thru) = 96/100
```
