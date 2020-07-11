import Data.List
import Control.Parallel.Strategies


order5x c3 a1 = sequence_ [ write2File
  [a1,b1,c1,d1,e1, 
   a2,b2,c2,d2,e2,
   a3,b3,c3,d3,e3, 
   a4,b4,c4,d4,e4,
   a5,b5,c5,d5,e5] |
      -- DIAG 1
      b2 <- ([1..25] \\ [c3,a1]),
      d4 <- ([1..25] \\ [c3,a1,b2]),
      e5 <- ([1..25] \\ [c3,a1,b2,d4]),
      65 == a1+b2+c3+d4+e5,
      -- DIAG 2
      e1 <- ([1..25] \\ [c3,a1,b2,d4,e5]),
      d2 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1]),
      b4 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2]),
      a5 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4]),
      65 == e1+d2+c3+b4+a5,
      -- ROW 1
      b1 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5]),
      c1 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1]),
      d1 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1]),
      65 == a1+b1+c1+d1+e1,
      -- COL 2
      b3 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1]),
      b5 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3]),
      65 == b1+b2+b3+b4+b5,
      -- COL 4
      d3 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5]),
      d5 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3]),
      65 == d1+d2+d3+d4+d5,
      -- ROW 5
      c5 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5]),
      65 == a5+b5+c5+d5+e5,
      -- ROW 3
      a3 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5]),
      e3 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3]),
      65 == a3+b3+c3+d3+e3,
      -- COL 3
      c2 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3]),
      c4 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3,c2]),
      65 == c1+c2+c3+c4+c5,
      -- COL 1
      a2 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3,c2,c4]),
      a4 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3,c2,c4,a2]),
      65 == a1+a2+a3+a4+a5,
      -- COL 5
      e2 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3,c2,c4,a2,a4]),
      e4 <- ([1..25] \\ [c3,a1,b2,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3,c2,c4,a2,a4,e2]),
      65 == e1+e2+e3+e4+e5,
      -- ROW 2
      65 == a2+b2+c2+d2+e2,
      -- ROW 4
      65 == a4+b4+c4+d4+e4
  ]



-- generateSquares [x]    = writeToFile x
-- generateSquares (x:xs) = do writeToFile x; generateSquares xs
-- writeToFile x = appendFile "again.txt" $ show x



write2File x = appendFile "order5-13-1.txt" $ show x
-- main = sequence_ [ wf (x,x*x) | x <- [1..9] ] 


main = do order5x 13 1


-- c3 <- [1..13]
-- a1 <- ([1..25] \\ c3)

  -- print order5
  -- print $ parMap rpar order5x [1,2]
  -- 



-- compile: 
-- ghc magic -O2 -prof

-- run:
-- ./magic -RTS



-- RUN WITH:
-- time ./order5 | tee output.txt