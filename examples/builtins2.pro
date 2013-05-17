clauses

insertSorted(X,[],[X]).
insertSorted(X,[H|T],[X,H|T]):-<(X,H).
insertSorted(X,[H|T],[H|T1]):->=(X,H), insertSorted(X,T,T1).

main():- insertSorted(5,[1,2,3],L1), insertSorted(1, [2,3],L2), insertSorted(3,[1,2,4],L3).
