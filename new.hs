-- inspired by Lin, Liu, Fang & Deng paper

import Data.List
import Control.Parallel.Strategies

-- length vectors = 1394
-- length with duplicates in different order = 167280
-- The use of < makes sure there are only unique numbers used and exludes 
-- duplicate results in different orders.
vectors :: [[Int]]
vectors = [ 
    [x1,x2,x3,x4,x5] | 
      x1 <- [1..25], 
      x2 <- [1..25], x1<x2,
      x3 <- [1..25], x2<x3,
      x4 <- [1..25], x3<x4,
      x5 <- [1..25], x4<x5,
      x1+x2+x3+x4+x5 == 65         
  ]

-- includes [1,4]
includes :: [Int] -> [[Int]]
includes z = [ v | v <- vectors, isSubsequenceOf (sort z) v ]

-- excludes [1,4,5,16,15,22,23,10]
excludes :: [Int] -> [[Int]]
excludes z = [ v | v <- ( [s | s <- vectors, (s \\ z) == s ] ) ]

--           incl      excl
-- feasable [1,2] [3,4,23,14,15]
feasable :: [Int] -> [Int] -> [[Int]]
feasable ns fs = intersect 
                  (includes ns) 
                  (excludes (fs \\ ns))


shuffle :: [[Int]] -> [[Int]]
shuffle fs = concat $ map permutations fs




-- squares c3 = sequence_ [ write2File
-- squares :: Int -> [[Int]]
-- squares c3 = [
squares c3 = sequence_ [ write2File
  [a1,b1,c1,d1,e1, 
   a2,b2,c2,d2,e2,
   a3,b3,c3,d3,e3, 
   a4,b4,c4,d4,e4,
   a5,b5,c5,d5,e5] |

    -- c3  <- [1..13], -- first num
    diag1 <- (filter (\x -> x!!2 == c3) (shuffle $ includes [c3])),
    let a1 = diag1 !! 0,
    let b2 = diag1 !! 1,
    let d4 = diag1 !! 3,
    let e5 = diag1 !! 4,
    -- a1 <- diag1,
    -- b2 <- diag1,
    -- b2 > a1, -- C > B
    -- d4 <- diag1,
    -- d4 > b2, -- D > C
    -- e5 <- diag1,
    -- e5 > a1, -- E > B
    -- a1+b2+c3+d4+e5 == 65,
    
    diag2 <- (filter (\x -> x!!2 == c3) (shuffle $ feasable [c3] [a1,b2,c3,d4,e5])),
    let e1 = diag2 !! 0,
    let d2 = diag2 !! 1,
    let b4 = diag2 !! 3,
    let a5 = diag2 !! 4,
    -- e1 <- diag2,
    -- e1 > a1, -- F > B
    -- d2 <- diag2,
    -- d2 > a1, -- H > B
    -- b4 <- diag2,
    -- b4 > a1, -- I > B
    -- a5 <- diag2,
    -- a5 > e1, -- G > F
    -- e1+d2+c3+b4+a5 == 65,

    row1 <- (filter (\x -> x!!0 == a1 && x!!4 == e1) (shuffle $ feasable [a1,e1] [b2,c3,d4,e5,d2,b4,a5])),
    let b1 = row1 !! 1,
    let c1 = row1 !! 2,
    let d1 = row1 !! 3,
    -- b1 <- row1,
    -- c1 <- row1,
    -- d1 <- row1,
    -- a1+b1+c1+d1+e1 == 65,

    col2 <- (filter (\x -> x!!0 == b1 && x!!1 == b2 && x!!3 == b4) (shuffle $ feasable [b1,b2,b4] [a1,b2,c3,d4,e5,e1,d2,b4,a5,b1,c1,d1])),
    let b3 = col2 !! 1,
    let b5 = col2 !! 4,
    -- b3 <- col2,
    -- b5 <- col2,
    -- b1+b2+b3+b4+b5 == 65,

    col4 <- (filter (\x -> x!!0 == d1 && x!!1 == d2 && x!!3 == d4) (shuffle $ feasable [d1,d2,d4] [a1,b2,c3,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5])),
    let d3 = col4 !! 2,
    let d5 = col4 !! 4,
    -- d3 <- col4,
    -- d5 <- col4,
    -- d1+d2+d3+d4+d5 == 65,

    row5 <- (filter (\x -> x!!0 == a5 && x!!1 == b5 && x!!3 == d5 && x!!4 == e5) (shuffle $ feasable [a5,b5,d5,e5] [a1,b2,c3,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5])),
    let c5 = row5 !! 4,
    -- c5 <- row5,
    -- a5+b5+c5+d5+e5 == 65,

    row3 <- (filter (\x -> x!!1 == b3 && x!!2 == c3 && x!!3 == d3) (shuffle $ feasable [b3,c3,d3] [a1,b2,c3,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5])),
    let a3 = row3 !! 0,
    let e3 = row3 !! 4,
    -- a3 <- row3,
    -- e3 <- row3,
    -- a3+b3+c3+d3+e3 == 65,

    col3 <- (filter (\x -> x!!0 == c1 && x!!2 == c3 && x!!4 == c5) (shuffle $ feasable [c1,c3,c5] [a1,b2,c3,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3])),
    let c2 = col3 !! 1,
    let c4 = col3 !! 3,
    -- c2 <- col3,
    -- c4 <- col3,
    -- c1+c2+c3+c4+c5 == 65,

    col1 <- (filter (\x -> x!!0 == a1 && x!!2 == a3 && x!!4 == a5) (shuffle $ feasable [a1,a3,a5] [a1,b2,c3,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3,c2,c4])),
    let a2 = col1 !! 1,
    let a4 = col1 !! 3,
    -- a2 <- col1,
    -- a4 <- col1,
    -- a1+a2+a3+a4+a5 == 65,

    col5 <- (filter (\x -> x!!0 == e1 && x!!2 == e3 && x!!4 == e5) (shuffle $ feasable [e1,e3,e5] [a1,b2,c3,d4,e5,e1,d2,b4,a5,b1,c1,d1,b3,b5,d3,d5,c5,a3,e3,c2,c4,a2,a4])),
    let e2 = col5 !! 1,
    let e4 = col5 !! 3,
    -- e2 <- col5,
    -- e4 <- col5,
    -- e1+e2+e3+e4+e5 == 65,
    
    a2+b2+c2+d2+e2 == 65,
    a4+b4+c4+d4+e4 == 65
  ]


write2File x = appendFile "order5-1new.txt" $ show x
-- main = sequence_ [ wf (x,x*x) | x <- [1..9] ] 


main = do squares 1

-- main = do
--   print vectors
--   print vectors2
