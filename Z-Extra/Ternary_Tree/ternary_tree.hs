
-- Ternary tree declaration
data TernaryTree  = Empty | Node String TernaryTree TernaryTree TernaryTree | NodeExists | NotReachable | NodeNotFound deriving (Eq,Show)




-- helper Functions

-- this fuction will decide if the nodeExists or should be added to left mid or right
-- if it returns 0 that means after droping from the original string only "" (incase of root node) or "*" left that means node already exist and taking the tail from "*" will result "" then return 0
-- otherwise after droping it will be  ".x" ,".x.y" or whatever how big it is and where x and y are any digit
-- then after taking the tail "x" or "x.y" then we take the head which is x and x will decide where to insert in the insertNode function
extractVersion :: Int -> String -> Int
extractVersion n ys = if l == [] || (tail l) == [] then 0 else charToInt (head (tail l))
    where l = (drop n ys )

--the number of '.' in the string represent the level of the node so I used this fact to find the level of the node
checklevel :: String -> Int
checklevel xs = sum [1 | x <- xs,  x == '.']

-- this function will return True if the string ends with *
reachable :: String -> Bool
reachable ys =  if last ys == '*' then True else False

-- this will remove the * before addig the string
clean :: String -> String
clean ys = filter (/= '*') ys


-- --Part One
charToInt :: Char -> Int
charToInt c = case  c of '0' -> 0
                         '1' -> 1
                         '2' -> 2
                         '3' -> 3
                         '4' -> 4
                         '5' -> 5
                         '6' -> 6
                         '7' -> 7
                         '8' -> 8
                         '9' -> 9
                         otherwise -> 0





--Part Two
-- '*' will be added to the string if it is being inserted in the rightlevel
-- therfore I am assuming that the string doesn't end with '*'
-- here I am inserting a node only if it is the first node (root) or if it's in the right level
-- if the tree is empty it will insert the node directly also root nodes don't contain '.' in them so first  pattern will succeed
--  the function will decide where to insert based on the value of version and if it is an error [NodeExist,NotReachable] then
-- the function will return only the error else it will return the whole tree

insertNode :: TernaryTree -> [Char] -> TernaryTree
insertNode Empty ys = if not ('.' `elem` ys) || (reachable ys) then (Node (clean ys) Empty Empty Empty) else NotReachable
insertNode (Node xs left mid right) ys
    | version == 0 = NodeExists
    | version == 1 = if leftInsert `elem` errorList then leftInsert else  (Node xs leftInsert mid right)
    | version == 2 = if midInsert `elem` errorList then midInsert else (Node xs left midInsert right)
    | version == 3 = if rightInsert `elem` errorList then rightInsert else (Node xs left mid rightInsert)
    | otherwise = NotReachable
    where
        version = extractVersion (length xs) ys
        level1 = checklevel xs
        level2 = checklevel ys
        rightlevel = level2 == (level1 + 1)
        yys = if rightlevel then (ys ++ "*") else ys
        leftInsert = (insertNode left yys)
        midInsert = (insertNode mid yys)
        rightInsert = (insertNode right yys)
        errorList = [NotReachable,NodeExists]


--Part Three
-- will count the total nodes reccursively one node olus the number of nodes of its children
totalNodes :: TernaryTree -> Int
totalNodes Empty = 0
totalNodes (Node _ left mid right) = 1 + totalNodes left + totalNodes mid + totalNodes right

--Part Four
-- will count the height reccursively by choosing the longest path then going upwards
height :: TernaryTree -> Int
height Empty = 0
height (Node _ left mid right) = 1 +  max (max (height left) (height mid)) (height right)


--Part Five
-- level corresponds to the number of '.' in the string
-- similar to total nodes except we count the nodes only if they are in the given level
levelcount :: TernaryTree -> Int -> Int
levelcount Empty _ = 0
levelcount (Node xs left mid right) x
    |level == x = 1
    |level < x = levelcount left x + levelcount mid x + levelcount right x
    where level = checklevel xs


--Part Six
-- here only if the version is 0 that means both strings are the same so we return the node
findNode :: TernaryTree-> [Char] -> TernaryTree
findNode Empty _ = NodeNotFound
findNode (Node xs left mid right) ys
    | version == 0 = (Node xs left mid right)
    | version == 1 = findNode left ys
    | version == 2 = findNode mid ys
    | version == 3 = findNode right ys
    | otherwise = NodeNotFound
    where version = extractVersion (length xs) ys
