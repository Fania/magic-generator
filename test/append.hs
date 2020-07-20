

-- main = appendFile "squares.txt" (show [(x,x*x) | x <- [0,0.1..2]])





-- xs = [ (x,x*x) | x <- [1..9] ] 

-- createSquares [x]    = wf x
-- createSquares (x:xs) = do wf x; createSquares xs

-- wf x = appendFile "squares.txt" $ show x

-- main = createSquares xs





wf x = appendFile "squares.txt" $ show x

main = sequence_ [ wf (x,x*x) | x <- [1..9] ] 