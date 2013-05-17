domains
list=nil();pair(any,any).
clauses

append([],X,X).
append([X|Y],Z,[X|W]) :- append(Y,Z,W).  

main():-append([1,2],[3,4,5],X).
