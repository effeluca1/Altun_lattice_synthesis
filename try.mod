param m;
param n;
param q;

# SETS
set I := 1 .. n; # rows
set J := 1 .. m; # column start
set U := 1 .. n;  
set V := 1 .. m;  
set L := 1 .. m; # column finish
set K := 1 .. q; # value

# DATA
param a{i in I , j in J , k in K};

#VARS
var y{j in J , l in L} binary; 
var z{i in I , j in J , k in K} binary;
var x{i in I, j in J, u in U, v in V, k in K} ;

#OBJ
#come definisco coppie u,v wrt i....
maximize R: sum{k in K} sum{i in I,j in J,u in U,v in V: abs(u-i) +  abs(v-j) = 1} x[i,j,u,v,k];

#CONSTR

subject to assign_try{i in I , j in J , k in K}: z[i,j,k]= sum{l in L} y[l,j] * a[i,l,k]; 
subject to consistency_I{l in L}: sum{j in J} y[l,j]=1; 
subject to consistency_L{j in J}: sum{l in L} y[l,j]=1; 
subject to mapping {i in I, j in J, u in U, v in V, k in K}: x[i,j,u,v,k] <=  z[i, j, k] + z[u, v, k] - 1 ;

solve;
#display L;
#display K;
#display a;
display y;


end;

