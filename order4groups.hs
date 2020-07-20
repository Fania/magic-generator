
main = do
    contents <- readFile "data/source4.json"
    let splitIntoLines = lines contents
    let groupedBySquare = map init $ group 6 splitIntoLines
    let splitEachLine = convert groupedBySquare
    let flatList = map concat splitEachLine
    let intLists = map toInts flatList
    -- writeFile "data/order4new.json" (show intLists)
    -- putStr $ show (take 12 intLists)
  
convert [] = []
convert (x:xs) = map (\c -> words c) x : convert xs

toInts xs = map (\x -> read x::Int) xs

group _ [] = []
group n l = (take n l) : (group n (drop n l))






