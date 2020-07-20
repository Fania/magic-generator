import Data.List

-- 1. Is it the right number of integers for an n x n matrix?

dataString = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16"
badString = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 14"

-- 1. split string into an array of numbers

dataArray = map (\s -> read s :: Int) $ words dataString
badArray = map (\s -> read s :: Int) $ words badString

-- 2. remove duplicates and get length

isUnique ns = length ns == (length $ nub ns)

-- 3. Is this an n x n (square) array

isNbyN ns = len `mod` order == 0
  where
    len = length ns 
    order = sqrt len

-- // TEST true if square root of length is an integer
-- let order = Math.sqrt(dataArrayUndup.length);
-- console.log(`Square root of array: ${order}`);
-- let isInteger = (dataArrayUndup.length % order == 0);
-- console.log( `TEST: this an n x n (square) array: ${isInteger}` );
