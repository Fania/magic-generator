import Transformations

main = do
    contents <- readFile "data/order4.txt"
    let allLines = lines contents
    let arrays = map (toInts . words) allLines
    -- putStr $ show (take 5 arrays) 
    -- putStr $ show (checker $ take 5 arrays)
    let checked = checker arrays
    writeFile "data/order4-classes.json" (show checked)
  



toInts xs = map (\x -> read x::Int) xs



checker xs = [
    "{ 'numbers':" ++ show x ++ ", 'flags': ["
    ++ (if (isSelfComplementary x) then "'self-compl'" else "")
    ++ (if (isSymmetric x) then ",'symmetric'" else "")
    ++ (if (isPanMagic x) then ",'pandiag'" else "")
    ++ (if (isUltraMagic x) then ",'ultra'" else "")
    ++ (if (isBordered x) then ",'bordered'" else "")
    ++ "] }"
    | x <- xs
  ]


-- isMagic,
-- isOdd,
-- isDoublyEven,
-- isSinglyEven,
-- isSelfComplementary,
-- isSymmetric,
-- isPanMagic,
-- isUltraMagic,
-- isBordered