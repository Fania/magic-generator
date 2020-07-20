
main = do
    contents <- readFile "data/Center1.txt"
    let allLines = lines contents
    let chunks = map (init) $ group 6 allLines
    let cleaned = convert chunks
    let flatList = map concat cleaned
    let intList = map toInts flatList
    writeFile "data/order5-1.json" (show intList)
    -- putStr $ show (take 12 intList)
  
convert [] = []
convert (x:xs) = map (\c -> words c) x : convert xs

toInts xs = map (\x -> read x::Int) xs

group _ [] = []
group n l = (take n l) : (group n (drop n l))