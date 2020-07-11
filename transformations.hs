import Data.List
import System.IO
import System.Environment


lsx :: [[Int]]
lsx = [[2,10,19,11,23,12,25,7,18,3,20,15,1,8,21,14,6,16,24,5,17,9,22,4,13],[2,15,20,5,23,8,25,11,18,3,22,9,1,14,19,16,6,12,24,7,17,10,21,4,13],[2,15,21,4,23,8,25,11,18,3,22,9,1,14,19,16,6,12,24,7,17,10,20,5,13],[2,15,21,4,23,10,25,9,18,3,20,11,1,14,19,16,6,12,24,7,17,8,22,5,13],[2,15,21,4,23,8,25,11,18,3,22,14,1,9,19,16,6,12,24,7,17,5,20,10,13],[2,15,21,4,23,10,25,9,18,3,20,14,1,11,19,16,6,12,24,7,17,5,22,8,13],[2,10,21,9,23,7,25,11,19,3,22,14,1,8,20,16,4,15,24,6,18,12,17,5,13],[2,12,20,8,23,10,25,6,19,5,21,15,1,11,17,14,4,16,24,7,18,9,22,3,13],[2,14,20,6,23,8,25,10,19,3,21,15,1,11,17,16,4,12,24,9,18,7,22,5,13]]


getChunks :: Int -> [Int] -> [[Int]]
getChunks n [] = []
getChunks n xs = (take n xs) : (getChunks n (drop n xs))

getOrder :: [Int] -> Int
getOrder xs = floor $ sqrt $ fromIntegral $ length xs


identity :: [Int] -> [Int]
identity xs = xs

reflectH :: [Int] -> [Int]
reflectH xs = concat $ map reverse rows
  where n = getOrder xs
        rows = getChunks n xs

reflectV :: [Int] -> [Int]
reflectV xs = concat $ reverse rows
  where n = getOrder xs
        rows = getChunks n xs

reflectD1 :: [Int] -> [Int]
reflectD1 xs = concat $ transpose rows
  where n = getOrder xs
        rows = getChunks n xs

reflectD2 :: [Int] -> [Int]
reflectD2 xs = concat $ map reverse $ reverse cols
  where n = getOrder xs
        rows = getChunks n xs
        cols = transpose rows

rotate90 :: [Int] -> [Int]
rotate90 xs = concat $ map reverse cols
  where n = getOrder xs
        rows = getChunks n xs
        cols = transpose rows

rotate180 :: [Int] -> [Int]
rotate180 xs = concat $ map reverse $ reverse rows
  where n = getOrder xs
        rows = getChunks n xs

rotate270 :: [Int] -> [Int]
rotate270 xs = concat $ reverse cols
  where n = getOrder xs
        rows = getChunks n xs
        cols = transpose rows

-- identity
-- 2,10,19,11,23
-- 12,25,7,18,3
-- 20,15,1,8,21
-- 14,6,16,24,5
-- 17,9,22,4,13


test = 
  (reflectH $ head lsx) == [23,11,19,10,2,3,18,7,25,12,21,8,1,15,20,5,24,16,6,14,13,4,22,9,17] 
  &&
  (reflectV $ head lsx) == [17,9,22,4,13,14,6,16,24,5,20,15,1,8,21,12,25,7,18,3,2,10,19,11,23]
  &&
  (reflectD1 $ head lsx) == [2,12,20,14,17,10,25,15,6,9,19,7,1,16,22,11,18,8,24,4,23,3,21,5,13]
  &&
  (reflectD2 $ head lsx) == [13,5,21,3,23,4,24,8,18,11,22,16,1,7,19,9,6,15,25,10,17,14,20,12,2]
  &&
  (rotate90 $ head lsx) == [17,14,20,12,2,9,6,15,25,10,22,16,1,7,19,4,24,8,18,11,13,5,21,3,23]
  &&
  (rotate180 $ head lsx) == [13,4,22,9,17,5,24,16,6,14,21,8,1,15,20,3,18,7,25,12,23,11,19,10,2]
  &&
  (rotate270 $ head lsx) == [23,3,21,5,13,11,18,8,24,4,19,7,1,16,22,10,25,15,6,9,2,12,20,14,17]


-- main = do
  -- s <- readFile "order5-1-2.txt"
  -- print $ getOrder $ head lsx
  -- print $ length order4s
  -- x <- getArgs
  -- y <- [ z::Int | z <- words getArgs ]
  -- print $ transform3 [ x | x <- words getArgs ]






-- cmpl order num
-- e.g. cmpl 4 1 = 16
-- cmpl :: Int -> Int -> Int
-- cmpl n x = ((n^2) + 1) - x

-- -- complement square -> square
-- complement :: [Int] -> [Int]
-- complement ns = Data.List.map (cmpl 4) ns

-- complement $ order4s !! 0


-- transform :: [Int] -> [[Int]]
-- transform ns = [ns,
--                 mirrorLR ns,
--                 mirrorUD ns,
--                 mirrorD1 ns,
--                 mirrorD2 ns,
--                 rotate90 ns,
--                 rotate180 ns,
--                 rotateM90 ns]

-- -- /8
-- reduceD4 :: [[Int]] -> [[Int]]
-- reduceD4 [] = []
-- reduceD4 (n:ns) = n : reduceD4 (ns \\ (tail $ transform n))

-- -- /2
-- removeCompls :: [[Int]] -> [[Int]]
-- removeCompls [] = []
-- removeCompls (n:ns) = n : removeCompls (delete (complement n) ns)


