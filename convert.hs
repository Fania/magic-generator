

main = do
    contents <- getContents
    putStr $ splitLine contents
  
-- main = interact splitLine

-- splitLine :: String -> String
splitLine x = unwords $ take 100 $ words x