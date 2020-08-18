import Transformations

main = do
    contents <- readFile "data/order4.txt"
    let allLines = lines contents
    let arrays = map (toInts . words) allLines
    -- putStr $ show (take 5 arrays) 
    -- putStr $ show (checker $ take 5 arrays)
    let checked = checker4 arrays
    writeFile "data/order4-classes.json" (show checked)
  



toInts xs = map (\x -> read x::Int) xs



checker4 xs = [
    "{ \"numbers\":" ++ show x ++ ", \"flags\": ["
    ++ (if (isSelfComplementary x) then "\"self-compl\"" else "")
    ++ (if (isSymmetric x) then ",\"symmetric\"" else "")
    ++ (if (isPanMagic x) then ",\"pandiag\"" else "")
    ++ (if (elara x) then ",\"elara\"" else "")
    ++ (if (asteria x) then ",\"asteria\"" else "")
    ++ (if (hestia x) then ",\"hestia\"" else "")
    ++ (if (hera x) then ",\"hera\"" else "")
    ++ (if (demeter x) then ",\"demeter\"" else "")
    ++ (if (niobe x) then ",\"niobe\"" else "")
    ++ (if (thaumas x) then ",\"thaumas\"" else "")
    ++ (if (nemesis x) then ",\"nemesis\"" else "")
    ++ (if (arges x) then ",\"arges\"" else "")
    ++ (if (eris x) then ",\"eris\"" else "")
    ++ (if (moros x) then ",\"moros\"" else "")
    ++ (if (cottus x) then ",\"cottus\"" else "")
    ++ "] }"
    | x <- xs
  ]
