import Control.Parallel.Strategies

fib 0 = 1
fib 1 = 1
fib n = fib (n-1) + fib (n-2)

main = print resultSeq where
  resultPlain = [fib 34, fib 34, fib 34, fib 34]
  resultPar   = parMap rpar id [fib 34, fib 34, fib 34, fib 34]
  resultSeq   = parMap rseq id [fib 34, fib 34, fib 34, fib 34]


-- compile:  ghc -threaded --make parMap
-- run:      time ./rpar +RTS -N4



-- try compile: ghc --make -threaded -eventlog -rtsopts -O2 parMap

