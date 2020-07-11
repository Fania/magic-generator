-- inspired by Lin, Liu, Fang & Deng paper

import Data.List


-- length vectors = 1394
-- length with duplicates in different order = 167280
-- The use of < makes sure there are only unique numbers used and exludes 
-- duplicate results in different orders.
vectors :: [[Int]]
vectors = [ [x1,x2,x3,x4,x5] | 
            x1 <- [1..25], 
            x2 <- [1..25], x1<x2,
            x3 <- [1..25], x2<x3,
            x4 <- [1..25], x3<x4,
            x5 <- [1..25], x4<x5,
            x1+x2+x3+x4+x5 == 65         
          ]






-- main = do
--   print vectors
--   print vectors2

