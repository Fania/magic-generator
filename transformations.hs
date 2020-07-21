module Transformations (
  isMagic,
  isOdd,
  isDoublyEven,
  isSinglyEven,
  isSelfComplementary,
  isSymmetric,
  isPanMagic,
  isUltraMagic,
  isBordered
) where

import Data.List


lsx :: [[Int]]
lsx = [[2,10,19,11,23,12,25,7,18,3,20,15,1,8,21,14,6,16,24,5,17,9,22,4,13],[2,15,20,5,23,8,25,11,18,3,22,9,1,14,19,16,6,12,24,7,17,10,21,4,13],[2,15,21,4,23,8,25,11,18,3,22,9,1,14,19,16,6,12,24,7,17,10,20,5,13],[2,15,21,4,23,10,25,9,18,3,20,11,1,14,19,16,6,12,24,7,17,8,22,5,13],[2,15,21,4,23,8,25,11,18,3,22,14,1,9,19,16,6,12,24,7,17,5,20,10,13],[2,15,21,4,23,10,25,9,18,3,20,14,1,11,19,16,6,12,24,7,17,5,22,8,13],[2,10,21,9,23,7,25,11,19,3,22,14,1,8,20,16,4,15,24,6,18,12,17,5,13],[2,12,20,8,23,10,25,6,19,5,21,15,1,11,17,14,4,16,24,7,18,9,22,3,13],[2,14,20,6,23,8,25,10,19,3,21,15,1,11,17,16,4,12,24,9,18,7,22,5,13]]

demo :: [Int]
demo = [2,10,19,11,23,12,25,7,18,3,20,15,1,8,21,14,6,16,24,5,17,9,22,4,13]

o5 :: [Int]
o5 = [18,22,10,14,1,9,11,3,17,25,2,20,24,6,13,21,8,12,5,19,15,4,16,23,7]

b7 :: [Int]
b7 = [64,4,9,54,63,3,10,53,60,15,16,47,48,49,20,5,7,44,22,42,41,25,21,58,51,33,37,29,30,28,38,14,6,32,34,35,36,31,27,59,8,26,40,24,23,43,39,57,52,45,46,18,17,19,50,13,12,61,56,11,2,62,55,1]


isMagic :: [Int] -> Bool
isMagic ms = all (\x -> sum x == mc) rows
          && all (\x -> sum x == mc) cols
          && sum diag1 == mc
          && sum diag2 == mc
  where n = getOrder ms
        rows = getChunks n ms
        cols = transpose rows
        diag1 = getDiag1 rows
        diag2 = getDiag2 rows
        mc = magicConstant n


isOdd :: [Int] -> Bool
isOdd ms = odd n
  where n = getOrder ms

isDoublyEven :: [Int] -> Bool
isDoublyEven ms = mod n 4 == 0
  where n = getOrder ms

isSinglyEven :: [Int] -> Bool
isSinglyEven ms = mod (n+2) 4 == 0
  where n = getOrder ms

-- Complement is rotation or reflection of original
isSelfComplementary :: [Int] -> Bool
isSelfComplementary ms = isMagic $ complement ms

-- Do not exist for singly even orders.
-- Will be self-complimentary too.
-- a.k.a. associative
isSymmetric :: [Int] -> Bool
isSymmetric ms = (not $ isSinglyEven ms)
              && all (\x -> cmpl n (fst x) == (snd x)) edps
  where n = getOrder ms
        edps = equidistantPairs ms

-- Do not exist for singly even orders.
-- a.k.a. pandiagonal, perfect, diabolic, Jain, Nasik
isPanMagic :: [Int] -> Bool
isPanMagic ms = (not $ isSinglyEven ms)
             && all (\d -> sum d == mc) (brokenDiags ms)
  where n = getOrder ms
        mc = magicConstant n

-- Only for orders n ≥ 5.
isUltraMagic :: [Int] -> Bool
isUltraMagic ms = n > 4 && isPanMagic ms && isSymmetric ms
  where n = getOrder ms

-- Only for orders n ≥ 5.
isBordered :: [Int] -> Bool
isBordered ms = n > 4
             && all (\x -> sum x == mc) rows
             && all (\x -> sum x == mc) cols
             && sum diag1 == mc
             && sum diag2 == mc
  where n = getOrder ms
        rows = removeBorders ms
        cols = transpose rows
        diag1 = getDiag1 rows
        diag2 = getDiag2 rows
        mc = sum $ rows !! 0



-- HELPER FUNCTIONS

getChunks :: Int -> [Int] -> [[Int]]
getChunks n [] = []
getChunks n ms = (take n ms) : (getChunks n (drop n ms))

getOrder :: [Int] -> Int
getOrder ms = floor $ sqrt $ fromIntegral $ length ms

cmpl :: Int -> Int -> Int
cmpl n x = ((n^2) + 1) - x

complement :: [Int] -> [Int]
complement ns = map (cmpl n) ns
  where n = getOrder ns

getDiag1 :: [[Int]] -> [Int]
getDiag1 ms = zipWith (!!) ms [0..]

getDiag2 :: [[Int]] -> [Int]
getDiag2 ms = zipWith (!!) ms [n-1,n-2..0]
  where n = length ms

magicConstant :: Int -> Int
magicConstant n = floor $ (fromIntegral n) * ( x / y )
  where x = fromIntegral (n^2 + 1) :: Float
        y = fromIntegral 2 :: Float

shuffle :: Int -> [Int] -> [Int]
shuffle 0 xs = xs
shuffle i (x:xs) = shuffle (i-1) (xs ++ [x])

shuffleDown :: Int -> [[Int]] -> [[Int]]
shuffleDown n [] = []
shuffleDown n (r:rs) = [shuffle n r] ++ (shuffleDown (n-1) rs)

shuffleUp :: Int -> [[Int]] -> [[Int]]
shuffleUp n [] = []
shuffleUp n (r:rs) = [shuffle n r] ++ (shuffleUp (n+1) rs)

brokenDiags1 :: [Int] -> [[Int]]
brokenDiags1 ms = init $ transpose shrows
  where n = getOrder ms
        rows = getChunks n ms
        shrows = shuffleDown n rows

brokenDiags2 :: [Int] -> [[Int]]
brokenDiags2 ms = tail $ transpose shrows
  where n = getOrder ms
        rows = getChunks n ms
        shrows = shuffleUp 0 rows

brokenDiags :: [Int] -> [[Int]]
brokenDiags ms = brokenDiags1 ms ++ brokenDiags2 ms

removeBorders :: [Int] -> [[Int]]
removeBorders ms = transpose $ map (tail . init) midCols
  where n = getOrder ms
        rows = getChunks n ms
        cols = transpose rows
        midCols = tail $ init cols

equidistantPairs :: [Int] -> [(Int,Int)]
equidistantPairs ms 
  | even l = zip f s
  | odd l  = zip f s ++ [(m,m)]
  where l = length ms
        h   = div l 2
        f = take h ms
        s = reverse $ drop h ms
        m = last s




-- Most-perfect magic square when it is a pandiagonal magic square with two further properties (i) each 2×2 subsquare add to 1/k of the magic constant where n = 4k, and (ii) all pairs of integers distant n/2 along any diagonal (major or broken) are complementary (i.e. they sum to n2 + 1). Only for squares of doubly even order. All pandiagonal squares of order 4 are also most perfect.



-- Composite can be partitioned into smaller magic subsquares, wholly or partly, which may or may not overlap with each other. By this definition, bordered magic squares are also composite magic squares.


-- Multimagic remains magic even if all its numbers are replaced by their k-th power for 1 ≤ k ≤ P. They are also known as P-multimagic square or satanic squares. They are also referred to as bimagic squares, trimagic squares, tetramagic squares, pentamagic squares when the value of P is 2, 3, 4, and 5 respectively.





-- D4 TRANSFORMATIONS

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
